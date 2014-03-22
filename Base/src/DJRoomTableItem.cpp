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

#include "DJRoomTableItem.h"
#include "DJRoomController.h"
#include "protocol.h"
DJRoomTableItem::DJRoomTableItem( quint8 tableId, DJRoomController *roomController, QGraphicsItem * parent, QGraphicsScene * scene )
    : QGraphicsPixmapItem( parent, scene ), m_tableId(tableId),m_roomController(roomController)
{    	
	djDebug()<<"DJRoomTableItem constructor";
	
	setPixmap( roomController->tablePix() );
	m_status	= DJGAME_TABLE_STATUS_IDLE;
}
DJRoomTableItem::~DJRoomTableItem()
{
	djDebug()<<"DJRoomTableItem destructor";
}
void DJRoomTableItem::setStatus( quint8 status )
{
	m_status	= status;
}
void DJRoomTableItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{	
	QGraphicsPixmapItem::paint( painter, option, widget );
	if ( 0 != m_tableId ) {
		painter->setFont( m_roomController->tableFont() );
		painter->setPen( m_roomController->tableColor() );
		painter->drawText( boundingRect(),Qt::AlignCenter, QString::number( m_tableId ) );
	}
	if ( DJGAME_TABLE_STATUS_IDLE != m_status ) {
		QPointF center	= boundingRect().center();
		const QPixmap& pix	= m_roomController->tablePlayingPix();
		center	-= QPointF( pix.width()/2, pix.height()/2);
		painter->drawPixmap( center, pix );
	}
}
