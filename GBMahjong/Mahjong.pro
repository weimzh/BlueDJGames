TEMPLATE		=	lib
RESOURCES		= 	Mahjong.qrc \
					MahjongMutable.qrc
TRANSLATIONS	= 	Mahjong_zh_CN.ts
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

FORMS			=	src/UIGBMJTing.ui

HEADERS			=	src/GBMJController.h \
					src/GBMJPanelController.h \
					src/GBMJDesktopController.h \
					src/UIGBMJTing.h

SOURCES			= 	src/GBMJController.cpp \
					src/GBMJPanelController.cpp \
					src/GBMJDesktopController.cpp \
					src/main.cpp \
					src/UIGBMJTing.cpp

