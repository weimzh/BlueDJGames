TEMPLATE		= 	lib
RESOURCES		= 	Chaodip.qrc \
					ChaodipMutable.qrc
TRANSLATIONS	= 	Chaodip_zh_CN.ts
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

HEADERS			=	src/CDPController.h \
					src/CDPPanelController.h \
					src/CDPDesktopController.h
					
SOURCES			= 	src/main.cpp \
					src/CDPController.cpp \
					src/CDPPanelController.cpp \
					src/CDPDesktopController.cpp

