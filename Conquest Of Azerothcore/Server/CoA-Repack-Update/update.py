"""Portable incremental CoA-Repack updater; run with the recipient's bundled Python."""
from __future__ import annotations

import argparse
import hashlib
import importlib.util
import json
import os
from pathlib import Path, PurePosixPath
import re
import shutil
import sys
import zipfile


STATIC = ('Core/worldserver.exe', 'Source/server-source.zip', 'RELEASE.json', 'RELEASE.txt', 'MANIFEST.json')
SQL_DIRS = ('data/sql/archive/db_world', 'data/sql/custom/db_world',
            'data/sql/updates/db_world', 'data/sql/updates/pending_db_world')
ISSUE_MIGRATIONS = ('rev_1789223314829733700.sql',
                    'rev_20260912_00_custom_class_stat_progression.sql')
WORLD_TABLES = {
    'spell_group': ('id,spell_id', 'id=1137', 'id,spell_id', ISSUE_MIGRATIONS[0]),
    'spell_group_stack_rules': ('group_id,stack_rule,description', 'group_id=1137', 'group_id', ISSUE_MIGRATIONS[0]),
    'player_class_stats': ('Class,Level,BaseHP,BaseMana,Strength,Agility,Stamina,Intellect,Spirit',
                           'Class BETWEEN 12 AND 32 AND Level BETWEEN 2 AND 80', 'Class,Level', ISSUE_MIGRATIONS[1]),
}


def rows_sha(rows):
    return hashlib.sha256(json.dumps(rows, ensure_ascii=False, separators=(',', ':')).encode('utf-8')).hexdigest()


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def sha(path):
    with Path(path).open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def read(path):
    return json.loads(Path(path).read_text(encoding='utf-8-sig'))


def save(path, data):
    path = Path(path)
    temp = safe(path.parent, path.name + '.tmp')
    temp.write_text(json.dumps(data, indent=2, ensure_ascii=False) + '\n', encoding='utf-8')
    os.replace(temp, path)


def safe(root, relative):
    require(isinstance(relative, str) and relative and '\\' not in relative and ':' not in relative,
            'Invalid package path.')
    parts = PurePosixPath(relative).parts
    require(not relative.startswith('/') and all(p not in ('.', '..') for p in parts), 'Unsafe package path.')
    root = Path(root).absolute()
    for ancestor in (root,) + tuple(root.parents):
        if ancestor.exists():
            require(not ancestor.is_symlink() and not (ancestor.stat(follow_symlinks=False).st_file_attributes & 0x400),
                    'Linked package roots are not supported.')
    path = root
    for part in (None,) + parts:
        if part is not None:
            path /= part
        if path.exists() or path.is_symlink():
            require(not path.is_symlink() and not (path.stat(follow_symlinks=False).st_file_attributes & 0x400),
                    'Linked paths are not supported: ' + str(path))
    require(path.resolve().is_relative_to(root.resolve()), 'Path escapes package.')
    return path


class Native:
    def __init__(self, root):
        self.root = root
        for name in ('mysql/data', 'mysql/my.ini', 'mysql/admin-client.ini', 'Core/configs/worldserver.conf',
                     'Core/configs/authserver.conf', 'Core/Logs', '.state/control.lock'):
            safe(root, name)
        spec = importlib.util.spec_from_file_location('coa_update_manager', safe(root, 'Scripts/manage.py'))
        self.m = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(self.m)
        self.config = self.m.settings()

    def own_processes(self):
        for name, relative in {'mysql': 'mysql/bin/mysqld.exe', 'world': 'Core/worldserver.exe',
                               'auth': 'Core/authserver.exe', 'supervisor': 'Runtime/python/python.exe',
                               'relay': 'Runtime/python/python.exe'}.items():
            record = self.m.process(name)
            require(not record or Path(record['exe']).resolve() == safe(self.root, relative).resolve(),
                    'A saved process belongs to another repack: ' + name)

    def lock(self):
        self.own_processes()
        return self.m.control_lock()

    def query(self, sql):
        # Validate the local endpoint before sending even a read-only SQL request.
        config = safe(self.root, 'mysql/admin-client.ini').read_text(encoding='utf-8')
        values = dict(re.findall(r'^([a-z]+)=(.*)$', config, re.M))
        require(values.get('host') == '127.0.0.1' and values.get('port') == str(self.config['mysqlPort']),
                'The database client configuration does not match this repack.')
        rows = self.m.mysql("SELECT '__COA_BEGIN__';" + sql + ";SELECT '__COA_END__';").splitlines()
        require(rows and rows[0] == '__COA_BEGIN__' and rows[-1] == '__COA_END__', 'Incomplete database response.')
        return [r.split('\t') for r in rows[1:-1]]

    def database(self):
        self.own_processes()
        if not self.m.process('mysql'):
            self.m.prepare(self.config)
            self.m.start_mysql(self.config)
        self.identity()

    def identity(self):
        port, directory = self.query('SELECT @@port,@@datadir')[0]
        require(port == str(self.config['mysqlPort']) and
                Path(directory.replace('\\\\', '\\')).resolve() == (self.root / 'mysql/data').resolve(),
                'Wrong MySQL instance: no update was applied.')

    def stop(self):
        self.own_processes()
        self.m.stop_all(self.config)
        require(not any(self.m.process(n) for n in ('world', 'supervisor', 'relay', 'auth', 'mysql')),
                'A repack service is still running.')

    def start(self, sql_source=None, *, skip_updates=False):
        self.own_processes()
        self.identity()
        path = safe(self.root, 'Core/configs/worldserver.conf')
        regular = path.read_text(encoding='utf-8')
        fields = dict(re.findall(r'^(WorldDatabaseInfo|LoginDatabaseInfo|CharacterDatabaseInfo)\s*=\s*"([^"]+)"',
                                 regular, re.M))
        world = fields.get('WorldDatabaseInfo', '').split(';')
        require(len(world) == 5 and world[0] == '127.0.0.1' and world[1] == str(self.config['mysqlPort'])
                and world[4] == 'acore_world', 'World configuration targets another database.')
        temporary = regular
        if sql_source:
            native_source = Path(self.m.native_root()) / sql_source.relative_to(self.root)
            for key, value in {'SourceDirectory': '"' + native_source.as_posix() + '"',
                               'Updates.EnableDatabases': '4', 'Updates.Redundancy': '1',
                               'Updates.CleanDeadRefMaxCount': '0'}.items():
                temporary, count = re.subn(r'^' + re.escape(key) + r'\s*=.*$', key + ' = ' + value,
                                           temporary, flags=re.M)
                require(count == 1, 'Missing updater configuration: ' + key)
        if skip_updates:
            temporary, count = re.subn(r'^Updates\.EnableDatabases\s*=.*$', 'Updates.EnableDatabases = 0',
                                       temporary, flags=re.M)
            require(count == 1, 'Missing updater configuration: Updates.EnableDatabases')
        path.write_text(temporary, encoding='utf-8', newline='\n')
        try:
            self.m.start_auth(self.config)
            self.m.start_world(self.config, 0)
        finally:
            path.write_text(regular, encoding='utf-8', newline='\n')

    def ready(self):
        return all(self.m.ready(n, self.config[k]) for n, k in
                   (('mysql', 'mysqlPort'), ('auth', 'authPort'), ('world', 'worldPort'))) and bool(
                       self.m.process('relay') and self.m.process('supervisor'))


class Update:
    def __init__(self, root, bundle, backend=None):
        self.root = Path(root).absolute()
        self.bundle = Path(bundle).absolute()
        safe(self.root, 'Core/worldserver.exe')
        self.manifest = read(safe(self.bundle, 'UPDATE.json'))
        self.id = self.manifest['id']
        require(re.fullmatch(r'[a-z0-9-]+', self.id) is not None, 'Invalid update ID.')
        require(self.manifest['format'] in (1, 2, 3, 4), 'Unsupported update format.')
        self.backup = safe(self.root, 'Update-Backups/' + self.id)
        self.state_path = safe(self.root, 'Update-Backups/' + self.id + '/state.json')
        self.native = backend
        self.migrations = self.manifest.get('migrations', [self.manifest.get('migration')])
        self.world_tables = self.manifest.get('worldTables') if self.manifest['format'] == 4 else None
        if self.manifest['format'] == 4:
            require(isinstance(self.migrations, list) and
                    [m.get('name') for m in self.migrations] == list(ISSUE_MIGRATIONS),
                    'Unsupported issue-fixes migration contract.')
            require(isinstance(self.world_tables, dict) and set(self.world_tables) == set(WORLD_TABLES),
                    'Unsupported world table scope.')
            for contract in self.world_tables.values():
                require(set(contract) == {'baseSha256', 'sha256'} and all(
                    re.fullmatch(r'[a-f0-9]{64}', value or '') for value in contract.values()),
                    'Invalid world table identity.')
            require(re.fullmatch(r'[a-f0-9]{64}', self.manifest.get('statInputsSha256', '')) is not None,
                    'Missing stat input identity.')
            require(re.fullmatch(r'[a-f0-9]{40}', self.manifest.get('targetSourceRevision', '')) is not None,
                    'Missing target source revision.')
        elif self.manifest['format'] == 3:
            require(self.migrations == [] and 'migration' not in self.manifest,
                    'A no-SQL update must explicitly declare an empty migrations list.')
            require(re.fullmatch(r'[a-f0-9]{40}', self.manifest.get('targetSourceRevision', '')) is not None,
                    'Missing target source revision.')
        else:
            require(isinstance(self.migrations, list) and 1 <= len(self.migrations) <= 2,
                    'Unsupported migration count.')
        self.sql = self.migrations[0] if self.migrations and not self.world_tables else None
        self.starters = None
        for index, migration in enumerate(self.migrations):
            require(isinstance(migration, dict), 'Missing migration contract.')
            require(re.fullmatch(r'rev_[a-z0-9_]+\.sql', migration['name']) is not None, 'Invalid migration name.')
            if self.world_tables:
                require(set(migration) == {'name', 'sha1'} and
                        re.fullmatch(r'[a-f0-9]{40}', migration['sha1']) is not None,
                        'Invalid issue-fixes migration identity.')
            elif index == 0:
                require(re.fullmatch(r'aura_[a-z0-9_]+', migration['script']) is not None, 'Invalid script name.')
                require(type(migration['spell']) is int and migration['spell'] > 0, 'Invalid spell ID.')
            else:
                require(migration.get('kind') == 'existing-starter-items', 'Unsupported migration contract.')
                entries = migration.get('entries')
                require(isinstance(entries, list) and entries and len(entries) == len(set(entries))
                        and all(type(entry) is int and entry > 0 for entry in entries), 'Invalid starter entries.')
                self.starters = migration
        require(len({m['name'] for m in self.migrations}) == len(self.migrations), 'Duplicate migration contract.')

    def payload(self, relative):
        return safe(self.bundle, 'Payload/' + relative)

    def file_checks(self):
        require(set(self.manifest['files']) == {'Core/worldserver.exe'}, 'Unsupported replacement scope.')
        for relative, contract in self.manifest['files'].items():
            source, target = self.payload(relative), safe(self.root, relative)
            require(sha(source) == contract['sha256'], 'Damaged update payload: ' + relative)
            require(sha(target) in [contract['baseSha256'], contract['sha256'], *contract.get('alternateBaseSha256', [])],
                    'This update does not support the installed server version.')
        for relative, contract in self.manifest['sourceFiles'].items():
            require(relative.startswith(('src/', 'modules/mod-ascension-compat/', 'data/sql/updates/pending_db_world/',
                                         'apps/codestyle/', 'apps/test-framework/')),
                    'Unsupported source patch path.')
            if not self.migrations:
                require(not relative.lower().endswith('.sql') and not relative.startswith('data/'),
                        'SQL payload is not allowed in a no-SQL update.')
            require(sha(self.payload('Source/' + relative)) == contract['sha256'], 'Damaged source payload: ' + relative)
        for name in ('Scripts/manage.py', 'Runtime/python/python.exe', 'Settings/repack.json',
                     'Settings/database.json', 'Source/server-source.zip', 'RELEASE.json', 'RELEASE.txt', 'MANIFEST.json'):
            require(safe(self.root, name).is_file(), 'Incomplete CoA-Repack: ' + name)
        with zipfile.ZipFile(safe(self.root, 'Source/server-source.zip')) as archive:
            require(len(archive.namelist()) == len(set(archive.namelist())), 'Duplicate source archive members.')
            meta = json.loads(archive.read('source-manifest.json'))
            require(meta['sourceRevision'] in (self.manifest['baseSourceRevision'],
                                               self.manifest.get('targetSourceRevision', self.manifest['baseSourceRevision'])), 'Unsupported source archive revision.')
            for relative, contract in self.manifest['sourceFiles'].items():
                name = 'server-source/' + relative
                actual = hashlib.sha256(archive.read(name)).hexdigest() if name in archive.namelist() else None
                require(actual in (contract['baseSha256'], contract['sha256']), 'Modified source member: ' + relative)
        for name in STATIC:
            safe(self.root, name)
        for contract in self.migrations:
            relative = 'data/sql/updates/pending_db_world/' + contract['name']
            require(relative in self.manifest['sourceFiles'], 'Migration is missing from source payload.')
            migration = self.payload('Source/' + relative)
            require(hashlib.sha1(migration.read_bytes().replace(b'\r\n', b'\n')).hexdigest() == contract['sha1'],
                    'Migration hash does not match the update contract.')

    def snapshot(self):
        self.native.identity()
        spell = self.sql['spell'] if self.sql else None
        result = {'updates': self.native.query('SELECT name,hash,state FROM acore_world.updates ORDER BY name'),
                  'bindings': self.native.query('SELECT spell_id,ScriptName FROM acore_world.spell_script_names '
                                                f'WHERE spell_id={spell} ORDER BY ScriptName') if self.sql else []}
        if self.starters:
            entries = ','.join(map(str, self.starters['entries']))
            result['starterItems'] = self.native.query(
                'SELECT * FROM acore_world.item_template WHERE entry IN (' + entries + ') ORDER BY entry')
        if self.world_tables:
            result['worldTables'] = {name: self.native.query(
                f'SELECT {columns} FROM acore_world.{name} WHERE {where} ORDER BY {order}')
                for name, (columns, where, order, _) in WORLD_TABLES.items()}
            columns = WORLD_TABLES['player_class_stats'][0]
            result['statInputs'] = {
                'classes': self.native.query('SELECT class,fallback_class FROM acore_world.ascension_custom_class '
                                             'WHERE class BETWEEN 12 AND 32 ORDER BY class'),
                'anchors': self.native.query(f'SELECT {columns} FROM acore_world.player_class_stats WHERE '
                    '(Class BETWEEN 1 AND 11 AND Level BETWEEN 1 AND 80) OR '
                    '(Class BETWEEN 12 AND 32 AND Level=1) ORDER BY Class,Level'),
            }
        return result

    def applied(self, snapshot):
        if not self.migrations:
            return True
        if self.world_tables:
            require(rows_sha(snapshot['statInputs']) == self.manifest['statInputsSha256'],
                    'Custom class stat inputs differ from this supported repack.')
            states = {}
            for migration in self.migrations:
                rows = [r for r in snapshot['updates'] if r[0] == migration['name']]
                require(rows in ([], [[migration['name'], migration['sha1'].upper(), 'PENDING']]),
                        'Conflicting migration identity.')
                states[migration['name']] = bool(rows)
            require(len(set(states.values())) == 1,
                    'Partially applied issue-fixes update; use its saved rollback for recovery.')
            for name, contract in self.world_tables.items():
                key = 'sha256' if states[WORLD_TABLES[name][3]] else 'baseSha256'
                require(rows_sha(snapshot['worldTables'][name]) == contract[key],
                        'Custom or partially applied world data: ' + name + '. Use the saved rollback for recovery.')
            return all(states.values())
        rows = [r for r in snapshot['updates'] if r[0] == self.sql['name']]
        expected = [[self.sql['name'], self.sql['sha1'].upper(), 'PENDING']]
        require(rows in ([], expected), 'Conflicting migration identity.')
        wanted = [[str(self.sql['spell']), self.sql['script']]]
        require(snapshot['bindings'] in ([], wanted), 'Custom spell binding conflicts with this update.')
        require(bool(rows) == bool(snapshot['bindings']), 'Incomplete migration: use its saved rollback before updating.')
        states = [bool(rows)]
        if self.starters:
            # Both supported full repacks already contain these harvested templates. The new SQL is
            # a clean-source installation fix and must be a no-op for existing recipient definitions.
            require([int(row[0]) for row in snapshot['starterItems']] == sorted(self.starters['entries']),
                    'Starter templates are missing: this database is not a supported repack baseline.')
            rows = [r for r in snapshot['updates'] if r[0] == self.starters['name']]
            expected = [[self.starters['name'], self.starters['sha1'].upper(), 'PENDING']]
            require(rows in ([], expected), 'Conflicting migration identity.')
            states.append(bool(rows))
        return all(states)

    def stage_source(self):
        path = safe(self.root, 'Update-Backups/' + self.id + '/candidate-source.zip')
        replacements = {'server-source/' + n: n for n in self.manifest['sourceFiles']}
        with zipfile.ZipFile(self.root / 'Source/server-source.zip') as source, zipfile.ZipFile(
                path, 'w', zipfile.ZIP_DEFLATED, compresslevel=6) as target:
            for entry in source.infolist():
                if entry.filename == 'source-manifest.json':
                    meta = json.loads(source.read(entry))
                    if 'targetSourceRevision' in self.manifest:
                        meta['sourceRevision'] = self.manifest['targetSourceRevision']
                    meta['files'].update({n: c['sha256'] for n, c in self.manifest['sourceFiles'].items()})
                    meta['localPatch'] = {'id': self.id, 'files': {n: c['sha256'] for n, c in self.manifest['sourceFiles'].items()}}
                    target.writestr(entry, json.dumps(meta, indent=2).encode())
                elif entry.filename not in replacements:
                    target.writestr(entry, source.read(entry))
            for name, relative in replacements.items():
                target.write(self.payload('Source/' + relative), name)
        with zipfile.ZipFile(path) as archive:
            for relative, contract in self.manifest['sourceFiles'].items():
                require(hashlib.sha256(archive.read('server-source/' + relative)).hexdigest() == contract['sha256'],
                        'Source payload changed during preparation: ' + relative)
        return path

    def stage_sql(self, archive_path):
        root = safe(self.root, 'Update-Backups/' + self.id + '/sql-source')
        for name in SQL_DIRS:
            safe(root, name).mkdir(parents=True, exist_ok=True)
        with zipfile.ZipFile(archive_path) as archive:
            for name in archive.namelist():
                if not name.startswith('server-source/') or not name.endswith('.sql'):
                    continue
                relative = name[len('server-source/'):]
                if not relative.startswith(tuple(d + '/' for d in SQL_DIRS) +
                                           ('modules/mod-ascension-compat/data/sql/db-world/',)):
                    continue
                path = safe(root, relative)
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_bytes(archive.read(name))
        return root

    def check_sql(self, root, snapshot):
        includes = self.native.query('SELECT path,state FROM acore_world.updates_include ORDER BY path')
        expected = {'$/' + d for d in SQL_DIRS}
        require({p for p, _ in includes} == expected, 'Custom SQL include directories need a separate update.')
        records = {r[0]: (r[1].lower(), r[2]) for r in snapshot['updates']}
        dirs = [(root / p[2:], state) for p, state in includes]
        dirs += [(p, 'MODULE') for p in (root / 'modules').glob('*/data/sql/*') if p.is_dir() and 'world' in p.name]
        contracts = {m['name']: m for m in self.migrations}
        seen, new = set(), []
        for directory, state in dirs:
            for path in directory.rglob('*.sql'):
                require(path.name not in seen, 'Duplicate SQL update filename.')
                seen.add(path.name)
                if path.name in contracts:
                    require(hashlib.sha1(path.read_bytes().replace(b'\r\n', b'\n')).hexdigest() == contracts[path.name]['sha1'],
                            'The staged migration changed during preparation.')
                if path.name not in records:
                    new.append(path.name)
                elif state != 'ARCHIVED':
                    digest = hashlib.sha1(path.read_bytes().replace(b'\r\n', b'\n')).hexdigest()
                    require(digest == records[path.name][0], 'An unrelated SQL migration differs: ' + path.name)
        require(set(new).issubset(contracts), 'Unrelated SQL updates would be applied.')
        require(not [n for n, (_, state) in records.items() if n not in seen and state != 'MODULE'],
                'The source archive is missing previously applied migrations.')

    def backup_files(self):
        hashes = {}
        for name in STATIC:
            target = safe(self.backup, name)
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(safe(self.root, name), target)
            require(sha(target) == sha(self.root / name), 'Backup verification failed.')
            hashes[name] = sha(target)
        return hashes

    def replace_file(self, source, relative, expected=None):
        target = safe(self.root, relative)
        temp = safe(self.root, relative + '.' + self.id + '.tmp')
        expected = expected or sha(source)
        require(sha(source) == expected, 'Replacement source changed: ' + relative)
        require(not temp.exists() or sha(temp) == expected, 'An unrelated temporary file occupies: ' + str(temp))
        shutil.copy2(source, temp)
        require(sha(temp) == expected, 'Incomplete staged replacement: ' + relative)
        os.replace(temp, target)

    def apply(self):
        print('Checking update compatibility...', flush=True)
        self.file_checks()
        n = self.native or Native(self.root)
        self.native = n
        with n.lock():
            n.database()
            before = self.snapshot()
            applied = self.applied(before)
            current = sha(self.root / 'Core/worldserver.exe') == self.manifest['files']['Core/worldserver.exe']['sha256']
            previous = read(self.state_path) if self.state_path.is_file() else None
            if applied and current:
                require(not self.backup.exists() or previous and previous.get('phase') == 'complete',
                        'A previous attempt is incomplete. Use Rollback_Update.bat before retrying.')
                self.verify_sources()
                print('This update is already installed. No files or player data were replaced.', flush=True)
                return
            require(not self.backup.exists() or previous and previous.get('phase') in ('prepared', 'preflight-failed', 'rolled-back'),
                    'A previous attempt has a backup. Use Rollback_Update.bat before retrying.')
            reuse = previous and previous.get('phase') == 'rolled-back'
            if reuse:
                require(before == previous['database'], 'Rollback baseline changed before retry.')
                for name in STATIC:
                    require(sha(self.root / name) == previous['files'][name] == sha(safe(self.backup, name)),
                            'Rollback file changed before retry.')
            self.backup.mkdir(parents=True, exist_ok=True)
            if not reuse:
                save(self.state_path, {'phase': 'prepared', 'id': self.id})
            print('Preparing the update and matching source archive...', flush=True)
            candidate = self.stage_source()
            sql_source = self.stage_sql(candidate)
            try:
                self.check_sql(sql_source, before)
            except Exception:
                # Retain the diagnostic candidate, but it contains no replaced files or database backup.
                if not reuse:
                    save(self.state_path, {'phase': 'preflight-failed', 'id': self.id})
                raise
            print('Compatibility verified. Stopping this repack and saving rollback files...', flush=True)
            n.stop()
            hashes = previous['files'] if reuse else self.backup_files()
            save(self.state_path, {'phase': 'backed-up', 'id': self.id, 'files': hashes, 'database': before,
                                  'installedSourceSha256': sha(candidate)})
            n.database()
            require(self.snapshot() == before, 'World database changed while stopping; backup retained.')
            print('Installing the update...', flush=True)
            for relative, contract in self.manifest['files'].items():
                self.replace_file(self.payload(relative), relative, contract['sha256'])
                require(sha(self.root / relative) == contract['sha256'], 'Installed binary hash mismatch.')
            self.replace_file(candidate, 'Source/server-source.zip')
            require(sha(candidate) == sha(self.root / 'Source/server-source.zip'), 'Installed source hash mismatch.')
            state = read(self.state_path)
            state['phase'] = 'files-installed'
            state['installedSourceSha256'] = sha(self.root / 'Source/server-source.zip')
            save(self.state_path, state)
            if self.migrations:
                print('Starting the server and applying the required database change...', flush=True)
                n.start(sql_source)
            else:
                print('Starting the server with database updates disabled for this installation...', flush=True)
                n.start(skip_updates=True)
            after = self.snapshot()
            require(self.applied(after) and n.ready(), 'The updated server is not ready.')
            expected = [r for r in before['updates'] if r[0] not in {m['name'] for m in self.migrations}]
            expected += [[m['name'], m['sha1'].upper(), 'PENDING'] for m in self.migrations]
            if self.starters:
                require(after['starterItems'] == before['starterItems'], 'Existing starter templates changed.')
            if self.world_tables:
                require(after['statInputs'] == before['statInputs'], 'Class stat inputs changed during installation.')
            require(sorted(after['updates']) == sorted(expected), 'Unexpected database updater changes.')
            self.verify_sources()
            self.record()
            state['phase'] = 'complete'
            save(self.state_path, state)
            print('Update installed. Characters, accounts and Settings were preserved. The server is ready.', flush=True)

    def verify_sources(self):
        with zipfile.ZipFile(self.root / 'Source/server-source.zip') as archive:
            for name, contract in self.manifest['sourceFiles'].items():
                require(hashlib.sha256(archive.read('server-source/' + name)).hexdigest() == contract['sha256'],
                        'Installed source does not match the binary.')

    def record(self):
        release = read(self.root / 'RELEASE.json')
        for binary in release['binaries']:
            if binary['Name'] == 'worldserver.exe':
                path = self.root / 'Core/worldserver.exe'
                binary.update(SHA256=sha(path).upper(), Bytes=path.stat().st_size)
        release['localPatch'] = {'id': self.id, 'migrations': [m['name'] for m in self.migrations],
                                 'rollbackDirectory': 'Update-Backups/' + self.id, 'inGameTested': False}
        release['releaseId'] = self.id
        if 'targetSourceRevision' in self.manifest:
            release['sourceRevision'] = self.manifest['targetSourceRevision']
        release.pop('incrementalUpdate', None)
        release['startupTested'] = True
        release['inGameTested'] = False
        release.pop('inGameTestSource', None)
        release.pop('validation', None)
        release.pop('pullRequest', None)
        if self.world_tables:
            release['releaseDate'] = '2026-09-12'
        release['emptyMySQLImportTested'] = False
        release['changes'].append(self.manifest['description'])
        save(self.root / 'RELEASE.json', release)
        with (self.root / 'RELEASE.txt').open('a', encoding='utf-8') as stream:
            stream.write('\nInstalled incremental update: ' + self.id + '\n' + self.manifest['description'] + '\n')
        manifest = read(self.root / 'MANIFEST.json')
        for name in STATIC:
            if name == 'MANIFEST.json':
                continue
            path = self.root / name
            old = manifest['files'][name]['bytes']
            manifest['files'][name] = {'bytes': path.stat().st_size, 'sha256': sha(path)}
            manifest['bytes'] += path.stat().st_size - old
        manifest['localPatch'] = {'id': self.id, 'releaseRecord': 'RELEASE.json'}
        save(self.root / 'MANIFEST.json', manifest)

    def rollback(self):
        require(self.state_path.is_file(), 'No backup for this update was found.')
        state = read(self.state_path)
        require(state.get('id') == self.id and 'files' in state, 'No files were replaced; there is nothing to roll back.')
        require(state.get('phase') != 'rolled-back', 'This update was already rolled back.')
        for name in STATIC:
            require(sha(safe(self.backup, name)) == state['files'][name], 'Rollback file is damaged: ' + name)
        contract = self.manifest['files']['Core/worldserver.exe']
        require(sha(safe(self.root, 'Core/worldserver.exe')) in
                [contract['baseSha256'], contract['sha256'], *contract.get('alternateBaseSha256', [])],
                'A different server update is installed; refusing to overwrite it.')
        require(sha(safe(self.root, 'Source/server-source.zip')) in
                (state['files']['Source/server-source.zip'], state.get('installedSourceSha256')),
                'The installed source archive changed after this update; keep it before a manual rollback.')
        n = self.native or Native(self.root)
        self.native = n
        with n.lock():
            n.database()
            snapshot = self.snapshot()
            # A failed two-statement migration can leave either old or new binding, independently of its bookkeeping.
            if self.sql:
                require(snapshot['bindings'] in ([], [[str(self.sql['spell']), self.sql['script']]]),
                        'Spell binding changed externally.')
            old_rows = state['database']['updates']
            names = {m['name'] for m in self.migrations}
            for migration in self.migrations:
                expected = [[migration['name'], migration['sha1'].upper(), 'PENDING']]
                require([r for r in snapshot['updates'] if r[0] == migration['name']] in ([], expected),
                        'Migration identity changed externally.')
            require([r for r in snapshot['updates'] if r[0] not in names] ==
                    [r for r in old_rows if r[0] not in names], 'Other updates changed; automatic rollback is unsafe.')
            self.applied(state['database'])
            if self.starters:
                require(snapshot['starterItems'] == state['database']['starterItems'],
                        'Starter templates changed externally; refusing automatic rollback.')
            if self.world_tables:
                require(snapshot['statInputs'] == state['database']['statInputs'],
                        'Class stat inputs changed externally; refusing automatic rollback.')
                for name, contract in self.world_tables.items():
                    require(rows_sha(snapshot['worldTables'][name]) in (contract['baseSha256'], contract['sha256']),
                            'World data changed externally; refusing automatic rollback: ' + name)
            n.stop()
            n.database()
            n.identity()
            require(self.snapshot() == snapshot, 'Database changed while stopping for rollback.')
            statements = []
            if self.world_tables:
                for name, (columns, where, _, _) in WORLD_TABLES.items():
                    statements.append(f'DELETE FROM acore_world.{name} WHERE {where}')
                    rows = state['database']['worldTables'][name]
                    if rows:
                        values = []
                        for row in rows:
                            encoded = []
                            for column, value in zip(columns.split(','), row, strict=True):
                                encoded.append("'" + value.replace('\\', '\\\\').replace("'", "''") + "'"
                                               if column == 'description' else str(int(value)))
                            values.append('(' + ','.join(encoded) + ')')
                        statements.append(f'INSERT INTO acore_world.{name} ({columns}) VALUES ' + ','.join(values))
            if self.sql and not state['database']['bindings']:
                statements.append(f"DELETE FROM acore_world.spell_script_names WHERE spell_id={self.sql['spell']} "
                                  f"AND ScriptName='{self.sql['script']}'")
            previous_names = {row[0] for row in old_rows}
            for migration in self.migrations:
                if migration['name'] not in previous_names:
                    statements.append(f"DELETE FROM acore_world.updates WHERE name='{migration['name']}'")
            if statements:
                n.query('START TRANSACTION;' + ';'.join(statements) + ';COMMIT' if self.world_tables
                        else ';'.join(statements))
            require(self.snapshot() == state['database'], 'Rollback database verification failed.')
            for name in STATIC:
                self.replace_file(safe(self.backup, name), name, state['files'][name])
                require(sha(self.root / name) == state['files'][name], 'Rollback copy verification failed.')
            state['phase'] = 'rolled-back'
            save(self.state_path, state)
            if self.migrations:
                n.start()
            else:
                n.start(skip_updates=True)
            print('Previous server restored. Player progress was not restored or reset.', flush=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, required=True)
    parser.add_argument('--check', action='store_true')
    parser.add_argument('--rollback', action='store_true')
    args = parser.parse_args()
    update = Update(args.root, Path(__file__).parent)
    if args.check:
        update.file_checks()
        print('Update payload and installed package version are compatible. No services were started or changed.')
    elif args.rollback:
        update.rollback()
    else:
        update.apply()


if __name__ == '__main__':
    try:
        main()
    except (OSError, RuntimeError, ValueError, KeyError, zipfile.BadZipFile) as error:
        print('UPDATE STOPPED: ' + str(error), file=sys.stderr, flush=True)
        print('If files were replaced, keep Update-Backups and run Rollback_Update.bat. Do not copy a database over your characters.',
              file=sys.stderr, flush=True)
        raise SystemExit(1)
