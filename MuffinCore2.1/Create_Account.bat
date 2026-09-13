@echo off
setlocal DisableDelayedExpansion
title CoA Repack - Create Account
"%~dp0Runtime\python\python.exe" -B "%~dp0Scripts\manage.py" create-account
if errorlevel 1 echo The action failed. Read the message above and the service logs.
echo.
pause
