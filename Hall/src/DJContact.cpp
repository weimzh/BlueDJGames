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

#include "DJContact.h"
#include "DJHallController.h"
#include "DJContactItem.h"
#include "Utility.h"
#include "DJChat.h"
#include "DJGlobal.h"

static const char*	CHAT_DIR			= "chat";

static const char*	KEY_CONTACT_FRIEND	= "friend";
static const char*	KEY_CONTACT_UNWELCOME	= "blacklist";

DJContact::DJContact( DJHallController* hallController, const QString& selfUserName, QWidget * parent )
	:QTabWidget( parent ),m_hallController(hallController),m_selfUserName(selfUserName)
{
	djDebug() << "DJContact constructor" << selfUserName;
	m_selfUserId	= 0;
	
    m_friendsTree	= new QTreeWidget;
    PaintBackground( m_friendsTree, QPoint(100,0), QPoint(0,100) );
    m_friendsTree->setHeaderLabels( QStringList( selfUserName + tr("'s") + tr("friend list") ) );
    m_friendsTree->setRootIsDecorated( false );
    m_friendsTree->setSortingEnabled( true );
    m_friendsTree->setContextMenuPolicy( Qt::CustomContextMenu );
	connect(m_friendsTree, SIGNAL(customContextMenuRequested ( const QPoint & )), SLOT( contextMenuOfFriend ( const QPoint & ) ) );
	connect(m_friendsTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)), SLOT( friendDoubleClicked(QTreeWidgetItem *,int)));
	m_actRemoveFriend	= new QAction( tr("remove"), m_friendsTree );
	connect(m_actRemoveFriend, SIGNAL(triggered()),SLOT(removeFriendItem()));
	m_menuFriend	= new QMenu( m_friendsTree );
	m_menuFriend->addAction( m_actRemoveFriend );
    
    m_unwelcomesTree	= new QTreeWidget;
    PaintBackground( m_unwelcomesTree, QPoint(100,0), QPoint(0,100) );
    m_unwelcomesTree->setHeaderLabels( QStringList( selfUserName + tr("'s") + tr("blacklist") ) );
    m_unwelcomesTree->setRootIsDecorated( false );
    m_unwelcomesTree->setContextMenuPolicy( Qt::CustomContextMenu );
	connect(m_unwelcomesTree, SIGNAL(customContextMenuRequested ( const QPoint & )), SLOT( contextMenuOfUnwelcome ( const QPoint & ) ) );
	m_actRemoveUnwelcome	= new QAction( tr("remove"), m_unwelcomesTree );
	connect(m_actRemoveUnwelcome, SIGNAL(triggered()),SLOT(removeUnwelcomeItem()));
	m_menuUnwelcome	= new QMenu( m_unwelcomesTree );
	m_menuUnwelcome->addAction( m_actRemoveUnwelcome );
	
    setTabPosition( QTabWidget::North );
    addTab( m_friendsTree, tr("friend") );
    addTab( m_unwelcomesTree, tr("blacklist") );
	
	m_timer	= new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(checkPendingChats()));
	m_timer->start(1000);
	
	m_toggle = true;
}
DJContact::~DJContact()
{
	djDebug() << "DJContact destructor";
}
bool DJContact::saveChat() const
{
	return m_hallController->saveChat();
}
bool DJContact::popupChat() const
{
	return m_hallController->popupChat();
}
bool DJContact::isFriendOnline( quint32 userId ) const
{
	if ( m_contactFriends.contains( userId ) ) {
		DJContactItem*	contactItem	= m_contactFriends.value( userId );
		return contactItem->isOnline();
	}
	return false;
}
void DJContact::userOnline( quint16 gameId, quint32 userId )
{
	djDebug() << "DJContact::userOnline"<<"gameid="<<gameId<<"userid="<<userId;
	
	if ( m_contactFriends.isEmpty() ) {
		djDebug() << "read contact file";
		//it's self user
		m_selfUserId	= userId;
		
		QString	selfIdKey	= QString::number(m_selfUserId);
        QString	contactFile	= QString(CHAT_DIR) + "/" + selfIdKey + "/" + "contact";
        QSettings contactSettings( contactFile, QSettings::IniFormat );
        
        //get friend
        contactSettings.beginGroup( KEY_CONTACT_FRIEND );
        if ( !contactSettings.contains( selfIdKey ) ) {
        	djDebug() << "contact is empty";
        	contactSettings.setValue( selfIdKey, m_selfUserName );
        }
        QStringList	friendKeys	= contactSettings.childKeys();
        for ( int i = 0; i < friendKeys.size(); i++ ) {
        	QString	strId	= friendKeys.at(i);
        	quint32 id	= strId.toULong();
        	QString	name	= contactSettings.value( strId ).toString();
        	DJContactItem *contactItem	=  new DJContactItem( this, id, name, m_friendsTree );
        	m_contactFriends.insert( id, contactItem );
        }
        contactSettings.endGroup();
        
        //get unwelcome
        contactSettings.beginGroup( KEY_CONTACT_UNWELCOME );
        QStringList	blacklistKeys	= contactSettings.childKeys();
        for ( int i = 0; i < blacklistKeys.size(); i++ ) {
        	QString	strId	= blacklistKeys.at(i);
        	quint32 id	= strId.toULong();
        	QString	name	= contactSettings.value( strId ).toString();
        	DJContactItem *contactItem	=  new DJContactItem( this, id, name, m_unwelcomesTree );
        	m_contactUnwelcomes.insert( id, contactItem );
        }
        contactSettings.endGroup();
	}
	if( !m_gameIds.contains( gameId ) )
		m_gameIds.insert( gameId );
	if ( m_contactFriends.contains( userId ) ) {
		DJContactItem*	contactItem	= m_contactFriends.value( userId );
		QVariant userInfo	= m_hallController->gameUserInfo( gameId, userId );
		QIcon	userIcon;
		if ( userInfo.isValid() ) {
			QList<QVariant> info	= userInfo.toList();
			userIcon	= info.at(1).value<QIcon>();
		}
		contactItem->userOnline( gameId, userIcon );
		m_friendsTree->sortItems( 0, m_friendsTree->header()->sortIndicatorOrder() );
	}
	setChatEnabled( userId, true );
}
void DJContact::userOffline( quint16 gameId, quint32 userId )
{	
	djDebug() << "DJContact::userOffline" << userId;
	if ( m_contactFriends.contains( userId ) ) {
		DJContactItem*	contactItem	= m_contactFriends.value( userId );
		contactItem->userOffline( gameId );
		m_friendsTree->sortItems( 0, m_friendsTree->header()->sortIndicatorOrder() );
		if ( !contactItem->isOnline() )
			setChatEnabled( userId, false );
	}
}
int DJContact::userType( quint32 userId ) const
{
	djDebug() << "DJContact::userType";
	if ( m_contactFriends.contains(userId) )
		return DJ_CONTACT_USER_TYPE_FRIEND;
	else if ( m_contactUnwelcomes.contains(userId) )
		return DJ_CONTACT_USER_TYPE_UNWELCOME;
	else
		return DJ_CONTACT_USER_TYPE_NONE;
}
void DJContact::addToContact( quint16 gameId, quint32 userId, int contactUserType )
{
	djDebug() << "DJContact::addToContact";
	if ( 0 == m_selfUserId )
		return;
	QVariant userInfo	= m_hallController->gameUserInfo( gameId, userId );
	if ( !userInfo.isValid() ) 
		return;
	QList<QVariant> info	= userInfo.toList();
	
	QString	userName	= info.at(0).toString();
	
	QString	selfIdKey	= QString::number(m_selfUserId);
	QString	contactFile	= QString(CHAT_DIR) + "/" + selfIdKey + "/" + "contact";
    QSettings contactSettings( contactFile, QSettings::IniFormat );
    
	switch( contactUserType ) {
	case DJ_CONTACT_USER_TYPE_FRIEND:
		if ( !m_contactFriends.contains( userId ) ) {
			//remove from unwelcome if there
			removeFromContact( userId, DJ_CONTACT_USER_TYPE_UNWELCOME );
			
			contactSettings.beginGroup( KEY_CONTACT_FRIEND );
			contactSettings.setValue( QString::number(userId), userName );
			contactSettings.endGroup();
			DJContactItem *contactItem	=  new DJContactItem( this, userId, userName, m_friendsTree );
        	m_contactFriends.insert( userId, contactItem );
        	userOnline( gameId, userId );
        	
        	m_hallController->contactUserTypeChanged( userId, DJ_CONTACT_USER_TYPE_FRIEND );
    	}
		break;
	case DJ_CONTACT_USER_TYPE_UNWELCOME:
		if ( !m_contactUnwelcomes.contains( userId ) ) {
			//remove from friend if there
			removeFromContact( userId, DJ_CONTACT_USER_TYPE_FRIEND );
			
			contactSettings.beginGroup( KEY_CONTACT_UNWELCOME );
			contactSettings.setValue( QString::number(userId), userName );
			contactSettings.endGroup();
			DJContactItem *contactItem	=  new DJContactItem( this, userId, userName, m_unwelcomesTree );
        	m_contactUnwelcomes.insert( userId, contactItem );
        	
        	m_hallController->contactUserTypeChanged( userId, DJ_CONTACT_USER_TYPE_UNWELCOME );
    	}
		break;
	default:
		break;
	}
}
void DJContact::removeFromContact( quint32 userId, int contactUserType )
{
	if ( 0 == m_selfUserId )
		return;
	
	QString	selfIdKey	= QString::number(m_selfUserId);
    QString	contactFile	= QString(CHAT_DIR) + "/" + selfIdKey + "/" + "contact";
    QSettings contactSettings( contactFile, QSettings::IniFormat );
    
	switch( contactUserType ) {
	case DJ_CONTACT_USER_TYPE_FRIEND:
		if ( m_contactFriends.contains( userId ) ) {
			DJContactItem *contactItem	= m_contactFriends.take( userId );
			delete contactItem;
			
			contactSettings.beginGroup( KEY_CONTACT_FRIEND );
			contactSettings.remove( QString::number(userId) );
			contactSettings.endGroup();
    	}
		break;
	case DJ_CONTACT_USER_TYPE_UNWELCOME:
		if ( m_contactUnwelcomes.contains( userId ) ) {
			DJContactItem *contactItem	= m_contactUnwelcomes.take( userId );
			delete contactItem;
			
			contactSettings.beginGroup( KEY_CONTACT_UNWELCOME );
			contactSettings.remove( QString::number(userId) );
			contactSettings.endGroup();
    	}
		break;
	default:
		break;
	}
	m_hallController->contactUserTypeChanged( userId, DJ_CONTACT_USER_TYPE_NONE );
}
void DJContact::addPendingChat( DJContactItem* contactItem )
{
	m_pendingChats.append( contactItem );
}
void DJContact::removePendingChat( DJContactItem* contactItem )
{
	contactItem->clearBlink();
	int index	= m_pendingChats.indexOf( contactItem );
	if ( -1 != index ) {
		m_pendingChats.removeAt( index );
	}	
}
void DJContact::removePendingChat( quint32 userId )
{
	DJContactItem* contactItem	= m_contactFriends.value( userId, 0 );
	if ( contactItem ) {
		removePendingChat( contactItem );
	}
}
void DJContact::checkPendingChats()
{
	int index	= indexOf(m_friendsTree);
	if ( m_pendingChats.isEmpty() ) {
		setTabIcon( index, QIcon() );
	}else {
		//set tab blink
		DJContactItem* contactItem	= m_pendingChats.first();
		QIcon icon	= contactItem->userIcon();
		if ( m_toggle )
			setTabIcon( index, icon );
		else
			setTabIcon( index, icon.pixmap( 16, QIcon::Disabled ) );
		//set contact item blink
		foreach( DJContactItem *contactItem, m_pendingChats ) {
			contactItem->setBlink( m_toggle );
		}
		m_toggle	= !m_toggle;
	}
}
void DJContact::insertGame( quint16 gameId )
{
	djDebug() << "DJContact::insertGame";
	m_gameIds.insert( gameId );
}
void DJContact::removeGame( quint16 gameId )
{
	djDebug() << "DJContact::gameOffline";
	m_gameIds.remove( gameId );
	QSet<quint32> userIds;
	for ( int i = 0; i < m_friendsTree->topLevelItemCount(); i++ ) {
		DJContactItem *userItem	= static_cast<DJContactItem *>(m_friendsTree->topLevelItem(i));
		userItem->userOffline( gameId );
		if ( !userItem->isOnline() ) {
			quint32 userId	= userItem->userId();
			if ( !userIds.contains( userId ) ) {
				userIds.insert( userId );
				setChatEnabled( userId, false );
			}
		}
	}
	m_friendsTree->sortItems( 0, m_friendsTree->header()->sortIndicatorOrder() );
}
bool DJContact::gameIsEmpty()
{
	return m_gameIds.isEmpty();
}
bool DJContact::containsGame( quint16 gameId )
{
	return m_gameIds.contains( gameId );
}
//gameId = this game id when admin chat, otherwise gameId = 0
void DJContact::chatWithUser( quint16 gameId, quint32 userId, const QString& userName )
{
	DJChat *chat	= m_chats.value( userId, 0 );
	if ( 0 == chat ) {
		chat	= new DJChat( selfUserId(), selfUserName(), userId, userName, this, this );
		m_chats.insert( userId, chat );
	}
	bool isEnabled	= isFriendOnline( userId );
	if ( 0 != gameId ) {
		//it is admin chat
		isEnabled	= true;
	}
	//set gameid because maybe admin is friend
	chat->setGameId( gameId );
	chat->setChatEnabled( isEnabled );
	chat->showChatWidget();
}
void DJContact::friendDoubleClicked( QTreeWidgetItem *friendItem,int column )
{
	Q_UNUSED(column);
	djDebug() << "friendDoubleClicked";
	DJContactItem*	contactItem	= static_cast<DJContactItem *>(friendItem);
	quint32 userId	= contactItem->userId();
	QString userName	= contactItem->userName();
	chatWithUser( 0, userId, userName );
}
void DJContact::receiveChat( const ChatContent *chatContent, quint8 extraInfo )
{
	djDebug() << "DJContact::receiveChat"<<extraInfo;
	quint32	speakerId	= letoh4( &chatContent->userid_speaker );
	quint32	listenerId	= letoh4( &chatContent->userid_listener );
	quint16 gameId	= makeint16(chatContent->chGameClass, chatContent->chGame);
	QString sentence	= QString::fromUtf8(chatContent->szSentence);
	if ( sentence.isEmpty() )
		return;
	
	quint32 otherUserId;
	if ( m_selfUserId == speakerId ) {
		otherUserId	= listenerId;
	}else
		otherUserId	= speakerId;
	
	if ( 1 == extraInfo ) {
		//speaker is admin
		//it's admi chat, you must accept admin's chat
		QString otherUserName;
		DJContactItem *contactItem	= m_contactFriends.value( otherUserId, 0 );
		if ( contactItem ) {
			otherUserName	+= contactItem->userName();
			//gameId	= 0;
		}else {
			otherUserName	+= QString::number(speakerId);
		}
			
		DJChat *chat	= m_chats.value( otherUserId, 0 );
		if ( 0 == chat ) {
			chat	= new DJChat( m_selfUserId, m_selfUserName, otherUserId, otherUserName, this, this );
			m_chats.insert( otherUserId, chat );
		}
		//set gameid because maybe admin is friend
		chat->setGameId( gameId );
		chat->setChatEnabled( true );
		chat->receiveChat( speakerId, sentence );
		chat->showChatWidget();
		return;
	}
	
	//ignore unwelcome chat
	if ( m_contactUnwelcomes.contains( otherUserId ) )
		return;

	//check if otherUserId is in friend list
	if ( m_contactFriends.contains( otherUserId ) ){
		djDebug() << "is friend";
		DJContactItem *contactItem	= m_contactFriends.value( otherUserId );
		DJChat *chat	= m_chats.value( otherUserId, 0 );
		if ( 0 == chat ) {
			chat	= new DJChat( selfUserId(), selfUserName(), otherUserId, contactItem->userName(), this, this );
			m_chats.insert( otherUserId, chat );
		}
		bool isEnabled	= contactItem->isOnline();
		chat->setChatEnabled( isEnabled );
		chat->receiveChat( speakerId, sentence );
		if ( !popupChat() && !chat->isVisible() ) {
			addPendingChat( contactItem );
		}else {
			chat->showChatWidget();
		}
	}else {
		djDebug() << "not friend";
		DJChat *chat	= m_chats.value( otherUserId, 0 );
		if ( chat ) {
			//admin talked to you
			djDebug()<<"admin chat";
			chat->setChatEnabled( true );
			chat->receiveChat( speakerId, sentence );
			chat->showChatWidget();
		}else {
			djDebug()<<"stranger chat";
    		QVariant userInfo	= m_hallController->gameUserInfo( gameId, otherUserId );
			if ( userInfo.isValid() )  {
				djDebug()<<"popup stranger widget";
    			
				QList<QVariant> info	= userInfo.toList();
    			QString otherUserName	= info.at(0).toString();
    			
    			DJStranger* stranger	= new DJStranger( gameId, otherUserId, otherUserName, sentence, this, Qt::Window );
    			connect(stranger, SIGNAL(addStrangerToContact(quint16, quint32, int)),SLOT(addToContact(quint16, quint32, int)));
    			stranger->setAttribute( Qt::WA_DeleteOnClose );
    			stranger->show();
    		}
		}
	}
}
void DJContact::sendChat( quint16 gameId, quint32 listenerId, const QString& contents )
{
	djDebug()<< "DJContact::sendChat"<<gameId<<listenerId<<contents;
	if ( 0 == gameId ) {
		//normal chat
		if ( !m_contactFriends.contains( listenerId ) ) 
			return;
		
		DJContactItem *contactItem	= m_contactFriends.value( listenerId );			
    	gameId	= contactItem->firstGameId();
    	if ( 0 == gameId )
    		return;
    }
    djDebug()<<"gameId="<<gameId<<"listenerId"<<listenerId;
    m_hallController->sendChat( m_selfUserId, listenerId, gameId, contents );
}
void DJContact::removeFriendItem()
{
	quint32 userId	= m_actRemoveFriend->data().toUInt();
	removeFromContact( userId, DJ_CONTACT_USER_TYPE_FRIEND );
}
void DJContact::removeUnwelcomeItem()
{
	quint32 userId	= m_actRemoveUnwelcome->data().toUInt();
	removeFromContact( userId, DJ_CONTACT_USER_TYPE_UNWELCOME );
}

void DJContact::contextMenuOfFriend ( const QPoint &point )
{
	DJContactItem *contactItem	= static_cast<DJContactItem *>(m_friendsTree->itemAt( point ));
	if ( contactItem ) {
		quint32 userId	= contactItem->userId();
		m_actRemoveFriend->setData( userId );
		m_menuFriend->exec( m_friendsTree->mapToGlobal( point ) );
	}
}
void DJContact::contextMenuOfUnwelcome ( const QPoint &point )
{
	DJContactItem *contactItem	= static_cast<DJContactItem *>(m_unwelcomesTree->itemAt( point ));
	if ( contactItem ) {
		quint32 userId	= contactItem->userId();
		m_actRemoveUnwelcome->setData( userId );
		m_menuUnwelcome->exec( m_unwelcomesTree->mapToGlobal( point ) );
	}
}
void DJContact::setChatEnabled( quint32 userId, bool isEnabled )
{
	DJChat *chat	= m_chats.value( userId, 0 );
	if ( chat ) {
		chat->setChatEnabled( isEnabled );
	}
}
//----------------------------------------------------------------------------------
DJStranger::DJStranger( quint16 gameId, quint32 strangerId, const QString& strangerName, const QString& contents, QWidget * parent, Qt::WFlags f )
	:QWidget( parent, f ),m_gameId(gameId),m_strangerId(strangerId)
{
	QVBoxLayout* mainLayout	= new QVBoxLayout( this );
	
	setWindowTitle(tr("message from a stranger"));
	
	QString text	= strangerName;
	text.append(tr(" want to talk with you."));
	text.append("\n");
	text.append( tr("he/she said: ") + contents );
	text.append("\n");
	text.append(tr("you're going to..."));
	QLabel* label	= new QLabel( text );
    mainLayout->addWidget( label );
    
	QHBoxLayout* buttonLayout	= new QHBoxLayout;
    mainLayout->addLayout( buttonLayout );
    
    QPushButton* b;
    b	= new QPushButton( tr("add to friendlist") );
    connect(b, SIGNAL(clicked()),SLOT(addToFriend()));
    buttonLayout->addWidget( b );
    
    b	= new QPushButton( tr("add to blacklist") );
    connect(b, SIGNAL(clicked()),SLOT(addToUnwelcome()));
    buttonLayout->addWidget( b );
    
    b	= new QPushButton( tr("ignore") );
    connect(b, SIGNAL(clicked()),SLOT(close()));
    buttonLayout->addWidget( b );
    
    QTimer::singleShot(30000, this, SLOT(close()));
}
DJStranger::~DJStranger()
{
}
void DJStranger::addToFriend()
{
	djDebug() << "DJStranger::addToFriend";
	emit addStrangerToContact( m_gameId, m_strangerId, DJ_CONTACT_USER_TYPE_FRIEND );
	close();
}
void DJStranger::addToUnwelcome()
{
	emit addStrangerToContact( m_gameId, m_strangerId, DJ_CONTACT_USER_TYPE_UNWELCOME );
	close();
}
