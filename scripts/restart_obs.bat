@echo off

set OBS_DIRECTORY="C:\Program Files (x86)\obs-studio\bin\64bit\"
set OLD_DIR=%CD%

CD %OBS_DIRECTORY%

START "" %OBS_DIRECTORY%obs64.exe /minimized
CD %OLD_DIR%
exit 
