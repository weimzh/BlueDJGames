TEMPLATE		= 	lib
RESOURCES		= 	LandBattle.qrc \
					LandBattleMutable.qrc
TRANSLATIONS	= 	LandBattle_zh_CN.ts
CONFIG			+= 	qt plugin
QT 				+=  svg
DEFINES			+=	DJ_BUILD_GAME_LIB

win32 {
	LIBS			+=	-lBase1 -L../Hall/build
}
!win32 {
	LIBS			+=	-lBase -L../Hall/build
}
INCLUDEPATH 	+=	../Base/src
UI_DIR			= 	tmp/ui
MOC_DIR			= 	tmp/moc
OBJECTS_DIR		= 	tmp/obj
RCC_DIR			=	tmp/rc
DESTDIR			=	../Hall/build/modules

HEADERS			=	src/4country.h \
					src/bufalloc.h \
					src/chessmap.h \
					src/JQController.h \
					src/JQPanelController.h \
					src/JQDesktopController.h

					
SOURCES			= 	src/main.cpp \
					src/4country.cc	\
					src/bufalloc.cc	\
					src/chessmap.cc \
					src/JQController.cpp	\
					src/JQPanelController.cpp \
					src/JQDesktopController.cpp

