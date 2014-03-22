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

#ifndef DJSOCKET_H
#define DJSOCKET_H

#include <QtGui>
#include <QtNetwork>

#include "DJClientRequest.h"

class DJGameGameItem;
class DJGameServerItem;
class DJHallController;
class DJSocket;
class DJProgressDialog;

typedef bool (*ProxyCallback)( const QByteArray& buf, DJSocket* socket );

class DJSocket: public QTcpSocket
{
	Q_OBJECT
public:
	enum Type {
		Hall,
		Game
	};
	DJSocket( Type type, DJHallController *hallController, QObject *parent = 0 );
   	virtual ~DJSocket();
	void description() const;
	
	void allGamesReloadSettings();
	void allGamesContactUserTypeChanged( quint32 userId, int contactUserType );
	void allGamesReconnect();
	
	void stopProgress();
	void clear();
	void connectToServer( const QString& ip, quint16 port, bool showProgress, const QString& text = QString(), int seconds = -1 );
	void insertGame( DJGameGameItem *gameItem, DJGameServerItem* serverItem );
	void removeGame( quint16 gameId );
	bool containsGame( quint16 gameId );
	void appendRequest( const DJClientRequest& request );
	void removeAcl( const QList<void*>& acls );
	bool sendRequest( const DJClientRequest& request, bool isDiscardable = false );
	void sendBufferedRequest();
	
	void socks5RequestAuth( QString username, QString password );
	void socks5RequestConnect();
	void appendProxyCallback( ProxyCallback proxyCallback );
	
	QString ip() const { return m_ip; }
	quint16	port() const { return m_port; }
	void getLoginNamePass( QString& userName, QString& password );
	quint32 ipStringToIp4( const QString& ip );
public slots:
	void cancel();
	void timeout();
protected:
	virtual void timerEvent ( QTimerEvent * event );

private slots:
	void socketHostFound();
	void socketConnected();
	void socketDisconnected();
	void socketError(QAbstractSocket::SocketError);
	void socketReadyRead();
	void socketBytesWritten(qint64);	
private:
	void allGamesCommand( int gameCommand, const QVariant& parameters );
	void aclHandler( const QByteArray& buf );
	
	void connectToSocks4Proxy();
	void connectToSocks5Proxy();
	void connectToHttpProxy();
	
	Type					m_socketType;
	
	QByteArray				m_buf;
	QList<DJClientRequest>	m_requests;
	uint					m_lastRequestId;
	QHash<quint16, DJGameGameItem *>	m_gameItems;
	DJHallController*		m_hallController;
	DJProgressDialog*		m_progressDlg;
	//proxy
	QList<ProxyCallback>	m_proxyCallbacks;
	QString					m_ip;
	quint16					m_port;
	int						m_proxyType;
};
QVariant hiHallCommandProc( int command, const QVariant& parameters );

struct Socks4Request
{
    quint8	vn;
    quint8 	cd;
    quint16 port;
    quint32 ipaddr;
    quint8 	other;
};
struct Socks4Reply
{
    quint8 	vn;
    quint8 	cd;
    quint16 port;
    quint32 ipaddr;
};
struct Socks5VersionRequest
{
    quint8	ver;
    quint8 	nmethods;
    quint8 	methods[255];
};

struct Socks5VersionReply
{
    quint8 	ver;
    quint8 	method;
};
struct Socks5AuthReply
{
    quint8 ver;
    quint8 status;
};
struct Socks5ConnectRequest
{
    quint8 	ver;
    quint8 	cmd;
    quint8 	rsv;
    quint8 	atyp;
    quint32 ipaddr;
    quint16 port;
};

struct Socks5ConnectReply
{
	quint8 	ver;
    quint8 	rep;
    quint8 	rsv;
    quint8 	atyp;
    quint32 ipaddr;
    quint16 port;
};

#endif
