# MuffinCore — Project Identity

> Shared context for every AI agent (Claude Code, Codex) and human working in this repo.
> This file is the single source of truth; `CLAUDE.md` imports it. Keep it current.

## What MuffinCore is

MuffinCore is **Tyler's (GitHub: Brbmuffins) Conquest-of-Azeroth-style World of Warcraft 3.3.5a server**, built for playing with friends.

- **Server:** an AzerothCore fork with the `mod-ascension-compat` module. That module provides 21 custom classes (IDs 12–32), CoA talents/specializations, Manastorm, account-wide collections, and custom resources.
- **Client:** a patched Ascension 3.3.5a client.
- **Origin:** it started as the portable Windows **"CoA Repack"** from upstream [jealous-sound/azerothcore-wotlk-coa](https://github.com/jealous-sound/azerothcore-wotlk-coa), bundled for local play by "DnD". MuffinCore is growing into its own project.

**Goal:** a stable, fun server that friends can install, update and play together, hosted **safely**. Target host: Tyler's Ubuntu VPS (details come once the build is stable).

## People and roles

| Who | Role |
|---|---|
| Tyler (Brbmuffins) | Owner/producer. Final say on direction, design, and anything structural. Commits with GitHub Desktop. |
| Claude Code | Head game engineer / technical co-founder: audits, plans, implements, reviews, orchestrates. |
| Codex (local CLI) | Offload worker for bounded, well-specified tasks (reviews, sweeps, mechanical edits). Its output is checked by Claude before anyone relies on it. |

## Working agreement (all agents)

1. **Ask before structural changes.** That includes new systems, refactors, moving or renaming folders, schema changes, and changes to release/update mechanics. Small, obvious bug fixes may be made directly and reported afterwards.
2. **The distribution/launcher work (VPS manifest, delta updates, launcher app) starts only when Tyler says so.** Planning documents are fine; implementation is not.
3. **Evidence over guesses.** Cite `file:line`. When intent is unclear, ask instead of guessing.
4. **Minimal diffs.** Don't rewrite working systems. Match surrounding style.
5. **Never commit:** the game client, databases, generated `.conf` files, logs, backups, secrets (`Settings/database.json`, `BugReport/relay.py` key), or large binaries. `.gitignore` enforces this; don't weaken it without asking.
6. **Protect player data.** Back up before anything touches a live database. Never overwrite `mysql/data` or restore over characters without explicit approval.

## Repository layout (repo root = `E:\MuffinCoreRepack\MuffinCore`)

| Path | What | Notes |
|---|---|---|
| `Conquest Of Azerothcore/Server/` | **Live server install** (the one Tyler plays; has real characters) | Windows repack: Core, MySQL, Python runtime, Data |
| `Conquest Of Azerothcore/Client/` | Patched Ascension client (44 GB) | Git-ignored; proprietary |
| `Conquest Of Azerothcore/Conquest of Azerothcore.bat` | Menu launcher (start/stop server, create account, start client) | DnD's |
| `MuffinCore2.1/` | Full upstream release `issue-fixes-20260912`, never run | Contains a stray copy of the update files in its root; clean baseline candidate |
| `AUDIT_2026-09-13.md` | Full codebase audit: architecture, bugs, unfinished work, hidden features, GM commands | Read this first |
| `AGENTS.md` / `CLAUDE.md` | This identity | |
| `tools/` | Dev helpers (e.g. `codex-task.ps1`) | |

Server C++ source is **not yet** in the repo as files. It ships inside each install as `Source/server-source.zip` (the AzerothCore fork plus `modules/mod-ascension-compat`). An extracted read-only copy may exist in an agent scratchpad.

## How the repack works (short)

- **Management scripts:** batch files call `Scripts/manage.py`, which uses the bundled `Runtime/python/python.exe`.
  - `manage.py` regenerates `Core/configs/*.conf` from `Settings/*.template` on **every start**. Edit the templates, not `Core/configs`.
  - It starts MySQL (127.0.0.1:3307), then authserver (3724), then worldserver (8085; RA console on 3443), plus the bug-report relay.
  - Stop the server with `Stop_All_Server.bat` (graceful RA shutdown). Never kill processes during saves.
- **Updates:** `CoA-Repack-Update/` goes *inside* the server folder.
  - `Check_Update.bat`, then `Apply_Update.bat` (`update.py`: hash-checked, backs up files, applies SQL migrations, restarts, verifies); `Rollback_Update.bat` to undo.
  - The updater hashes `Core/worldserver.exe`, `Source/server-source.zip`, `RELEASE.*` and `MANIFEST.json`. Don't hand-edit those.
- **SQL:** migrations are immutable once applied. Fix mistakes with a *new* migration.
- **Factory reset:** `Clean_Database.bat` (auto-backup first). Destructive; only with explicit approval.

## Versions

Identify a server version by the `Core/worldserver.exe` SHA-256 prefix:

| Release | worldserver.exe | Notes |
|---|---|---|
| startup-fixes-20260911 | `de206e14…` | |
| client-compat-20260911 | `c7f5149b…` | Login and regen fixes |
| issue-fixes-20260912 | `6633ba33…` | Upstream issues #21–#35 fixed (stances, stat progression, talents, restrictions, racials, models) |

## Key technical facts (read before designing anything)

- **Local-only today.**
  - The patched `Ascension.exe` only changes the *default* logon realmlist to `127.0.0.1`; `Data\enUS\realmlist.wtf` (25 bytes, `set realmlist 127.0.0.1`) is still honored.
  - `Extensions.dll` only accepts world servers on an allowlist; the only loopback entries are 127.0.0.1:8085/8087/8088.
  - Upstream `main` (`6b935188`, PR #61) adds `AscensionCompat.AllowRemoteClients` plus a one-byte DLL patch; see `E:\MuffinCoreRepack\codex-out\HOSTING_PLAN.draft.md`.
  - The server enables the Ascension protocol quirks (plaintext world headers, extension opcodes 1311–2515, 5-second ping, spell-modifier layout, class-10 mapping) **only for loopback connections**.
  - All services bind `127.0.0.1`, and `manage.py` forces the realmlist to `127.0.0.1`.
  - Hosting for friends therefore needs deliberate changes; see the audit, F1.
- **Custom-class GM gap.** `.learn all my class` does nothing for classes 12–32 (they have no trainers). CoA talents go through `.localtalent <entry> <rank>`; starter kit and progression resync through `.localclassrepair`.
- **Known default credentials** (local play only): console/GM `local`/`local`, example GM `Admin`/`Password`. They must not survive into any hosted setup.
- **No reproducible build yet.** The binaries were built upstream (VS2022). Standing up a build (Windows and Linux) is a prerequisite for C++ changes and for hosting on Ubuntu.
- **Licensing.** AzerothCore server code is open source (AGPL-3.0, so hosting a modified server means offering its source to players). The WoW/Ascension client and extracted game data are **not** open source. Don't publish the client.

## Environment notes for agents

- Windows 11. The default shell is **Windows PowerShell 5.1**: no `&&`, and native-exe arguments containing quotes get mangled, so pass long prompts or text via stdin or files. Git Bash is also available.
- Bundled Python (stdlib only): `Conquest Of Azerothcore/Server/Runtime/python/python.exe`.
- Server logs: `Core/Logs/` (`world-console.log` grows without limit), `mysql/logs/`, `BugReport/Logs/`. Client logs: `Client/Logs/`.
- Read-only by default. Don't start or stop services or launch the client unless the task says so.

## Glossary

- **CoA:** Conquest of Azeroth, Ascension's class-based game mode.
- **Repack:** a self-contained server bundle (binaries, DB, runtime) that runs without installation.
- **AE / TE:** Ability / Talent Essence, CoA talent currencies.
- **Manastorm:** roguelike instanced dungeon mode (`.manastorm`).
- **RA:** Remote Access console (telnet-style admin console on 3443).
- **Overlay MPQ:** client patch archive (`Data/patch-*.MPQ`) carrying custom UI and art.
