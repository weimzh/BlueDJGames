TEMPLATE		= 	lib
RESOURCES		= 	ChineseChess.qrc \
					ChineseChessMutable.qrc
TRANSLATIONS	= 	ChineseChess_zh_CN.ts
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

HEADERS			=	src/xqprofile.h	\
					src/xqrule.h	\
					src/XQController.h \
					src/XQPanelController.h \
					src/XQDesktopController.h
					
SOURCES			= 	src/main.cpp \
					src/xqrule.cc	\
					src/XQController.cpp	\
					src/XQPanelController.cpp \
					src/XQDesktopController.cpp

