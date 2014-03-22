@echo off

if not exist "Base" goto EndBase
cd "Base"
qmake "DEFINES+=DJ_DEBUG_OUTPUT" "CONFIG+=release"
mingw32-make
qmake "DEFINES+=DJ_DEBUG_OUTPUT" "CONFIG+=release" BaseStatic.pro
mingw32-make
qmake "DEFINES+=DJ_DEBUG_OUTPUT" "CONFIG+=release"
cd ..
:EndBase

if not exist "Hall" goto EndHall
cd "Hall"
qmake "DEFINES+=DJ_DEBUG_OUTPUT" "CONFIG+=release"
mingw32-make
mingw32-make install
cd ..
:EndHall

if not exist "UpdateHall" goto EndUpdateHall
cd "UpdateHall"
qmake "DEFINES+=DJ_DEBUG_OUTPUT" "CONFIG+=release"
mingw32-make
cd ..
:EndUpdateHall

if not exist "Blackjack" goto EndBlackjack
cd "Blackjack"
qmake "DEFINES+=DJ_DEBUG_OUTPUT" "CONFIG+=release"
mingw32-make
cd ..
:EndBlackjack

if not exist "Chaodip" goto EndChaodip
cd "Chaodip"
qmake "DEFINES+=DJ_DEBUG_OUTPUT" "CONFIG+=release"
mingw32-make
cd ..
:EndChaodip

if not exist "ChineseChess" goto EndChineseChess
cd "ChineseChess"
qmake "DEFINES+=DJ_DEBUG_OUTPUT" "CONFIG+=release"
mingw32-make
cd ..
:EndChineseChess

if not exist "Chudd" goto EndChudd
cd "Chudd"
qmake "DEFINES+=DJ_DEBUG_OUTPUT" "CONFIG+=release"
mingw32-make
cd ..
:EndChudd

if not exist "GBMahjong" goto EndGBMahjong
cd "GBMahjong"
qmake "DEFINES+=DJ_DEBUG_OUTPUT" "CONFIG+=release"
mingw32-make
cd ..
:EndGBMahjong

if not exist "GDMahjong" goto EndGDMahjong
cd "GDMahjong"
qmake "DEFINES+=DJ_DEBUG_OUTPUT" "CONFIG+=release"
mingw32-make
cd ..
:EndGDMahjong

if not exist "GongZhu" goto EndGongZhu
cd "GongZhu"
qmake "DEFINES+=DJ_DEBUG_OUTPUT" "CONFIG+=release"
mingw32-make
cd ..
:EndGongZhu

if not exist "Interlink" goto EndInterlink
cd "Interlink"
qmake "DEFINES+=DJ_DEBUG_OUTPUT" "CONFIG+=release"
mingw32-make
cd ..
:EndInterlink

if not exist "LandBattle" goto EndLandBattle
cd "LandBattle"
qmake "DEFINES+=DJ_DEBUG_OUTPUT" "CONFIG+=release"
mingw32-make
cd ..
:EndLandBattle

if not exist "LandLord" goto EndLandLord
cd "LandLord"
qmake "DEFINES+=DJ_DEBUG_OUTPUT" "CONFIG+=release"
mingw32-make
cd ..
:EndLandLord

if not exist "NiuNiu" goto EndNiuNiu
cd "NiuNiu"
qmake "DEFINES+=DJ_DEBUG_OUTPUT" "CONFIG+=release"
mingw32-make
cd ..
:EndNiuNiu

if not exist "Renju" goto EndRenju
cd "Renju"
qmake "DEFINES+=DJ_DEBUG_OUTPUT" "CONFIG+=release"
mingw32-make
cd ..
:EndRenju

if not exist "Shisensho" goto EndShisensho
cd "Shisensho"
qmake "DEFINES+=DJ_DEBUG_OUTPUT" "CONFIG+=release"
mingw32-make
cd ..
:EndShisensho

if not exist "Suoha" goto EndSuoha
cd "Suoha"
qmake "DEFINES+=DJ_DEBUG_OUTPUT" "CONFIG+=release"
mingw32-make
cd ..
:EndSuoha

if not exist "Upgrade" goto EndUpgrade
cd "Upgrade"
qmake "DEFINES+=DJ_DEBUG_OUTPUT" "CONFIG+=release"
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






