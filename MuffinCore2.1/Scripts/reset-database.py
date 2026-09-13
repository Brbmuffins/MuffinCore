"""Factory reset all three repack game databases from the packaged clean snapshot."""
from __future__ import annotations

import argparse
from datetime import datetime, timezone
import gzip
import hashlib
import importlib.util
from pathlib import Path
import re
import shutil
import subprocess
import sys

DATABASES = ("acore_auth", "acore_characters", "acore_world")
SERVICES = {"mysql": "mysql/bin/mysqld.exe", "world": "Core/worldserver.exe",
            "auth": "Core/authserver.exe", "supervisor": "Runtime/python/python.exe",
            "relay": "Runtime/python/python.exe"}


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def safe(root, relative):
    root = Path(root).absolute()
    path = root / relative
    require(not Path(relative).is_absolute() and ".." not in Path(relative).parts,
            "Invalid package path.")
    for part in (path,) + tuple(path.parents):
        if part.exists() or part.is_symlink():
            require(not part.is_symlink() and not (part.lstat().st_file_attributes & 0x400),
                    "Linked package paths are not supported: " + str(part))
    require(path.resolve().is_relative_to(root.resolve()), "Path escapes this repack.")
    return path


def sha(path):
    with path.open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest()


class Reset:
    def __init__(self, root):
        self.root = Path(root).absolute()
        for relative in ("Scripts/manage.py", "Settings/database.json", "Settings/repack.json",
                         "mysql/data", "mysql/my.ini", "mysql/admin-client.ini", "mysql/logs",
                         "Core/configs", "Core/Logs", "BugReport/reports", "BugReport/Logs",
                         ".state", "Database-Backups", "Database", *SERVICES.values(),
                         "mysql/bin/mysql.exe", "mysql/bin/mysqldump.exe"):
            safe(self.root, relative)
        spec = importlib.util.spec_from_file_location("coa_reset_manager", self.root / "Scripts/manage.py")
        self.m = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(self.m)
        self.config = self.m.settings()
        self.journal = safe(self.root, ".state/database-reset-incomplete.json")

    def server_identity(self):
        return {name: sha(safe(self.root, "Core/" + name)) for name in ("authserver.exe", "worldserver.exe")}

    def own_processes(self):
        for name, relative in SERVICES.items():
            record = self.m.process(name)
            require(not record or Path(record["exe"]).resolve() == safe(self.root, relative).resolve(),
                    "A saved process belongs to another repack: " + name)

    def endpoint(self):
        self.own_processes()
        require(self.m.ready("mysql", self.config["mysqlPort"]),
                "This repack does not own the MySQL listener.")
        values = dict(re.findall(r"^([a-z]+)=(.*)$", safe(
            self.root, "mysql/admin-client.ini").read_text(encoding="utf-8"), re.M))
        require(values.get("host") == "127.0.0.1" and values.get("protocol") == "tcp"
                and values.get("port") == str(self.config["mysqlPort"]),
                "Database client settings do not target this repack.")

    def command(self, executable):
        self.endpoint()
        return [str(safe(self.root, "mysql/bin/" + executable)),
                "--defaults-file=" + str(safe(self.root, "mysql/admin-client.ini")),
                "--no-login-paths", "--host=127.0.0.1", "--protocol=tcp",
                "--port=" + str(self.config["mysqlPort"])]

    def query(self, sql):
        result = subprocess.run(self.command("mysql.exe") + ["--batch", "--skip-column-names"],
            input=("SELECT '__RESET_BEGIN__';\n" + sql + "\n;SELECT '__RESET_END__';").encode(),
            capture_output=True, timeout=900, creationflags=self.m.HIDDEN)
        require(result.returncode == 0, "Database command failed. No automatic retry was attempted.")
        lines = result.stdout.decode("utf-8").splitlines()
        require(lines and lines[0] == "__RESET_BEGIN__" and lines[-1] == "__RESET_END__",
                "Incomplete database response. No automatic retry was attempted.")
        return [line.split("\t") for line in lines[1:-1]]

    def identity(self):
        rows = self.query("SELECT @@port,@@datadir;")
        require(len(rows) == 1 and len(rows[0]) == 2, "Incomplete database identity.")
        port, directory = rows[0]
        require(port == str(self.config["mysqlPort"]) and
                Path(directory.replace("\\\\", "\\")).resolve() == safe(self.root, "mysql/data").resolve(),
                "Wrong MySQL data directory; no reset was applied.")

    def quiescent(self):
        self.identity()
        require(not any(self.m.process(name) for name in SERVICES if name != "mysql"),
                "A repack service is still running.")
        require(self.query("SELECT COUNT(*) FROM information_schema.PROCESSLIST "
                           "WHERE ID<>CONNECTION_ID() AND USER NOT IN ('event_scheduler','system user');")
                == [["0"]], "Another database client is connected. Close it before resetting.")

    def inventory(self):
        rows = self.query("SELECT TABLE_SCHEMA,TABLE_NAME,ENGINE,TABLE_TYPE FROM information_schema.TABLES "
                          "WHERE TABLE_SCHEMA IN ('acore_auth','acore_characters','acore_world') "
                          "ORDER BY TABLE_SCHEMA,TABLE_NAME;")
        tables = {name: [] for name in DATABASES}
        for schema, table, engine, kind in rows:
            require(engine == "InnoDB" and kind == "BASE TABLE",
                    "Reset requires InnoDB tables without views: " + table)
            require(bool(re.fullmatch(r"[A-Za-z0-9_]+", table)), "Unsupported table name.")
            tables[schema].append(table)
        require(all(tables.values()), "A repack database is missing or empty.")
        require(self.query("SELECT COUNT(*) FROM information_schema.EVENTS WHERE EVENT_SCHEMA IN "
                           "('acore_auth','acore_characters','acore_world') AND STATUS='ENABLED';") == [["0"]],
                "Disable custom scheduled database events before reset.")
        return tables

    def row_counts(self, tables):
        names = [(db, table) for db in DATABASES for table in tables[db]]
        rows = self.query("\n".join(f"SELECT '{db}.{table}',COUNT(*) FROM `{db}`.`{table}`;"
                                   for db, table in names))
        require([r[0] for r in rows] == [db + "." + table for db, table in names],
                "Incomplete database row counts.")
        return {name: int(count) for name, count in rows}

    def export(self, folder):
        """Export only game schemas; MySQL users/passwords stay package-local."""
        self.quiescent()
        tables = self.inventory()
        counts = self.row_counts(tables)
        dump = folder / "databases.sql"
        result = subprocess.run(self.command("mysqldump.exe") + ["--single-transaction", "--quick",
            "--hex-blob", "--skip-lock-tables", "--no-tablespaces", "--set-gtid-purged=OFF",
            "--column-statistics=0", "--routines", "--events", "--triggers", "--add-drop-database",
            "--default-character-set=utf8mb4", "--result-file=" + str(dump), "--databases", *DATABASES],
            capture_output=True, timeout=1800, creationflags=self.m.HIDDEN)
        require(result.returncode == 0 and dump.is_file() and dump.stat().st_size > 100,
                "Database backup failed; no reset was applied. Incomplete backup: " + str(folder))
        require(self.row_counts(tables) == counts, "Database changed while backing up; no reset was applied.")
        with dump.open("rb") as source, gzip.open(folder / "databases.sql.gz", "wb", compresslevel=6) as target:
            shutil.copyfileobj(source, target)
        info = {"format": 1, "databases": list(DATABASES), "serverIdentity": self.server_identity(),
                "gzipSHA256": sha(folder / "databases.sql.gz"), "sqlSHA256": sha(dump),
                "tables": tables, "rowCounts": counts}
        dump.unlink()
        return info

    def backup(self):
        folder = safe(self.root, "Database-Backups/" + datetime.now(timezone.utc).strftime("%Y%m%d-%H%M%S-%f"))
        folder.mkdir(parents=True)
        print("Backing up accounts, characters and world: " + str(folder), flush=True)
        info = self.export(folder)
        info["databaseCredentialSHA256"] = sha(self.root / "Settings/database.json")
        info["console"] = {key: self.config[key] for key in ("raUsername", "raPassword")}
        self.m.atomic_json(folder / "snapshot.json", info)
        (folder / "Restore_Backup.bat").write_text(
            '@echo off\nsetlocal DisableDelayedExpansion\n'
            '"%~dp0..\\..\\Runtime\\python\\python.exe" -B "%~dp0..\\..\\Scripts\\reset-database.py" '
            '--restore ' + folder.name + '\nset "RESULT=%ERRORLEVEL%"\necho.\npause\nexit /b %RESULT%\n',
            encoding="ascii", newline="\r\n")
        (folder / "README.txt").write_text(
            "Private backup: accounts/passwords, characters and world database. Do not distribute.\n"
            "Run Restore_Backup.bat to restore this snapshot. Current data is backed up first.\n"
            "Keep this folder under Database-Backups in its matching repack. Services stay stopped.\n",
            encoding="utf-8")
        return folder

    def snapshot(self, restore):
        if restore:
            require(bool(re.fullmatch(r"\d{8}-\d{6}-\d{6}", restore)), "Invalid backup name.")
        relative = "Database-Backups/" + restore if restore else "Database/Clean"
        folder = safe(self.root, relative)
        info = self.m.read_json(safe(self.root, relative + "/snapshot.json"))
        archive = safe(self.root, relative + "/databases.sql.gz")
        require(info.get("format") == 1 and info.get("databases") == list(DATABASES)
                and info.get("gzipSHA256") == sha(archive), "The database snapshot is incomplete or changed.")
        require(info.get("serverIdentity") == self.server_identity(),
                "This database snapshot is for another server version; keep the matching repack.")
        if restore:
            require(info.get("databaseCredentialSHA256") == sha(self.root / "Settings/database.json"),
                    "The backup belongs to different database credentials.")
        return folder, info

    def import_snapshot(self, sql):
        with sql.open("rb") as source:
            result = subprocess.run(self.command("mysql.exe") + ["--binary-mode"], stdin=source,
                capture_output=True, timeout=3600, creationflags=self.m.HIDDEN)
        require(result.returncode == 0, "Database import failed. Use the printed recovery backup before starting.")

    def apply(self, restore=None):
        folder, info = self.snapshot(restore)
        print("Repack: " + str(self.root))
        print("This REPLACES ALL accounts, characters and WORLD database changes.")
        print("Restore: " + str(folder) if restore else "Factory defaults: LOCAL / local, GM level 3, no characters.")
        print("Services stop first. All three databases are backed up. Client files and report queues stay intact.")
        prompt = "Type RESTORE to continue (anything else cancels): " if restore else "Delete everything? (y/n): "
        answer = input(prompt).strip()
        confirmed = answer == "RESTORE" if restore else answer.lower() == "y"
        if not confirmed:
            print("Cancelled. No services or databases were changed.")
            return
        with self.m.control_lock():
            # Validate the entire compressed stream before stopping services or deleting data.
            sql = safe(self.root, ".state/database-reset.sql")
            try:
                with gzip.open(folder / "databases.sql.gz", "rb") as source, sql.open("wb") as target:
                    shutil.copyfileobj(source, target)
                require(sha(sql) == info["sqlSHA256"], "The SQL snapshot is damaged.")
                self.own_processes()
                if self.m.process("mysql"):
                    self.identity()
                self.m.stop_all(self.config)
                require(not any(self.m.process(name) for name in SERVICES), "A repack service is still running.")
                self.m.prepare(self.config)
                self.m.start_mysql(self.config, recovery=True)
                try:
                    self.quiescent()
                    if self.journal.exists():
                        # Never replace the last good backup with a partial failed import.
                        prior = self.m.read_json(self.journal)
                        recovery, _ = self.snapshot(prior["backup"])
                        print("Reusing pre-interruption recovery backup: " + str(recovery), flush=True)
                    else:
                        recovery = self.backup()
                    self.quiescent()
                    self.m.atomic_json(self.journal, {"backup": recovery.name, "snapshot": folder.name})
                    print("Restoring all three databases; this can take several minutes...", flush=True)
                    self.import_snapshot(sql)
                    require(self.inventory() == info["tables"] and self.row_counts(info["tables"]) == info["rowCounts"],
                            "Restored database tables/row counts differ from the snapshot.")
                    # The portable snapshot may have been captured on alternate test ports.
                    self.query("UPDATE acore_auth.realmlist SET name='AzerothCore',address='127.0.0.1',"
                               "localAddress='127.0.0.1',flag=0,port=" + str(self.config["worldPort"]) + " WHERE id=1;")
                    self.config.update(info["console"] if restore else {"raUsername": "local", "raPassword": "local"})
                    self.m.atomic_json(self.root / "Settings/repack.json", self.config)
                    self.journal.unlink()
                    print("Restore complete." if restore else "Full reset complete. Account: local / local (GM 3).")
                    print("Recovery backup: " + str(recovery), flush=True)
                finally:
                    if self.journal.exists():
                        print("Reset incomplete. Recovery backup: Database-Backups/" +
                              self.m.read_json(self.journal)["backup"], file=sys.stderr, flush=True)
                    self.own_processes()
                    self.identity()
                    self.m.stop_all(self.config)
            finally:
                if sql.exists():
                    sql.unlink()
        print("All services are stopped. Run Start_All_Server.bat to test from here.")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", metavar="BACKUP_NAME", help="restore a folder under Database-Backups")
    args = parser.parse_args()
    Reset(Path(__file__).resolve().parents[1]).apply(args.restore)


if __name__ == "__main__":
    try:
        main()
    except (EOFError, KeyboardInterrupt):
        print("Cancelled/interrupted. Keep any printed recovery backup.", file=sys.stderr)
        raise SystemExit(1)
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError) as error:
        print("ERROR: " + str(error), file=sys.stderr, flush=True)
        raise SystemExit(1)
