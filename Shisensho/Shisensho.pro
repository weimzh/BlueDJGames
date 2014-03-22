TEMPLATE		= 	lib
RESOURCES		= 	Shisensho.qrc \
					ShisenshoMutable.qrc
TRANSLATIONS	= 	Shisensho_zh_CN.ts
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

HEADERS			=	src/LLKController.h	\
					src/LLKPanelController.h	\
					src/LLKDesktopController.h	\
					src/shssprofile.h \
					src/shssrule.h	\
					src/DJGameRank.h
					
					
SOURCES			= 	src/main.cpp \
					src/LLKController.cpp	\
					src/LLKPanelController.cpp	\
					src/LLKDesktopController.cpp	\
					src/shssrule.cc \
					src/DJGameRank.cc
					
