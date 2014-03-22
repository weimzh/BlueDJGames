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

#ifndef DJHALLCONTROLLER_H
#define DJHALLCONTROLLER_H

#include <QtGui>
#include <QtNetwork>
#include "protocol.h"
#include "DJDownloadManager.h"

class DJGameRoomItem;
class DJGameServerItem;
class DJGameGameItem;
class DJGameClassItem;
class DJGameItem;
class DJSocket;
class DJContact;

class DJHallController: public QObject
{
	Q_OBJECT
public:
	
	DJHallController( QMainWindow* mainWindow, QObject *parent = 0 );
   ~DJHallController();
	
	bool saveChat() const;
	bool popupChat() const;
	
	void requestGameList( const QString& ip, quint16 port );
	void requestServerList( const QString& ip, quint16 port, quint16 gameId );
	void requestLogin( DJGameGameItem* gameItem, DJGameServerItem *serverItem );
	void requestRelogin( DJGameGameItem* gameItem, DJSocket* socket );
	void reloadSettings();
	void contactUserTypeChanged( quint32 userId, int contactUserType );
	
	void refreshGameItem( DJGameGameItem* gameItem );
	void addGameItem( quint16 gameId, const QString& gameName );
	void addServerItem( quint16 gameId, quint16 serverId, const QString& ip, quint16 port, const QString& serverName, int numberOfUsers );
	void changeServerUsers( quint16 gameId, quint16 serverId, int numberOfUsers );
	void receiveChat( const ChatContent *chatContent, quint8 extraInfo );
	void sendChat( quint32 speakerId, quint32 listenerId, quint16 gameId, const QString& contents );
	
	void locateTreeItem( uint gameClassId, uint gameId, uint serverId,
	DJGameClassItem * &gameClassItem, DJGameGameItem * &gameItem, DJGameServerItem * &serverItem );
	
	QMainWindow*	mainWindow() const { return m_mainWindow; }
	QTreeWidget*	gameTree() const { return m_gameTree; }
	QStackedWidget*	treeContainer() const { return m_treeContainer; }
	QTabWidget*	tabContainer() const { return m_tabContainer; }
	DJContact*	contact( QString selfUserName );
	void insertContact( QString selfUserName, DJContact* contact );
	void removeContact( QString selfUserName );
	QVariant gameUserInfo( quint16 gameId, quint32 userId );
	
	static quint32 version();
	//void findLoginNamePass( QString & userName, QString & password );
	
	QString	loginName() const { return m_loginName; }
	QString	loginPassword() const { return m_loginPassword; }
	void clearLogin();
	bool isBanned() const { return m_isBanned; }
	void setBanned();
public slots:	
	void clearBanned();
	void handleQuitHall();
private slots:
	void launchAppFinished();
	void itemDoubleClicked( QTreeWidgetItem * item, int column );
	void gameContext( int index );
	void gameTreeContext( const QPoint& );
	void refreshGameList();
	void refreshServerList();
	void queryGameVersion();
	void downloadFinished( QFile* file, const QString& localFile, int fileType, quint16 id, const QString& name, int lastStatusCode );
	void downloadIni();
	void downloadResource();
	void downloadLogin();
	void loginDone( bool error );
//public slots:
	//void debugGameSocket() const;
	void setup();
	void home();
	void about();
	void tabChanged(int index);
private:
	void createHallBar( QWidget* parent );
	//void downloadHallUpdator();
	void loadBaseLibrary();
	void extractFaceIcon();
	bool iniExpired();
	void getGameList();
	void getServerList( DJGameGameItem* gameItem );
	void downloadExecutable( DJDownloadManager::FileType type, quint16 id, const QString& section, const QString& name );

	QMainWindow*		m_mainWindow;
	QSplitter*			m_gameSplitter;
	QTreeWidget*		m_gameTree;
	QStackedWidget*		m_treeContainer;
	QTabWidget*			m_tabContainer;
	QList<DJSocket *>	m_gameSockets;
	DJSocket*			m_hallSocket;
	QMenu*				m_gameContextMenu;
	QAction*			m_actQuitHall;
	QAction*			m_actRefreshGame;
	QAction				*m_actRefreshServer,*m_actQueryGameVersion;
	QAction*			m_actDownloadIni;
	QHash<QString,DJContact*>	m_contacts;
	quint8				m_language;
	bool				m_saveChat;
	bool				m_popupChat;
	DJDownloadManager*	m_downloadManger;
	QTimer*				m_timer;
	QHash<QString,QString>	m_resources;
	
	quint32				m_baseVersion;
	quint32				m_whbVersion;
	
	QString				m_loginName,m_loginPassword;
	bool				m_isBanned;
	
	QTemporaryFile		*m_httpFile;
	QHttp				*m_http;
};
#endif

