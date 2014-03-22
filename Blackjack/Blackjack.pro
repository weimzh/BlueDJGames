TEMPLATE		= 	lib
RESOURCES		= 	Blackjack.qrc \
					BlackjackMutable.qrc
TRANSLATIONS	= 	Blackjack_zh_CN.ts
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


HEADERS			=	src/BlackJackController.h \
					src/BlackJackPanelController.h \
					src/BlackJackDesktopController.h \
					src/bjrule.h
					
SOURCES			= 	src/main.cpp \
					src/BlackJackController.cpp \
					src/BlackJackPanelController.cpp \
					src/BlackJackDesktopController.cpp \
					src/bjrule.cc

