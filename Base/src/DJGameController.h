//****************************************************************************
//Copyright (C) 2005-2006 Beijing BlueDJ Technology Co.,Ltd. All rights reserved.

//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with this program (in the file LICENSE.GPL); if not, write to the Free Software
//Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

//Please visit http://www.bluedj.com for more infomation about us.
//Contact us at ggwizard@gmail.com or darkdong@gmail.com.
//****************************************************************************/

#ifndef DJGAMECONTROLLER_H
#define DJGAMECONTROLLER_H

#include <QtGui>
#include "DJBaseGlobal.h"
#include "DJHallInterface.h"
#include "DJGameSetup.h"

class DJRoomController;
class DJTableController;
class DJGroupChat;
class DJGameUserItem;
class DJGameUser;
class DJGameRoom;
class DJPanelController;
class DJClientRequest;
class DJGameGameItem;
class DJWidget;
class DJGameUserContext;
class DJAdminGroupChat;
class DJCard;
class UIDropMsg;
class DJToolButton;
class DJDesktop;
class DJShowView;

class DJ_BASE_EXPORT DJGameController: public QObject
{
	Q_OBJECT
public:
	enum UserItemSectionId{
    	UserName,
    	GameStatus,
    	TableSeat,
    	Wins,
    	Loses,
    	Draws,
    	WinRate,
    	BreakRate,
    	Score,
    	Speed,
    	Platform,
    	Money,
    	Magic,
    	Chips,
    	OnlineTime,
    	Rank,
    	Unwelcome,
    	NickName,
    	LastSectionId
    };
	DJGameController( quint16 gameId, const QString& gameName, const DJHallInterface& hallInterface, QObject *parent = 0 );
  	virtual ~DJGameController();
  	
  	QVariant gameCommand( int command, const QVariant& parameters );
  	virtual void gameDataHandler( const QByteArray& gameData );
	
  	quint16				gameId() const { return m_gameId; }
  	QString				gameName() const { return m_gameName; }
  	QIcon				gameIcon() const;
  	//QString				loginName() const;
  	QString				gameDisplayName() const;
  	QString				serverDisplayName() const;
  	quint8				language() const { return m_language; }
  	bool				sound() const { return m_sound; }
  	QString				gameResDir() const { return m_gameResDir; }
  	QMainWindow*		mainWindow() const { return m_mainWindow; }
  	DJGameUserContext*	userContextWidget() const { return m_userContextWidget; }
  	
  	DJGameUser*			selfUser() const { return m_selfUser; }
  	quint32				selfUserId() const;
  	quint8 				selfRoomId() const;
	DJGameRoom*			selfRoom() const;
	DJGameRoom*			gameRoom( quint8 roomId ) const;
	int 				indexOfSection( int sectionId ) const;
	QString				sectionName( int sectionId ) const;
	QString				baseSectionName( int sectionId ) const;
	void clearAllItemsInThisRoom();
	void clearAllRooms();

	DJGameUser*	gameUser( quint32 userId ) const { return m_users.value( userId, 0 ); }
	DJGameUserItem* gameUserItem( quint32 userId ) const { return m_userItems.value( userId, 0 ); }
	DJTableController* tableController( quint8 tableId ) const;
	QSlider* scaleSlider() const { return m_scaleSlider; }
	QList<DJPanelController*> allPanelControllers();
	QList<quint8> allPanelTableIds();
	void playWave( const QString& filePath ) const;
	bool sendRequest( const DJClientRequest& request, bool isDiscardable = false ) const;
	void removeAcl( const QList<void*>& acls );
	
	void requestAdminCommand( quint8 type, quint8 subtype, const DJGameAdminCommand& command, const QString& text = QString() );
	void requestChangeRoom( quint8 roomId );
	void requestUserInfo( quint32 userId );
	void requestTableMap( quint8 minTableId, quint8 maxTableId, quint8 roomId = 0 );
	void requestLookOn( quint32 userId );
	void requestInviteUser( quint32 userId );
	void requestForceStandUp( quint32 userId );
	void requestUseCard( const ReqUseCard& reqUseCard );
	void requestChat( const ChatContent* pChatContent, int size );
	void chatWithUser( quint32 userId, bool isForced );
	void userSelected( DJGameUser* );
	void userDoubleClicked( DJGameUser* );
	
	
	void requestReady();
	void requestStandUp();
	
	int contactUserType( quint32 userId );
	void addContactUserFriend( quint32 userId );
	void removeContactUserFriend( quint32 userId );
	void addContactUserUnwelcome( quint32 userId );
	void removeContactUserUnwelcome( quint32 userId );
	
	void quit();
	void loginOk();
	void gameTabSelected();
	void reloadSettings();
	void createRoom( DJGameRoom *currentRoom );
	void addRoomItem( DJGameRoom* room );
	void changeToRoom( DJGameRoom* room );
	void changeNumberOfServerUsers();
	void showMessage( quint8 reason );
	void showMessage( const QString& text );
	QStringList roomMsgs() const { return m_roomMsgs; }
	QStringList panelMsgs() const { return m_panelMsgs; }
	QStringList adMsgs() const { return m_adMsgs; }
	QHash<quint8,DJCard*> cards() const { return m_cards; }
	DJCard *card( quint8 id ) const { return m_cards.value(id,0); }
	void decreaseCard( quint8 id );
	UIDropMsg* dropMsgWidget() const { return m_dropMsgWidget; }
signals:
	void cardsReloaded();
public slots:
	void requestSitDown( quint8 tableId, quint8 seatId );
private slots:
	void setup();
	void reconnect();
	void callAdmin();
	void issueMessage();
	void requestQuit();
	void userItemDoubleClicked( QTreeWidgetItem * item, int column );
	void userContext( const QPoint & );
	
	void roomItemDoubleClicked( QTreeWidgetItem * item, int column );
	void roomTreeContext( const QPoint & );
	void showMessage(DJGameUser* selfUser);
private:
	void sendFeature();
	void sendSetup();
	void addGameContextMenuItem( QMenu* gameContextMenu );
	
	void addUserToRoomView( DJGameUser* user );
	void removeUserFromRoomView( DJGameUser* user );
	void removeUser( DJGameUser* user);
	void removeUser( quint32 userId );
public:
	virtual QString	translatedGameName() const;
	virtual QString roomName( DJGameRoom* normalRoom ) const;
	virtual QString roomTitle() const;
	virtual QString	roomDescription( DJGameRoom* normalRoom ) const;
	
	virtual QSize	avatarSize() const;
	virtual QPixmap	readyPixmap() const;
	virtual QFont	nameFont() const;
	virtual QColor	nameColor() const;
	virtual	QPixmap	tablePixmap( quint8 numberOfSeats ) const;
	virtual QFont	tableFont() const;
	virtual QColor	tableColor() const;
	virtual QList<QPixmap> seatPixmaps( quint8 numberOfSeats ) const;
	virtual QPixmap playingPixmap() const;
	virtual QPixmap selfReadyPixmap() const;
	virtual QSize cellSize( quint8 numberOfSeats ) const;
	virtual QList<QPoint> seatOffsets( quint8 numberOfSeats ) const;
	virtual QList<QPoint> avatarOffsets( quint8 numberOfSeats ) const;
	
	virtual void showUserContext( DJGameUser* user );
	virtual QString userContextInfo( DJGameUser* user ) const;
	virtual QSize desktopSize() const;
	virtual	DJPanelController* createPanelController( DJTableController *tc, quint8 seatId );
	
	virtual DJGameUser*	createGameUser( const NormalUserDes *userDsc );
	virtual DJGameUser*	createGameUser( quint32 userId );
	
	virtual DJGameUserItem*	createGameUserItem( DJGameUser *user, QTreeWidget* parent, int type = QTreeWidgetItem::Type );
	virtual void createGameUserItemHeader( QTreeWidget* treeWidget );
	virtual void gameUserItemHeaderSections( QList<int> &sectionIds, QStringList &sectionNames );
	
	virtual void unknownGameDataHandler( const QByteArray& gameData );
	virtual void unknownBroadcastHandler( const BroadcastInfo* info );
private:
	void createGameBar( QWidget* parent );
	
	HIHallCommandProc	m_hiHallCommandProc;
	
	DJGameGameItem		*m_hallGameItem;
	void				*m_hallGameSocket;
	void				*m_hallController;
	
	QTabWidget			*m_hallContactTabWidget;
	QStackedWidget		*m_hallGameTreeContainer;
	QTabWidget			*m_hallGameTabContainer;
	QMainWindow			*m_mainWindow;
	
	quint16				m_gameId;
	QString				m_gameName;
	QString				m_gameResDir;
	quint8				m_language;
	QTranslator			*m_translator;

	QSplitter			*m_left;
	QTreeWidget			*m_roomTree;
	QSplitter			*m_right;
	DJGroupChat			*m_gameGroupChat;
	DJAdminGroupChat	*m_adminGroupChat;
	DJRoomController	*m_roomController;
	QTreeWidget			*m_userTree;
	QSlider				*m_scaleSlider;
	DJWidget			*m_gameBar;
	bool				m_sound;
	quint8				m_lookOnSeat;
	QMenu				*m_roomTreeMenu;
	DJToolButton			*m_btnQuit,*m_btnSetup,*m_btnReconnect,*m_btnCallAdmin,*m_btnAdminIssueMsg;
	DJGameUserContext	*m_userContextWidget;
	
	QHash<quint8,DJGameRoom *>		m_rooms;
	QHash<quint32,DJGameUser *>		m_users;
	QHash<quint32,DJGameUserItem *>	m_userItems;
	DJGameUser						*m_selfUser;
	DJGameSetup						m_selfSetup;
	QList<int>						m_sectionIds;
	QStringList						m_sectionNames;
	QStringList						m_roomMsgs;
	QStringList						m_panelMsgs;
	QStringList						m_adMsgs;
	QHash<quint8,DJCard*>			m_cards;
	UIDropMsg						*m_dropMsgWidget;
	bool							m_isConnected;
};

#endif
