@echo off

if not exist "Base" goto EndBase
cd "Base"
call ..\SubClean.bat
cd ..
:EndBase

if not exist "Hall" goto EndHall
cd "Hall"
call ..\SubClean.bat
cd ..
:EndHall

if not exist "UpdateHall" goto EndUpdateHall
cd "UpdateHall"
call ..\SubClean.bat
cd ..
:EndUpdateHall

if not exist "Blackjack" goto EndBlackjack
cd "Blackjack"
call ..\SubClean.bat
cd ..
:EndBlackjack

if not exist "Chaodip" goto EndChaodip
cd "Chaodip"
call ..\SubClean.bat
cd ..
:EndChaodip

if not exist "ChineseChess" goto EndChineseChess
cd "ChineseChess"
call ..\SubClean.bat
cd ..
:EndChineseChess

if not exist "Chudd" goto EndChudd
cd "Chudd"
call ..\SubClean.bat
cd ..
:EndChudd

if not exist "GBMahjong" goto EndGBMahjong
cd "GBMahjong"
call ..\SubClean.bat
cd ..
:EndGBMahjong

if not exist "GDMahjong" goto EndGDMahjong
cd "GDMahjong"
call ..\SubClean.bat
cd ..
:GDMahjong

if not exist "GongZhu" goto EndGongZhu
cd "GongZhu"
call ..\SubClean.bat
cd ..
:EndGongZhu

if not exist "GoodLuck" goto EndGoodLuck
cd "GoodLuck"
call ..\SubClean.bat
cd ..
:EndGoodLuck

if not exist "Interlink" goto EndInterlink
cd "Interlink"
call ..\SubClean.bat
cd ..
:EndInterlink

if not exist "LandBattle" goto EndLandBattle
cd "LandBattle"
call ..\SubClean.bat
cd ..
:EndLandBattle

if not exist "LandLord" goto EndLandLord
cd "LandLord"
call ..\SubClean.bat
cd ..
:EndLandLord

if not exist "NiuNiu" goto EndNiuNiu
cd "NiuNiu"
call ..\SubClean.bat
cd ..
:EndNiuNiu

if not exist "Renju" goto EndRenju
cd "Renju"
call ..\SubClean.bat
cd ..
:EndRenju

if not exist "Shisensho" goto EndShisensho
cd "Shisensho"
call ..\SubClean.bat
cd ..
:EndShisensho

if not exist "Suoha" goto EndSuoha
cd "Suoha"
call ..\SubClean.bat
cd ..
:EndSuoha

if not exist "Upgrade" goto EndUpgrade
cd "Upgrade"
call ..\SubClean.bat
cd ..
:EndUpgrade





