CoA-Repack-Update: issue fixes, 12.09.2026

Place CoA-Repack-Update inside CoA-Repack and run Apply_Update.bat. Exit the game first.
Supports client-compat-20260911 (the login/regeneration release). Older repacks need that release first.
The updater checks the exact binary, source and affected world data before replacement.
Customized affected stat/stance definitions or unknown states are rejected for manual review.
Check_Update.bat checks file compatibility only; application also checks the local database.

Includes the new worldserver, corresponding source changes, and two native SQL migrations:
- Necromancer Undead Stances become mutually exclusive per caster.
- Custom-class level 2-80 stats continue from their calibrated level-one values.
The server applies these migrations once through its normal updater. Do not copy Payload manually.

Accounts, characters, inventory, learned spells, Settings and game profiles are preserved.
No databases, credentials, Settings or game client are included in this ZIP.
Keep Update-Backups/issue-fixes-20260912 until you have checked the release.
Rollback_Update.bat restores the saved binaries, source, and only the affected world rows.
Player progress is never restored or reset. Repeat installation is safe.
Externally modified or incomplete states stop for recovery; retain the backup and error message.

The game client is a separate download. No in-game combat or visual acceptance is claimed.
