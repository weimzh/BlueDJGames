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

#include "DJGraphicsLineItem.h"
#include "Utility.h"

static int	countLineItem	= 0;

DJGraphicsLineItem::DJGraphicsLineItem( QGraphicsItem * parent, QGraphicsScene * scene )
	:QGraphicsPathItem( parent, scene )
{
	djDebug()<<"DJGraphicsLineItem constructor"<<countLineItem++;
}
DJGraphicsLineItem::~DJGraphicsLineItem()
{
	djDebug()<<"DJGraphicsLineItem destructor"<<countLineItem--;
}
int DJGraphicsLineItem::type() const
{
	return DJGraphicsLineItemType;
}
void DJGraphicsLineItem::setPoints( const QList<QPointF> & points )
{
	m_points	= points;
}
void DJGraphicsLineItem::adjustPos( const QMatrix& m )
{
	QList<QPointF> points	= m_points;
	if ( !points.isEmpty() ) {
		QPointF startPoint	= points.takeFirst();
		QPainterPath painterPath( m.map(startPoint) );
		foreach( QPointF point, points ) {
			painterPath.lineTo( m.map(point) );
		}
		setPath( painterPath );
	}
}
