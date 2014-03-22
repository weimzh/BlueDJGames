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

#ifndef DJCONTACT_H
#define DJCONTACT_H

#include <QtGui>
#include "protocol.h"

class DJHallController;
class DJGameUser;
class DJContactItem;
class DJChat;

class DJContact :public QTabWidget
{
	Q_OBJECT
public:
    DJContact( DJHallController* hallController, const QString& selfUserName, QWidget * parent = 0 );
    virtual ~DJContact();
	
	DJHallController* hallController() const { return m_hallController; }
	bool saveChat() const;
	bool popupChat() const;
	quint32	selfUserId() const { return m_selfUserId; }
	QString	selfUserName() const { return m_selfUserName; }
	bool isFriendOnline( quint32 userId ) const;
	void userOnline( quint16 gameId, quint32 userId );
	void userOffline( quint16 gameId, quint32 userId );
	int	userType( quint32 userId ) const;
	void insertGame( quint16 gameId );
	void removeGame( quint16 gameId );
	bool gameIsEmpty();
	bool containsGame( quint16 gameId );
	void receiveChat( const ChatContent *chatContent, quint8 extraInfo );
	void removeFromContact( quint32 userId, int contactUserType );
	void addPendingChat( DJContactItem* contactItem );
	void removePendingChat( DJContactItem* contactItem );
	void removePendingChat( quint32 userId );
	void chatWithUser( quint16 gameId, quint32 userId, const QString& userName );
	void sendChat( quint16 gameId, quint32 listenerId, const QString& contents );
public slots:
	void addToContact( quint16 gamId, quint32 userId, int contactUserType );
private slots:
	void friendDoubleClicked( QTreeWidgetItem *friendItem,int column );
	void contextMenuOfFriend ( const QPoint & );
	void contextMenuOfUnwelcome ( const QPoint & );
	void removeFriendItem();
	void removeUnwelcomeItem();
	void checkPendingChats();
private:
	void setChatEnabled( quint32 userId, bool isEnabled );
	
	DJHallController* 				m_hallController;
	QString							m_selfUserName;
	quint32							m_selfUserId;
	QTreeWidget*					m_friendsTree;
	QTreeWidget*					m_unwelcomesTree;
	QSet<quint16>					m_gameIds;
	QHash<quint32, DJContactItem*>	m_contactFriends;
	QHash<quint32, DJContactItem*>	m_contactUnwelcomes;
	QHash<quint32, DJChat*>			m_chats;
	QList<DJContactItem*>			m_pendingChats;
	//QList<quint32>					m_pendingChats;
	QTimer*							m_timer;
	bool							m_toggle;
	QMenu*							m_menuFriend;
	QMenu*							m_menuUnwelcome;
	QAction*						m_actRemoveFriend;
	QAction*						m_actRemoveUnwelcome;
};

class DJStranger : public QWidget
{
	Q_OBJECT
public:
    DJStranger( quint16 gameId, quint32 strangerId, const QString& strangerName, const QString& contents, QWidget * parent = 0, Qt::WFlags f = 0 );
    virtual ~DJStranger();

signals:
	void addStrangerToContact( quint16 gameId, quint32 strangerId, int contactUserType );
private slots:
	void addToFriend();
	void addToUnwelcome();
private:
	quint16		m_gameId;
	quint32		m_strangerId;
};
#endif
