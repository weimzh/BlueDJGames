TEMPLATE		= 	app
RESOURCES		= 	Hall.qrc \
					HallMutable.qrc
TRANSLATIONS	= 	Hall_zh_CN.ts
CONFIG			+= 	qt

ppc {
	DEFINES		+=	DJ_MAC_ARCH_PPC
}
x86 {
	DEFINES		+=	DJ_MAC_ARCH_X86
}
QT 				+= 	network xml svg

LIBS			+=	../Base/build/libBaseStatic.a
#LIBS			+=	./libspeex.a

INCLUDEPATH 	+=	../Base/src include
UI_DIR			= 	tmp/ui
MOC_DIR			= 	tmp/moc
OBJECTS_DIR		= 	tmp/obj
RCC_DIR			=	tmp/rc
DESTDIR			=	build


FORMS			=	src/UILogin.ui \
					src/UIHallSetup.ui \
					src/UISelectDownloadServer.ui \
					src/UIAboutHall.ui

HEADERS			= 	src/DJHallGlobal.h \
					src/DJContact.h \
					src/DJChat.h \
					src/DJContactItem.h \
					src/DJSocket.h \
					src/DJHallController.h \
					src/DJDownloadManager.h \
					src/DJProgressDialog.h \
					src/DJStyle.h \
					src/DJTabWidget.h \
					src/DJMainWindow.h \
					src/HallUtility.h \
#					src/wave_out.h \
					src/UILogin.h \
					src/UIHallSetup.h \
					src/UISelectDownloadServer.h \
					src/UIAboutHall.h
					

SOURCES			= 	src/main.cpp \
					src/DJContact.cpp \
					src/DJChat.cpp \
					src/DJContactItem.cpp \
					src/DJSocket.cpp \
					src/DJHallController.cpp \
					src/DJDownloadManager.cpp \
					src/DJProgressDialog.cpp \
					src/DJStyle.cpp \
					src/DJTabWidget.cpp \
					src/DJMainWindow.cpp \
					src/HallUtility.cpp \
#					src/wave_out.cpp \
					src/UILogin.cpp \
					src/UIHallSetup.cpp \
					src/UISelectDownloadServer.cpp \
					src/UIAboutHall.cpp

unix{
	macx {
		LIBS			+=	-framework Cocoa -framework WebKit
		HEADERS			+=	src/MacWebWidget.h \
							src/MacWebPage.h \
							src/MacWebDelegate.h
		SOURCES			+=	src/MacWebWidget.cpp \
							src/MacWebPage.mm \
							src/MacWebDelegate.m
	
		QMAKE_INFO_PLIST	= 	HallRes/platform/mac/Info.plist
		macresources.path	=	build/Hall.app/Contents/Resources		
		macresources.files	=	HallRes/platform/mac/Resources/* 
		INSTALLS += macresources
	}else {
		HEADERS		+=	src/DJApplication.h
		SOURCES		+=	src/DJApplication.cpp
		linuxresources.path	=	build	
		linuxresources.files	=	HallRes/platform/x11/*
		INSTALLS += linuxresources
	}
}
win32{
#	LIBS			+=	-lWinmm
	HEADERS			+=	src/WinWebWidget.h
	SOURCES			+=	src/WinWebWidget.cpp
	RC_FILE		 	= 	HallRes/platform/win/logo.rc
	windowsresources.path	=	build	
	windowsresources.files	=	HallRes/platform/win/dll/*
	INSTALLS += windowsresources
}
install.path	=	build
install.files	=	HallRes/install/*
INSTALLS += install

