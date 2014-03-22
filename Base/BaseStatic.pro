TEMPLATE		= 	lib
CONFIG			+= 	qt plugin static
QT 				+=  network xml svg

UI_DIR			= 	tmp/ui
MOC_DIR			= 	tmp/moc
OBJECTS_DIR		= 	tmp/obj
DESTDIR			=	build
					
HEADERS			=	src/DJGameItem.h \
					src/DJHallInterface.h \
					src/DJClientRequest.h \
					src/DJGameTab.h \
					src/DJSymbol.h \
					src/DJTextBrowser.h \
					src/DJAnimationImage.h \
					src/DJBrowserImage.h \
					src/DJRemoteToLocalFile.h \
					src/DJMessageBox.h \
					src/Utility.h \
					src/LaunchUrl.h \
					src/PlayWave.h \
					src/DJWidget.h \
					src/DJToolButton.h
					
					
SOURCES			= 	src/main.cpp \
					src/DJGameItem.cpp \
					src/DJHallInterface.cpp \
					src/DJClientRequest.cpp \
					src/DJGameTab.cpp \
					src/DJSymbol.cpp \
					src/DJTextBrowser.cpp \
					src/DJAnimationImage.cpp \
					src/DJBrowserImage.cpp \
					src/DJRemoteToLocalFile.cpp \
					src/DJMessageBox.cpp \
					src/Utility.cpp \
					src/LaunchUrl.cpp \
					src/PlayWave.cpp \
					src/DJWidget.cpp \
					src/DJToolButton.cpp
