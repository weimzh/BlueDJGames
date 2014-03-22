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

#include "DJGraphicsTextItem.h"
#include "Utility.h"

static int	countTextItem	= 0;

DJGraphicsTextItem::DJGraphicsTextItem( QGraphicsItem * parent, QGraphicsScene * scene, bool isMatrimoveMode )
	:QGraphicsTextItem( parent, scene ),m_isMatrimoveMode(isMatrimoveMode)
{
	djDebug()<<"DJGraphicsTextItem constructor"<<countTextItem++;
	init();
}
DJGraphicsTextItem::DJGraphicsTextItem( const QString & text, QGraphicsItem * parent, QGraphicsScene * scene, bool isMatrimoveMode )
	:QGraphicsTextItem(text,parent,scene),m_isMatrimoveMode(isMatrimoveMode)
{
	djDebug()<<"DJGraphicsTextItem constructor"<<countTextItem++;
	init();
}
DJGraphicsTextItem::~DJGraphicsTextItem()
{
	djDebug()<<"DJGraphicsTextItem destructor"<<countTextItem--;
}
void DJGraphicsTextItem::init()
{
	m_alignment	= 0;
	m_externalScale	= 1;
	m_internalScale	= 1;
	m_angleOfRotation	= 0;
}
int DJGraphicsTextItem::type() const
{
	return DJGraphicsTextItemType;
}
void DJGraphicsTextItem::setMatrimoveMode( bool isMatrimoveMode )
{
	m_isMatrimoveMode	= isMatrimoveMode;
}
void DJGraphicsTextItem::setAlignment( Qt::Alignment alignment )
{
	m_alignment	= alignment;
}
void DJGraphicsTextItem::setVirtualPos ( const QPointF &vpos )
{
	m_virtualPos	= vpos;
}
void DJGraphicsTextItem::setExternalScale( qreal externalScale )
{
	m_externalScale	= externalScale;
	qreal totalScale	= m_externalScale * m_internalScale;
	setMatrix( QMatrix(totalScale, 0, 0, totalScale, 0, 0) );
	rotate( m_angleOfRotation );
}
void DJGraphicsTextItem::setInternalScale( qreal internalScale )
{
	m_internalScale	= internalScale;
	qreal totalScale	= m_externalScale * m_internalScale;
	setMatrix( QMatrix(totalScale, 0, 0, totalScale, 0, 0) );
	rotate( m_angleOfRotation );
}
void DJGraphicsTextItem::setAngleOfRotation( qreal angleOfRotation )
{
	m_angleOfRotation	= angleOfRotation;
	qreal totalScale	= m_externalScale * m_internalScale;
	setMatrix( QMatrix(totalScale, 0, 0, totalScale, 0, 0) );
	rotate( m_angleOfRotation );
}
void DJGraphicsTextItem::adjustPos( const QMatrix& m )
{
	QPoint pos	= m_virtualPos.toPoint();
	
	QSize size	= boundingRect().size().toSize();
	size *= internalScale();
	if ( isMatrimoveMode() ) {
		PosByAlign( pos, alignment(), size, m_angleOfRotation );
		pos	= m.map( pos );
	}else {
		size *= externalScale();
		PosByAlign( pos, alignment(), size, m_angleOfRotation );
	}
	setPos( pos );
}
