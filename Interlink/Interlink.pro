TEMPLATE		= 	lib
RESOURCES		= 	Interlink.qrc \
					InterlinkMutable.qrc
TRANSLATIONS	= 	Interlink_zh_CN.ts
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

HEADERS			=	src/DominoController.h \
					src/DominoPanelController.h \
					src/DominoDesktopController.h \
					src/dmnrule.h
					
					
SOURCES			= 	src/main.cpp \
					src/DominoController.cpp \
					src/DominoPanelController.cpp \
					src/DominoDesktopController.cpp \
					src/dominorule.cc
					