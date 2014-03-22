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

#include "DJSocket.h"
#include "DJHallController.h"
#include "DJGameItem.h"
#include "DJProgressDialog.h"
#include "DJContact.h"
#include "DJHallInterface.h"
#include "HallUtility.h"
#include "GameInterface.h"
#include "protocol.h"
#include "Utility.h"
#include "DJMessageBox.h"
#include "DJBaseGlobal.h"

static const int KeepOnLine	= 60000;//60 sec
static const int DJGameProtocolLength	= 5;
static const char SpeedPackage[DJGameProtocolLength] = {0x05,DJGAME_PROTOCOL_TYPE_SPEED,0,0,0};
static const char KeepOnLinePackage[DJGameProtocolLength] = {0x05,DJGAME_PROTOCOL_TYPE_ACL,0,0,0};

static	QString	hallResDir	= ":/HallRes";

bool socks4ConnectCallback( const QByteArray& buf, DJSocket* socket )
{
	Q_UNUSED(socket);
	djDebug() << "socks4ConnectCallback";
	
    if ( buf.size() >= 8 ) {
        const Socks4Reply *reply = reinterpret_cast<const Socks4Reply *>(buf.data());
        if( 0 != reply->vn || 90 != reply->cd ) {
        	QMessageBox::information( 0,DJSocket::tr("network error"),DJSocket::tr("socks4 proxy connect failed!"),QMessageBox::Ok);
        	return false;
        }
	}
	return true;
}
bool socks5VersionCallback( const QByteArray& buf, DJSocket* socket )
{
	djDebug() << "socks5VersionCallback";

    if ( buf.size() == 2 ) {
    	djDebug() << "===";
    	const Socks5VersionReply *reply= reinterpret_cast<const Socks5VersionReply *>(buf.data()); 
    	if( 5 != reply->ver || ( 0 != reply->method && 2 != reply->method) ) { 
    		QMessageBox::information( 0,DJSocket::tr("network error"),DJSocket::tr("socks5 proxy version error!"),QMessageBox::Ok);
            return false;
        }else {
            djDebug() << "version ok" << reply->method;
            if ( 0 == reply->method ) {
            	//no auth need
            	djDebug() << "no need auth";
            	socket->socks5RequestConnect();
            }else {
            	//need auth
            	djDebug() << "need auth";
            	QString	username,password;
				GetSettingProxyAuth( username,password ); 
            	socket->socks5RequestAuth( username, password );
            }
        }
        return true;
    }else {
    	QMessageBox::information( 0,DJSocket::tr("network error"),DJSocket::tr("socks5 proxy version length error!"),QMessageBox::Ok);
    	return false;
    }
}
bool socks5AuthCallback( const QByteArray& buf, DJSocket* socket )
{
	djDebug() << "socks5AuthCallback";

    if ( buf.size() == 2 ) {
    	const Socks5AuthReply *reply= reinterpret_cast<const Socks5AuthReply *>(buf.data()); 
    	if( 1 != reply->ver || 0 != reply->status ) {
    		djDebug() << "auth fail";
    		QMessageBox::information( 0,DJSocket::tr("network error"),DJSocket::tr("socks5 proxy auth failed!"),QMessageBox::Ok);
            return false;
        }else {
        	socket->socks5RequestConnect();
        }
        return true;
    }else {
    	return false;
    }
}
bool socks5ConnectCallback( const QByteArray& buf, DJSocket* socket )
{
	Q_UNUSED(socket);
	djDebug() << "socks5ConnectCallback";
	
    if ( buf.size() >= 10 ) {
    	const Socks5ConnectReply *reply= reinterpret_cast<const Socks5ConnectReply *>(buf.data()); 
    	if( 5 != reply->ver || 0 != reply->rep ) {
    		QMessageBox::information( 0,DJSocket::tr("network error"),DJSocket::tr("socks5 proxy connect failed!"),QMessageBox::Ok);
            return false;
        }
    }
    return true;
}
bool httpConnectCallback( const QByteArray& buf, DJSocket* socket )
{
	djDebug() << "httpConnectCallback";
	djDebug() << socket->peerAddress().toString() << socket->peerPort();
	
    QString	string( buf );	
	djDebug() << string;
	//if ( buf.contains("HTTP/1.0 200 Connection established") )
	if ( buf.contains("200 Connection established") )
		return true;
	else {
		QMessageBox::information( 0,DJSocket::tr("network error"),DJSocket::tr("http proxy connect failed!"),QMessageBox::Ok);
		return false;
	}
}
DJSocket::DJSocket( Type type, DJHallController *hallController, QObject *parent )
	:QTcpSocket(parent),m_socketType(type),m_hallController(hallController)
{
	djDebug() << "DJSocket constructor" << m_socketType;
    
    m_lastRequestId	= 0;
	m_port	= 0;
	
	m_progressDlg	= new DJProgressDialog( this, m_hallController->mainWindow() );
	//m_progressDlg	= new DJProgressDialog( this );
	
    connect(this, SIGNAL( hostFound() ), SLOT( socketHostFound()) );
    connect(this, SIGNAL( connected() ), SLOT( socketConnected()) );
    connect(this, SIGNAL( disconnected() ), SLOT( socketDisconnected()) );
    connect(this, SIGNAL( error(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));
	connect(this, SIGNAL( readyRead()), SLOT(socketReadyRead()) );
	connect(this, SIGNAL( bytesWritten(qint64)), SLOT(socketBytesWritten(qint64)));
	
	startTimer(KeepOnLine);
}
DJSocket::~DJSocket()
{
	djDebug() << "DJSocket destructor";
	clear();
	//delete m_progressDlg;
	//m_progressDlg	= 0;
}
void DJSocket::description() const
{
	djDebug() << "DJSocket::description";
}
void DJSocket::allGamesReloadSettings()
{
	allGamesCommand( GAME_COMMAND_RELOAD_SETTING, QVariant() );
}
void DJSocket::allGamesContactUserTypeChanged( quint32 userId, int contactUserType )
{
	QList<QVariant>	list;
	list << QVariant(userId) << QVariant(contactUserType);
	QVariant	parameters(list);
	
	allGamesCommand( GAME_COMMAND_CONTACT_USER_TYPE_CHANGED, parameters );
}
void DJSocket::allGamesReconnect()
{
	djDebug() << "allGamesReconnect"<<state();
	//if ( QAbstractSocket::UnconnectedState != state() ) {
		//abort();
	//}
	//djDebug() << "state is unconnected"<<m_ip<<m_port;
	//if ( m_ip.isEmpty() || 0 == m_port )
		//return;
	/*	
	if ( m_gameItems.isEmpty() ) 
		return;
	DJGameGameItem	*gameItem	= m_gameItems.constBegin().value();
	if ( !gameItem )
		return;
	if ( 0 == gameItem->curServerId() )//game has been closed
		return;
	//DJGameServerItem	*serverItem	= (DJGameServerItem*)gameItem->childItem( gameItem->curServerId() );
	//if ( !serverItem )
		//return;
	m_hallController->requestRelogin( gameItem, this, m_ip, m_port );
	//allGamesCommand( GAME_COMMAND_RECONNECTING, QVariant() );
	*/
	foreach( DJGameGameItem *gameItem, m_gameItems ) {
		if ( 0 == gameItem->curServerId() )//game has been closed
			continue;
		QVariant state	= gameItem->gameControllerCommand( GAME_COMMAND_STATE, QVariant() );
		if ( state.isValid() ) {
			bool isConnected	= state.toBool();
			djDebug()<<"isConnected"<<isConnected;
			if ( !isConnected ) {
				m_hallController->requestRelogin( gameItem, this );
			}
		}
	}
}
void DJSocket::connectToServer( const QString& ip, quint16 port, bool showProgress, const QString& text, int seconds )
{
	djDebug() << "connectToServer";
	m_ip	= ip;
	m_port	= port;
	
	QString	targetIp	= ip;
	quint16	targetPort	= port;
	m_proxyType	= -1;
	
	do {
		if ( !GetSettingUseProxy() )
			break;
		m_proxyType	= GetSettingProxyType();
		if ( -1 == m_proxyType )
			break;
		QList<QVariant> data	= GetSettingProxy( m_proxyType ).toList();
		targetIp	= data.at(0).toString();
		targetPort	= data.at(1).toString().toUInt();
		djDebug() << "targetIp = " << targetIp << "targetPort = " << targetPort;
	}while(false);
	
	djDebug() << "ip=" << ip << "targetIp=" << targetIp << "port=" << port << "targetPort=" << targetPort;


#if QT_VERSION < 0x040100 && defined( Q_WS_WIN )
	djDebug() << "QT_VERSION < 4.1.0";
	QHostInfo info	= QHostInfo::fromName(targetIp);
	if ( !info.addresses().isEmpty() ) {
		QHostAddress address = info.addresses().first();
		djDebug() << "address =" << address.toString();
		connectToHost( address, targetPort );
		if ( showProgress )
			m_progressDlg->startProgress( seconds, text );
	}else {
		QMessageBox::information( 0,tr("DNS failed"),tr("Can't resolve host name: ")+ip,QMessageBox::Ok);
	}
#else
	djDebug() << "QT_VERSION >= 4.1.0";
	connectToHost( targetIp, targetPort );
	if ( showProgress )
		m_progressDlg->startProgress( seconds, text );
#endif
}
void DJSocket::timeout()
{
	djDebug() << "DJSocket::timeout";
	abort();
	clear();
	//save the last server
	//SetSettingLastServer( QString(), 0 );
	
	QMessageBox::information( 0,tr("Timeout"),tr("Can't connect to game server"),QMessageBox::Ok);
}
void DJSocket::cancel()
{
	djDebug() << "DJSocket::cancel";
	abort();
	clear();
	//save the last server
	//SetSettingLastServer( QString(), 0 );
}
void DJSocket::stopProgress()
{
	djDebug() << "DJSocket::stopProgress";
	m_progressDlg->stopProgress();
}
void DJSocket::socketHostFound()
{
	djDebug() << "socketHostFound";
}
void DJSocket::socketConnected()
{
	djDebug() << "socketConnected" << "proxyType =" << m_proxyType;
	
	switch(m_proxyType) {
	case 0:
		connectToSocks5Proxy();
		break;
	case 1:
		connectToSocks4Proxy();
		break;
	case 2:
		connectToHttpProxy();
		break;
	default:
		break;
	}
}
void DJSocket::socketDisconnected()
{
	djDebug() << "socketDisconnected" << state();
	if ( Hall == m_socketType ) {
		clear();
	}else {
		stopProgress();
		m_buf.clear();
		m_requests.clear();
		
		allGamesCommand( GAME_COMMAND_DISCONNECTED, QVariant() );
	}
}
void DJSocket::socketError(QAbstractSocket::SocketError socketError)
{
	djDebug() << "socketError" << "errorcode" << socketError << errorString() << "state" << state();
	
	if ( Hall == m_socketType ) {
		clear();
		DJMessageBox::information( 10, 0,tr("Socket Error"),errorString(), QMessageBox::Ok);
	}else {
		stopProgress();
		m_buf.clear();
		m_requests.clear();
	}
	if ( QAbstractSocket::UnconnectedState == state() )
		socketDisconnected();
	else
		abort();
	//DJMessageBox::information( 10, 0,tr("Socket Error"),QAbstractSocket::tr(errorString()), QMessageBox::Ok);
		
	//save the last server
	SetSettingLastServer( QString(), 0 );	
}
void DJSocket::socketReadyRead()
{
	while ( bytesAvailable() || m_buf.size() ) {
		
		djDebug() << "socketReadyRead " << "bytesAvailable=" << bytesAvailable() << "m_buf size=" << m_buf.size();
	
		//djDebug() << "before append data address = " << (void*)m_buf.data();
		m_buf.append( readAll() );
		//djDebug() << "after append data address = " << (void*)m_buf.data();
		
		if ( !m_proxyCallbacks.isEmpty() ) {
			ProxyCallback proxyCallback	= m_proxyCallbacks.takeFirst();
			if ( !proxyCallback( m_buf, this ) ) {
				djDebug() << "proxy fail";
				abort();
			}else {
				djDebug() << "proxy ok";
			}
			m_buf.clear();
		}
		
		if ( m_buf.size() < DJGameProtocolLength ) {
			break;
		}
	
		const DJGameProtocol	*protocol	= reinterpret_cast<const DJGameProtocol *>(m_buf.data());
		quint8	len	= protocol->chTotalLen;
		
		if ( m_buf.size() < len ) {
			break;
		}
		
		DebugBuffer( m_buf, len );

		switch( protocol->chType ) {
		case DJGAME_PROTOCOL_TYPE_ACL:
			aclHandler(m_buf);
			break;
		case DJGAME_PROTOCOL_TYPE_GAMELIST: {
			const GameDescritpor *gameDes		= reinterpret_cast<const GameDescritpor *>(protocol->chBuf);
			quint16	gameId =  makeint16(gameDes->chGameClass, gameDes->chGame);
			QString gameName	= QString::fromUtf8(gameDes->szGameName);
			m_hallController->addGameItem( gameId, gameName );
			break;
		}
		case DJGAME_PROTOCOL_TYPE_SERVERLIST2: {
			const ServerDescritpor2 *serverDes		= reinterpret_cast<const ServerDescritpor2 *>(protocol->chBuf);
			quint16	gameId =  makeint16(serverDes->chGameClass, serverDes->chGame);
			quint16	serverId	= letoh2( &serverDes->chServerID );
			QString ip;
        	if(IP_VERSION == 6)
            	ip = QString("%1.%2.%3.%4.%5.%6").arg(serverDes->chIP[0]).arg(serverDes->chIP[1]).arg(serverDes->chIP[2])
                                        .arg(serverDes->chIP[3]).arg(serverDes->chIP[4]).arg(serverDes->chIP[5]);
        	else
            	ip = QString("%1.%2.%3.%4").arg(serverDes->chIP[0]).arg(serverDes->chIP[1])
            						.arg(serverDes->chIP[2]).arg(serverDes->chIP[3]);
            quint16	port	= letoh2( &serverDes->chPort );
			QString serverName	= QString::fromUtf8( serverDes->szServerName );
			int numberOfPlayers	= letoh4( serverDes->uiPlayers );
			m_hallController->addServerItem( gameId, serverId, ip, port, serverName, numberOfPlayers );
			break;
		}
		case DJGAME_PROTOCOL_TYPE_GAMEDATA: {
   			const GameDataHead	*gameDataHead	= reinterpret_cast<const GameDataHead *>( protocol->chBuf );
   			quint16	gameId	= makeint16(gameDataHead->chGameClass, gameDataHead->chGame);
   			if( m_gameItems.contains( gameId ) ) {
   				DJGameGameItem *gameItem	= m_gameItems.value( gameId );
   				gameItem->gameControllerCommand( GAME_COMMAND_GAME_DATA, m_buf );
   			}else {
   				djDebug() << "new game data" << gameId;
   				//quint16 existingGameId	= m_gameItems.constBegin().key();
   				DJGameGameItem *existingGameItem	= m_gameItems.constBegin().value();
   				//QString userName,password;
   				//existingGameItem->getLoginNamePass( userName, password );
   				
   				quint16 serverId	= existingGameItem->curServerId();
   				DJGameServerItem*	existingServerItem	= static_cast<DJGameServerItem *>(existingGameItem->childItem(serverId));
   				
   				DJGameClassItem	*gameClassItem	= 0;
                DJGameGameItem		*gameItem	= 0;
                DJGameServerItem	*serverItem	= 0;
                
                m_hallController->locateTreeItem( gameId>>8, gameId, 0, gameClassItem, gameItem, serverItem );
               	
            	if ( gameItem ) {
            		quint32 hallVersion	= DJHallController::version();
               		quint32 gameVersion	= gameItem->version();
               		if ( FirstVersion(hallVersion) == FirstVersion(gameVersion) &&
               			SecondVersion(hallVersion) == SecondVersion(gameVersion) ) {
               			serverItem	= static_cast<DJGameServerItem *>(gameItem->childItem(serverId));
                		if ( 0 == serverItem ) {
                			serverItem	= new DJGameServerItem(serverId,gameItem,DJGameItemServerType);
                		}
                		serverItem->assign( existingServerItem );
                		
                		m_hallController->gameTree()->setItemExpanded( gameItem, true );
                		
                		//gameItem->setLoginNamePass( userName, password );
                		
                		insertGame( gameItem ,serverItem );
                		
                		gameItem->gameControllerCommand( GAME_COMMAND_GAME_DATA, m_buf );
               		}
            	}
   			}
			break;
		}
		case DJGAME_PROTOCOL_TYPE_CHAT: {
			djDebug()<<"***HALL***"<<"DJGAME_PROTOCOL_TYPE_CHAT";
			const ChatContent	*chatContent	= reinterpret_cast<const ChatContent *>( protocol->chBuf );
			quint32	speakerId	= letoh4( &chatContent->userid_speaker );
			quint32	listenerId	= letoh4( &chatContent->userid_listener );
			quint16 gameId	= makeint16(chatContent->chGameClass, chatContent->chGame);
			djDebug()<<hex<<"speakerId"<<speakerId<<"listenerId"<<listenerId<<"gameId"<<gameId;
			if ( DJGAME_GROUPCHAT == gameId ) {
				//group chat,extract gameid from listenerid
				gameId	= listenerId >> 16;
				if( m_gameItems.contains( gameId ) ) {
   					DJGameGameItem *gameItem	= m_gameItems.value( gameId );
   					gameItem->gameControllerCommand( GAME_COMMAND_GAME_DATA, m_buf );
   				}
			}else {
				if ( listenerId < MIN_USER_ID || speakerId < MIN_USER_ID ) {
					//some special group chat
					if( m_gameItems.contains( gameId ) ) {
   						DJGameGameItem *gameItem	= m_gameItems.value( gameId );
   						gameItem->gameControllerCommand( GAME_COMMAND_GAME_DATA, m_buf );
   					}
				}else {
					//user to user chat
					m_hallController->receiveChat( chatContent, protocol->chLanguage );
				}
			}
			break;
		}
		case DJGAME_PROTOCOL_TYPE_BROADCAST: {
			const BroadcastHead	*boardcastHead	= reinterpret_cast<const BroadcastHead *>(protocol->chBuf);
   			quint16	gameId	= makeint16(boardcastHead->chGameClass, boardcastHead->chGame);
   			if( m_gameItems.contains( gameId ) ) {
   				DJGameGameItem *gameItem	= m_gameItems.value( gameId );
   				gameItem->gameControllerCommand( GAME_COMMAND_GAME_DATA, m_buf );
   			}
			break;
		}
		case DJGAME_PROTOCOL_TYPE_SPEED: {
			djDebug()<<"***HALL***" << "DJGAME_PROTOCOL_TYPE_SPEED";
			write( SpeedPackage, DJGameProtocolLength );
			break;
		}
		case DJGAME_PROTOCOL_TYPE_P2P: {
			djDebug()<<"***HALL***" << "DJGAME_PROTOCOL_TYPE_P2P";
			if ( DJGAME_PROTOCOL_P2P_SUBTYPE_AGREE == protocol->chSubType ) {
				djDebug() << "DJGAME_PROTOCOL_P2P_SUBTYPE_AGREE";
				const DJGameP2P*	p2p	= reinterpret_cast<const DJGameP2P *>(protocol->chBuf);
				quint32	hostId	= letoh4( &p2p->host );
				quint32	clientId	= letoh4( &p2p->client );
				djDebug() << hostId << clientId << p2p->shPort << p2p->addr_len;
			}
			break;
		}
		case DJGAME_PROTOCOL_TYPE_GETCARDS:
			djDebug()<<"***HALL***"<<"DJGAME_PROTOCOL_TYPE_GETCARDS";
			allGamesCommand( GAME_COMMAND_GAME_DATA, m_buf );
			break;
		default:
			djDebug() << "----------------------------- unknown event";
			break;
		}
		
		//m_buf	= m_buf.mid(len);
		m_buf.remove(0,len);
	}
}
void DJSocket::socketBytesWritten(qint64 bytes)
{
	djDebug() << "socketBytesWritten = " << bytes;
}
void DJSocket::insertGame( DJGameGameItem *gameItem, DJGameServerItem* serverItem )
{
	djDebug() << "DJSocket::insertGame" << gameItem->id();
	serverItem->setIcon(0,QIcon(hallResDir + "/image/login.png"));
	quint16	gameId	= gameItem->id();
	QString	loginName	= m_hallController->loginName();
	djDebug() << "loginName" << loginName;
	DJContact* contact	= m_hallController->contact( loginName );
	if ( 0 == contact ) {
		djDebug() << "new contact" ;
		contact	= new DJContact( m_hallController, loginName );
		m_hallController->insertContact( loginName, contact );
	}
	contact->insertGame( gameId );
	
	gameItem->setCurServerId( serverItem->id() );
	QList<QVariant>	list;
	list << QVariant::fromValue(static_cast<void*>(gameItem))
			<< QVariant::fromValue(static_cast<void*>(this))
			<< QVariant::fromValue(static_cast<void*>(m_hallController))
			<< QVariant::fromValue(static_cast<void*>(contact));
				
			//<< QVariant::fromValue(static_cast<void*>(m_hallController->treeContainer()))
			//<< QVariant::fromValue(static_cast<void*>(m_hallController->tabContainer()));
			
	DJHallInterface	hallInterface( hiHallCommandProc, list );
	gameItem->createGameController( hallInterface );
	
	m_gameItems.insert( gameId, gameItem );
}
void DJSocket::removeGame( quint16 gameId )
{
	djDebug() << "DJSocket::removeGame" << gameId << m_gameItems.size();
	
	if ( m_gameItems.contains( gameId ) ) {
		djDebug() << "contains";
		DJGameGameItem	*gameItem	= m_gameItems.take( gameId );
    	gameItem->setCurServerId( 0 );
    	for ( int i = 0; i < gameItem->childCount(); i++ ) {
			QTreeWidgetItem *item	= (gameItem->child(i));
			item->setIcon(0,QIcon());
		}
    	djDebug() << "cleanup game";
		
		gameItem->gameControllerCommand( GAME_COMMAND_REMOVE_GAME, QVariant() );
		
       	QString	loginName	= m_hallController->loginName();
       	djDebug() << "loginName" << loginName;
       	DJContact* contact	= m_hallController->contact( loginName );
		if ( contact ) {
			djDebug() << "contact";
			contact->removeGame( gameId );
			if ( contact->gameIsEmpty() ) {
				djDebug() << "delete contact";
				m_hallController->removeContact( loginName );
				delete contact;
			}
		}
		gameItem->deleteGameController();
	}
	
		
	if ( m_gameItems.isEmpty() ) {
		djDebug() << "the last game removed, close this socket";
		close();
		djDebug() << "socket closed";
	}
	djDebug() << "remove end";
}
bool DJSocket::containsGame( quint16 gameId )
{
	return m_gameItems.contains( gameId );
}
void DJSocket::appendRequest( const DJClientRequest& request )
{
	m_requests.append( request );
}
void DJSocket::removeAcl( const QList<void*>& acls )
{
	djDebug() << "DJSocket::removeAcl";
	QList<DJClientRequest>::iterator it = m_requests.begin();
	while ( it != m_requests.end() ) {
        void* acl	= (void*)(*it).aclProc();
        djDebug() << "acl = " << acl;
        if ( acl && acls.contains( acl ) ) {
        	djDebug() << "removed";
        	it	= m_requests.erase(it);
        }else {
        	++it;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////private method
void DJSocket::allGamesCommand( int gameCommand, const QVariant& parameters )
{
	/*
	QHash<quint16, DJGameGameItem *>::const_iterator it = m_gameItems.constBegin();
	while ( it != m_gameItems.constEnd() ) {
        DJGameGameItem* gameItem	= it.value();
        gameItem->gameControllerCommand( gameCommand, parameters );
        ++it;
    }
    */
    foreach( DJGameGameItem *gameItem, m_gameItems ) {
    	gameItem->gameControllerCommand( gameCommand, parameters );
    }
}
void DJSocket::aclHandler( const QByteArray& buf )
{
	djDebug() << "aclHandler BEGIN";
	if ( m_requests.isEmpty() ) {
		djDebug()<<"request is empty";
		return;	
	}
	
	DJClientRequest	request	= m_requests.takeFirst();
	request.invokeAclProc( buf );
	
	sendBufferedRequest();
	djDebug() << "aclHandler END ";
}
bool DJSocket::sendRequest( const DJClientRequest& request ,bool isDiscardable )
{
	djDebug() << "DJSocket::sendRequest";
	if ( QAbstractSocket::ConnectedState != state() )
		return false;
		
	DebugBuffer( request.content() );
		
	if ( m_requests.isEmpty() ){		
		djDebug() << "request list is empty, just send it";
		m_requests.append( request );
		const QByteArray& buf	= request.content();
		if ( buf.size() > DJGAME_MAX_PROTOCOL_SIZE ) {
			djDebug()<<"too long";
			write( buf, DJGAME_MAX_PROTOCOL_SIZE );
		}else {
			write( buf );
		}
		return true;
	}else {
		djDebug() << "request list is NOT empty";
		if ( !isDiscardable ) {
			m_requests.append( request );
			djDebug() << "buffered request";
			return true;
		}else {
			djDebug() << "discard request ";
			return false;
		}
	}
}
void DJSocket::sendBufferedRequest()
{
	djDebug() << "DJSocket::sendBufferedRequest";
	
	if ( QAbstractSocket::ConnectedState != state() ) {
		djDebug()<<"socket is NOT connected";
		return;
	}
	if ( m_requests.isEmpty() ) {
		djDebug()<<"no buffered request";
		return;
	}
	const QByteArray& buf	= m_requests.first().content();	
	DebugBuffer( buf );
	if ( buf.size() > DJGAME_MAX_PROTOCOL_SIZE ) {
		djDebug()<<"too long";
		write( buf, DJGAME_MAX_PROTOCOL_SIZE );
	}else {
		write( buf );
	}
}

void DJSocket::clear()
{
	djDebug() << "------------DJSocket::clear-------------";

	m_proxyCallbacks.clear();
	m_ip.clear();
	m_port	= 0;
	
	stopProgress();
	
	djDebug() << "receive buf count = " << m_buf.size();
	DebugBuffer( m_buf );
	m_buf.clear();

	djDebug() << "pending requests count = "<< m_requests.count();
	foreach( DJClientRequest request, m_requests ) {
		DebugBuffer( request.content() );
	}
	m_requests.clear();
	
	djDebug() << "contain gameitems count =" << m_gameItems.count();
	foreach( DJGameGameItem *gameItem, m_gameItems ) {
		djDebug() << "game id = "<<hex << gameItem->id();
		removeGame( gameItem->id() );
	}
    m_gameItems.clear();

    djDebug() << "DJSocket::clear end";
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//proxy
/*
void DJSocket::getLoginNamePass( QString& userName, QString& password )
{
	if ( m_gameItems.size() ) {
		DJGameGameItem *gameItem	= m_gameItems.constBegin().value();
		gameItem->getLoginNamePass( userName, password );
	}else {
		userName.clear();
		password.clear();
	}
}
*/
quint32 DJSocket::ipStringToIp4( const QString& ip )
{
	QHostInfo info	= QHostInfo::fromName(ip);
	quint32 ip4	= 0;
	if ( !info.addresses().isEmpty() ) {
		QHostAddress host = info.addresses().first();
		ip4	= host.toIPv4Address();
	}
	return ip4;
}
void DJSocket::appendProxyCallback( ProxyCallback proxyCallback )
{
	m_proxyCallbacks.append( proxyCallback );
}
void DJSocket::connectToSocks4Proxy()
{
    djDebug() << "connectToSocks4Proxy";
    
    QByteArray	buf( 9, 0 );
    QDataStream	stream( &buf, QIODevice::ReadWrite );
    stream.setByteOrder( QDataStream::BigEndian );
    
    stream << static_cast<quint8>( 4 );//1 byte,version number
    stream << static_cast<quint8>( 1 );//1 byte,command code
    stream << static_cast<quint16>( m_port );//2 bytes,port
    stream << static_cast<quint32>( ipStringToIp4(m_ip) );//4 bytes,ip
    stream << static_cast<quint8>( 0 );//1 byte,end
    
    qint64	bytesWritten	= 0;    
	while ( bytesWritten < buf.size() ) {
		bytesWritten	= write(buf);
		if ( -1 != bytesWritten ) {
			buf	= buf.mid(bytesWritten);
		}else {
			//write socket error
			abort();
			return;
		}
	}
	appendProxyCallback( socks4ConnectCallback );
}

void DJSocket::connectToSocks5Proxy()
{
    djDebug() << "connectToSocks5Proxy";
    
    QByteArray	buf;
    buf.append( 5 );
    buf.append( 2 );
    buf.append( static_cast<char>( 0 ) );
    buf.append( 2 );
    
    qint64	bytesWritten	= 0;    
	while ( bytesWritten < buf.size() ) {
		bytesWritten	= write(buf);
		if ( -1 != bytesWritten ) {
			buf	= buf.mid(bytesWritten);
		}else {
			//write socket error
			abort();
			return;
		}
	}
	appendProxyCallback( socks5VersionCallback );
}
void DJSocket::socks5RequestAuth( QString username, QString password )
{
    djDebug("socks5RequestAuth");
    
    QByteArray	buf;
    
    buf.append( 1 );//version
    buf.append( username.toUtf8().size() );//username length
    buf.append( username.toUtf8() );//username string
    buf.append( password.toUtf8().size() );//password length
    buf.append( password.toUtf8() );//password string
    
    qint64	bytesWritten	= 0;    
	while ( bytesWritten < buf.size() ) {
		bytesWritten	= write(buf);
		if ( -1 != bytesWritten ) {
			buf	= buf.mid(bytesWritten);
		}else {
			//write socket error
			abort();
			return;
		}
	}
	appendProxyCallback( socks5AuthCallback );
}
void DJSocket::socks5RequestConnect()
{
    djDebug("socks5RequestConnect");

	QByteArray	buf( 10, 0 );
    QDataStream	stream( &buf, QIODevice::ReadWrite );
    stream.setByteOrder( QDataStream::BigEndian );
    
    stream << static_cast<quint8>( 5 );//1 byte,version number
    stream << static_cast<quint8>( 1 );//1 byte,command code
    stream << static_cast<quint8>( 0 );//1 byte,rsv
    stream << static_cast<quint8>( 1 );//1 byte,atyp
    stream << static_cast<quint32>( ipStringToIp4(m_ip) );//4 bytes,ip
    stream << static_cast<quint16>( m_port );//2 bytes,port
    
    
    qint64	bytesWritten	= 0;    
	while ( bytesWritten < buf.size() ) {
		bytesWritten	= write(buf);
		if ( -1 != bytesWritten ) {
			buf	= buf.mid(bytesWritten);
		}else {
			//write socket error
			abort();
			return;
		}
	}
	appendProxyCallback( socks5ConnectCallback );
}
void DJSocket::connectToHttpProxy()
{
    djDebug("connectToHttpProxy");
    
    QByteArray	buf;
    buf.append("CONNECT ");
    buf.append( m_ip );
    buf.append( ":" );
    buf.append( QByteArray::number(m_port) );
    buf.append( " HTTP/1.1\r\n" );
    buf.append( "User-Agent: BlueDJ/0.1\r\n" );
    buf.append( "Proxy-Authorization: Basic " );
    QString username,password;
    GetSettingProxyAuth( username,password );
    QString auth	= username+":"+password;
    buf.append( auth.toLocal8Bit().toBase64() );
    buf.append( "\r\n" );
    buf.append( "\r\n" );
    
    qint64	bytesWritten	= 0;    
	while ( bytesWritten < buf.size() ) {
		bytesWritten	= write(buf);
		if ( -1 != bytesWritten ) {
			buf	= buf.mid(bytesWritten);
		}else {
			//write socket error
			abort();
			return;
		}
	}
	appendProxyCallback( httpConnectCallback );
}
void DJSocket::timerEvent ( QTimerEvent * event )
{
	djDebug()<<"DJSocket::timerEvent"<<m_socketType;
	if ( !m_requests.isEmpty() ) {
		uint id	= m_requests.first().id();
		djDebug()<<"id="<<id<<"m_lastRequestId"<<m_lastRequestId;
		if ( m_lastRequestId == id ) {
			//time up,remove this request
			m_requests.clear();
		}
		m_lastRequestId	= id;
	}
	if ( Hall == m_socketType ) {
		//djDebug() << "hall socket";
		return;
	}
	djDebug() << "timerEvent game socket state = " << state();
	if ( QAbstractSocket::ConnectedState == state() ) {
		djDebug() << "send a speed package";
		write( KeepOnLinePackage, DJGameProtocolLength );
	}
	allGamesReconnect();
}
