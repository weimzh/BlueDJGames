TEMPLATE		= 	lib
RESOURCES		= 	GongZhu.qrc \
					GongZhuMutable.qrc
TRANSLATIONS	= 	GongZhu_zh_CN.ts
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

HEADERS			=	src/GZController.h \
					src/GZPanelController.h \
					src/GZDesktopController.h \
					src/gozhrule.h
					
SOURCES			= 	src/main.cpp \
					src/GZController.cpp \
					src/GZPanelController.cpp \
					src/GZDesktopController.cpp \
					src/gozhrule.cc
					
