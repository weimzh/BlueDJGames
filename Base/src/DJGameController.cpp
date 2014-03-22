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

#include "DJGameController.h"
#include "DJGameUser.h"
#include "DJGameUserItem.h"
#include "DJGameRoom.h"
#include "DJClientRequest.h"
#include "DJGroupChat.h"
#include "DJGameItem.h"
#include "Utility.h"
#include "GameUtility.h"
#include "PlayWave.h"
#include "protocol.h"
#include "DJTreeItemDelegate.h"
#include "DJTreeWidget.h"
#include "DJGameTab.h"
#include "GameInterface.h"
#include "HallCommand.h"
#include "UIGameSetup.h"
#include "DJRoomController.h"
#include "DJTableController.h"
#include "DJPanelController.h"
#include "DJPanel.h"
#include "DJWidget.h"
#include "DJMessageBox.h"
#include "DJGameUserContext.h"
#include "DJTextBrowser.h"
#include "UIIssueMsg.h"
#include "DJAdminGroupChat.h"
#include "DJSchemeEvent.h"
#include "UICallAdmin.h"
#include "DJCard.h"
#include "UIDropMsg.h"
#include "DJAdminBrowser.h"
#include "DJToolButton.h"
#include "DJShowView.h"
#include "DJDesktop.h"
#include "DJGraphicsPixmapItem.h"
#include "DJGraphicsTextItem.h"
#include "DJGraphicsTimerItem.h"
#include "DJGraphicsChessTimerItem.h"

static void aclQuitGameProc( const QByteArray& buf, const QVariant& parameters )
{
	const DJGameProtocol *protocol	= reinterpret_cast<const DJGameProtocol *>(buf.data());
	quint8 status	= protocol->chBuf[0];
	djDebug() << "aclQuitGameProc" << hex << status;
	if ( DJGAME_SUCCESS(status) ) {
		DJGameController* gameController = static_cast<DJGameController *>(parameters.value<void*>());
		gameController->quit();
	}
}
static void aclSitDownProc( const QByteArray& buf, const QVariant& parameters )
{
	const DJGameProtocol *protocol	= reinterpret_cast<const DJGameProtocol *>(buf.data());
	quint8 status	= protocol->chBuf[0];
	djDebug() << "aclSitDownProc" << hex << status;
	if ( DJGAME_FAILED( status ) ) {
		DJGameController* gameController = static_cast<DJGameController *>(parameters.value<void*>());
		gameController->showMessage( status );
	}
}

DJGameController::DJGameController( quint16 gameId, const QString& gameName, const DJHallInterface& hallInterface, QObject *parent )
	:QObject(parent),m_gameId(gameId),m_gameName(gameName),m_selfSetup(gameName)
{	
	djDebug() << "game controller contructor ";
	
	m_hiHallCommandProc	= hallInterface.hallCommandProc();
	
	QList<QVariant>	list	= hallInterface.hallPointers();
	m_hallGameItem	= static_cast<DJGameGameItem*>(list.at(0).value<void*>());
	m_hallGameSocket	= list.at(1).value<void*>();
	m_hallController	= list.at(2).value<void*>();
	m_hallContactTabWidget	= static_cast<QTabWidget*>(list.at(3).value<void*>());
	
	QVariant	parameters;
	QList<QVariant>	paralist;
	QVariant	result;
	
	parameters.clear();
	paralist.clear();
	paralist << QVariant::fromValue((void*)m_hallController);
	parameters.setValue( paralist );
	result	= m_hiHallCommandProc( HALL_COMMAND_TREE_CONTAINER, parameters );
    m_hallGameTreeContainer	= static_cast<QStackedWidget*>(result.value<void*>());
    
	paralist.clear();
	paralist << QVariant::fromValue((void*)m_hallController);
	parameters.setValue( paralist );
	result	= m_hiHallCommandProc( HALL_COMMAND_TAB_CONTAINER, parameters );
    m_hallGameTabContainer	= static_cast<QTabWidget*>(result.value<void*>());
	
	paralist.clear();
	paralist << QVariant::fromValue((void*)m_hallController);
	parameters.setValue( paralist );
	result	= m_hiHallCommandProc( HALL_COMMAND_MAIN_WINDOW, parameters );
    m_mainWindow	= static_cast<QMainWindow*>(result.value<void*>());
    
	m_selfUser		= 0;
	m_gameGroupChat		= 0;
	m_adminGroupChat		= 0;
	m_gameResDir	= QString(":/%1Res").arg(m_gameName);
	m_isConnected	= false;
	
	reloadSettings();
	
    QTranslator *m_translator	= new QTranslator( this );
    QString localeName	= FindSupportedLocaleName();
    m_translator->load( m_gameResDir + "/lang/"  + m_gameName + "_" + localeName );
    QCoreApplication::installTranslator( m_translator );
    	
	djDebug() << "game controller create game tree";

	m_left	= new QSplitter(Qt::Vertical);
	m_left->setOpaqueResize( false );
	m_hallGameTreeContainer->addWidget( m_left );
	
	m_roomTree	= new QTreeWidget(m_left);
	PaintBackground( m_roomTree, QPoint(0,0), QPoint(100,100) );
	m_roomTree->setHeaderLabels( QStringList( gameDisplayName() + "--" + serverDisplayName() ) );
	m_roomTree->setContextMenuPolicy( Qt::CustomContextMenu );
	connect(m_roomTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)), SLOT( roomItemDoubleClicked(QTreeWidgetItem *,int)));
	connect(m_roomTree, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(roomTreeContext(const QPoint&)));
	
	m_roomTreeMenu	= new QMenu( m_roomTree );
	
	djDebug() << "game controller create game tab";
	m_right	= new DJGameTab( m_hallGameItem, Qt::Vertical );
	m_right->setOpaqueResize( false );
	m_hallGameTabContainer->addTab( m_right, QIcon( m_gameResDir + "/image/base/game.png"), gameDisplayName() );
	
	
	createGameBar( m_right );
	 
	djDebug() << "game controller create canvas view ";
	
	m_roomController	= new DJRoomController( this, m_right );
	
	djDebug() << "game controller create bottom splitter ";
        
    m_userTree	= new DJTreeWidget( m_right );
    connect(m_userTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)), SLOT(userItemDoubleClicked(QTreeWidgetItem *,int)));
	connect(m_userTree, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(userContext(const QPoint&)));
	
    m_userTree->setItemDelegate(new DJTreeItemDelegate(this));
    m_userTree->setRootIsDecorated( false );
    m_userTree->setContextMenuPolicy( Qt::CustomContextMenu );
	m_userTree->setSortingEnabled( true );
	
	m_userContextWidget	= new DJGameUserContext( this, 0, Qt::Popup );
	
	m_dropMsgWidget	= new UIDropMsg( this );
	
    QSettings settings;
    settings.beginGroup( m_gameName );
    if ( settings.contains( KEY_GAMETAB_SPLITTER_SIZES ) ) {
    	m_right->restoreState(settings.value( KEY_GAMETAB_SPLITTER_SIZES ).toByteArray());
    }else {
    	QList<int> sizes;
    	sizes << 10 << 200 << 200;
   	 	m_right->setSizes( sizes );
    }
    settings.endGroup();
}
DJGameController::~DJGameController()
{
	djDebug()<<"DJGameController destructor";
		
	clearAllItemsInThisRoom();
	clearAllRooms();
	
	QSettings settings;
    settings.beginGroup( m_gameName );
	settings.setValue( KEY_GAMETAB_SPLITTER_SIZES, m_right->saveState());
	settings.endGroup();
	
	//WARNING : can not delete m_left or m_right by ourself
	//or app will crash if switching game
	//QStackedWidget and QTabWidget will delete their child
	
	m_left->hide(); 
	m_hallGameTreeContainer->removeWidget( m_left );
	//delete m_left;
	//m_left	= 0;
	
	int index	= m_hallGameTabContainer->indexOf( m_right );
	m_hallGameTabContainer->setCurrentIndex( 0 );
	m_right->hide();
	if ( -1 != index ) {
		m_hallGameTabContainer->removeTab( index );
		int count	= m_hallGameTabContainer->count();
		if ( count > index )
			m_hallGameTabContainer->setCurrentIndex( index );
		else
			m_hallGameTabContainer->setCurrentIndex( count -1 );
	}
	//delete m_right;
	//m_right	= 0;
	
	delete m_userContextWidget;
	m_userContextWidget	= 0;
	
	qDeleteAll( m_cards );
	m_cards.clear();
	
	QList<void*> acls;
    acls << (void*)aclQuitGameProc << (void*)aclSitDownProc;
     	//<< (void*)aclTableMapProc
    	//<< (void*)aclUseCardProc;
    removeAcl( acls );
    
    QCoreApplication::removeTranslator( m_translator );
    
    djDebug()<<"DJGameController destructor end";
}
void DJGameController::createGameBar( QWidget* parent )
{
	int barHeight	= 24;
	int space	= 16;
	QSize	iconSize(24,24);
	
	m_gameBar	= new DJWidget( parent );
	m_gameBar->setFixedHeight( barHeight );
	QHBoxLayout* layout	= new QHBoxLayout( m_gameBar );
	layout->setMargin( 0 );
	layout->setSpacing( space );
	//layout->addSpacing( space );
	
	QGradientStops stops;
	stops << QGradientStop( 0, QColor(172,206,247) )
			<< QGradientStop( 0.5, Qt::blue )
			<< QGradientStop( 1, QColor(172,206,247) );
	m_gameBar->setStops( stops );
	
	m_scaleSlider	= new QSlider( Qt::Horizontal );
	layout->addWidget( m_scaleSlider );
	
	layout->addStretch();
		
	m_btnReconnect	= new DJToolButton;
#ifdef DJ_TOOL_BUTTON_HAS_TEXT
	m_btnReconnect->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
#endif
	m_btnReconnect->setText(tr("reconnect"));
	m_btnReconnect->setToolTip( tr("reconnect to server now") );
	m_btnReconnect->setIcon( QIcon(":/BaseRes/image/hall/bar/reconnect.png") );
	m_btnReconnect->setIconSize( iconSize );
	connect( m_btnReconnect, SIGNAL(clicked()), SLOT(reconnect()) );
	layout->addWidget( m_btnReconnect );
	m_btnReconnect->hide();
	
	m_btnAdminIssueMsg	= new DJToolButton;
#ifdef DJ_TOOL_BUTTON_HAS_TEXT
	m_btnAdminIssueMsg->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
#endif
	m_btnAdminIssueMsg->setText( tr("issue message") );
	m_btnAdminIssueMsg->setToolTip( tr("admin issue message") );
	m_btnAdminIssueMsg->setIcon( QIcon(":/BaseRes/image/hall/bar/issuemsg.png") );
	m_btnAdminIssueMsg->setIconSize( iconSize );
	connect( m_btnAdminIssueMsg, SIGNAL(clicked()), SLOT(issueMessage()) );
	layout->addWidget( m_btnAdminIssueMsg );
	m_btnAdminIssueMsg->hide();
	
	m_btnCallAdmin	= new DJToolButton;
#ifdef DJ_TOOL_BUTTON_HAS_TEXT
	m_btnCallAdmin->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
#endif
	m_btnCallAdmin->setText( tr("call admin") );
	m_btnCallAdmin->setToolTip( tr("call admin to help") );
	m_btnCallAdmin->setIcon( QIcon(":/BaseRes/image/hall/bar/calladmin.png") );
	m_btnCallAdmin->setIconSize( iconSize );
	connect( m_btnCallAdmin, SIGNAL(clicked()), SLOT(callAdmin()) );
	layout->addWidget( m_btnCallAdmin );
	m_btnCallAdmin->hide();
	
	m_btnSetup	= new DJToolButton;
#ifdef DJ_TOOL_BUTTON_HAS_TEXT
	m_btnSetup->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
#endif
	m_btnSetup->setText( tr("setup game") );
	m_btnSetup->setToolTip( tr("setup game") );
	m_btnSetup->setIcon( QIcon(":/BaseRes/image/hall/bar/gamesetup.png") );
	m_btnSetup->setIconSize( iconSize );
	connect( m_btnSetup, SIGNAL(clicked()), SLOT(setup()) );
	layout->addWidget( m_btnSetup );
	m_btnSetup->hide();
	
	m_btnQuit	= new DJToolButton;
#ifdef DJ_TOOL_BUTTON_HAS_TEXT
	m_btnQuit->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
#endif
	m_btnQuit->setText( tr("quit this game") );
	m_btnQuit->setToolTip( tr("quit this game") );
	m_btnQuit->setIcon( QIcon(":/BaseRes/image/hall/bar/quit.png") );
	m_btnQuit->setIconSize( iconSize );
	connect( m_btnQuit, SIGNAL(clicked()), SLOT(requestQuit()) );
	layout->addWidget( m_btnQuit );
	m_btnQuit->show();
}
void DJGameController::gameTabSelected()
{
	djDebug() << "DJGameController::gameTabSelected";
	djDebug() <<"num of tab = "<<m_hallContactTabWidget->count();
	int index;
	//delete all tab widgets except friendlist and blacklist
	while( m_hallContactTabWidget->count() > 2 ) {
		index	= m_hallContactTabWidget->count() - 1;
		QWidget* widget	= m_hallContactTabWidget->widget(index);
		widget->hide();
		m_hallContactTabWidget->removeTab( index );
	}

	djDebug() <<"num of tab = "<<m_hallContactTabWidget->count();
	index	= m_hallContactTabWidget->indexOf(m_gameGroupChat);
	if ( -1 == index ) {
		djDebug()<<"add game chat";
		m_hallContactTabWidget->addTab( m_gameGroupChat, gameDisplayName() );
		m_hallContactTabWidget->setCurrentWidget(m_gameGroupChat);
	}
	index	= m_hallContactTabWidget->indexOf(m_adminGroupChat);
	if ( -1 == index ) {
		djDebug()<<"add admin chat";
		m_hallContactTabWidget->addTab( m_adminGroupChat, tr("admin chat") );
	}
	djDebug() <<"num of tab = "<<m_hallContactTabWidget->count();
	m_left->addWidget( m_hallContactTabWidget );
	m_hallGameTreeContainer->setCurrentWidget( m_left );
}
void DJGameController::showMessage( quint8 reason )
{
	switch( reason ) {
	case DJGAME_PROTOCOL_STATUS_SETUP_CONFLICT:
		showMessage(tr("Sit down failed")+ tr(", because of setup conflict with other players at this table"));
		break;
	case DJGAME_PROTOCOL_STATUS_MONEY_LACK:
		showMessage(tr("Sit down failed")+ tr(", because lack of %1").arg(baseSectionName(Money)) );
		break;
	case DJGAME_PROTOCOL_STATUS_COUNTER_LACK:
		showMessage(tr("Sit down failed")+ tr(", because lack of %1").arg(baseSectionName(Chips)) );
		break;
	default:
		showMessage(tr("Sit down failed")+ tr(", becuase of unknown reason"));
		break;
	}	
}
void DJGameController::showMessage( DJGameUser* selfUser )
{
	quint8 gameStatus	= selfUser->gameStatus();
	switch( gameStatus ) {
	case GAME_STATUS_IDLE:
		showMessage(tr("click on the chair to sitdown"));
		break;
	case GAME_STATUS_DOWN:
		showMessage(tr("click on the hand to ready"));
		break;
	case GAME_STATUS_READY:
		showMessage(tr("waiting for other players join in, game will start when all are ready"));
		break;
	case GAME_STATUS_PLAYING:
		showMessage(tr("you are playing the game now"));
		break;
	default:
		break;
	}
}
void DJGameController::showMessage( const QString& text )
{
	mainWindow()->statusBar()->showMessage( text );
}
void DJGameController::decreaseCard( quint8 id )
{
	djDebug()<<"DJGameController::decreaseCard"<<DJCard::name(id);
	DJCard *theCard	= card(id);
	if ( theCard ) {
		djDebug()<<"pages before deccrease"<<theCard->pages();
		theCard->decreasePages();
		djDebug()<<"pages after deccrease"<<theCard->pages();
	}
}
void DJGameController::loginOk()
{
	djDebug() << m_gameName << "loginOk, set feature and request all users";
	
	if ( m_selfUser ) {
		/*
		QString title	= tr("bluedj games")+ "-" + 
							tr("welcome %1(%2) to the world of bluedj games")
							.arg(m_selfUser->userName())
							.arg(m_selfUser->nickName());
		
		m_mainWindow->setWindowTitle( title );
		*/
		QList<QVariant>	list;
		list	<< QVariant::fromValue((void*)m_mainWindow)
				<< QVariant(m_selfUser->userName())
				<< QVariant(m_selfUser->nickName());
		QVariant	parameters(list);
    	m_hiHallCommandProc( HALL_COMMAND_SELF_LOGIN, parameters );
	}
	m_hallGameTreeContainer->setCurrentWidget( m_left );
	m_hallGameTabContainer->setCurrentWidget( m_right );
	
    sendFeature();
    sendSetup();
}
/*
QString	DJGameController::loginName() const
{
	return m_hallGameItem->loginName(); 
}
*/
QIcon DJGameController::gameIcon() const
{ 
	return QIcon( QString(":/%1Res").arg(m_gameName) + "/image/base/game.png" );
}
QString	DJGameController::gameDisplayName() const
{ 
	return m_hallGameItem->text(0);
}
QString	DJGameController::serverDisplayName() const
{
	DJGameGameItem* gameItem	= m_hallGameItem;
	quint16 curServerId	= gameItem->curServerId();
	DJGameServerItem* serverItem	= static_cast<DJGameServerItem*>(gameItem->childItem(curServerId));
	if ( serverItem ) {
		return serverItem->text(0);
	}else {
		return QString();
	}
}
quint32	DJGameController::selfUserId() const
{
	if ( m_selfUser )
		return m_selfUser->userId();
	else
		return 0;
}
quint8 DJGameController::selfRoomId() const
{
	if ( m_selfUser )
		return m_selfUser->roomId();
	else
		return 0;
}
DJGameRoom *DJGameController::selfRoom() const
{
	quint8 roomId	= selfRoomId();
	if ( roomId && m_rooms.contains( roomId ) )
		return m_rooms.value( roomId );
	else
		return 0;
}
DJGameRoom *DJGameController::gameRoom( quint8 roomId ) const
{
	if ( roomId && m_rooms.contains( roomId ) )
		return m_rooms.value( roomId );
	else
		return 0;
}
int DJGameController::indexOfSection( int sectionId ) const
{
	return m_sectionIds.indexOf( sectionId );
}
QString DJGameController::sectionName( int sectionId ) const
{
	QString name;
	int index	 = indexOfSection( sectionId );
	if ( -1 != index )
		name	= m_sectionNames.at( index );
	return name;
}
QString	DJGameController::baseSectionName( int sectionId ) const
{
	static const char *sections[] = {
       	QT_TR_NOOP("username"),
        QT_TR_NOOP("status"),
        QT_TR_NOOP("table-seat"),
        QT_TR_NOOP("wins"),
        QT_TR_NOOP("loses"),
        QT_TR_NOOP("draws"),
        QT_TR_NOOP("win rate"),
        QT_TR_NOOP("break rate"),
        QT_TR_NOOP("score"),
        QT_TR_NOOP("speed"),
        QT_TR_NOOP("platform"),
        QT_TR_NOOP("money"),
        QT_TR_NOOP("magic"),
        QT_TR_NOOP("chips"),
        QT_TR_NOOP("online time"),
        QT_TR_NOOP("rank"),
        QT_TR_NOOP("unwelcome"),
        QT_TR_NOOP("nickname"),
	};
	if ( sectionId < LastSectionId )
		return tr(sections[sectionId]);
	else
		return tr("error");
}
void DJGameController::clearAllItemsInThisRoom()
{
	djDebug() << "clearAllItemsInThisRoom" << m_users.size() << m_userItems.size();
	
	//m_requestedTableMaps.clear();
	
	qDeleteAll( m_users );
	m_users.clear();
	
	m_userTree->clear();
	m_userItems.clear();
	
	//m_userCanvasView->clearAllItems();
	m_roomController->clearRoomItems();
	
	m_roomMsgs.clear();
	m_panelMsgs.clear();
	m_adMsgs.clear();
}
void DJGameController::clearAllRooms()
{
	djDebug() << "DJGameController::clearAllRooms count = " << m_rooms.size();
	qDeleteAll( m_rooms );
	m_rooms.clear();
}
void DJGameController::reloadSettings()
{
	djDebug() << "DJGameController::reloadSettings";
	
	m_language	= GetSettingLanguageCode();
	m_sound		= GetSettingSound();
}
QVariant DJGameController::gameCommand( int command, const QVariant& parameters )
{
	QVariant	result;
	
	//djDebug() << "DJGameController::gameCommand" << parameters;
	switch( command ) {
	case GAME_COMMAND_GAME_DATA: {
		gameDataHandler( parameters.toByteArray() );
		break;
	}
	case GAME_COMMAND_QUIT:
		requestQuit();
		break;
	case GAME_COMMAND_RELOAD_SETTING:
		reloadSettings();
		break;
	case GAME_COMMAND_LOGIN_OK:
		loginOk();
		break;
	case GAME_COMMAND_GAME_CONTEXT: {
		QList<QVariant> list	= parameters.toList();
		QMenu* gameContextMenu	= static_cast<QMenu*>(list.at(0).value<void*>());
		addGameContextMenuItem( gameContextMenu );
		break;
	}
	case GAME_COMMAND_GAMETAB_SELECTED:
		gameTabSelected();
		break;
	case GAME_COMMAND_CONTACT_USER_TYPE_CHANGED: {
		QList<QVariant> list	= parameters.toList();
		quint32 userId	= list.at(0).toUInt();
		int	contactUserType	= list.at(1).toInt();
		DJGameUserItem *userItem	= gameUserItem(userId);
		if ( userItem )
			userItem->setContactUserType( contactUserType );
		break;
	}
	case GAME_COMMAND_GAME_USER_INFO: {
		quint32 userId	= parameters.toUInt();
		DJGameUser* user	= gameUser( userId );
		if ( user ) {
			QList<QVariant> info;
			info << user->userName() << user->icon();
			result	= info;
		}
		break;
	}
	case GAME_COMMAND_DISCONNECTED: {
		m_btnReconnect->show();
		QString	text	= tr("Disconnected, system will try to connect again in a minute");
		if ( m_isConnected ) {
			m_isConnected	= false;
			m_gameGroupChat->insertText2Browser( text, true );
		}
		showMessage( text );
		break;
	}
	case GAME_COMMAND_RECONNECTING: {
		QString	text	= tr("system is reconnecting to the server...");
		showMessage( text );
		break;
	}
	case GAME_COMMAND_STATE: {
		result.setValue( m_isConnected );
		break;
	}
	case GAME_COMMAND_REMOVE_GAME: {
		//WARNING : can not delete m_left or m_right by ourself
		//or app will crash if switching game
		//QStackedWidget and QTabWidget will delete their child
		if ( m_gameGroupChat ) {
    		djDebug()<<"delete m_gameGroupChat";
    		m_gameGroupChat->hide();
    		int index	= m_hallContactTabWidget->indexOf( m_gameGroupChat );
    		djDebug()<<"index"<<index;
    		if ( -1 != index )
    			m_hallContactTabWidget->removeTab( index );
    		m_gameGroupChat->textBrowser()->clearBrowserContents();
    		//delete m_gameGroupChat;
    		//m_gameGroupChat	= 0;
    	}
    	if ( m_adminGroupChat ) {
    		m_adminGroupChat->hide();
    		int index	= m_hallContactTabWidget->indexOf( m_adminGroupChat );
    		if ( -1 != index )
    			m_hallContactTabWidget->removeTab( index );
    		m_adminGroupChat->adminBrowser()->clear();
    		//delete m_adminGroupChat;
    		//m_adminGroupChat	= 0;
    	}
	}
	default:
		djDebug() << "unknown gameCommand";
		break;
	}
	return result;
}
void DJGameController::gameDataHandler( const QByteArray& buf )
{
	djDebug() << m_gameName << "gameDataHandler";
	
	const DJGameProtocol	*protocol	= reinterpret_cast<const DJGameProtocol *>(buf.data());
	quint8 	packageLen	= protocol->chTotalLen;
	quint8	type	= protocol->chType;
	quint8	subtype	= protocol->chSubType;
	
	do {
		if ( m_rooms.isEmpty() || m_users.isEmpty() ) {
			//allow the following event to pass
			if ( DJGAME_PROTOCOL_TYPE_GAMEDATA == type && DJGAME_PROTOCOL_SUBTYPE_ROOMINFO == subtype )
				break;
			if ( DJGAME_PROTOCOL_TYPE_GAMEDATA == type && DJGAME_PROTOCOL_SUBTYPE_USERINFO == subtype )
				break;
			if ( DJGAME_PROTOCOL_TYPE_GETCARDS == type )
				break;
			//reject other events
			return;
		}
	}while(false);
	
	switch( type ) {
	case DJGAME_PROTOCOL_TYPE_GAMEDATA: {
		const GameDataHead	*gameDataHead	= reinterpret_cast<const GameDataHead *>(protocol->chBuf);
		switch( subtype ) {
		case DJGAME_PROTOCOL_SUBTYPE_ROOMINFO: {
			djDebug() << "DJGAME_PROTOCOL_SUBTYPE_ROOMINFO";
			const NormalRoomDes *roomDsc	= reinterpret_cast<const NormalRoomDes *>(gameDataHead->chBuf);
			
			DJGameRoom	newRoom( roomDsc, protocol->chTotalLen );
			
			quint8	roomId	= newRoom.roomId();
			if ( 0 == roomId )
				break;
				
			if ( m_rooms.contains( roomId ) ) {
				djDebug() << "this room is contained ";
				DJGameRoom	*room	= m_rooms.value( roomId );
				*room	= newRoom;
			}else {
				djDebug() << "this room is new ";
				DJGameRoom	*room	= new DJGameRoom( newRoom );
				
				m_rooms.insert( roomId, room );
				djDebug() << "add room";
				addRoomItem( room );
				djDebug() << "add room end";
			}
			break;
		}
		case DJGAME_PROTOCOL_SUBTYPE_USERINFO: {
			djDebug() << "DJGAME_PROTOCOL_SUBTYPE_USERINFO";
			djDebug() << "m_users.size() = " << m_users.size() << "m_rooms.size()" << m_rooms.size() << m_selfUser;
			if (m_rooms.isEmpty() )
				break;
			
			const NormalUserDes *newUserDsc	= reinterpret_cast<const NormalUserDes *>(gameDataHead->chBuf);
			djDebug() << "NormalUserDes size=" << sizeof(NormalUserDes);
			
			DJGameUser	theNewUser( newUserDsc, this );
			quint32 newUserId		= theNewUser.userId();
			quint8	newUserRoomId	= theNewUser.roomId();
			bool	isMyself		= false;
			bool	isChangingRoom	= false;
				
			if ( !m_isConnected ) {
				isMyself	= true;
				isChangingRoom	= true;
			}else if ( newUserId == selfUserId() ) {
				isMyself	= true;
				if ( newUserRoomId != selfRoomId() ) {
					isChangingRoom	= true;
				}
			}
			djDebug() << "****************user info***********************";
			djDebug() << "userid=" << newUserId << "username=" <<  theNewUser.userName() <<  "roomid="<< theNewUser.roomId();
			djDebug() << "nickName"<< theNewUser.nickName();
			djDebug() << "tableid = " << theNewUser.tableId() << "seatid = " << theNewUser.seatId() << "gamestatus = " << theNewUser.gameStatus();
			djDebug() << "draws="<<theNewUser.draws()<<"onlinetime"<<theNewUser.onlineTime();
			djDebug() << "avatarid = " << theNewUser.avatarId() << "showId" << theNewUser.showId() <<"chips"<< theNewUser.chips();//<< "lastLoginDate" << theNewUser.lastLoginDate().toString("yyyy.MM.dd--hh:mm:ss");
			if ( !isMyself && ( newUserRoomId != selfRoomId() ) ) {
				//remove the user who is not in this room
				removeUser( newUserId );
				break;
			}
			if ( isChangingRoom ) {
				djDebug() << "I'm Changing Room";
				
				//change room
				QList<quint32> userIds	= m_users.keys();
            	foreach( quint32 userId, userIds ) {
            		QList<QVariant>	list;
					list << QVariant::fromValue((void*)m_hallContactTabWidget) << QVariant(m_gameId)
						<< QVariant(userId) << QVariant(false);
					QVariant	parameters(list);
            		m_hiHallCommandProc( HALL_COMMAND_USER_ONOFF_LINE, parameters );
            	}
            	
    			DJGameRoom *currentRoom	= m_rooms.value( newUserRoomId );
    			//clear all items in this room and create another room
    			clearAllItemsInThisRoom();
				createRoom( currentRoom );
				changeToRoom( currentRoom );
            	
            	//request all users info
            	requestUserInfo( 0 );
			}
			
			DJGameUser *newUser	= gameUser( newUserId );
			DJGameUserItem	*newUserItem	= 0;
			if ( 0 == newUser ) {
				djDebug() << "---------- new user info ------------ ";
				//create a new user
				newUser	= createGameUser( newUserDsc );
				m_users.insert( newUserId, newUser );
								
				if ( isMyself ) {
					djDebug() << "isMyself";
					//first user is self , do initilization here
					
					m_selfUser	= newUser;
					
					connect( m_selfUser, SIGNAL(trinityChanged(DJGameUser*,quint8,quint8,quint8)),SLOT(showMessage(DJGameUser*)) );
					showMessage(m_selfUser);
					//DJTreeItemDelegate* delegate	= static_cast<DJTreeItemDelegate*>(m_userTree->itemDelegate());
					//delegate->setSelfUserId( selfUserId() );
				
					createGameUserItemHeader( m_userTree );
					if ( 0 == m_gameGroupChat ) {
    					m_gameGroupChat	= new DJGroupChat( this );
    					connect( m_gameGroupChat->textBrowser(), SIGNAL(acceptInvitation(quint8,quint8)), SLOT(requestSitDown(quint8,quint8)) );
    					connect( m_gameGroupChat->textBrowser(), SIGNAL(acceptInvitation(quint8,quint8)), m_roomController, SLOT(setFocusTable(quint8)) );
    					connect( m_gameGroupChat->textBrowser(), SIGNAL(checkInvitation(quint8)), m_roomController, SLOT(setFocusTable(quint8)) );
    				}
    				//if ( m_needInitialize ) {
						//m_needInitialize	= false;
						//m_gameGroupChat->insertText2Browser( tr("server connected"), true );
					//}
    				//QString description	= roomDescription( selfRoom() );
    				//m_gameGroupChat->insertText2Browser( description );
    				DJGameRoom *room	= selfRoom();
    				if ( room && 1 != room->numberOfSeats() )
    					m_btnSetup->show();
    				else
    					m_btnSetup->hide();
    					
    				if ( m_selfUser->gameRank() >= DJGAME_USERTYPE_GAMEADMIN ) {
    					m_btnCallAdmin->hide();
    					m_btnAdminIssueMsg->show();
    					if ( 0 == m_adminGroupChat ) {
    						m_adminGroupChat	= new DJAdminGroupChat( this, MakeGroupId(gameId(),0,DJGAME_ADMINISTRATOR_TABLEGROUP) );
    					}
    				}else {
    					m_btnCallAdmin->show();
    					m_btnAdminIssueMsg->hide();
    				}
    				m_btnReconnect->hide();
    				if ( !m_isConnected ) {
						m_isConnected	= true;
						m_gameGroupChat->insertText2Browser( tr("server connected"), true );
					}
					if ( m_selfUser->gameRank() >= DJGAME_USERTYPE_SERVICE ) {
						QVariant	parameters;
						QVariant result	= m_hiHallCommandProc( HALL_COMMAND_CLOSE_MUTEX_HANDLE, parameters );
					}
				}
							
				//add user to tree
				newUserItem	= createGameUserItem( newUser, m_userTree );
				m_userItems.insert( newUserId, newUserItem );
				newUserItem->initialize();
				
				//add user to canvas
				addUserToRoomView( newUser );
				
				DJGameRoom* room	= selfRoom();
				if ( room ) {
					room->setCurUsers( m_users.size() );
					addRoomItem( room );
				}
				//change number of server users
            	changeNumberOfServerUsers();
			}else {
				djDebug() << "duplicated user info,use the latest data";
				newUser->reload( newUserDsc );
				newUserItem	= gameUserItem( newUserId );
			}
            				
			QList<QVariant>	list;
			list << QVariant::fromValue((void*)m_hallContactTabWidget) << QVariant(m_gameId)
				 << QVariant(newUserId) << QVariant(true);
			QVariant	parameters(list);
            m_hiHallCommandProc( HALL_COMMAND_USER_ONOFF_LINE, parameters );
			
			if ( newUserItem ) {
				QList<QVariant>	list;
				list << QVariant::fromValue((void*)m_hallContactTabWidget) << QVariant(newUserId);
				QVariant	parameters(list);
    			QVariant	result	= m_hiHallCommandProc( HALL_COMMAND_USER_CONTACT_TYPE, parameters );
    			int contactUserType	= result.toInt();
				newUserItem->setContactUserType( contactUserType );
			}
			quint8	tableId	= newUser->tableId();
			quint8	seatId	= newUser->seatId();
			
			DJTableController *tc	= tableController( tableId );
			if ( 0 == tc )
				break;
			if ( newUser->isPlaying() ) {
				tc->setTableStatus( DJGAME_TABLE_STATUS_WAITSTART );
				DJPanelController* pc	= tc->panelController();
    			if ( 0 == pc && newUser->isSelf() ) {
    				pc	= createPanelController( tc, seatId );
    				tc->setPanelController( pc );
    				QSize size	= desktopSize();
    				pc->createPanel( size );
    				pc->showPanel();
    			}
    			if ( pc ) {
					djDebug() << "repaint this seat user";
					pc->userArrived( newUser );
				}else {
					djDebug() << "no panel";
				}
			}
			break;
		}
		case DJGAME_PROTOCOL_SUBTYPE_TABLEMAP:{
			djDebug() << "DJGAME_PROTOCOL_SUBTYPE_TABLEMAP";
	
			const TableMapHead *tableMapHead	= reinterpret_cast<const TableMapHead *>(gameDataHead->chBuf);
			quint8	roomId	= tableMapHead->chRoom;
			quint8	numberOfSeats	= tableMapHead->chSites;
			if ( roomId != selfRoomId() )
				break;
			const DJGameTableMap *tableMap	= reinterpret_cast<const DJGameTableMap *>(tableMapHead->chBuf);
			
			do {
    			quint8	tableId	= tableMap->chTable;
    			quint8	tableStatus	= tableMap->chStatus;
    			djDebug() << "tableId=" << tableId << "tableStatus"<<tableStatus;
    			
    			DJTableController* tc	= m_roomController->tableController( tableId );
    			if ( tc ) {
        			bool isMe	= false;
        			
        			QList<DJGameUser*>	tableMapUsers;
        			for ( int seatIndex = 0; seatIndex < numberOfSeats; seatIndex++ ) {
        				quint8	seatId	= seatIndex + 1;
        				quint32 userId	= letoh4( &tableMap->userid[seatIndex] );
        				DJGameUser*	user	= gameUser( userId );
        				djDebug() << "userid = " << userId;
        				if ( userId == m_selfUser->userId() ) {
        					djDebug() << "include me";
        					isMe	= true;
        				}
        				if ( 0 != userId ) {
            				if ( 0 == user ) {
            					//user is NOT in the hash, create a temp user and request this user info
            					djDebug()<<"user is NOT in list, create a temp user and request this user info";
            					user	= createGameUser( userId );
            					m_users.insert( userId, user );
            				
            					//add user to tree and show it
            					DJGameUserItem*	userItem	= createGameUserItem( user, m_userTree );
            					m_userItems.insert( userId, userItem );
            					userItem->initialize();
            					
            					//add user to canvas
            					addUserToRoomView( user );
            					
            					requestUserInfo( userId );
            				}
            				if ( DJGAME_TABLE_STATUS_IDLE == tableStatus ) {
            					if ( !user->isReady() )
                        			user->setTrinity( tableId, seatId, GAME_STATUS_DOWN );
                        	}else {
                        		user->setTrinity( tableId, seatId, GAME_STATUS_PLAYING );
                        	}
            				tableMapUsers << user;
        				}
        			}
        			tc->synchronize( tableStatus, tableMapUsers );
        			if ( DJGAME_TABLE_STATUS_IDLE != tableStatus && isMe ) {
        				DJPanelController* pc	= tc->panelController();
            			if ( 0 == pc ) {
            				pc	= createPanelController( tc, m_selfUser->seatId() );
            				tc->setPanelController( pc );
            				QSize size	= desktopSize();
            				pc->createPanel( size );
            				pc->showPanel();
            			}else {
            				pc->resetPlayers();
            			}
        			}
    			}
    			tableMap	= DJGAME_TABLEMAP_NEXT( tableMapHead, tableMap );
			}while( packageLen - ((char*)tableMap - (char*)protocol) >= (quint8)DJGAME_TABLEMAP_SIZE(tableMapHead) );
			break;
		}
		case DJGAME_PROTOCOL_SUBTYPE_UP: {
			djDebug() << "DJGAME_PROTOCOL_SUBTYPE_UP";
			
			quint8 tableId	= gameDataHead->chBuf[0];
			djDebug() << "tableId = " << tableId;
			/*
			DJCell* table	= cell(tableId);
    		if ( 0 == table )
    			break;
    		DJGamePanel* panel	= table->gamePanel();
    		if( panel ) {
    			djDebug() << "has panel,deleteGamePanel";
    			table->deleteGamePanel();
    		}
    		m_selfUser->setTrinity( 0, 0, GAME_STATUS_IDLE );
    		*/
    		//quint8 tableId	= gameDataHead->chBuf[0];
    		//DJTableController* tc	= m_roomController->tableController( tableId );
    		//if ( 0 == tc )
    			//break;
    		DJTableController* tc	= tableController(tableId);
			if ( 0 == tc )
				break;
			tc->setPanelController( 0 );
    		m_selfUser->setTrinity( 0, 0, GAME_STATUS_IDLE );
			break;
		}
		case DJGAME_PROTOCOL_SUBTYPE_READY: {
			djDebug() << "DJGAME_PROTOCOL_SUBTYPE_READY";
			quint8 tableId	= gameDataHead->chBuf[0];
			quint8 seatId	= gameDataHead->chBuf[1];
			m_selfUser->setTrinity( tableId, seatId, GAME_STATUS_READY );
			break;
		}
		case DJGAME_PROTOCOL_SUBTYPE_CHANGEROOM: {
			djDebug() << "DJGAME_PROTOCOL_SUBTYPE_CHANGEROOM";
			//do change room when receive self userinfo
			break;
		}
		case DJGAME_PROTOCOL_SUBTYPE_VIEW: {
			djDebug() << "DJGAME_PROTOCOL_SUBTYPE_VIEW";
			quint8	tableId	= gameDataHead->chBuf[0];
			djDebug() << "tableId = " << tableId;
			DJTableController* tc	= tableController(tableId);
			if ( 0 == tc )
				break;
			DJPanelController* pc	= tc->panelController();
			djDebug()<<"pc="<<pc;
			if ( 0 == pc ) {
				pc	= createPanelController( tc, m_lookOnSeat );
				tc->setPanelController( pc );
				QSize size	= desktopSize();
				pc->createPanel( size );
			}
			pc->lookerInOut( m_selfUser, true );
			pc->showPanel();
			break;
		}
		case DJGAME_PROTOCOL_SUBTYPE_QUITVIEW: {
			djDebug() << "DJGAME_PROTOCOL_SUBTYPE_QUITVIEW";
							
			quint8 tableId	= gameDataHead->chBuf[0];
			djDebug() << "tableId = " << tableId;
			DJTableController* tc	= tableController(tableId);
			if ( 0 == tc )
				break;
			tc->setPanelController( 0 );
			break;
		}
		case DJGAME_PROTOCOL_SUBTYPE_TABLE2IDLE: {
			djDebug() << "DJGAME_PROTOCOL_SUBTYPE_TABLE2IDLE";
		
			quint8	tableId	= gameDataHead->chBuf[0];
			djDebug() << "tableId = " << tableId;
			
			DJTableController *tc	= tableController( tableId );
			if ( 0 == tc )
				break;
			tc->synchronize( DJGAME_TABLE_STATUS_IDLE );
			DJPanelController *pc	= tc->panelController();
			if ( pc ) {
				djDebug()<<"information before";
				DJMessageBox::information( 10, 0, tr("GameOver"),tr("someone quit game"),QMessageBox::Ok );
				djDebug()<<"information after";
				tc->setPanelController( 0 );
			}
			break;
		}
		case DJGAME_PROTOCOL_SUBTYPE_INVITE: {
			djDebug() << "DJGAME_PROTOCOL_SUBTYPE_INVITE";
			
			const InviteJoinIn* invite	= reinterpret_cast<const InviteJoinIn*>(gameDataHead->chBuf);
			quint32 userId	= letoh4( &invite->userid );
			quint8	tableId	= invite->chTable;
			
			DJGameUser* whoInviteU	= gameUser( userId );
			if ( whoInviteU ) {
				QString text	= whoInviteU->userName() + tr(" invited you to join table %1").arg(tableId);
				showMessage( text );
				playWave("wav/invited.wav");
				
				
				m_gameGroupChat->insertText2Browser( text );
				text	= DJSCHEME_AHREF
							.arg( DJSCHEME )
							.arg( DJSCHEME_EVENT_ACCEPT_INVITATION )
							.arg( tableId )
							.arg( DJSCHEME_HOST )
							.arg( tr("join in") );
				text	+= QString( QChar(QChar::Nbsp) ) + QString( QChar(QChar::Nbsp) );
				text	+= DJSCHEME_AHREF
							.arg( DJSCHEME )
							.arg( DJSCHEME_EVENT_CHECK_INVITATION )
							.arg( tableId )
							.arg( DJSCHEME_HOST )
							.arg( tr("check table") );
				m_gameGroupChat->insertText2Browser( text );										
			}
			break;
		}
		case DJGAME_PROTOCOL_SUBTYPE_CURRENT_GAMEINFO: {
			djDebug() << "DJGAME_PROTOCOL_SUBTYPE_CURRENT_GAMEINFO";
			
			quint8	tableId	= protocol->chLanguage;
			DJTableController *tc	= tableController( tableId );
			if ( 0 == tc )
				break;
			DJPanelController *pc	= tc->panelController();
			if ( pc ) {
				pc->gameInfo( QByteArray(reinterpret_cast<const char*>(gameDataHead->chBuf),packageLen-6) );
			}
			break;
		}
		case DJGAME_PROTOCOL_SUBTYPE_WAIT_USER2: {
			djDebug() << "DJGAME_PROTOCOL_SUBTYPE_WAIT_USER2";
				
			const DJGameWaitUser2 *waitUser2	= reinterpret_cast<const DJGameWaitUser2 *>(gameDataHead->chBuf);
			quint8	tableId	= waitUser2->chTable;
			quint8	status	= waitUser2->chTableStatus;
			quint16 timeout	= letoh2( waitUser2->shTimeout );
			quint16	mask	= letoh2( waitUser2->shWaitMask );
			djDebug() << "tablestatus = "<< status << "mask = " << mask << "timeout = "<< timeout;
			DJTableController *tc	= tableController( tableId );
			if ( 0 == tc )
				break;
			DJPanelController *pc	= tc->panelController();
			if ( pc ) {
				pc->gameWait( mask, status ,timeout );
			}
			break;
		}
		case DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2: {
			djDebug() << "DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2";
			
			const GeneralGameTrace2Head *gameTrace	= reinterpret_cast<const GeneralGameTrace2Head*>(gameDataHead->chBuf);
			quint8 	tableId	= gameTrace->chTable;
			djDebug()<<hex<<"table="<<tableId<<"seat="<<gameTrace->chSite<<"traceId="<<gameTrace->chType;
			DJTableController *tc	= tableController( tableId );
			if ( 0 == tc )
				break;
			DJPanelController *pc	= tc->panelController();
			if ( pc ) {
				pc->gameTrace( gameTrace );
			}
			break;
		}
		case DJGAME_PROTOCOL_SUBTYPE_MAGIC_RESULT: {
			djDebug() << "DJGAME_PROTOCOL_SUBTYPE_MAGIC_RESULT";
				
			const MagicResult* magicResult	= reinterpret_cast<const MagicResult*>(gameDataHead->chBuf);
			quint8 tableId	= m_selfUser->tableId();
			DJTableController *tc	= tableController( tableId );
			if ( 0 == tc )
				break;
			DJPanelController *pc	= tc->panelController();
			if ( pc ) {
				pc->magicResult( magicResult );
			}
			break;
		}
		case DJGAME_PROTOCOL_SUBTYPE_GAMEOVER2: {
			djDebug() << "DJGAME_PROTOCOL_SUBTYPE_GAMEOVER2";
				
			const DJGameOver* gameOver	= reinterpret_cast<const DJGameOver*>(gameDataHead->chBuf);
			quint8	tableId	= protocol->chLanguage;
			DJTableController *tc	= tableController( tableId );
			if ( 0 == tc )
				break;
			DJPanelController *pc	= tc->panelController();
			if ( pc ) {
				pc->gameOver( gameOver );
			}
			break;
		}
		case DJGAME_PROTOCOL_SUBTYPE_JOINGROUP: {
			djDebug() << "DJGAME_PROTOCOL_SUBTYPE_JOINGROUP";
			
			quint8	tableId	= protocol->chLanguage;
			DJTableController* tc	= tableController(tableId);
			if ( 0 == tc )
				break;
			DJPanelController* pc	= tc->panelController();
			if ( pc ) {
				quint32 userId	= letoh4( gameDataHead->chBuf );
				DJGameUser *user	= gameUser( userId );
				pc->lookerInOut( user, true );
			}
			break;
		}
		case DJGAME_PROTOCOL_SUBTYPE_QUITGROUP: {
			djDebug() << "DJGAME_PROTOCOL_SUBTYPE_QUITGROUP";
			
			quint8	tableId	= protocol->chLanguage;
			DJTableController* tc	= tableController(tableId);
			if ( 0 == tc )
				break;
			DJPanelController* pc	= tc->panelController();
			if ( pc ) {
				quint32 userId	= letoh4( gameDataHead->chBuf );
				DJGameUser *user	= gameUser( userId );
				pc->lookerInOut( user, false );
			}
			break;
		}
		case DJGAME_PROTOCOL_SUBTYPE_KICK: {
			djDebug() << "DJGAME_PROTOCOL_SUBTYPE_KICK";
			
			quint32 userId	= letoh4( gameDataHead->chBuf );
			DJGameUser* whoKickU	= gameUser( userId );
			if( whoKickU )
				showMessage( whoKickU->userName() + " " + tr("don't wanna play with you"));
			break;
		}
		case DJGAME_PROTOCOL_SUBTYPE_REQUEST_PRIVATE_VIEW: {
			djDebug() << "DJGAME_PROTOCOL_SUBTYPE_REQUEST_PRIVATE_VIEW";
			
			DJGameRoom* room	= selfRoom();
			if ( !room || !(room->options() & DJGAME_ROOMOPTION_PRIVATE_VIEW) )
				break;

			const DJGamePrivateView* gameGrivateView	= reinterpret_cast<const DJGamePrivateView*>(gameDataHead->chBuf);
			quint32 lookerId	= letoh4( &gameGrivateView->lookin );
			quint32 hostId	= letoh4( &gameGrivateView->host );
			quint8 tableId	= gameGrivateView->chTable;
			quint8 seatId	= gameGrivateView->chSite;
			djDebug() << lookerId << hostId << tableId << seatId;
			DJTableController* tc	= tableController(tableId);
			if ( 0 == tc )
				break;
			DJPanelController* pc	= tc->panelController();
			DJGameUser *looker	= gameUser( lookerId );
			DJGameUser *hoster	= gameUser( hostId );
			if ( pc && looker && hoster ) {
				QString text	= tr("%1 request to acquire a private view on %2")
										.arg(looker->userName())
										.arg(hoster->userName());
										
				pc->insertText2Browser( text );
				
				if ( hostId == m_selfUser->userId() && tableId == m_selfUser->tableId() && seatId == m_selfUser->seatId() ) {
					text	= DJSCHEME_AHREF
								.arg( DJSCHEME )
								.arg( DJSCHEME_EVENT_ACCEPT_PRIVATE_VIEW )
								.arg( lookerId )
								.arg( DJSCHEME_HOST )
								.arg( tr("Accept") );
															
					pc->insertText2Browser( text );
				}
			}
			break;
		}
		case DJGAME_PROTOCOL_SUBTYPE_AGREE_PRIVATE_VIEW:
		case DJGAME_PROTOCOL_SUBTYPE_CLOSE_PRIVATE_VIEW: {
			djDebug() << "DJGAME_PROTOCOL_SUBTYPE_AGREE_PRIVATE_VIEW,DJGAME_PROTOCOL_SUBTYPE_CLOSE_PRIVATE_VIEW";
			
			DJGameRoom* room	= selfRoom();
			if ( !room || !(room->options() & DJGAME_ROOMOPTION_PRIVATE_VIEW) )
				break;

			const DJGamePrivateView* gameGrivateView	= reinterpret_cast<const DJGamePrivateView*>(gameDataHead->chBuf);
			quint32 lookerId	= letoh4( &gameGrivateView->lookin );
			quint32 hostId	= letoh4( &gameGrivateView->host );
			quint8 tableId	= gameGrivateView->chTable;
			quint8 seatId	= gameGrivateView->chSite;
			djDebug() << lookerId << hostId << tableId << seatId;
			DJTableController* tc	= tableController(tableId);
			if ( 0 == tc )
				break;
			DJPanelController* pc	= tc->panelController();
			DJGameUser *looker	= gameUser( lookerId );
			DJGameUser *hoster	= gameUser( hostId );	
			if ( pc && looker && hoster ) {
				QString contents;
				if ( DJGAME_PROTOCOL_SUBTYPE_AGREE_PRIVATE_VIEW == subtype )
					contents	= tr("%1 has accepted to give %2 a private view")
										.arg(hoster->userName())
										.arg(looker->userName());
				else if ( DJGAME_PROTOCOL_SUBTYPE_CLOSE_PRIVATE_VIEW == subtype )
					contents	= tr("%1 has closed the private view to %2")
										.arg(hoster->userName())
										.arg(looker->userName());
				pc->insertText2Browser( contents );
			}
			break;
		}
		case DJGAME_PROTOCOL_SUBTYPE_GAMECARD: {
			djDebug()<<"DJGAME_PROTOCOL_SUBTYPE_GAMECARD";
			const ReqUseCard* useCard	= reinterpret_cast<const ReqUseCard*>(gameDataHead->chBuf);
			quint32 hostId	= letoh4( &useCard->hostid );
			quint8 id	= useCard->chCard;
			qint32 seconds	= letoh4( useCard->iParam );
			djDebug()<<"seconds="<<seconds;
			DJCard *theCard	= card(id);
			if ( !theCard ) {
				djDebug()<<"new card"<<id;
				theCard	= new DJCard( id, 0 );
				m_cards.insert( id, theCard );
			}
			theCard->setLeftSeconds( seconds );
			
			DJGameUser *user	= gameUser( hostId );
			if ( !user )
				break;
			quint8 tableId	= user->tableId();
			DJTableController* tc	= tableController(tableId);
			if ( 0 == tc )
				break;
			DJPanelController* pc	= tc->panelController();
			if ( pc ) {
				pc->cardUsed( useCard );
			}
			break;
		}
		case DJGAME_PROTOCOL_SUBTYPE_BONUS: {
			djDebug()<<"DJGAME_PROTOCOL_SUBTYPE_BONUS";
			const DJGameBonus* bonusCondition	= reinterpret_cast<const DJGameBonus*>(gameDataHead->chBuf);
			quint8 tableId	= bonusCondition->chTable;
			DJTableController* tc	= tableController(tableId);
			if ( 0 == tc )
				break;
			DJPanelController* pc	= tc->panelController();
			if ( pc ) {
				pc->bonusCondition( bonusCondition );
			}
			break;
		}
		case DJGAME_PROTOCOL_SUBTYPE_BONUSFULFILL: {
			djDebug()<<"DJGAME_PROTOCOL_SUBTYPE_BONUSFULFILL";
			const DJGameBonusFulfill* bonusFulfill	= reinterpret_cast<const DJGameBonusFulfill*>(gameDataHead->chBuf);
			quint8 tableId	= bonusFulfill->chTable;
			DJTableController* tc	= tableController(tableId);
			if ( 0 == tc )
				break;
			DJPanelController* pc	= tc->panelController();
			if ( pc ) {
				pc->bonusFulfilled( bonusFulfill );
			}
			break;
		}
		default:
			djDebug() << "******unknown game data*********";
			unknownGameDataHandler( buf );
			break;
		}
		break;
	}
	case DJGAME_PROTOCOL_TYPE_BROADCAST	: {
		djDebug() << "DJGAME_PROTOCOL_TYPE_BROADCAST";
		const BroadcastHead	*boardcastHead	= reinterpret_cast<const BroadcastHead *>( protocol->chBuf );
				
   		quint16	gameId	= makeint16( boardcastHead->chGameClass, boardcastHead->chGame );
   		if ( gameId != m_gameId )
   			break;
   		quint8	roomId	= boardcastHead->chRoom;
   		if ( roomId != selfRoomId() )
   			break;
   		quint8	numberOfMsg	= boardcastHead->chMessages;
   		djDebug() << "numberOfMsg = " << numberOfMsg;
   		const BroadcastInfo	*info	= reinterpret_cast<const BroadcastInfo *>(reinterpret_cast<const char *>(boardcastHead) + sizeof(BroadcastHead));
   		QSet<quint32> requestedUserIds;
   		QSet<quint8> requestedTableIds;
   		while ( numberOfMsg-- ) {
   			quint32	userId	= letoh4( &info->userid );
   			DJGameUser* user	= gameUser( userId );
   			if ( 0 == user ) {
   				djDebug() << "user : " << userId << " is not in hash!!!!! ";
   				if ( DJGAME_MSGTYPE_EVENT == info->chType && DJGAME_MSG_EVENT_OUT == info->chMsg ){
   					djDebug() << "user has quit,no info needed any more";
   				}else {
   					if ( !requestedUserIds.contains( userId ) ) {
   						djDebug() << "request its info";
   						requestedUserIds.insert( userId );
   						requestUserInfo( userId );
   					}else {
   						djDebug() << "don't repeat request";
   					}
   				}
			}else {
				djDebug() << userId << user->userName() << "msgtype = "<<info->chType << "event = "<<info->chMsg;
				switch( info->chType ) {
				case DJGAME_SYSTEM_ID:
					switch( info->chMsg ) {
					case DJGAME_SYSTEM_MSG_QUIT:
						djDebug() << "DJGAME_SYSTEM_MSG_QUIT";
						break;
					case DJGAME_SYSTEM_MSG_SHUTUP: {
						djDebug() << "DJGAME_SYSTEM_MSG_SHUTUP";
						DJGameUser *user	= gameUser( userId );
						if ( user ) {
							QString text	= tr("%1 is banned to talk for %2 minutes").arg(user->userName()).arg(info->chBuf[0]);
							m_gameGroupChat->insertText2Browser( text );
						}	
						break;
					}
					default:
						break;
					}
					break;
				case DJGAME_MSGTYPE_EVENT:
					//don't handle self broadcase msg except score
					if ( userId == selfUserId() 
							&& DJGAME_MSG_EVENT_SCORE != info->chMsg
							&& DJGAME_MSG_EVENT_MAGIC != info->chMsg
							&& DJGAME_MSG_EVENT_MONEY != info->chMsg
							&& DJGAME_MSG_EVENT_COUNTER != info->chMsg )
						break;
					switch( info->chMsg ) {
					case DJGAME_MSG_EVENT_IN:
						djDebug() << "DJGAME_MSG_EVENT_IN";
						break;
					case DJGAME_MSG_EVENT_OUT:
					case DJGAME_MSG_EVENT_OFFLINE: {
						djDebug() << "DJGAME_MSG_EVENT_OUT";
						removeUser( user );
						break;
					}
					case DJGAME_MSG_EVENT_BREAK: {
						djDebug() << "DJGAME_MSG_EVENT_BREAK";
						user->setGeneralStatus( DJGAME_USER_STATUS_BREAK );
						QList<QVariant>	list;
						list	<< QVariant::fromValue((void*)m_hallContactTabWidget)
								<< QVariant(m_gameId)
								<< QVariant(userId) << QVariant(false);
						QVariant	parameters(list);
						m_hiHallCommandProc( HALL_COMMAND_USER_ONOFF_LINE, parameters );
						break;
					}
					case DJGAME_MSG_EVENT_CONTINUE: {
						djDebug() << "DJGAME_MSG_EVENT_CONTINUE";
						user->setGeneralStatus( DJGAME_USER_STATUS_NORMAL );
						QList<QVariant>	list;
						list 	<< QVariant::fromValue((void*)m_hallContactTabWidget)
							 	<< QVariant(m_gameId)
								<< QVariant(userId) << QVariant(true);
						QVariant	parameters(list);
						m_hiHallCommandProc( HALL_COMMAND_USER_ONOFF_LINE, parameters );
						break;
					}
					case DJGAME_MSG_EVENT_DOWN: 
					case DJGAME_MSG_EVENT_READY:{
						djDebug() << "DJGAME_MSG_EVENT_DOWN";
						quint8 tableId	= info->chBuf[0];
						quint8 seatId	= info->chBuf[1];
						
						DJTableController *tc	= tableController( tableId );
						if ( 0 == tc )
							break;
						DJGameUser* conflictUser	= tc->userAtSeat( seatId );
						if ( conflictUser && conflictUser != user ) {
							//this table map is unauthentic,request again
							djDebug()<<"conflictuser"<<conflictUser->userName()<<"user"<<user->userName();
							requestTableMap( tableId, tableId );
						}else {
							if ( DJGAME_MSG_EVENT_DOWN == info->chMsg )
								user->orTrinity( tableId, seatId, GAME_STATUS_DOWN );
							else
								user->orTrinity( tableId, seatId, GAME_STATUS_READY );
						}
						break;
					}
					case DJGAME_MSG_EVENT_UP: {
						djDebug() << "DJGAME_MSG_EVENT_UP";
						if ( user->isPlaying() ) {
							djDebug() << "from playing to idle";
							//do nothing, wait table2idle to sync
						}else {
							//if user status is not playing,no need to request table map
							//just set status to idle directly
							user->setTrinity( 0, 0, GAME_STATUS_IDLE );
						}
						break;
					}
					case DJGAME_MSG_EVENT_VIEW:
						djDebug() << "DJGAME_MSG_EVENT_VIEW";
						break;
					case DJGAME_MSG_EVENT_QUITVIEW:
						djDebug() << "DJGAME_MSG_EVENT_QUITVIEW";
						break;
					case DJGAME_MSG_EVENT_START: {
						djDebug() << "DJGAME_MSG_EVENT_START";
						quint8 tableId	=  info->chBuf[0];
						requestTableMap( tableId, tableId );
						break;
					}
					case DJGAME_MSG_EVENT_SCORE: {
						qint32	score	= letoh4( info->chBuf );
						djDebug() << "DJGAME_MSG_EVENT_SCORE"<<score;
						user->setScore( score );
						break;
					}
					case DJGAME_MSG_EVENT_MAGIC: {
						qint32	magic	= letoh4( info->chBuf );
						djDebug() << "DJGAME_MSG_EVENT_MAGIC"<<magic;
						user->setMagic( magic );
						break;
					}
					case DJGAME_MSG_EVENT_MONEY: {
						qint32	money	= letoh4( info->chBuf );
						djDebug() << "DJGAME_MSG_EVENT_MONEY"<<money;
						user->setMoney( money );
						break;
					}
					case DJGAME_MSG_EVENT_COUNTER: {
						qint32	chips	= letoh4( info->chBuf );
						djDebug() << "DJGAME_MSG_EVENT_COUNTER"<<chips;
						user->setChips( chips );
						break;
					}
					default:
						djDebug() << "*********unknown broadcast event subtype*********";
						unknownBroadcastHandler( info );
						break;
					}
					break;
				case DJGAME_MSGTYPE_SPEED:
					djDebug() << "DJGAME_MSGTYPE_SPEED";
					user->setSpeed( info->chMsg );
					break;
				case DJGAME_MSGTYPE_ROOM: {
					djDebug() << "DJGAME_MSGTYPE_ROOM";
					quint8 roomId	= info->chMsg;
					quint32	numberOfPlayers	= letoh4( info->chBuf );
					DJGameRoom	*room	= gameRoom( roomId );
					room->setCurUsers( numberOfPlayers );
					addRoomItem( room );
					changeNumberOfServerUsers();
					break;
				}
				default:
					djDebug() << "*********unknown broadcast msg type*********";
					unknownBroadcastHandler( info );
					break;
				}
			}
			info++;
   		}
		break;
	}
	case DJGAME_PROTOCOL_TYPE_CHAT: {
		djDebug() << "DJGAME_PROTOCOL_TYPE_CHAT";
		
		const ChatContent	*chatContent	= reinterpret_cast<const ChatContent *>( protocol->chBuf );
		quint32	speakerId	= letoh4( &chatContent->userid_speaker );
		quint32	listenerId	= letoh4( &chatContent->userid_listener );
		quint16 gameId	= makeint16(chatContent->chGameClass, chatContent->chGame);
		QString contents	= QString::fromUtf8( chatContent->szSentence );
		quint8	msgIndex	= protocol->chLanguage;
		djDebug()<<hex<<"speakerId"<<speakerId<<"listenerId"<<listenerId<<"gameId"<<gameId;
		//gameId must equal to 0xFFFF. e.g group chat
		if ( gameId == 0xFFFF ) {
			if ( speakerId <= 0xFF && msgIndex ) {
				djDebug()<<"speaker is system id";
				switch(speakerId) {
				case DJGAME_BLUEDJ_ROOM_ID:
					djDebug()<<"room msg"<<"index"<<msgIndex;
					if ( 1 == msgIndex ) {
						m_roomMsgs.clear();
					}
					//if msg len is less than or equal to 4, this msg will be delete 
					if ( subtype > 4 )
						m_roomMsgs.insert( msgIndex - 1, contents );
					break;
				case DJGAME_BLUEDJ_GAME_ID:
					djDebug()<<"panel msg"<<"index"<<msgIndex;
					if ( 1 == msgIndex ) {
						m_panelMsgs.clear();
					}
					if ( subtype > 4 )
						m_panelMsgs.insert( msgIndex - 1, contents  );
					break;
				case DJGAME_AD_ID:
					djDebug()<<"ad msg"<<"index"<<msgIndex;
					if ( 1 == msgIndex ) {
						m_adMsgs.clear();
					}
					if ( subtype > 4 )
						m_adMsgs.insert( msgIndex - 1, contents  );
					break;
				default:
					break;
				}
			}
			
			if ( GroupIsTable(listenerId) ) {
				djDebug() << "group is table";
				quint8	tableId	= ExtractTableId(listenerId);
				DJTableController* tc	= tableController(tableId);
				if ( 0 == tc )
					break;
				DJPanelController* pc	= tc->panelController();
				if ( pc ) {
					pc->receiveChat( chatContent );
				}
			}else if ( GroupIsRoom(listenerId) ) {
				djDebug() << "group is room";
				quint8	roomId	= ExtractRoomId(listenerId);
				djDebug()<<"roomId="<<roomId;
				if ( roomId == selfRoomId() ) {
					if ( m_gameGroupChat ) {
						m_gameGroupChat->receiveChat( chatContent );
					}
				}
			}else if ( GroupIsAdmin(listenerId) ) {
				djDebug() << "group is admin";
				if ( m_adminGroupChat ) {
					m_adminGroupChat->receiveChat( chatContent );
				}
			}else {
				djDebug() << "********group is unknown********";
			}
		}else if ( listenerId < MIN_USER_ID || speakerId < MIN_USER_ID ) {
			//some special group chat
   			if ( DJGAME_PLAYER_BROADCAST == listenerId ) {
   				//use broadcast card
   				if ( m_gameGroupChat ) {
					m_gameGroupChat->receiveChat( chatContent );
				}
				QList<DJPanelController*> pcs	= allPanelControllers();
				foreach( DJPanelController* pc, pcs ) {
					pc->receiveChat( chatContent );
				}
   			}
		}
		break;
	}
	case DJGAME_PROTOCOL_TYPE_GETCARDS: {
		djDebug()<<"DJGAME_PROTOCOL_TYPE_GETCARDS";
		quint8 totolCards	= subtype;
		const DJGamePlayerCards	*pcards	= reinterpret_cast<const DJGamePlayerCards*>( protocol->chBuf );
		for ( int i = 0; i < totolCards; ++i ) {
			quint8	id	= pcards->chCardID;
			quint8 	pages	= pcards->chPages;
			DJCard *card	= m_cards.value( id, 0 );
			if ( !card ) {
				card	= new DJCard( id, pages );
				m_cards.insert( id, card );
			}else {
				card->setPages( pages );
			}
			pcards++;
		}
		djDebug()<<"m_cards items="<<m_cards.size();
		foreach( DJCard *card, m_cards ) {
			djDebug()<<DJCard::name(card->id())<<hex<<"id="<<card->id()<<dec<<"leftSeconds="<<card->leftSeconds()<<card->pages();
		}
		emit cardsReloaded();
		break;
	}
	default:
		break;
	}
	djDebug() << "gameDataHandler end";
}
void DJGameController::createRoom( DJGameRoom *currentRoom )
{
	djDebug() << "createRoom";
	
	quint8	numberOfTables	= currentRoom->numberOfTables();
	quint8 	numberOfSeats	= currentRoom->numberOfSeats();
	
	djDebug() << "numberOfTables" << numberOfTables << "numberOfSeats" << numberOfSeats;
	
	QPixmap theTablePixmap	= tablePixmap( numberOfSeats );
	QFont	theTableFont	= tableFont();
	QColor	theTableColor	= tableColor();
	QFont	thePlayerFont	= nameFont();
	QColor	thePlayerColor	= nameColor();
	QList<QPixmap>	theSeatPixmaps	= seatPixmaps( numberOfSeats );
	djDebug() << numberOfSeats << theSeatPixmaps.size();
	QPixmap theSelfReadyPixmap	= selfReadyPixmap();
	QPixmap thePlayingPixmap	= playingPixmap();
	
	QSize theCellSize	= cellSize( numberOfSeats );
	QList<QPoint>	theSeatOffsets	= seatOffsets( numberOfSeats );
	QList<QPoint>	theAvatarOffsets	= avatarOffsets( numberOfSeats );
	//m_userCanvasView->initialize( numberOfTables, theTablePixmap, theTableFont, theTableColor,
		//					theSeatPixmaps, theSelfReadyPixmap, thePlayingPixmap,
			//				theCellSize, theSeatOffsets, theAvatarOffsets );
	QPixmap	backgoundPix	= QPixmap( gameResDir() + "/image/base/floor.png" );
	m_roomController->createRoomItems( theCellSize, backgoundPix, 
						theSelfReadyPixmap, thePlayingPixmap,
						numberOfTables, theTablePixmap,
					 	theTableFont, theTableColor,
					 	thePlayerFont, thePlayerColor, 
					 	theSeatPixmaps, theSeatOffsets );
	m_roomController->repaintAll();
}
void DJGameController::addRoomItem( DJGameRoom* room )
{
	quint8	roomId	= room->roomId();
	DJGameRoomItem*	roomItem	= 0;
	for ( int i = 0; i < m_roomTree->topLevelItemCount(); i++) {
		DJGameRoomItem* item	= static_cast<DJGameRoomItem*>(m_roomTree->topLevelItem(i));
		if ( item->id() == roomId ) {
			roomItem	= item;
			break;
		}
	}
	if ( 0 == roomItem )
		roomItem	= new DJGameRoomItem( roomId, m_roomTree, DJGameItemRoomType );
	
	QString name	= roomName(room);
	name += QString("(%1/%2)").arg(room->curUsers()).arg(room->maxUsers());
	roomItem->setText(0,name);
	
	qreal	rate	= static_cast<qreal>(room->curUsers()) / static_cast<qreal>(room->maxUsers());
	djDebug() << "room rate = " << rate;
	/*if ( rate < 0.3 ) {
		roomItem->setTextColor(0,Qt::green);
	}else */if ( rate < 0.8 ) {
		roomItem->setTextColor(0,Qt::black);
	}else {
		roomItem->setTextColor(0,Qt::red);
	}
}
void DJGameController::changeToRoom( DJGameRoom* room )
{
	djDebug() << "changeToRoom";
	quint8	roomId	= room->roomId();
	for ( int i = 0; i < m_roomTree->topLevelItemCount(); i++) {
		DJGameRoomItem* item	= static_cast<DJGameRoomItem*>(m_roomTree->topLevelItem(i));
		if ( item->id() == roomId ) {
			item->setIcon( 0, QIcon( ":/BaseRes/image/hall/openingdoor.png") );
		}else {
        	item->setIcon( 0, QIcon( ":/BaseRes/image/hall/closeddoor.png") );
        }
	}
}
void DJGameController::changeNumberOfServerUsers()
{
	djDebug() << "changeNumberOfServerUsers";
	int numberOfServerUsers	= 0;
	foreach( DJGameRoom* room, m_rooms ) {
		numberOfServerUsers	+= room->curUsers();
	}
	QList<QVariant>	list;
	list << QVariant::fromValue((void*)m_hallController)
		<< QVariant(m_gameId) << QVariant(m_hallGameItem->curServerId()) << QVariant(numberOfServerUsers);
	QVariant	parameters(list);
	m_hiHallCommandProc( HALL_COMMAND_CHANGE_SERVER_USERS, parameters );
	m_roomTree->setHeaderLabels( QStringList( gameDisplayName() + "--" + serverDisplayName() ) );
}
DJTableController* DJGameController::tableController( quint8 tableId ) const
{
	return m_roomController->tableController( tableId );
}
void DJGameController::playWave( const QString& filePath ) const
{
	//djDebug() << "DJGameController::playWave" << m_sound << filePath;
	//return;
	
	if ( m_sound ) {
		djDebug() << "playWave" << filePath;
		PlayWave( filePath.toLocal8Bit() );
		djDebug() << "playWave end";
	}
}
bool DJGameController::sendRequest( const DJClientRequest& request, bool isDiscardable ) const
{
	QList<QVariant>	list;
	list << QVariant::fromValue((void*)m_hallGameSocket) << QVariant::fromValue((void*)&request) << QVariant(isDiscardable);
	QVariant	parameters(list);
    QVariant result	= m_hiHallCommandProc( HALL_COMMAND_SEND_REQUEST, parameters );
    return result.toBool();
}
void DJGameController::removeAcl( const QList<void*>& acls )
{
	djDebug() << m_gameName << "removeAcl";
	QList<QVariant>	list;
	list << QVariant::fromValue((void*)m_hallGameSocket) << QVariant::fromValue((void*)&acls);
	QVariant	parameters(list);
    m_hiHallCommandProc( HALL_COMMAND_REMOVE_ACL, parameters );
}

void DJGameController::requestChangeRoom( quint8 roomId )
{
	djDebug() << m_gameName << "requestChangeRoom";
	
	QList<DJPanelController*>	pcs	= allPanelControllers();
	if ( !m_selfUser->isIdle() || !pcs.isEmpty()  ) {
		djDebug() << "panels are not empty";
		DJMessageBox::information( 10, 0,tr("Can't change room"),tr("You must be idle when you change room"),QMessageBox::Ok );
		return;
	}
	QByteArray gameData;
	gameData.append( roomId );
	QByteArray buf	= MakeGameData( m_gameId, m_language, DJGAME_PROTOCOL_SUBTYPE_CHANGEROOM, gameData );
	DJClientRequest request( buf );
	
	sendRequest( request );
}
void DJGameController::quit()
{
	QList<QVariant>	list;
	list << QVariant::fromValue((void*)m_hallGameSocket) << QVariant(m_gameId);
	QVariant	parameters(list);
    m_hiHallCommandProc( HALL_COMMAND_GAME_QUITTED, parameters );
}
//private slots
void DJGameController::setup()
{
	UIGameSetup uiSetup( m_gameName, gameDisplayName(), gameIcon() );
	if ( QDialog::Accepted == uiSetup.exec() ) {
		m_selfSetup	= DJGameSetup( m_gameName );
		sendSetup();
	}
}
void DJGameController::reconnect()
{
	djDebug() << m_gameName << "reconnect";
	QList<QVariant>	list;
	list 	<< QVariant::fromValue((void*)m_hallController)
			<< QVariant::fromValue((void*)m_hallGameItem)
		 	<< QVariant::fromValue((void*)m_hallGameSocket);
	QVariant	parameters(list);
    m_hiHallCommandProc( HALL_COMMAND_RECONNECT, parameters );
}
void DJGameController::callAdmin()
{
	djDebug() << m_gameName << "callAdmin";
	
	UICallAdmin	callAdminDlg;
	if ( QDialog::Accepted == callAdminDlg.exec() ) {
		CallAdminInfo	info;
		memset( &info, 0, sizeof(CallAdminInfo) );
    	info.callerId	= selfUserId();
    	info.roomId		= selfRoomId();
    	info.tableId	= m_selfUser->tableId();
    	info.reason		= callAdminDlg.reason();
    	info.eventType	= ADMIN_CHAT_EVENT_TYPE_CALL_ADMIN;
    	
    	QString		sentence;
    	QTextStream	stream( &sentence, QIODevice::ReadWrite );
    	stream <<info.callerId<<" "<<info.roomId<<" "<<info.tableId
    			<<" "<<info.reason<<" "<<info.eventType;
    	sentence.prepend("{{");
    	sentence.append("}}");
    	
    	sentence.append( callAdminDlg.additionalMsg() );
    	QByteArray	utf8	= sentence.toUtf8();
    	
    	QByteArray	chatContent( sizeof(ChatContent) + utf8.size() + 1, 0 );
        ChatContent	*pChatContent	= reinterpret_cast<ChatContent *>(chatContent.data());
        setle4( &pChatContent->userid_speaker, selfUserId() );
        setle4( &pChatContent->userid_listener, MakeGroupId(gameId(),0,DJGAME_ADMINISTRATOR_TABLEGROUP) );
        pChatContent->chGameClass = 0xFF;
        pChatContent->chGame = 0xFF;
        qstrcpy( pChatContent->szSentence, utf8.data() );
        
    	requestChat( pChatContent, chatContent.size() );
	}
}
void DJGameController::issueMessage()
{
	djDebug() << m_gameName << "issueMessage";
	UIIssueMsg	issueMsgDlg(this);
	issueMsgDlg.exec();
}
void DJGameController::requestAdminCommand( quint8 type, quint8 subtype, const DJGameAdminCommand& command, const QString& text )
{	
	QByteArray utf8	= text.toUtf8();
	djDebug()<<"DJGameController::requestAdminCommand"<<text.size()<<utf8.size();
	QByteArray	adminData	= QByteArray::fromRawData( reinterpret_cast<const char *>(&command),
									sizeof(DJGameAdminCommand) + utf8.size() + 1 );
	if ( !utf8.isEmpty() ) {
		DJGameAdminCommand *adminCommand	= reinterpret_cast<DJGameAdminCommand*>(adminData.data());
		adminCommand->chBufLen	= utf8.size();
		qstrcpy( reinterpret_cast<char*>(adminCommand->chBuf), utf8.data() );
		DebugBuffer( adminData );
	}
	QByteArray buf;
	if ( DJGAME_PROTOCOL_TYPE_SUBMIT_GAMEDATA == type ) {
		buf	= MakeGameData( gameId(), language(), subtype, adminData );
	}else {
		buf	= MakeGeneralData( type, subtype, language(), adminData );
	}
	
	DJClientRequest request( buf );
	sendRequest( request );
}
void DJGameController::requestQuit()
{
	djDebug() << m_gameName << "requestQuit";
	QByteArray gameIdStream;
	gameIdStream.append( hibyte(m_gameId) );
	gameIdStream.append( lobyte(m_gameId) );
	
	QByteArray buf	= MakeGeneralData( DJGAME_PROTOCOL_TYPE_QUIT, 0, m_language, gameIdStream );
	QVariant parameters	= QVariant::fromValue((void*)this);
	DJClientRequest request( buf, aclQuitGameProc, parameters );
	
	sendRequest( request, true );
}
void DJGameController::requestUserInfo( quint32 userId )
{
	djDebug() << m_gameName << "requestUserInfo" << userId;
	QByteArray	useridStream	= htoa4( userId, QDataStream::LittleEndian );
	QByteArray buf	= MakeGameData( m_gameId, m_language, DJGAME_PROTOCOL_SUBTYPE_GET_USERINFO, useridStream );
	DJClientRequest request( buf );
	
	sendRequest( request );
}
void DJGameController::requestTableMap( quint8 minTableId, quint8 maxTableId, quint8 roomId )
{
	djDebug() << m_gameName << "requestTableMap" << minTableId << maxTableId << roomId;
	if ( 0 == roomId ) {
		//user m_selfuser roomId
		roomId	= selfRoomId();			
	}
	if ( 0 == roomId ) {
		return;
	}
	
	djDebug() << "request table map room id" << roomId;
	
	QByteArray	data;
	data.append( roomId );
	data.append( minTableId );
	data.append( maxTableId );
	QByteArray buf	= MakeGameData( m_gameId, m_language, DJGAME_PROTOCOL_SUBTYPE_GET_TABLEMAP, data );
	//QVariant parameters	= QVariant::fromValue((void*)this);
	//DJClientRequest request( buf, aclTableMapProc, parameters );
	DJClientRequest request( buf );
	sendRequest( request );
	/*
	if ( 0 == minTableId || 0 == maxTableId ) {
		minTableId	= 1;
		maxTableId	= m_rooms.value( roomId )->numberOfTables();
	}
	QList<quint8>	tables;
	for (int i = minTableId; i <= maxTableId; ++i ) {
		tables << i;
	}
	m_requestedTableMaps.append( tables );
	*/
}
void DJGameController::requestLookOn( quint32 userId )
{
	djDebug() << m_gameName << "requestLookOn" << userId;
	DJGameUser* user	= gameUser( userId );
	if ( 0 == user )
		return;
	m_lookOnSeat	= user->seatId();
	
	QByteArray	data;
	data.append( user->tableId() );
	data.append( user->seatId() );
	QByteArray buf	= MakeGameData( m_gameId, m_language, DJGAME_PROTOCOL_SUBTYPE_VIEW, data );
	DJClientRequest request( buf );
	
	sendRequest( request );
}
void DJGameController::requestInviteUser( quint32 userId )
{
	QByteArray	data	= htoa4( userId, QDataStream::LittleEndian );
	data.append( m_selfUser->tableId() );
	
	QByteArray buf	= MakeGameData( m_gameId, m_language, DJGAME_PROTOCOL_SUBTYPE_INVITE, data );
	DJClientRequest request( buf );
	
	sendRequest( request );
}
void DJGameController::requestForceStandUp( quint32 userId )
{
	QByteArray	data	= htoa4( userId, QDataStream::LittleEndian );
	QByteArray buf	= MakeGameData( m_gameId, m_language, DJGAME_PROTOCOL_SUBTYPE_KICK, data );
	DJClientRequest request( buf );
	
	sendRequest( request );
}
void DJGameController::requestUseCard( const ReqUseCard& reqUseCard )
{
	djDebug()<<"DJGameController::requestUseCard";
	QByteArray	data	= QByteArray::fromRawData( reinterpret_cast<const char *>(&reqUseCard), sizeof(ReqUseCard) );
	
	QByteArray buf	= MakeGameData( m_gameId,
									m_language,
									DJGAME_PROTOCOL_SUBTYPE_GAMECARD,
									data );
	//QList<QVariant> list;
	//list << QVariant::fromValue((void*)this) << QVariant(reqUseCard.chCard);
	//QVariant	parameters(list);
    //DJClientRequest request( buf, aclUseCardProc, parameters );
    DJClientRequest request( buf );
	sendRequest( request );	
}
void DJGameController::requestChat( const ChatContent* pChatContent, int size )
{
	QByteArray	data	= QByteArray::fromRawData( reinterpret_cast<const char *>(pChatContent), size );
	QByteArray buf	= MakeGeneralData( DJGAME_PROTOCOL_TYPE_CHAT, 0, m_language, data );
	DJClientRequest request( buf );
	sendRequest( request );	
}
void DJGameController::chatWithUser( quint32 userId, bool isForced )
{
	djDebug() << "DJGameController::chatWithUser";
	QList<QVariant>	list;
	DJGameUser *user	= gameUser( userId );
	QString userName;
	if ( user ) {
		userName	= user->userName();
	 }else {
	 	userName	= QString::number(userId);
	}
		
	list << QVariant::fromValue((void*)m_hallContactTabWidget)
			<< QVariant(userId) << QVariant(userName);
	if ( isForced )
		list << QVariant(m_gameId);
	else
		list << QVariant(0);
	QVariant	parameters(list);
    m_hiHallCommandProc( HALL_COMMAND_USER_LITERAL_CHAT, parameters );
}
int DJGameController::contactUserType( quint32 userId )
{
	QList<QVariant>	list;
	list << QVariant::fromValue((void*)m_hallContactTabWidget) << QVariant(userId);
	QVariant	parameters(list);
    QVariant	result	= m_hiHallCommandProc( HALL_COMMAND_USER_CONTACT_TYPE, parameters );
    return result.toInt();
}
void DJGameController::addContactUserFriend( quint32 userId )
{
	QList<QVariant>	list;
	list << QVariant::fromValue((void*)m_hallContactTabWidget) << QVariant(m_gameId) 
			<< QVariant(userId) << QVariant(DJ_CONTACT_USER_TYPE_FRIEND);
	QVariant	parameters(list);
    m_hiHallCommandProc( HALL_COMMAND_USER_ADD_CONTACT, parameters );
}
void DJGameController::removeContactUserFriend( quint32 userId )
{
	QList<QVariant>	list;
	list << QVariant::fromValue((void*)m_hallContactTabWidget) << QVariant(userId) << QVariant(DJ_CONTACT_USER_TYPE_FRIEND);
	QVariant	parameters(list);
    m_hiHallCommandProc( HALL_COMMAND_USER_REMOVE_CONTACT, parameters );
}
void DJGameController::addContactUserUnwelcome( quint32 userId )
{
	djDebug()<<"addContactUserUnwelcome"<<userId;
	QList<QVariant>	list;
	list << QVariant::fromValue((void*)m_hallContactTabWidget) << QVariant(m_gameId) 
			<< QVariant(userId) << QVariant(DJ_CONTACT_USER_TYPE_UNWELCOME);
	QVariant	parameters(list);
    m_hiHallCommandProc( HALL_COMMAND_USER_ADD_CONTACT, parameters );
    
    QByteArray	useridStream	= htoa4( userId, QDataStream::LittleEndian );
    QByteArray buf	= MakeGeneralData( DJGAME_PROTOCOL_TYPE_UNWELCOME, 0, m_language, useridStream );
	DJClientRequest request( buf );
	
	sendRequest( request );
}
void DJGameController::removeContactUserUnwelcome( quint32 userId )
{
	QList<QVariant>	list;
	list << QVariant::fromValue((void*)m_hallContactTabWidget) << QVariant(userId) << QVariant(DJ_CONTACT_USER_TYPE_UNWELCOME);
	QVariant	parameters(list);
    m_hiHallCommandProc( HALL_COMMAND_USER_REMOVE_CONTACT, parameters );
}
void DJGameController::requestSitDown( quint8 tableId, quint8 seatId )
{
	djDebug() << m_gameName << "requestSitDown" << tableId << seatId;
	if ( m_selfUser->isPlaying() )
		return;
	/*
	DJCell* table	= cell(tableId);
	if ( 0 == table )
		return;
	bool isUnwelcome	= false;
	QList<DJGameUser*> users	= table->orderedUsers();
	foreach( DJGameUser* user, users ) {
		if ( m_selfSetup.maxUnwelcome() && user->unwelcome() >= m_selfSetup.maxUnwelcome() ) {
			isUnwelcome	= true;
		}
	}
	if ( isUnwelcome ) {
		int result	= DJMessageBox::information( 10, 0, tr("Game Setup Conflicts"),tr("There is unwelcome player at this table,\n Are you still going to sit down?"),QMessageBox::Yes, QMessageBox::No );
		if ( QMessageBox::No == result )
			return;
	}
	*/
    QByteArray	data;
    data.append( tableId );
    data.append( seatId );
    QByteArray buf	= MakeGameData( m_gameId, m_language, DJGAME_PROTOCOL_SUBTYPE_DOWN, data );
    QVariant parameters	= QVariant::fromValue((void*)this);
    DJClientRequest request( buf, aclSitDownProc, parameters );
    	
    sendRequest( request, true );
}
void DJGameController::requestStandUp()
{
	djDebug() << m_gameName << "requestStandUp";
	QByteArray buf	= MakeGameData( m_gameId, m_language, DJGAME_PROTOCOL_SUBTYPE_UP );
	DJClientRequest request( buf );
	
	sendRequest( request );
}
void DJGameController::requestReady()
{
	djDebug() << m_gameName << "requestReady";
	QByteArray buf	= MakeGameData( m_gameId, m_language, DJGAME_PROTOCOL_SUBTYPE_READY );
	DJClientRequest request( buf );
	
	sendRequest( request );
}
void DJGameController::userSelected( DJGameUser* user )
{
	quint32 userId	= user->userId();
	DJGameUserItem *userItem	= gameUserItem(userId);
	if ( userItem ) {
		m_userTree->setFocus( Qt::OtherFocusReason );
		m_userTree->setCurrentItem( userItem );
	}
}
void DJGameController::userContext( const QPoint &point )
{
	djDebug() << "userContext";
	DJGameUserItem *userItem	= static_cast<DJGameUserItem *>(m_userTree->itemAt( point ));
	if ( userItem ) {
		DJGameUser	*user	= userItem->user();
		showUserContext( user );
	}
}
void DJGameController::userItemDoubleClicked( QTreeWidgetItem * item, int column )
{
	djDebug() << "roomItemDoubleClicked";
	DJGameUserItem *userItem	= static_cast<DJGameUserItem *>(item);
	userDoubleClicked( userItem->user() );
}
void DJGameController::userDoubleClicked( DJGameUser* user )
{
	djDebug() << "userDoubleClicked";
	if ( m_gameGroupChat )
		m_gameGroupChat->insertText2Editor( user->userName() + "," );
}
void DJGameController::roomItemDoubleClicked( QTreeWidgetItem * item, int column )
{
	djDebug() << "roomItemDoubleClicked";
	switch ( item->type() ) {
	case DJGameItemRoomType: {
		DJGameRoomItem	*roomItem	= static_cast<DJGameRoomItem *>(item);
		quint8 newRoomId	= roomItem->id();
		if ( newRoomId != selfRoomId() )
			requestChangeRoom( roomItem->id() );
		break;
	}
	default:
		break;
	}
}
void DJGameController::roomTreeContext( const QPoint &point )
{
	djDebug() << "roomTreeContext";
	/*
	m_roomTreeMenu->clear();
	m_roomTreeMenu->addAction(m_actSetup);
	m_roomTreeMenu->addAction(m_actQuit);
	if ( m_selfUser && m_selfUser->userType() >= DJGAME_USERTYPE_ADMIN2 )
		m_roomTreeMenu->addAction(m_actIssueMsg);
	if ( m_needInitialize )
		m_roomTreeMenu->addAction(m_actReconnect);
	m_roomTreeMenu->exec( QCursor::pos() );
	*/
}
//private methods
void DJGameController::sendFeature()
{
	djDebug() << m_gameName << "sendFeature";
	QByteArray	buf(sizeof(DJGameProtocol)+sizeof(DJGameProtocolFeature),0);
	
    DJGameProtocol *protocol	= reinterpret_cast<DJGameProtocol *>(buf.data());
    protocol->chTotalLen	= buf.size(); 	
	protocol->chType = DJGAME_PROTOCOL_TYPE_SETFEATURE;
	protocol->chLanguage = 1;
	DJGameProtocolFeature	*feature	= reinterpret_cast<DJGameProtocolFeature *>(protocol->chBuf);
    feature->chProtocolVersion	= 2;
    DJClientRequest request( buf );

	sendRequest( request );
}
void DJGameController::sendSetup()
{
	djDebug() << m_gameName << "sendSetup";
	GameNormalSetup	setup	= m_selfSetup;
	QByteArray setupStream( reinterpret_cast<const char*>(&setup), sizeof(GameNormalSetup) );
	QByteArray buf	= MakeGameData( m_gameId, m_language, DJGAME_PROTOCOL_SUBTYPE_SETUP, setupStream );
	DJClientRequest request( buf );

	sendRequest( request );
}
void DJGameController::addGameContextMenuItem( QMenu* gameContextMenu )
{
	if ( m_btnAdminIssueMsg->isVisible() )
		gameContextMenu->addAction( m_btnAdminIssueMsg->icon(),m_btnAdminIssueMsg->text(), this, SLOT(issueMessage()) );
	if ( m_btnCallAdmin->isVisible() )
		gameContextMenu->addAction( m_btnCallAdmin->icon(),m_btnCallAdmin->text(), this, SLOT(callAdmin()) );
	if ( m_btnReconnect->isVisible() )
		gameContextMenu->addAction( m_btnReconnect->icon(),m_btnReconnect->text(), this, SLOT(reconnect()) );
	if ( m_btnSetup->isVisible() )
		gameContextMenu->addAction( m_btnSetup->icon(),m_btnSetup->text(), this, SLOT(setup()) );
	if ( m_btnQuit->isVisible() )
		gameContextMenu->addAction( m_btnQuit->icon(),m_btnQuit->text(), this, SLOT(requestQuit()) );
}
void DJGameController::addUserToRoomView( DJGameUser* user )
{	
	m_roomController->addUser( user );
}
void DJGameController::removeUserFromRoomView( DJGameUser* user )
{
	m_roomController->removeUser( user );
}
QList<DJPanelController*> DJGameController::allPanelControllers()
{
	QList<DJPanelController*> pcs;
	
	quint8 tables	= selfRoom()->numberOfTables();
	for ( quint8 tableId = 1; tableId <= tables; tableId++ ) {
		DJTableController *tc	= tableController( tableId );
		if ( tc ) {
			DJPanelController *pc	= tc->panelController();
			if ( pc )
				pcs << pc;
		}
	}
	return pcs;
}
QList<quint8> DJGameController::allPanelTableIds()
{
	QList<quint8> tableIds;
	quint8 tables	= selfRoom()->numberOfTables();
	for ( quint8 tableId = 1; tableId <= tables; tableId++ ) {
		DJTableController *tc	= tableController( tableId );
		if ( tc ) {
			DJPanelController *pc	= tc->panelController();
			if ( pc )
				tableIds << tableId;
		}
	}
	return tableIds;
}
void DJGameController::removeUser( DJGameUser* user)
{
	quint32 userId = user->userId();
	
	if ( !m_users.contains(userId) )
		return;
		
	removeUserFromRoomView( user );
	
	DJGameUserItem *userItem	= m_userItems.take( userId );
	delete userItem;
	m_users.remove( userId );
	delete user;
	QList<QVariant>	list;
	list << QVariant::fromValue((void*)m_hallContactTabWidget) << QVariant(m_gameId)
		<< QVariant(userId) << QVariant(false);
	QVariant	parameters(list);
	m_hiHallCommandProc( HALL_COMMAND_USER_ONOFF_LINE, parameters );

	DJGameRoom* room	= selfRoom();
	if ( room ) {
		room->setCurUsers( m_users.size() );
		addRoomItem( room );
	}
	changeNumberOfServerUsers();
}
void DJGameController::removeUser( quint32 userId )
{
	DJGameUser* user	= gameUser( userId );
   	if ( user ) {
   		removeUser( user );
   	}
}
QSize DJGameController::avatarSize() const
{
	return QSize(75,75);
}
QPixmap	DJGameController::readyPixmap() const
{
	return QPixmap( ":/BaseRes/image/hall/ready.png" );
}
QFont DJGameController::nameFont() const
{
	QFont	font;
	font.setPointSize( 16 );
	font.setBold( true );
	return font;
}
QColor DJGameController::nameColor() const
{
	return Qt::black;
}
QPixmap	DJGameController::tablePixmap( quint8 numberOfSeats ) const
{
	QPixmap pixmap	= QPixmap( m_gameResDir + QString("/image/base/table%1.png").arg(numberOfSeats) );
	QSize size	= pixmap.size();
	size *= 0.8;
	pixmap	= pixmap.scaled( size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
	return pixmap;
}
QFont DJGameController::tableFont() const
{
	QFont	font;
	font.setPointSize( 24 );
	font.setBold( true );
	return font;
}
QColor DJGameController::tableColor() const
{
	return Qt::white;
}
QList<QPixmap> DJGameController::seatPixmaps( quint8 numberOfSeats ) const
{
	QList<QPixmap>	pixmaps;
	for ( int i = 0; i < numberOfSeats; i++ ) {
		pixmaps << QPixmap( m_gameResDir + QString("/image/base/seat%1_%2.png").arg(numberOfSeats).arg(i+1) );
	}
	return pixmaps;
}
QPixmap DJGameController::playingPixmap() const
{
	QPixmap pix( m_gameResDir + "/image/base/game.png");
	pix	= pix.scaled(32,32,Qt::KeepAspectRatio,Qt::SmoothTransformation);
	return pix;
}
QPixmap DJGameController::selfReadyPixmap() const
{
	return QPixmap( ":/BaseRes/image/hall/ready.png" );
}
QSize DJGameController::cellSize( quint8 numberOfSeats ) const
{
	if ( 2 == numberOfSeats ) {
		return QSize( 240, 160 );
	}else {
		return QSize( 240, 240 );
	}
}
QList<QPoint> DJGameController::seatOffsets( quint8 numberOfSeats ) const
{
	QList<QPoint>	offsets;
	int offset	= 86;
	if ( 4 == numberOfSeats ) {
		offsets << QPoint(0,-offset) << QPoint(offset,0) << QPoint(0,offset) << QPoint(-offset,0);
	}else if ( 3 == numberOfSeats ) {
		offsets << QPoint(0,-offset) << QPoint( static_cast<int>(0.866 * offset),offset/2) << QPoint(-(static_cast<int>(0.866 * offset)),offset/2);
	}else if ( 2 == numberOfSeats ) {
		offsets << QPoint(-offset,0) << QPoint(offset,0);
	}else if ( 1 == numberOfSeats ) {
		offsets << QPoint(0,offset);
	}else {
		for ( int i = 0; i < numberOfSeats; i++ )
			offsets << QPoint(0,0);
	}
	return offsets;
}
QList<QPoint> DJGameController::avatarOffsets( quint8 numberOfSeats ) const
{
	return seatOffsets( numberOfSeats );
}
QString	DJGameController::translatedGameName() const
{
	return m_gameName;
}
QString DJGameController::roomName( DJGameRoom* normalRoom ) const
{
	QString	name;
	name	+= roomTitle();
	name	+= QString("%1").arg(normalRoom->roomId());
	if ( normalRoom->options() & DJGAME_ROOMOPTION_COUNTER ) {
		quint8 multiple	= normalRoom->multiple();
		if ( multiple >= 2 ) {
			name	+= QString("(%1x%2)").arg(baseSectionName(Chips))
											.arg(multiple);
		}else {
			name	+= QString("(%1)").arg(baseSectionName(Chips));
		}
	}
	return name;
}
QString DJGameController::roomTitle() const
{
	return tr("room");
}
QString	DJGameController::roomDescription( DJGameRoom* normalRoom ) const
{
	return QString();
}
void DJGameController::showUserContext( DJGameUser* user )
{
	/*
	//disconnect all signals from previous user to userContextWidget
	DJGameUserContext	*contextWidget	= userContextWidget();
	quint32 prevUserId	= contextWidget->clickedUserId();
	if ( prevUserId ) {
		DJGameUser	*prevUser	= gameUser( prevUserId );
		if ( prevUser ) {
			prevUser->disconnect( contextWidget );
		}
	}	
	
	connect( user, SIGNAL(speedChanged(DJGameUser*)), contextWidget, SLOT(createUserInfo(DJGameUser*)) );
	connect( user, SIGNAL(moneyChanged(DJGameUser*)), contextWidget, SLOT(createUserInfo(DJGameUser*)) );
	connect( user, SIGNAL(magicChanged(DJGameUser*)), contextWidget, SLOT(createUserInfo(DJGameUser*)) );
	connect( user, SIGNAL(scoreChanged(DJGameUser*)), contextWidget, SLOT(createUserInfo(DJGameUser*)) );
	connect( user, SIGNAL(chipsChanged(DJGameUser*)), contextWidget, SLOT(createUserInfo(DJGameUser*)) );
	connect( user, SIGNAL(winsChanged(DJGameUser*)), contextWidget, SLOT(createUserInfo(DJGameUser*)) );
	connect( user, SIGNAL(losesChanged(DJGameUser*)), contextWidget, SLOT(createUserInfo(DJGameUser*)) );
	connect( user, SIGNAL(drawsChanged(DJGameUser*)), contextWidget, SLOT(createUserInfo(DJGameUser*)) );
	connect( user, SIGNAL(unwelcomeChanged(DJGameUser*)), contextWidget, SLOT(createUserInfo(DJGameUser*)) );
	*/
	userContextWidget()->showUserContext( user );
}
QString DJGameController::userContextInfo( DJGameUser* user ) const
{
	DJGameUserItem	*userItem	= gameUserItem( user->userId() );
	QString rank;
	if ( userItem ) {
		int index	= indexOfSection( Rank );
		rank	= userItem->text( index );
	}
	QString	info;
	info	+= user->userName() + " : " + rank;
	info	+= "\n";
	info	+= baseSectionName(NickName) + " : " + user->nickName();
	info	+= "\n";
	DJGameRoom	*room	= selfRoom();
	if ( room && room->options() & DJGAME_ROOMOPTION_COUNTER ) {
		info	+= baseSectionName(Money)+ QString(":%1").arg(user->money()) + " ";
		info	+= baseSectionName(Chips)+ QString(":%1").arg(user->chips()) + "\n";
	}else {
		info	+= baseSectionName(Wins)+ QString(":%1").arg(user->wins()) + " ";
		info	+= baseSectionName(Loses)+ QString(":%1").arg(user->loses()) + " ";

		info	+= baseSectionName(Score)+ QString(":%1").arg(user->score()) + "\n";
		
		info	+= baseSectionName(Money)+ QString(":%1").arg(user->money()) + " ";
		info	+= baseSectionName(Chips)+ QString(":%1").arg(user->chips()) + "\n";
	}
	return info;
}
QSize DJGameController::desktopSize() const
{
	return QSize(800,800);
}
DJPanelController* DJGameController::createPanelController( DJTableController *tc, quint8 seatId )
{
	djDebug()<<"DJGameController::createPanelController";
	return new DJPanelController( tc, seatId );
}
DJGameUser*	DJGameController::createGameUser( const NormalUserDes *userDsc )
{
	return new DJGameUser( userDsc, this );
}
DJGameUser*	DJGameController::createGameUser( quint32 userId )
{
	return new DJGameUser( userId, this );
}
void DJGameController::gameUserItemHeaderSections( QList<int> &sectionIds, QStringList &sectionNames )
{
	sectionIds.clear();
	sectionNames.clear();
	
	DJGameRoom	*room	= selfRoom();
	if ( room && room->options() & DJGAME_ROOMOPTION_COUNTER ) {
		sectionIds << UserName << NickName << GameStatus 
				<< TableSeat
#ifndef DJ_OEM				
				<< Platform
#endif				
				<< Speed << Unwelcome
				<< Money << Chips 
				<< BreakRate << OnlineTime << Rank;
	}else {
		sectionIds << UserName << NickName << GameStatus 
				<< TableSeat
#ifndef DJ_OEM				
				<< Platform
#endif				
				<< Speed << Unwelcome
				<< Score << Wins << Loses << WinRate
				<< Money << Chips 
				<< BreakRate << OnlineTime << Rank;
	}
	
	for ( int i = 0; i < sectionIds.size(); ++i ) {
		sectionNames << baseSectionName( sectionIds.at(i) );
	}
	//sectionNames << "-";
}
void DJGameController::createGameUserItemHeader( QTreeWidget* treeWidget )
{	
	gameUserItemHeaderSections( m_sectionIds, m_sectionNames );
	
	QTreeWidgetItem* headerItem	= new QTreeWidgetItem;
    for ( int i = 0; i < m_sectionNames.size(); i++) {
    	headerItem->setTextAlignment( i, Qt::AlignCenter );
    	headerItem->setText( i, m_sectionNames.at(i) );
    }
	treeWidget->setHeaderItem( headerItem );
}
DJGameUserItem*	DJGameController::createGameUserItem( DJGameUser *user, QTreeWidget* parent, int type )
{
	if ( user->userId() == selfUserId() )
		type	= QTreeWidgetItem::UserType;
	return new DJGameUserItem( user, parent, type );
}
void DJGameController::unknownGameDataHandler( const QByteArray& gameData )
{
}
void DJGameController::unknownBroadcastHandler( const BroadcastInfo *info )
{
}
