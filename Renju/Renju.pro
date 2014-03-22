TEMPLATE		= 	lib
RESOURCES		= 	Renju.qrc \
					RenjuMutable.qrc
TRANSLATIONS	= 	Renju_zh_CN.ts
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

HEADERS			=	src/RenjuController.h \
					src/RenjuPanelController.h \
					src/RenjuDesktopController.h \
					src/renjurule.h
					
					
SOURCES			= 	src/main.cpp \
					src/RenjuController.cpp \
					src/RenjuPanelController.cpp \
					src/RenjuDesktopController.cpp \
					src/renjurule.cc
