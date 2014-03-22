TEMPLATE		= 	lib
RESOURCES		= 	NiuNiu.qrc \
					NiuNiuMutable.qrc
TRANSLATIONS	= 	NiuNiu_zh_CN.ts
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

HEADERS			=	src/NiuNiuController.h \
					src/NiuNiuPanelController.h \
					src/NiuNiuDesktopController.h
					
					
SOURCES			= 	src/main.cpp \
					src/NiuNiuController.cpp \
					src/NiuNiuPanelController.cpp \
					src/NiuNiuDesktopController.cpp

