@echo off
setlocal DisableDelayedExpansion
title CoA Repack - Clean All Databases
"%~dp0Runtime\python\python.exe" -B "%~dp0Scripts\reset-database.py"
set "RESULT=%ERRORLEVEL%"
if not "%RESULT%"=="0" echo Reset stopped. Read the message above and keep the recovery backup.
echo.
exit /b %RESULT%
