@echo off
setlocal DisableDelayedExpansion
title CoA Repack - Start MySQL
"%~dp0Runtime\python\python.exe" -B "%~dp0Scripts\manage.py" start-mysql
if errorlevel 1 echo The action failed. Read the message above and the service logs.
echo.
pause
