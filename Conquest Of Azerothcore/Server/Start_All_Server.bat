@echo off
setlocal DisableDelayedExpansion
title CoA Repack - Start All Server
"%~dp0Runtime\python\python.exe" -B "%~dp0Scripts\manage.py" start-all
if errorlevel 1 echo The action failed. Read the message above and the service logs.
echo.
pause
