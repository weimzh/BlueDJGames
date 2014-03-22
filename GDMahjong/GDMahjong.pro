TEMPLATE		=	lib
RESOURCES		= 	GDMahjong.qrc \
					GDMahjongMutable.qrc
TRANSLATIONS	= 	GDMahjong_zh_CN.ts
CONFIG			+= 	qt plugin
QT				+=	svg
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


HEADERS			=	src/GDMJController.h \
					src/GDMJPanelController.h \
					src/GDMJDesktopController.h

SOURCES			= 	src/main.cpp \
					src/GDMJController.cpp \
					src/GDMJPanelController.cpp \
					src/GDMJDesktopController.cpp
					
