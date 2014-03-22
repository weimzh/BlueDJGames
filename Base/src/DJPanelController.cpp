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

#include "DJPanelController.h"
#include "DJTableController.h"
#include "DJDesktopController.h"
#include "DJRoomController.h"
#include "DJGameController.h"
#include "DJTreeWidget.h"
#include "DJTextBrowser.h"
#include "DJShowView.h"
#include "GameUtility.h"
#include "Utility.h"
#include "DJClientRequest.h"
#include "DJPanel.h"
#include "DJPanelUserItem.h"
#include "DJGameUser.h"
#include "DJGameRoom.h"
#include "DJTreeItemDelegate.h"
#include "DJGameUserContext.h"
#include "DJGroupChat.h"
#include "DJToolButton.h"
#include "DJWidget.h"
#include "DJCardsWidget.h"
#include "DJCard.h"
#include "LaunchUrl.h"
#include "UIDropMsg.h"

static	QString		WWW_HOST_HELP_URL	= QString(HTTP_WWW_HOST) + "down/";

DJPanelController::DJPanelController( DJTableController* tableController, quint8 seatId )
	:m_tableController(tableController),m_seatId(seatId)
{	
	djDebug() << "DJPanelController constructor";
	m_players	= tableController->allUsers();
	m_panel	= 0;
	
	m_usersWidget	= 0;
	m_showView	= 0;
	m_chatWidget	= 0;
	m_desktopController	= 0;
	
	m_logo	= 0;
	//m_color	= 0;
	m_help	= 0;
	
	m_playerContextMenu	= 0;
	m_actRequestPrivateView	= 0;
	m_actAcceptPrivateView	= 0;
	m_actClosePrivateView	= 0;
}

DJPanelController::~DJPanelController()
{
	djDebug() << "DJPanelController destructor";
	delete m_desktopController;
	m_desktopController	= 0;
	
	delete m_panel;
	m_panel	= 0;
}
void DJPanelController::createPanel( const QSize& desktopSize )
{
	djDebug() << "DJPanelController createPanel";
	if ( m_panel )
		return;
	m_panel	= new DJPanel( this );
	m_panel->setWindowIcon( gameController()->gameIcon() );
	m_panel->setWindowTitle( gameController()->gameDisplayName() );
	QStatusBar*	statusBar	= new QStatusBar;
    statusBar->setMinimumHeight( 24 );
    m_panel->setStatusBar( statusBar );
#ifdef Q_WS_MAC
	m_panel->setAttribute( Qt::WA_MacMetalStyle );
#endif    

	QSplitter* mainSplitter = new QSplitter(Qt::Horizontal);
	m_panel->setCentralWidget( mainSplitter );
	QSplitter	*accessorySplitter, *gameSplitter;
	 	
	accessorySplitter	= new QSplitter( Qt::Vertical , mainSplitter );
	gameSplitter 	= new QSplitter( Qt::Vertical , mainSplitter );
	
	
	createAccessoryWidgets( accessorySplitter );
	createDesktopWidgets( desktopSize, gameSplitter );
	
	m_playerContextMenu	= new QMenu( m_panel );
	
	m_actRequestPrivateView	= new QAction( tr("Request Private View"), m_panel );
	m_actAcceptPrivateView	= new QAction( tr("Accept Private View"), m_panel );
	m_actClosePrivateView	= new QAction( tr("Close Private View"), m_panel );
}
void DJPanelController::createAccessoryWidgets( QWidget *parent )
{
	createUsersWidget( parent );
	createShowWidget( parent );
	createChatWidget( parent );
}
void DJPanelController::createDesktopWidgets( const QSize& desktopSize, QWidget *parent )
{
	createBarWidget( parent );
	m_desktopController	= createDesktopController( desktopSize, parent );
	m_desktopController->locateAll();
	m_desktopController->repaintAll();
}
void DJPanelController::createUsersWidget( QWidget* parent )
{
	if ( m_usersWidget )
		return;
		
    m_usersWidget	= new DJTreeWidget( parent );
    	
    createUserWidgetHeader( m_usersWidget );
    m_usersWidget->setRootIsDecorated(false);
    m_usersWidget->setSortingEnabled( true );
    	
    m_usersWidget->setItemDelegate(new DJTreeItemDelegate(this));
    m_usersWidget->setContextMenuPolicy( Qt::CustomContextMenu );
    connect(m_usersWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)), SLOT(handleUserItemDoubleClicked(QTreeWidgetItem *,int)));
    connect(m_usersWidget, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(handleUsersWidgetContext(const QPoint&)));
    	
    repaintAllPlayerItems();
}
void DJPanelController::createShowWidget( QWidget* parent )
{
	if ( m_showView )
		return;
		
	DJGameUser	*user	= player( m_seatId );
	if ( 0 == user )
		return;
	DJGameController *gc	= gameController();
	DJGameUserContext	*userContextWidget	= new DJGameUserContext( gc, parent );
	m_showView	= userContextWidget->showView();
	DJCardsWidget *cardsWidget	= userContextWidget->cardsWidget();
	cardsWidget->setPanelController(this);
	connect( gameController(), SIGNAL(cardsReloaded()), cardsWidget, SLOT(loadCards()));
	
	connect( m_showView, SIGNAL(rightClicked(DJGameUser*)), SLOT(showUserContextMenu(DJGameUser*)) );
	
	userContextWidget->showUserContext( user );
	connect( user, SIGNAL(speedChanged(DJGameUser*)), userContextWidget, SLOT(createUserInfo(DJGameUser*)) );
	connect( user, SIGNAL(moneyChanged(DJGameUser*)), userContextWidget, SLOT(createUserInfo(DJGameUser*)) );
	connect( user, SIGNAL(magicChanged(DJGameUser*)), userContextWidget, SLOT(createUserInfo(DJGameUser*)) );
	connect( user, SIGNAL(scoreChanged(DJGameUser*)), userContextWidget, SLOT(createUserInfo(DJGameUser*)) );
	connect( user, SIGNAL(chipsChanged(DJGameUser*)), userContextWidget, SLOT(createUserInfo(DJGameUser*)) );
	connect( user, SIGNAL(winsChanged(DJGameUser*)), userContextWidget, SLOT(createUserInfo(DJGameUser*)) );
	connect( user, SIGNAL(losesChanged(DJGameUser*)), userContextWidget, SLOT(createUserInfo(DJGameUser*)) );
	connect( user, SIGNAL(drawsChanged(DJGameUser*)), userContextWidget, SLOT(createUserInfo(DJGameUser*)) );
	connect( user, SIGNAL(unwelcomeChanged(DJGameUser*)), userContextWidget, SLOT(createUserInfo(DJGameUser*)) );
}
void DJPanelController::createChatWidget( QWidget* parent )
{
	if ( m_chatWidget )
		return;
		
    m_chatWidget	= new DJGroupChat( gameController(), MakeGroupId( gameController()->gameId(), gameRoom()->roomId(), tableId() ), DJGroupChat::GamePanel, parent );
    connect( m_chatWidget->textBrowser(), SIGNAL(acceptPrivateView(quint32)), SLOT(handleAcceptPrivateView(quint32)));
}
void DJPanelController::receiveChat( const ChatContent *chatContent )
{
	quint32	speakerId	= letoh4( &chatContent->userid_speaker );
	if ( m_chatWidget )
		m_chatWidget->receiveChat( chatContent );
	
	DJGameUser *speaker	= gameController()->gameUser(speakerId);
	if ( speaker && !m_players.contains( speaker ) && !m_lookers.contains( speaker ) ) {
		lookerInOut( speaker, true );
	}
}
void DJPanelController::insertText2Editor( const QString& text )
{
	if ( m_chatWidget && !text.isEmpty() )
		m_chatWidget->insertText2Editor( text );
}
void DJPanelController::insertText2Browser( const QString& text )
{
	if ( m_chatWidget && !text.isEmpty() ) {
		m_chatWidget->insertText2Browser( text );
	}
}
void DJPanelController::insertGameResult2Browser( const QString& text )
{
	if ( m_chatWidget && !text.isEmpty() ) {
		QString tableText;
		tableText	+= "<table border>";
		DJGameRoom *room	= gameRoom();
		if ( room->options() & DJGAME_ROOMOPTION_COUNTER )
			tableText	+= QString("<tr><th>%1<th>%2")
						.arg(baseSectionName(UserName))
						.arg(baseSectionName(PanelChips));
		else
			tableText	+= QString("<tr><th>%1<th>%2")
						.arg(baseSectionName(UserName))
						.arg(baseSectionName(PanelScore));
		tableText	+= text;
		tableText	+= "</table>";
		m_chatWidget->insertText2Browser( tr("game result:") );
		m_chatWidget->insertText2Browser( tableText );
	}
}
void DJPanelController::deltaUserData( DJGameUser *user, int value )
{
	djDebug()<<"DJPanelController::deltaUserData";
	if ( 0 == user )
		return;
	djDebug()<<user->userName()<<value;
	DJPanelUserItem *item	= findUserItem( user );
	DJGameRoom *room	= gameRoom();
	if ( room->options() & DJGAME_ROOMOPTION_COUNTER  ) { 
     	user->deltaChips( value );
     	if ( item ) {
     		item->deltaPanelChips( value );
     	}
	}else {
		user->deltaScore( value );
		if ( value > 0 )
    		user->increaseWins();
    	else if ( value < 0 ) 
    		user->increaseLoses();
   	 	else
    		user->increaseDraws();
    	if ( item ) {
     		item->deltaPanelScore( value );
     	}	
    }
}
void DJPanelController::createBarWidget( QWidget* parent )
{
	int barHeight	= 64;
	int space	= 32;
	QSize	iconSize(48,48);
	
	DJWidget*	bar	= new DJWidget( parent );
	bar->setFixedHeight( barHeight );
	QHBoxLayout* layout	= new QHBoxLayout( bar );
	layout->setMargin( 0 );
	layout->setSpacing( space );
	layout->addSpacing( space );
	
	QGradientStops stops;
	stops << QGradientStop( 0, QColor(97,148,185) )
			<< QGradientStop( 1, QColor(75,130,170) );
	bar->setStops( stops );
	
	m_logo	= new DJToolButton;
	QPixmap	pix	= QPixmap( ":/HallRes/image/bar/logo.png" );
	m_logo->setIcon( pix );
	m_logo->setIconSize( pix.size() );
	connect(m_logo, SIGNAL(clicked()), SLOT(handleLogoClicked()) );
	layout->addWidget( m_logo );
	
	layout->addStretch();
	
	/*
	m_color	= new DJToolButton;
#ifdef DJ_TOOL_BUTTON_HAS_TEXT
	m_color->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
#endif
	m_color->setText(tr("color"));
	m_color->setToolTip(tr("set background color"));
	m_color->setIcon( QIcon(":/BaseRes/image/panel/color.png") );
	m_color->setIconSize( iconSize );
	connect(m_color, SIGNAL(clicked()),SLOT(handleColorClicked()));
	layout->addWidget( m_color );
	*/
	
	m_help	= new DJToolButton;
#ifdef DJ_TOOL_BUTTON_HAS_TEXT
	m_help->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
#endif
	m_help->setText(tr("help"));
	m_help->setToolTip(tr("help of this game"));
	m_help->setIcon( QIcon(":/BaseRes/image/panel/help.png") );
	m_help->setIconSize( iconSize );
	connect(m_help, SIGNAL(clicked()),SLOT(handleHelpClicked()));
	layout->addWidget( m_help );
	
	layout->addSpacing( space );
} 
DJDesktopController* DJPanelController::createDesktopController( const QSize& desktopSize, QWidget *parent )
{
	return new DJDesktopController( this, desktopSize, parent );
}
void DJPanelController::closeChildDialog()
{
	djDebug() << "DJPanelController::closeChildDialog";
	
	QObjectList list	= m_panel->children();
	foreach( QObject* obj, list ){
		if ( obj->inherits("QDialog") ) {
			djDebug() << "find QDialog";
			QDialog *dlg	= qobject_cast<QDialog *>(obj);
			if ( dlg ) {
				dlg->close();
			}
		}
 	}
}
void DJPanelController::removePendingAcl()
{
	djDebug() << "DJPanelController::removePendingAcl";
	djDebug() << "m_acls = " << m_acls;
	if ( !m_acls.isEmpty() )
		gameController()->removeAcl( m_acls );
}
void DJPanelController::showPanel()
{
	m_panel->showMaximized();
}
quint32 DJPanelController::selfUserId() const
{
	return gameController()->selfUserId();
}
DJGameController* DJPanelController::gameController() const
{
	return m_tableController->roomController()->gameController();
}
DJGameRoom* DJPanelController::gameRoom() const
{
	return gameController()->selfRoom();
}
quint8 DJPanelController::tableId() const
{
	return m_tableController->tableId();
}
int DJPanelController::numberOfSeats() const
{
	return m_tableController->roomController()->numberOfSeats();
}
DJGameUser* DJPanelController::player( quint8 seatId ) const
{
	return m_players.value( seatId, 0 );
}
bool DJPanelController::isLookingOn() const
{
	return !m_players.contains( gameController()->selfUser() );
}
bool DJPanelController::isPlayer( DJGameUser *user ) const
{
	return m_players.contains(user);
}
QByteArray DJPanelController::makeGameData( quint8 subType, const QByteArray& gameData, quint8 type )
{
	DJGameController *gc	= gameController();
	return MakeGameData( gc->gameId(), gc->language(), subType, gameData, type );
}
bool DJPanelController::sendRequest( const DJClientRequest& request, bool isDiscardable )
{
	void* acl	= (void*)request.aclProc();
	if ( acl && !m_acls.contains( acl ) ) {
		m_acls << acl;
	}
	return gameController()->sendRequest( request, isDiscardable );
}
void DJPanelController::userArrived( DJGameUser* user )
{
	if ( 0 == user )
		return;
	resetPlayers();
}
void DJPanelController::resetPlayers()
{
	QList<DJGameUser*>	oldPlayers	= m_players;
	
	removeAllPlayerItems();
	m_players	= m_tableController->allUsers();
	repaintAllPlayerItems();
	
	//remove player who is not in newplayers from oldplayers
	foreach ( DJGameUser *user, oldPlayers ) {
		if ( user && !m_players.contains( user ) ) {
			insertText2Browser(tr("%1 quit game").arg(user->userName()));
		}
	}
	//add player who is not in oldplayers from newplayers
	foreach ( DJGameUser* user, m_players ) {
		if ( m_desktopController )
			m_desktopController->reloadPlayer( user );
		if ( user && !oldPlayers.contains( user ) ) {
			insertText2Browser(tr("%1 joined in game").arg(user->userName()));
		}
	}
	if ( m_desktopController ) {
		m_desktopController->locateAll();
		m_desktopController->repaintAll();
	}
}
void DJPanelController::removeAllPlayerItems()
{
	foreach ( DJGameUser *player, m_players ) {
		removeUserItem( player );
	}
}
void DJPanelController::repaintAllPlayerItems()
{
	foreach ( DJGameUser *player, m_players ) {
		addUserItem( player, true );
	}
}
QString	DJPanelController::userItemNameSuffix( DJGameUser* user ) const
{
	djDebug()<<"DJPanelController::userItemNameSuffix";
	if ( m_desktopController )
		return m_desktopController->userItemNameSuffix( user );
	else
		return QString();
}
DJPanelUserItem* DJPanelController::createUserItem( DJGameUser *user, QTreeWidget* parent, int type )
{
	if ( user->userId() == selfUserId() )
		type	= QTreeWidgetItem::UserType;
	DJPanelUserItem *item	= new DJPanelUserItem( user, this, parent, type );
	connect( user, SIGNAL(chipsChanged(DJGameUser*)), item, SLOT(setChips()) );
	connect( user, SIGNAL(scoreChanged(DJGameUser*)), item, SLOT(setScore()) );
	return item;
}
DJPanelUserItem* DJPanelController::addUserItem( DJGameUser* user, bool isPlayer )
{
	djDebug() << "DJPanelController::addUserItem"<<isPlayer;
	if ( 0 == user ) {
		return 0;
	}
	DJPanelUserItem* item	= findUserItem( user );
	if ( 0 == item ) {
		item	= createUserItem( user, m_usersWidget );
		item->setPanelScore( 0 );
		item->setScore();
		item->setPanelChips( 0 );
		item->setChips();
	}
	if ( isPlayer ) {
		QString suffix	= userItemNameSuffix( user );
		item->setName( user->userName() + suffix );
		item->setNickName( user->nickName() );
		item->setSeat( user->seatId() );
	}else {
		item->setName( user->userName() + tr("(looker)") );
		item->setNickName( user->nickName() );
		item->setSeat( 0 );
	}
	return item;
}
void DJPanelController::removeUserItem( DJGameUser* user )
{
	if ( 0 == user )
		return;
	DJPanelUserItem* item	= findUserItem( user );
	delete item;
}
DJPanelUserItem* DJPanelController::findUserItem( DJGameUser* user )
{
	if ( 0 != m_usersWidget ) {
    	for ( int i = 0; i < m_usersWidget->topLevelItemCount(); i++ ) {
    		DJPanelUserItem *userItem	= static_cast<DJPanelUserItem *>(m_usersWidget->topLevelItem(i));
    		if ( userItem && user == userItem->user() ) {
    			return userItem;
    		}
    	}
	}
	return 0;
}
void DJPanelController::createUserWidgetHeader( QTreeWidget* treeWidget )
{
	userWidgetHeaderSections( m_sectionIds, m_sectionNames );
	
	QTreeWidgetItem* headerItem	= new QTreeWidgetItem;
    for ( int i = 0; i < m_sectionNames.size(); i++) {
    	headerItem->setTextAlignment( i, Qt::AlignCenter );
    	headerItem->setText( i, m_sectionNames.at(i) );
    }
	treeWidget->setHeaderItem( headerItem );
}
void DJPanelController::userWidgetHeaderSections( QList<int> &sectionIds, QStringList &sectionNames )
{
	DJGameRoom *room	= gameRoom();
	sectionIds.clear();
	sectionNames.clear();
	
	sectionIds << UserName << NickName;
	if ( room->options() & DJGAME_ROOMOPTION_COUNTER )
		sectionIds << PanelChips << Chips;
	else
		sectionIds << PanelScore << Score;
	sectionIds << Seat;
	for ( int i = 0; i < sectionIds.size(); ++i ) {
		sectionNames << baseSectionName( sectionIds.at(i) );
	}
}
int DJPanelController::indexOfSection( int sectionId ) const
{
	return m_sectionIds.indexOf( sectionId );
}
QString DJPanelController::sectionName( int sectionId ) const
{
	QString name;
	int index	 = indexOfSection( sectionId );
	if ( -1 != index )
		name	= m_sectionNames.at( index );
	return name;
}
QString	DJPanelController::baseSectionName( int sectionId )
{
	static const char *sections[] = {
       	QT_TR_NOOP("username"),
       	QT_TR_NOOP("nickname"),
        QT_TR_NOOP("seat"),
        QT_TR_NOOP("panel score"),
        QT_TR_NOOP("score"),
        QT_TR_NOOP("panel chips"),
        QT_TR_NOOP("chips"),
        QT_TR_NOOP("doorwind"),
        QT_TR_NOOP("ringwind")
	};
	if ( sectionId < LastSectionId )
		return tr(sections[sectionId]);
	else
		return tr("error");
}
void DJPanelController::lookerInOut( DJGameUser *looker, bool in )
{
	djDebug() << "DJPanelController::lookerInOut" << in;
	if ( 0 == looker )
		return;
	if ( in ) {
		//looker in
		if ( !m_lookers.contains(looker) )
			m_lookers << looker;
		addUserItem( looker, false );
	}else {
		//looker out
		int index	= m_lookers.indexOf( looker );
		if ( -1 != index ) {
			m_lookers.removeAt( index );
    	}
    	removeUserItem( looker );
	}
}
void DJPanelController::showUserContextMenu( DJGameUser *user )
{
	djDebug()<<"showUserContextMenu";
	if ( 0 == user )
		return;
	djDebug()<<"user name is"<<user->userName();
	
	bool bPlayer	= isPlayer( user );
	m_playerContextMenu->clear();
	int count	= 0;
	if ( isLookingOn() ) {
		if ( bPlayer ) {
			if ( gameRoom()->options() & DJGAME_ROOMOPTION_PRIVATE_VIEW ) {
				m_playerContextMenu->addAction( m_actRequestPrivateView );
				count++;
			}
		}
	}else {
		if ( !bPlayer ) {
			if ( gameRoom()->options() & DJGAME_ROOMOPTION_PRIVATE_VIEW ) {			
				m_playerContextMenu->addAction( m_actAcceptPrivateView );
				m_playerContextMenu->addAction( m_actClosePrivateView );
				count	+= 2;
			}
		}
	}
	if ( 0 == count )
		return;
	QAction *action	= m_playerContextMenu->exec( QCursor::pos() );
	if ( action ) {
		if ( m_actRequestPrivateView == action ) {
			requestPrivateView( user );
		}else if ( m_actAcceptPrivateView == action ) {
			acceptPrivateView( user );
		}else if ( m_actClosePrivateView == action ) {
			closePrivateView( user );
		}
	}
}
void DJPanelController::handleLogoClicked()
{
	LaunchURL( HTTP_WWW_HOST );
}
/*
void DJPanelController::handleColorClicked()
{
	QColor	color	= QColorDialog::getColor( QColor(75,130,170), m_panel );
	if ( color.isValid() ) {
		emit changeDesktopBrush( QBrush(color) );
	}
}
*/
void DJPanelController::handleHelpClicked()
{
	LaunchURL( WWW_HOST_HELP_URL );
}
void DJPanelController::gameInfo( const QByteArray& buf )
{
	djDebug()<<"DJPanelController::gameInfo";
	if ( m_desktopController ) {
		m_desktopController->gameInfo( buf );
		m_desktopController->clearAutoDisappearText();
		
		if ( gameRoom()->options() & DJGAME_ROOMOPTION_ANYPLAYER ) {
			//foreach ( DJGameUser* user, m_players ) {
			m_desktopController->locateAll();
			m_desktopController->repaintAll();
		}
	}
	//repaint player with gameinfo
	repaintAllPlayerItems();
}
void DJPanelController::gameWait( quint16 mask, quint8 status ,quint16 timeout )
{
	djDebug()<<"DJPanelController::gameWait";
	if ( m_desktopController ) {
		m_desktopController->gameWait( mask, status, timeout );
	}
}
void DJPanelController::gameTrace( const GeneralGameTrace2Head *gameTrace )
{
	djDebug()<<"DJPanelController::gameTrace";
	
	if ( m_desktopController ) {
		m_desktopController->gameTrace( gameTrace );
	}
}
void DJPanelController::magicResult( const MagicResult* magicResult )
{
	if ( m_desktopController ) {
		m_desktopController->magicResult( magicResult );
	}
}
void DJPanelController::gameOver( const DJGameOver* gameOver )
{
	djDebug()<<"DJPanelController::gameOver";
	
	if ( m_desktopController ) {
		m_desktopController->gameOver( gameOver );
	}
}
void DJPanelController::bonusCondition( const DJGameBonus* condition )
{	
	djDebug()<<"DJPanelController::bonusCondition";
	
	if ( m_desktopController ) {
		m_desktopController->bonusCondition( condition );
	}
}
void DJPanelController::bonusFulfilled( const DJGameBonusFulfill* fulfill )
{	
	djDebug()<<"DJPanelController::bonusFulfilled";
	
	if ( m_desktopController ) {
		m_desktopController->bonusFulfilled( fulfill );
	}
	quint32 userId	= letoh4( &fulfill->uid );
	DJGameUser *player	= gameController()->gameUser( userId );
	if ( player ) {
		QString	text	= player->userName();
		text	+= " ";
		text	+= tr("obtained bonus : ");
		quint16	bonus	= letoh2( fulfill->shBonus );
		switch( fulfill->chBonusType ) {
		case DJGAME_PROTOCOL_GAMEBONUS_TYPE_CARD:
			text	+= DJCard::name(bonus);
			break;
		case DJGAME_PROTOCOL_GAMEBONUS_TYPE_COUNTER:
			text	+= QString::number( bonus );
			text	+= " ";
			text	+= gameController()->baseSectionName(DJGameController::Chips);
			break;
		case DJGAME_PROTOCOL_GAMEBONUS_TYPE_MONEY:
			text	+= QString::number( bonus );
			text	+= " ";
			text	+= gameController()->baseSectionName(DJGameController::Money);
			break;
		default:
			text	+= tr("unknown bonus");
			break;
		}
		insertText2Browser( text );
	}
}
void DJPanelController::cardUsed( const ReqUseCard *useCard )
{	
	djDebug()<<"DJGamePanel::cardUsed";
	
	quint32 hostId	= letoh4( &useCard->hostid );
	quint32 targetId	= letoh4( &useCard->targetid );
	quint8 cardId	= useCard->chCard;
	qint32 parameter	= letoh4( useCard->iParam );
	
	DJGameUser *host	= gameController()->gameUser( hostId );
	DJGameUser *target	= gameController()->gameUser( targetId );
	if ( 0 == target || 0 == host )
		return;
	DJCard *card	= gameController()->card(cardId);
	if ( host && target && card ) {
		if ( hostId == targetId )
			insertText2Browser( tr("%1 used %2").arg(host->userName())
													.arg(DJCard::name(cardId)) );
		else
			insertText2Browser( tr("%1 used %2 to %3").arg(host->userName())
													.arg(DJCard::name(cardId))
													.arg(target->userName()) );
	}
	QString movie;
	if ( DJGAME_GAMECARD_BROADCAST == cardId ) {
		if ( hostId == selfUserId() ) {
			gameController()->dropMsgWidget()->show();
		}
	}else if ( DJGAME_GAMECARD_EMOTION == cardId ) {
		movie	= DJCard::emotionMovieName(parameter);
	}else {
		movie	= DJCard::movieName(cardId);
	}
	if ( movie.isEmpty() )
		return;
	djDebug()<<"movie"<<movie;
	quint8 targetSeatId	= target->seatId();
   	if ( 0 != targetSeatId && m_desktopController ) {
   		DJShowView	*show	= m_desktopController->playerShowView( targetSeatId );
   		show->addEffect( movie );
   	}
   	if ( targetSeatId == seatId() && m_showView ) {
    	m_showView->addEffect( movie );
    }
}
void DJPanelController::requestPrivateView( DJGameUser *host )
{
	quint32 hostId	= host->userId();
	quint8 hostSeatId	= host->seatId();
	//self is looker
	QByteArray	lookerIdStream	= htoa4( selfUserId(), QDataStream::LittleEndian );
	QByteArray	hostIdStream	= htoa4( hostId, QDataStream::LittleEndian );
	
	QByteArray	gameData;
	
	gameData.append( lookerIdStream );
	gameData.append( hostIdStream );
	gameData.append( tableId() );
	gameData.append( hostSeatId );
	
	QByteArray buf	= makeGameData( DJGAME_PROTOCOL_SUBTYPE_REQUEST_PRIVATE_VIEW, gameData );
	DJClientRequest request( buf );
	
	sendRequest( request );
}
void DJPanelController::acceptPrivateView( DJGameUser *looker )
{
	quint32 lookerId	= looker->userId();
	
	//self is host
	QByteArray	lookerIdStream	= htoa4( lookerId, QDataStream::LittleEndian );
	QByteArray	hostIdStream	= htoa4( selfUserId(), QDataStream::LittleEndian );
	
	QByteArray	gameData;
	
	gameData.append( lookerIdStream );
	gameData.append( hostIdStream );
	gameData.append( tableId() );
	gameData.append( seatId() );
	
	QByteArray buf	= makeGameData( DJGAME_PROTOCOL_SUBTYPE_AGREE_PRIVATE_VIEW, gameData );
	DJClientRequest request( buf );
	
	sendRequest( request );
}
void DJPanelController::closePrivateView( DJGameUser *looker )
{
	quint32 lookerId	= looker->userId();
	
	//self is host
	QByteArray	lookerIdStream	= htoa4( lookerId, QDataStream::LittleEndian );
	QByteArray	hostIdStream	= htoa4( selfUserId(), QDataStream::LittleEndian );
	
	QByteArray	gameData;
	
	gameData.append( lookerIdStream );
	gameData.append( hostIdStream );
	gameData.append( tableId() );
	gameData.append( seatId() );
	
	QByteArray buf	= makeGameData( DJGAME_PROTOCOL_SUBTYPE_CLOSE_PRIVATE_VIEW, gameData );
	DJClientRequest request( buf );
	
	sendRequest( request );
}
void DJPanelController::handleUsersWidgetContext( const QPoint& pos )
{
	djDebug()<<"DJPanelController::handleUsersWidgetContext";
	if ( 0 == m_usersWidget )
		return;
	DJPanelUserItem *userItem	= static_cast<DJPanelUserItem *>(m_usersWidget->itemAt( pos ));
	if ( userItem ) {
		showUserContextMenu( userItem->user() );
	}
}
void DJPanelController::handleUserItemDoubleClicked( QTreeWidgetItem * item, int column )
{
	DJPanelUserItem *userItem	= static_cast<DJPanelUserItem *>(item);
	if ( userItem ) {
		insertText2Editor( userItem->user()->userName() + "," );
	}
}
void DJPanelController::handleAcceptPrivateView( quint32 userId )
{
	foreach ( DJGameUser *looker, m_lookers ) {
		if ( userId == looker->userId() ) {
			acceptPrivateView( looker );
		}
	}
}
