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

#include "DJRoomPlayerAvatarItem.h"
#include "DJRoomController.h"
#include "DJGameUser.h"

DJRoomPlayerAvatarItem::DJRoomPlayerAvatarItem( DJRoomController *roomController, DJGameUser *user, QGraphicsItem * parent, QGraphicsScene * scene )
    : QGraphicsPixmapItem( parent, scene ), m_roomController(roomController),m_user(user)
{
	djDebug()<<"DJRoomPlayerAvatarItem constructor";
	
	QPixmap pix	= user->avatar();
	setPixmap( pix );
}
DJRoomPlayerAvatarItem::~DJRoomPlayerAvatarItem()
{
	djDebug()<<"DJRoomPlayerAvatarItem destructor";
}
DJGameUser* DJRoomPlayerAvatarItem::user() const
{
	return m_user;
}
void DJRoomPlayerAvatarItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{	
	QGraphicsPixmapItem::paint( painter, option, widget );
	
	if ( m_user->isReady() ) {
		QPixmap	readyPix	= m_roomController->readyPix();
		QPointF	bf	= boundingRect().bottomLeft();
		bf.ry()	-=	readyPix.height();
		painter->drawPixmap( bf, readyPix );
	}
	if ( m_user->isSelf() ) {
		QPen pen( Qt::green );
		pen.setWidth( 3 );
		painter->setPen( pen );
		painter->drawRect( boundingRect() );
	}
}
void DJRoomPlayerAvatarItem::mousePressEvent( QGraphicsSceneMouseEvent * event )
{
	emit playerClicked( m_user, event->buttons() );
}
void DJRoomPlayerAvatarItem::mouseDoubleClickEvent( QGraphicsSceneMouseEvent * event )
{
	emit playerDoubleClicked( m_user, event->buttons() );
}
