@echo off

if not exist "Base" goto EndBase
cd "Base"
qmake "CONFIG+=release"
mingw32-make
qmake "CONFIG+=release" BaseStatic.pro
mingw32-make
qmake "CONFIG+=release"
cd ..
:EndBase

if not exist "Hall" goto EndHall
cd "Hall"
qmake "CONFIG+=release"
mingw32-make
mingw32-make install
cd ..
:EndHall

if not exist "UpdateHall" goto EndUpdateHall
cd "UpdateHall"
qmake "CONFIG+=release"
mingw32-make
cd ..
:EndUpdateHall

if not exist "Blackjack" goto EndBlackjack
cd "Blackjack"
qmake "CONFIG+=release"
mingw32-make
cd ..
:EndBlackjack

if not exist "Chaodip" goto EndChaodip
cd "Chaodip"
qmake "CONFIG+=release"
mingw32-make
cd ..
:EndChaodip

if not exist "ChineseChess" goto EndChineseChess
cd "ChineseChess"
qmake "CONFIG+=release"
mingw32-make
cd ..
:EndChineseChess

if not exist "Chudd" goto EndChudd
cd "Chudd"
qmake "CONFIG+=release"
mingw32-make
cd ..
:EndChudd

if not exist "GBMahjong" goto EndGBMahjong
cd "GBMahjong"
qmake "CONFIG+=release"
mingw32-make
cd ..
:EndGBMahjong

if not exist "GDMahjong" goto EndGDMahjong
cd "GDMahjong"
qmake "CONFIG+=release"
mingw32-make
cd ..
:EndGDMahjong

if not exist "GongZhu" goto EndGongZhu
cd "GongZhu"
qmake "CONFIG+=release"
mingw32-make
cd ..
:EndGongZhu

if not exist "Interlink" goto EndInterlink
cd "Interlink"
qmake "CONFIG+=release"
mingw32-make
cd ..
:EndInterlink

if not exist "LandBattle" goto EndLandBattle
cd "LandBattle"
qmake "CONFIG+=release"
mingw32-make
cd ..
:EndLandBattle

if not exist "LandLord" goto EndLandLord
cd "LandLord"
qmake "CONFIG+=release"
mingw32-make
cd ..
:EndLandLord

if not exist "NiuNiu" goto EndNiuNiu
cd "NiuNiu"
qmake "CONFIG+=release"
mingw32-make
cd ..
:EndNiuNiu

if not exist "Renju" goto EndRenju
cd "Renju"
qmake "CONFIG+=release"
mingw32-make
cd ..
:EndRenju

if not exist "Shisensho" goto EndShisensho
cd "Shisensho"
qmake "CONFIG+=release"
mingw32-make
cd ..
:EndShisensho

if not exist "Suoha" goto EndSuoha
cd "Suoha"
qmake "CONFIG+=release"
mingw32-make
cd ..
:EndSuoha

if not exist "Upgrade" goto EndUpgrade
cd "Upgrade"
qmake "CONFIG+=release"
mingw32-make
cd ..
:EndUpgrade

if not exist "Hall" goto EndRemove
cd "Hall"
if not exist "build" goto EndRemove
cd "build"
del *.a /q
del *.prl /q
if not exist "modules" goto EndRemove
cd "modules"
del *.a /q
:EndRemove






