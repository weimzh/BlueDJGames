@echo off

if not exist "Base" goto EndBase
cd "Base"
call ..\SubCleanKeepQrc.bat
cd ..
:EndBase




