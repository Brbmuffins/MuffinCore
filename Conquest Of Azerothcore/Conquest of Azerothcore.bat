@ECHO OFF
SETLOCAL EnableExtensions
TITLE MMOPP Launcher
MODE CON: COLS=38 LINES=23
COLOR 06

REM ============================================================
REM  Portable paths - Launcher.bat sits beside Client and Server
REM ============================================================
SET "SERVER_ROOT=%~dp0Server\"
SET "SERVER_BAT=%~dp0Server\Start_All_Server.bat"
SET "STOP_SERVER_BAT=%~dp0Server\Stop_All_Server.bat"
SET "CLIENT_EXE=%~dp0Client\Ascension.exe"
SET "ACCOUNT_BAT=%~dp0Server\Create_Account.bat"

REM Option 3 starts disabled, but check once at launcher startup for an existing worldserver.exe.
SET "SERVER_STARTED=0"
TASKLIST /FI "IMAGENAME eq worldserver.exe" /NH 2>NUL | FIND /I "worldserver.exe" >NUL
IF NOT ERRORLEVEL 1 SET "SERVER_STARTED=1"

REM ANSI escape character for the gray OFFLINE status.
FOR /F "delims=" %%E IN ('echo prompt $E^| cmd') DO SET "ESC=%%E"

:START_MENU
CLS
CALL :DRAW_MENU

SET "CHOICE_VAR="
SET /P "CHOICE_VAR=Enter your choice: "

IF "%CHOICE_VAR%"=="1" GOTO OPTION_ONE
IF "%CHOICE_VAR%"=="2" GOTO OPTION_TWO
IF "%CHOICE_VAR%"=="3" GOTO OPTION_THREE
IF "%CHOICE_VAR%"=="4" GOTO OPTION_FOUR
IF "%CHOICE_VAR%"=="5" GOTO OPTION_FIVE

ECHO.
ECHO   Invalid choice. Please enter 1, 2, 3, 4, or 5.
TIMEOUT /T 2 /NOBREAK >NUL
GOTO START_MENU

:DRAW_MENU
ECHO +------------------------------------+
ECHO ^|                                    ^|
ECHO ^| ___  ______  ____________________  ^|
ECHO ^| ^|  \/  ^|^|  \/  ^|  _  ^| ___ \ ___ \ ^|
ECHO ^| ^| .  . ^|^| .  . ^| ^| ^| ^| ^|_/ / ^|_/ / ^|
ECHO ^| ^| ^|\/^| ^|^| ^|\/^| ^| ^| ^| ^|  __/   __/  ^|
ECHO ^| ^| ^|  ^| ^|^| ^|  ^| \ \_/ / ^|   ^| ^|     ^|
ECHO ^| \_^|  ^|_/\_^|  ^|_/\___/\_^|   \_^|     ^|
ECHO ^|       Conquest Of Azerothcore      ^|
ECHO ^|                                    ^|
ECHO ^|      Select an option:             ^|
ECHO ^|                                    ^|
ECHO ^|      1. Start Server               ^|
ECHO ^|      2. Shutdown Server            ^|
IF "%SERVER_STARTED%"=="1" (
    ECHO ^|      3. Create Account [Ready]     ^|
) ELSE (
    ECHO %ESC%[90m^|      3. Create Account [Run Server]^|%ESC%[33m
)
ECHO ^|      4. Start Client               ^|
ECHO ^|      5. Exit                       ^|
ECHO ^|                                    ^|
ECHO +------------------------------------+
ECHO.
EXIT /B

:OPTION_ONE
CLS
IF NOT EXIST "%SERVER_BAT%" (
    ECHO.
    ECHO   ERROR: Server launcher was not found:
    ECHO   "%SERVER_BAT%"
    ECHO.
    PAUSE
    GOTO START_MENU
)

ECHO.
ECHO   Starting Server...
START "Ascension Server" CMD /C CALL "%SERVER_BAT%"
SET "SERVER_STARTED=1"
TIMEOUT /T 2 /NOBREAK >NUL
GOTO START_MENU

:OPTION_TWO
CLS
IF NOT EXIST "%STOP_SERVER_BAT%" (
    ECHO.
    ECHO   ERROR: Server shutdown script was not found:
    ECHO   "%STOP_SERVER_BAT%"
    ECHO.
    PAUSE
    GOTO START_MENU
)

ECHO.
ECHO   Shutting down Server...
START "Ascension Server Shutdown" CMD /C CALL "%STOP_SERVER_BAT%"
SET "SERVER_STARTED=0"
TIMEOUT /T 2 /NOBREAK >NUL
GOTO START_MENU

:OPTION_THREE
IF NOT "%SERVER_STARTED%"=="1" (
    CLS
    ECHO.
    ECHO   Create Account is unavailable.
    ECHO.
    ECHO   Choose Start Server first, then try again.
    ECHO.
    PAUSE
    GOTO START_MENU
)

CLS
IF NOT EXIST "%ACCOUNT_BAT%" (
    ECHO.
    ECHO   ERROR: Account creator was not found:
    ECHO   "%ACCOUNT_BAT%"
    ECHO.
    PAUSE
    GOTO START_MENU
)

ECHO.
ECHO   Opening Create Account...
START "Create Account" CMD /C CALL "%ACCOUNT_BAT%"
GOTO START_MENU

:OPTION_FOUR
CLS
IF NOT EXIST "%CLIENT_EXE%" (
    ECHO.
    ECHO   ERROR: Client executable was not found:
    ECHO   "%CLIENT_EXE%"
    ECHO.
    PAUSE
    GOTO START_MENU
)

ECHO.
ECHO   Starting Client as Administrator...
POWERSHELL -NoProfile -ExecutionPolicy Bypass -Command ^
    "Start-Process -FilePath $env:CLIENT_EXE -WorkingDirectory ([IO.Path]::GetDirectoryName($env:CLIENT_EXE)) -Verb RunAs"
GOTO START_MENU

:OPTION_FIVE
EXIT /B
