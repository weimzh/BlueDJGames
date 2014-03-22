TEMPLATE		= 	lib
RESOURCES		= 	Upgrade.qrc \
					UpgradeMutable.qrc
TRANSLATIONS	= 	Upgrade_zh_CN.ts
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

HEADERS			=	src/TLJController.h \
					src/TLJPanelController.h \
					src/TLJDesktopController.h
					
SOURCES			= 	src/main.cpp \
					src/TLJController.cpp \
					src/TLJPanelController.cpp \
					src/TLJDesktopController.cpp

