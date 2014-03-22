TEMPLATE		= 	lib
RESOURCES		= 	Base.qrc \
					BaseMutable.qrc \
					BasePoker.qrc \
					BaseMahjong.qrc
					

TRANSLATIONS	= 	Base_zh_CN.ts
CONFIG			+= 	qt
QT 				+=  network xml svg
DEFINES			+=	DJ_BUILD_BASE_LIB
VERSION			=	1.0.0

UI_DIR			= 	tmp/ui
MOC_DIR			= 	tmp/moc
OBJECTS_DIR		= 	tmp/obj
RCC_DIR			=	tmp/rc
DESTDIR			=	../Hall/build

FORMS			=	src/UIGameSetup.ui \
					src/UIIssueMsg.ui \
					src/UIAdminCommand.ui \
					src/UICallAdmin.ui \
					src/UIDropMsg.ui \
					src/UIMJSelect.ui \
					src/UIUseCard.ui
						

HEADERS			=	src/DJAdminBrowser.h \
                    src/DJAdminGroupChat.h \
                    src/DJAnimationImage.h \
                    src/DJBaseGlobal.h \
                    src/DJBrowserImage.h \
                    src/DJCard.h \
                    src/DJCardButton.h \
                    src/DJCardsWidget.h \
                    src/DJClientRequest.h \
                    src/DJDesktop.h \
                    src/DJDesktopController.h \
                    src/DJDesktopItem.h \
                    src/DJDesktopMahjongController.h \
#                    src/DJDesktopMahjongItem.h \
                    src/DJDesktopPokerController.h \
                    src/DJDesktopPokerItem.h \
                    src/DJDesktopScene.h \
                    src/DJGameController.h \
                    src/DJGameItem.h \
                    src/DJGameRoom.h \
                    src/DJGameSetup.h \
                    src/DJGameTab.h \
                    src/DJGameUser.h \
                    src/DJGameUserContext.h \
                    src/DJGameUserItem.h \
                    src/DJGlobal.h \
                    src/DJGraphicsAnimationItem.h \
                    src/DJGraphicsChessTimerItem.h \
                    src/DJGraphicsLineItem.h \
                    src/DJGraphicsPixmapItem.h \
                    src/DJGraphicsRectItem.h \
                    src/DJGraphicsSvgItem.h \
                    src/DJGraphicsTextItem.h \
                    src/DJGraphicsTimerItem.h \
                    src/DJGroupChat.h \
                    src/DJHallInterface.h \
                    src/DJHotKeyWindow.h \
                    src/DJMenuButton.h \
                    src/DJMessageBox.h \
                    src/DJPanel.h \
                    src/DJPanelController.h \
                    src/DJPanelUserItem.h \
                    src/DJPoker.h \
                    src/DJPokerPattern.h \
                    src/DJRemoteToLocalFile.h \
                    src/DJRoomCellItem.h \
                    src/DJRoomController.h \
                    src/DJRoomPlayerAvatarItem.h \
                    src/DJRoomPlayerItem.h \
                    src/DJRoomPlayerNameItem.h \
                    src/DJRoomReadyItem.h \
                    src/DJRoomSeatItem.h \
                    src/DJRoomTableItem.h \
                    src/DJRoomView.h \
                    src/DJSchemeEvent.h \
                    src/DJShowView.h \
                    src/DJSymbol.h \
                    src/DJTableController.h \
                    src/DJTextBrowser.h \
                    src/DJToolButton.h \
                    src/DJTreeItemDelegate.h \
                    src/DJTreeWidget.h \
                    src/DJWidget.h \
                    src/GameInterface.h \
                    src/GameUtility.h \
                    src/HallCommand.h \
                    src/LaunchUrl.h \
#                    src/mjdebug.h \
                    src/mjfans.h \
                    src/mjrule.h \
                    src/MJUtility.h \
                    src/PlayWave.h \
                    src/protocol.h \
                    src/UIAdminCommand.h \
                    src/UICallAdmin.h \
                    src/UIDropMsg.h \
                    src/UIGameSetup.h \
                    src/UIIssueMsg.h \
                    src/UIMJSelect.h \
                    src/UIUseCard.h \
                    src/Utility.h
					
SOURCES			= 	src/main.cpp \
					src/DJAdminBrowser.cpp \
                    src/DJAdminGroupChat.cpp \
                    src/DJAnimationImage.cpp \
                    src/DJBrowserImage.cpp \
                    src/DJCard.cpp \
                    src/DJCardButton.cpp \
                    src/DJCardsWidget.cpp \
                    src/DJClientRequest.cpp \
                    src/DJDesktop.cpp \
                    src/DJDesktopController.cpp \
                    src/DJDesktopItem.cpp \
                    src/DJDesktopMahjongController.cpp \
#                    src/DJDesktopMahjongItem.cpp \
                    src/DJDesktopPokerController.cpp \
                    src/DJDesktopPokerItem.cpp \
                    src/DJDesktopScene.cpp \
                    src/DJGameController.cpp \
                    src/DJGameItem.cpp \
                    src/DJGameRoom.cpp \
                    src/DJGameSetup.cpp \
                    src/DJGameTab.cpp \
                    src/DJGameUser.cpp \
                    src/DJGameUserContext.cpp \
                    src/DJGameUserItem.cpp \
                    src/DJGraphicsAnimationItem.cpp \
                    src/DJGraphicsChessTimerItem.cpp \
                    src/DJGraphicsLineItem.cpp \
                    src/DJGraphicsPixmapItem.cpp \
                    src/DJGraphicsRectItem.cpp \
                    src/DJGraphicsSvgItem.cpp \
                    src/DJGraphicsTextItem.cpp \
                    src/DJGraphicsTimerItem.cpp \
                    src/DJGroupChat.cpp \
                    src/DJHallInterface.cpp \
                    src/DJHotKeyWindow.cpp \
                    src/DJMenuButton.cpp \
                    src/DJMessageBox.cpp \
                    src/DJPanel.cpp \
                    src/DJPanelController.cpp \
                    src/DJPanelUserItem.cpp \
                    src/DJPoker.cpp \
                    src/DJPokerPattern.cpp \
                    src/DJRemoteToLocalFile.cpp \
                    src/DJRoomCellItem.cpp \
                    src/DJRoomController.cpp \
                    src/DJRoomPlayerAvatarItem.cpp \
                    src/DJRoomPlayerItem.cpp \
                    src/DJRoomPlayerNameItem.cpp \
                    src/DJRoomReadyItem.cpp \
                    src/DJRoomSeatItem.cpp \
                    src/DJRoomTableItem.cpp \
                    src/DJRoomView.cpp \
                    src/DJShowView.cpp \
                    src/DJSymbol.cpp \
                    src/DJTableController.cpp \
                    src/DJTextBrowser.cpp \
                    src/DJToolButton.cpp \
                    src/DJTreeItemDelegate.cpp \
                    src/DJTreeWidget.cpp \
                    src/DJWidget.cpp \
                    src/GameUtility.cpp \
                    src/LaunchUrl.cpp \
#                    src/mjdebug.cpp \
                    src/mjrule.cpp \
                    src/MJUtility.cpp \
                    src/PlayWave.cpp \
                    src/UIAdminCommand.cpp \
                    src/UICallAdmin.cpp \
                    src/UIDropMsg.cpp \
                    src/UIGameSetup.cpp \
                    src/UIIssueMsg.cpp \
                    src/UIMJSelect.cpp \
                    src/UIUseCard.cpp \
                    src/Utility.cpp
