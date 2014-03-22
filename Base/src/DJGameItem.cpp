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

#include "DJGameItem.h"

const	char*	const	DJGAME_SERVER_ITEM_USERS	= QT_TRANSLATE_NOOP("DJGameServerItem","users");

DJGameItem::DJGameItem( int id, int type )
	:QTreeWidgetItem(type),m_id(id)
{
	//djDebug() << "DJGameItem constructor";
}
DJGameItem::DJGameItem( int id, QTreeWidget * parent, int type )
	:QTreeWidgetItem(parent,type),m_id(id)
{
	//djDebug() << "DJGameItem constructor";
}
DJGameItem::DJGameItem( int id, QTreeWidgetItem * parent, int type )
	:QTreeWidgetItem(parent,type),m_id(id)
{
	//djDebug() << "DJGameItem constructor";
}
DJGameItem* DJGameItem::childItem( uint id ) const
{
	for ( int i = 0; i < childCount(); i++ ) {
		DJGameItem *item	= static_cast<DJGameItem *>(child(i));
		if ( id == item->id() ) {
			return item;
		}
	}
	return 0;
}
DJGameItem::~DJGameItem()
{
	//djDebug() << "DJGameItem destructor";
}
////////////////////////////////////////////////////////////////////////////////////////
DJGameClassItem::DJGameClassItem( int id, QTreeWidget * parent, int type )
	:DJGameItem(id,parent,type)
{
	//djDebug() << "DJGameClassItem constructor";
}
DJGameClassItem::~DJGameClassItem()
{
	//djDebug() << "DJGameClassItem destructor";
}
uint DJGameClassItem::id() const
{
	return m_id;
}
////////////////////////////////////////////////////////////////////////////////////////
DJGameGameItem::DJGameGameItem( int id, QTreeWidgetItem * parent, int type )
	:DJGameItem(id,parent,type)
{
	//djDebug() << "DJGameGameItem constructor";
	m_curServerId		= 0;
	m_version			= 0;
	
	m_createGameControllerProc	= 0;
	m_gameControllerCommandProc	= 0;
	m_deleteGameControllerProc	= 0;
}
DJGameGameItem::~DJGameGameItem()
{
	//djDebug() << "DJGameGameItem destructor";
}
uint DJGameGameItem::id() const
{
	return m_id;
}
QString	DJGameGameItem::libraryPath() const
{
	return m_library.fileName();
}
void DJGameGameItem::setLibraryPath( const QString& libPath )
{
	m_library.setFileName( libPath );
	
	m_createGameControllerProc	= (GICreateGameControllerProc)m_library.resolve("CreateGameController");
	m_gameControllerCommandProc	= (GIGameControllerCommandProc)m_library.resolve("GameControllerCommand");
	m_deleteGameControllerProc	= (GIDeleteGameControllerProc)m_library.resolve("DeleteGameController");
}
/*
void DJGameGameItem::getLoginNamePass( QString& userName, QString& password )
{
	userName	= m_userName;
	password	= m_password;
}
void DJGameGameItem::setLoginNamePass( const QString& userName, const QString& password )
{
	m_userName	= userName;
	m_password	= password;
}
*/
void DJGameGameItem::createGameController( const DJHallInterface& hallInterface )
{
	if ( m_createGameControllerProc )
		m_createGameControllerProc( hallInterface );
}
void DJGameGameItem::deleteGameController()
{
	if ( m_deleteGameControllerProc )
		m_deleteGameControllerProc();
}
QVariant DJGameGameItem::gameControllerCommand( int command, const QVariant& parameters )
{
	if ( m_gameControllerCommandProc )
		return m_gameControllerCommandProc( command, parameters );
	else
		return QVariant();
}
////////////////////////////////////////////////////////////////////////////////////////
DJGameServerItem::DJGameServerItem( int id, int type )
	:DJGameItem(id,type)
{
	//djDebug() << "DJGameServerItem constructor";
	m_port		= 0;
	m_numberOfUsers	= 0;
}
DJGameServerItem::DJGameServerItem( int id, QTreeWidgetItem * parent, int type )
	:DJGameItem(id,parent,type)
{
	//djDebug() << "DJGameServerItem constructor";
	m_port		= 0;
	m_numberOfUsers	= 0;
}
DJGameServerItem::~DJGameServerItem()
{
	//djDebug() << "DJGameServerItem destructor";
}
uint DJGameServerItem::id() const
{
	return m_id;
}
QTreeWidgetItem* DJGameServerItem::clone() const
{
	djDebug() << "DJGameServerItem::clone";
	DJGameServerItem* serverItem	= new DJGameServerItem(id(),DJGameItemServerType);
	serverItem->assign( this );
	return serverItem;	
}
void DJGameServerItem::assign( const DJGameServerItem* serverItem )
{
	setIp( serverItem->ip() );
	setPort( serverItem->port() );
	setDescription( serverItem->name(), serverItem->numberOfUsers() );
}
void DJGameServerItem::setName( const QString& name )
{
	m_name	= name;
	setText( 0, m_name + QString("(%1 %2)").arg(m_numberOfUsers).arg(qApp->translate("DJGameServerItem",DJGAME_SERVER_ITEM_USERS)) );
}
int DJGameServerItem::numberOfUsers() const
{
	return m_numberOfUsers;
}
void DJGameServerItem::setNumberOfUsers( int numberOfUsers )
{
	m_numberOfUsers	= numberOfUsers;
	setText( 0, m_name + QString("(%1 %2)").arg(m_numberOfUsers).arg(qApp->translate("DJGameServerItem",DJGAME_SERVER_ITEM_USERS)) );
}
void DJGameServerItem::setDescription( const QString& name, int numberOfUsers )
{
	m_name	= name;
	m_numberOfUsers	= numberOfUsers;
	setText( 0, m_name + QString("(%1 %2)").arg(m_numberOfUsers).arg(qApp->translate("DJGameServerItem",DJGAME_SERVER_ITEM_USERS)) );
}
////////////////////////////////////////////////////////////////////////////////////////
DJGameRoomItem::DJGameRoomItem( int id, QTreeWidget* parent, int type )
	:DJGameItem(id,parent,type)
{
	//djDebug() << "DJGameRoomItem constructor";
}
DJGameRoomItem::~DJGameRoomItem()
{
	//djDebug() << "DJGameRoomItem destructor";
}
uint DJGameRoomItem::id() const
{
	return m_id;
}
