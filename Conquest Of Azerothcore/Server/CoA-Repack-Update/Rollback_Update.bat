@echo off
setlocal DisableDelayedExpansion
title CoA Repack Update
set "REPACK=%~dp0.."
if not exist "%REPACK%\Runtime\python\python.exe" set "REPACK=%~dp0..\CoA-Repack"
if not exist "%REPACK%\Runtime\python\python.exe" (
  echo Put the CoA-Repack-Update folder inside your CoA-Repack folder first.
  pause
  exit /b 1
)
"%REPACK%\Runtime\python\python.exe" -B "%~dp0update.py" --root "%REPACK%" --rollback
set "RESULT=%ERRORLEVEL%"
echo.
if not "%RESULT%"=="0" echo Update stopped. Read the message above. Keep Update-Backups for recovery.
pause
exit /b %RESULT%
