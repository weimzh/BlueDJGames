@echo off

if not exist "Blackjack" goto EndBlackjack
cd "Blackjack"
call ..\SubCleanKeepQrc.bat
cd ..
:EndBlackjack

if not exist "Chaodip" goto EndChaodip
cd "Chaodip"
call ..\SubCleanKeepQrc.bat
cd ..
:EndChaodip

if not exist "ChineseChess" goto EndChineseChess
cd "ChineseChess"
call ..\SubCleanKeepQrc.bat
cd ..
:EndChineseChess

if not exist "Chudd" goto EndChudd
cd "Chudd"
call ..\SubCleanKeepQrc.bat
cd ..
:EndChudd

if not exist "GBMahjong" goto EndGBMahjong
cd "GBMahjong"
call ..\SubCleanKeepQrc.bat
cd ..
:EndGBMahjong

if not exist "GDMahjong" goto EndGDMahjong
cd "GDMahjong"
call ..\SubCleanKeepQrc.bat
cd ..
:GDMahjong

if not exist "GongZhu" goto EndGongZhu
cd "GongZhu"
call ..\SubCleanKeepQrc.bat
cd ..
:EndGongZhu

if not exist "GoodLuck" goto EndGoodLuck
cd "GoodLuck"
call ..\SubCleanKeepQrc.bat
cd ..
:EndGoodLuck

if not exist "Interlink" goto EndInterlink
cd "Interlink"
call ..\SubCleanKeepQrc.bat
cd ..
:EndInterlink

if not exist "LandBattle" goto EndLandBattle
cd "LandBattle"
call ..\SubCleanKeepQrc.bat
cd ..
:EndLandBattle

if not exist "LandLord" goto EndLandLord
cd "LandLord"
call ..\SubCleanKeepQrc.bat
cd ..
:EndLandLord

if not exist "NiuNiu" goto EndNiuNiu
cd "NiuNiu"
call ..\SubCleanKeepQrc.bat
cd ..
:EndNiuNiu

if not exist "Renju" goto EndRenju
cd "Renju"
call ..\SubCleanKeepQrc.bat
cd ..
:EndRenju

if not exist "Shisensho" goto EndShisensho
cd "Shisensho"
call ..\SubCleanKeepQrc.bat
cd ..
:EndShisensho

if not exist "Suoha" goto EndSuoha
cd "Suoha"
call ..\SubCleanKeepQrc.bat
cd ..
:EndSuoha

if not exist "Upgrade" goto EndUpgrade
cd "Upgrade"
call ..\SubCleanKeepQrc.bat
cd ..
:EndUpgrade

if not exist "Hall" goto EndHall
cd "Hall"
if not exist "build" goto EndRelease
cd "build"
rd modules /s/q
cd ..
:EndRelease
cd ..
:EndHall

