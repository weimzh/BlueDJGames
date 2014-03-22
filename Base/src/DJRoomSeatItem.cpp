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

#include "DJRoomSeatItem.h"

DJRoomSeatItem::DJRoomSeatItem( const QPixmap& pix, quint8 tableId, quint8 seatId, QGraphicsItem * parent, QGraphicsScene * scene )
    : QGraphicsPixmapItem( pix, parent, scene ), m_tableId(tableId), m_seatId(seatId)
{
	djDebug()<<"DJRoomSeatItem constructor";
}
DJRoomSeatItem::~DJRoomSeatItem()
{
	djDebug()<<"DJRoomSeatItem destructor";
}
void DJRoomSeatItem::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
	emit seatClicked( m_tableId, m_seatId, mouseEvent );
}
