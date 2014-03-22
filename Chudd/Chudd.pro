TEMPLATE		= 	lib
RESOURCES		= 	Chudd.qrc \
					ChuddMutable.qrc
TRANSLATIONS	= 	Chudd_zh_CN.ts
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

HEADERS			=	src/CDDController.h \
					src/CDDPanelController.h \
					src/CDDDesktopController.h \
					src/chuddrule.h
					
SOURCES			= 	src/main.cpp \
					src/CDDController.cpp \
					src/CDDPanelController.cpp \
					src/CDDDesktopController.cpp \
					src/chuddrule.cc

