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

#include "DJGraphicsPixmapItem.h"
#include "Utility.h"

static int	countPixmapItem	= 0;

DJGraphicsPixmapItem::DJGraphicsPixmapItem( QGraphicsItem * parent, QGraphicsScene * scene, bool isMatrimoveMode )
	:QGraphicsPixmapItem( parent, scene ),m_isMatrimoveMode(isMatrimoveMode)
{
	djDebug()<<"DJGraphicsPixmapItem constructor"<<countPixmapItem++;
	init();
}
DJGraphicsPixmapItem::DJGraphicsPixmapItem( const QPixmap & pixmap, QGraphicsItem * parent, QGraphicsScene * scene, bool isMatrimoveMode )
	:QGraphicsPixmapItem(pixmap,parent,scene),m_isMatrimoveMode(isMatrimoveMode)
{
	djDebug()<<"DJGraphicsPixmapItem constructor"<<countPixmapItem++;
	init();
}
DJGraphicsPixmapItem::~DJGraphicsPixmapItem()
{
	djDebug()<<"DJGraphicsPixmapItem destructor"<<countPixmapItem--;
}
void DJGraphicsPixmapItem::init()
{
	m_alignment	= 0;
	m_selected	= false;
	m_externalScale	= 1;
	m_internalScale	= 1;
	m_angleOfRotation	= 0;
	setTransformationMode( Qt::SmoothTransformation );
}
int DJGraphicsPixmapItem::type() const
{
	return DJGraphicsPixmapItemType;
}
void DJGraphicsPixmapItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{
	QGraphicsPixmapItem::paint( painter, option, widget );
	if ( isDJSelected() ) {
		painter->setPen( m_pen );
		painter->drawRect( boundingRect() );
	}
}
void DJGraphicsPixmapItem::setDeltaPos( const QPointF & deltaPos )
{
	m_deltaPos	= deltaPos;
}
void DJGraphicsPixmapItem::setPen( const QPen& pen )
{
	m_pen	= pen;
}
void DJGraphicsPixmapItem::setDJSelected( bool selected )
{
	m_selected	= selected;
}
void DJGraphicsPixmapItem::setMatrimoveMode( bool isMatrimoveMode )
{
	m_isMatrimoveMode	= isMatrimoveMode;
}
void DJGraphicsPixmapItem::setAlignment( Qt::Alignment alignment )
{
	m_alignment	= alignment;
}
void DJGraphicsPixmapItem::setVirtualPos ( const QPointF & vpos )
{
	m_virtualPos	= vpos;
}
void DJGraphicsPixmapItem::setExternalScale( qreal externalScale )
{
	m_externalScale	= externalScale;
	qreal totalScale	= m_externalScale * m_internalScale;
	setMatrix( QMatrix(totalScale, 0, 0, totalScale, 0, 0) );
	rotate( m_angleOfRotation );
}
void DJGraphicsPixmapItem::setInternalScale( qreal internalScale )
{
	m_internalScale	= internalScale;
	qreal totalScale	= m_externalScale * m_internalScale;
	setMatrix( QMatrix(totalScale, 0, 0, totalScale, 0, 0) );
	rotate( m_angleOfRotation );
}
void DJGraphicsPixmapItem::setAngleOfRotation( qreal angleOfRotation )
{
	m_angleOfRotation	= angleOfRotation;
	qreal totalScale	= m_externalScale * m_internalScale;
	setMatrix( QMatrix(totalScale, 0, 0, totalScale, 0, 0) );
	rotate( m_angleOfRotation );
}
void DJGraphicsPixmapItem::adjustPos( const QMatrix& m )
{
	QPoint pos	= m_virtualPos.toPoint();
	if ( isDJSelected() ) {
		pos	+= m_deltaPos.toPoint();
	}
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