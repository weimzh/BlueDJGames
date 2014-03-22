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

#ifndef DJGAMEITEM_H
#define DJGAMEITEM_H

#include <QtGui>
#include "DJBaseGlobal.h"
#include "GameInterface.h"

const int DJGameItemGameClassType	= 1;
const int DJGameItemGameType		= 2;
const int DJGameItemServerType		= 3;
const int DJGameItemRoomType		= 4;

class DJ_BASE_EXPORT DJGameItem :public QTreeWidgetItem
{
public:
	DJGameItem( int id, int type = Type );
    DJGameItem( int id, QTreeWidget * parent, int type = Type );
    DJGameItem( int id, QTreeWidgetItem * parent, int type = Type );
    virtual ~DJGameItem();
    
    virtual uint	id() const = 0;
    DJGameItem* childItem( uint id ) const;
protected:
   	uint	m_id;
};
class DJ_BASE_EXPORT DJGameClassItem :public DJGameItem
{
public:
    DJGameClassItem( int id, QTreeWidget * parent, int type = Type );
    virtual ~DJGameClassItem();
    
    virtual uint	id() const;
};
class DJ_BASE_EXPORT DJGameGameItem :public DJGameItem
{
public:
    DJGameGameItem( int id, QTreeWidgetItem * parent, int type = Type );
    virtual ~DJGameGameItem();
   
    virtual uint	id() const;
    quint32 version() const { return m_version; }
    void setVersion( quint32 version ) { m_version	= version; }
    QString	libraryPath() const;
    void setLibraryPath( const QString& libPath );
    quint16 curServerId() const { return m_curServerId; }
    void setCurServerId( quint16 curServerId ) { m_curServerId	= curServerId; }
    //QString	loginName() const { return m_userName; }
    //QString password() const { return m_password; }
    //void getLoginNamePass( QString& userName, QString& password );
    //void setLoginNamePass( const QString& userName, const QString& password );
    QString name() const { return m_name; }
    void setName( const QString& name ) { m_name	= name; }
    
    void createGameController( const DJHallInterface& hallInterface );
    void deleteGameController();
    QVariant gameControllerCommand( int command, const QVariant& parameters );
private:
	QLibrary			m_library;
	quint16				m_curServerId;
	QString				m_userName;
	QString				m_password;
	quint32				m_version;
	QString				m_name;
	
	GICreateGameControllerProc	m_createGameControllerProc;
	GIGameControllerCommandProc	m_gameControllerCommandProc;
	GIDeleteGameControllerProc	m_deleteGameControllerProc;
};
class DJ_BASE_EXPORT DJGameServerItem :public DJGameItem
{
public:
	DJGameServerItem( int id, int type = Type );
    DJGameServerItem( int id, QTreeWidgetItem * parent, int type = Type );
    virtual ~DJGameServerItem();
   
    virtual uint	id() const;
    virtual QTreeWidgetItem* clone() const;
    void assign( const DJGameServerItem* serverItem );
    
    QString ip() const { return m_ip; }
    void setIp( const QString& ip) { m_ip	= ip; }
    quint16 port() const { return m_port; }
    void setPort( quint16 port) { m_port	= port; }
    QString name() const { return m_name; }
    void setName( const QString& name );
    int numberOfUsers() const;
    void setNumberOfUsers( int numberOfUsers );
    void setDescription( const QString& name, int numberOfUsers );
private:
	QString		m_ip;
	quint16		m_port;
	QString		m_name;
	int			m_numberOfUsers;
};
class DJ_BASE_EXPORT DJGameRoomItem :public DJGameItem
{
public:
    DJGameRoomItem( int id, QTreeWidget* parent, int type = Type );
    virtual ~DJGameRoomItem();
   
    virtual uint	id() const;
};
#endif
