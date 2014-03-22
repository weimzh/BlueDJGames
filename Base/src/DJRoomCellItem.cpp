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

#include "DJRoomCellItem.h"
#include "DJRoomController.h"
#include "DJRoomTableItem.h"
#include "DJRoomSeatItem.h"

static	qreal	ZVALUE_TABLE	=	100;
static	qreal	ZVALUE_SEAT		=	200;

DJRoomCellItem::DJRoomCellItem( quint8 tableId, DJRoomController *roomController,
						QGraphicsItem * parent, QGraphicsScene * scene )
    : QGraphicsItem(parent, scene ), m_roomController( roomController )
{
	const QSize& cellSize	= m_roomController->cellSize();
	QPoint centerPoint(m_roomController->cellSize().width()/2,m_roomController->cellSize().height()/2);
		
	m_tableItem	= new DJRoomTableItem( tableId, roomController, this, scene ) ;
	QPoint tableOrigin	= centerPoint - QPoint(m_tableItem->boundingRect().width()/2,m_tableItem->boundingRect().height()/2);
	m_tableItem->setPos( tableOrigin );
	m_tableItem->setZValue(ZVALUE_TABLE);
	
	for ( int seatId = 1; seatId <= m_roomController->seatPixs().size(); seatId++ ) {
		DJRoomSeatItem	*seatItem	= new DJRoomSeatItem( m_roomController->seatPixs().at(seatId-1), tableId, seatId, this, scene );
		QPoint seatOffset	= m_roomController->seatOffsets().at(seatId-1);
		QPoint	seatOrigin	= centerPoint + seatOffset - QPoint(seatItem->boundingRect().width()/2,seatItem->boundingRect().height()/2);
		seatItem->setPos( seatOrigin );
		seatItem->setZValue( ZVALUE_SEAT );
		
		QObject::connect( seatItem, SIGNAL(seatClicked(quint8,quint8,QGraphicsSceneMouseEvent*)), roomController, SLOT(handleSeatClicked(quint8,quint8,QGraphicsSceneMouseEvent*)) );
	}
}
DJRoomCellItem::~DJRoomCellItem()
{
}
QRectF DJRoomCellItem::boundingRect() const
{	
	return QRectF(0,0,m_roomController->cellSize().width(),m_roomController->cellSize().height());
}
void DJRoomCellItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{
}
DJRoomTableItem* DJRoomCellItem::tableItem() const
{
	return m_tableItem;
}
