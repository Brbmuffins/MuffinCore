CONQUEST OF AZEROTH - LOCAL SERVER REPACK

START PLAYING
1. Extract/copy this entire folder to a writable location, for example
   C:\Games\CoA-Repack. Prefer a path with Latin letters. Spaces are supported.
2. Double-click Start_All_Server.bat. Wait for the ready message.
3. Open the matching patched CoA client and connect to your local AzerothCore realm.
   Account: local
   Password: local
   This is a GM level 3 account. It has NO characters; create your own in game.
4. When finished, close the game and run Stop_All_Server.bat.
   Wait for all services to stop before moving, copying or uploading this folder.

Everything required to run the server is included: Core, game data, MySQL,
the populated world database, Python and the required runtime DLLs.
No Python, MySQL, Git or Visual Studio installation is needed.
Use 64-bit Windows 10 or 11. The game client is distributed separately.
Keep the whole folder together, including Settings, Runtime and .state.
After a move, launchers automatically regenerate paths for the new location.

BUG REPORTS
Start_All_Server.bat and Start_World.bat automatically start the bug-report
relay with worldserver. It closes when worldserver exits and restarts if the
relay process stops unexpectedly.

LAUNCHERS
Start_All_Server.bat  - Start MySQL, authentication, world and bug reporting.
Start_MySQL.bat       - Start only the database.
Start_Auth.bat        - Start authentication and its database.
Start_World.bat       - Start world, database and bug reporting.
Stop_All_Server.bat   - Save and stop this repack's services safely.
Status_Server.bat     - Show which services from this folder are running.
Create_Account.bat    - Create another account while worldserver is running.
World_Console.bat     - Enter server commands while worldserver is running.

The background services remain running when you close a launcher window.
Use the batch files to start worldserver; opening Core\worldserver.exe directly
does not start its report relay.

LOCAL SETTINGS
Authentication: 127.0.0.1:3724   World: 127.0.0.1:8085
Database:       127.0.0.1:3307   World console: 127.0.0.1:3443
This package is configured for playing on the same PC. Its realm must be AzerothCore.
No firewall changes, Windows services or administrator launcher are required.

If a port is occupied, stop your other local server first. The launcher refuses
to take over another server. Repeated clicks do not create duplicate services.
Advanced port/console settings are in Settings\repack.json. Keep authentication
on 3724 for the supplied local client. Edit server options in the .template files
in Settings while stopped; generated Core configuration files are overwritten.
If you change the LOCAL password, also update raPassword in Settings\repack.json
so Stop All and World Console continue to work.

TROUBLESHOOTING
Core\Logs             - World, authentication and supervisor logs.
mysql\logs            - Database logs.
BugReport\Logs        - Report relay log.
BugReport\reports     - Report queue and delivery journal. Preserve this folder
                        after real reports have been sent.

RELEASE OPTIONS
Manastorm is enabled by default. Change Ascension.Manastorm.Enable in
Settings\mod_ascension_compat.conf.template while the server is stopped.
See RELEASE.txt for the included fixes and update compatibility.
