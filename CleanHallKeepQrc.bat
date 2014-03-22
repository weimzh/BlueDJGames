@echo off

if not exist "Hall" goto EndHall
cd "Hall"
call ..\SubCleanKeepQrc.bat
cd ..
:EndHall

if not exist "UpdateHall" goto EndUpdateHall
cd "UpdateHall"
call ..\SubCleanKeepQrc.bat
cd ..
:EndUpdateHall


