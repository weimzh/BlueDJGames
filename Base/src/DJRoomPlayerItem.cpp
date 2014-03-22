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

#include "DJRoomPlayerItem.h"
#include "DJGameUser.h"
#include "DJRoomPlayerAvatarItem.h"
#include "DJRoomPlayerNameItem.h"

static	qreal	ZVALUE_AVATAR		=	300;
static	qreal	ZVALUE_NAME			=	400;

DJRoomPlayerItem::DJRoomPlayerItem( DJRoomController *roomController, DJGameUser *user, const QFont& font, const QColor& color, QGraphicsItem * parent, QGraphicsScene * scene )
{
	djDebug()<<"DJRoomPlayerItem constructor";
	
	m_avatarItem	= new DJRoomPlayerAvatarItem( roomController, user, parent, scene );
	m_avatarItem->setZValue( ZVALUE_AVATAR );
	
	m_nameItem	= new DJRoomPlayerNameItem( user, font, color, parent, scene );
	m_nameItem->setZValue( ZVALUE_NAME );
}
DJRoomPlayerItem::~DJRoomPlayerItem()
{
	djDebug()<<"DJRoomPlayerItem destructor";
	
	delete m_avatarItem;
	delete m_nameItem;
}
DJRoomPlayerAvatarItem* DJRoomPlayerItem::avatarItem() const
{
	return m_avatarItem;
}
DJRoomPlayerNameItem*	DJRoomPlayerItem::nameItem() const
{
	return m_nameItem;
}
void DJRoomPlayerItem::reload()
{
	DJGameUser* user	= m_avatarItem->user();
	m_avatarItem->setPixmap( user->avatar() );
	m_nameItem->setPlainText( user->userName() );
}
void DJRoomPlayerItem::update()
{
	m_avatarItem->update();
	m_nameItem->update();
}