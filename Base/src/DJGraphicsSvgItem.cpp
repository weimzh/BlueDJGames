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

#include "DJGraphicsSvgItem.h"
#include "Utility.h"

static int	countSvgItem	= 0;

DJGraphicsSvgItem::DJGraphicsSvgItem( QGraphicsItem * parent, bool isMatrimoveMode )
	:QGraphicsSvgItem( parent ),m_isMatrimoveMode(isMatrimoveMode)
{
	djDebug()<<"DJGraphicsSvgItem constructor"<<countSvgItem++;
	init();
}
DJGraphicsSvgItem::DJGraphicsSvgItem( const QString & fileName, QGraphicsItem * parent, bool isMatrimoveMode )
	:QGraphicsSvgItem(fileName,parent),m_isMatrimoveMode(isMatrimoveMode)
{
	djDebug()<<"DJGraphicsSvgItem constructor"<<countSvgItem++;
	init();
}
DJGraphicsSvgItem::~DJGraphicsSvgItem()
{
	djDebug()<<"DJGraphicsSvgItem destructor"<<countSvgItem--;
}
void DJGraphicsSvgItem::init()
{
	m_alignment	= 0;
	m_selected	= false;
	m_externalScale	= 1;
	m_internalScale	= 1;
	m_angleOfRotation	= 0;
	setCachingEnabled( false );
}
int DJGraphicsSvgItem::type() const
{
	return DJGraphicsSvgItemType;
}
void DJGraphicsSvgItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{
	QGraphicsSvgItem::paint( painter, option, widget );
	if ( isDJSelected() ) {
		painter->setPen( m_pen );
		//painter->drawRect( sceneBoundingRect() );
		painter->drawRect( boundingRect() );
	}
}
void DJGraphicsSvgItem::setDeltaPos( const QPointF & deltaPos )
{
	m_deltaPos	= deltaPos;
}
void DJGraphicsSvgItem::setPen( const QPen& pen )
{
	m_pen	= pen;
}
void DJGraphicsSvgItem::setDJSelected( bool selected )
{
	m_selected	= selected;
}
void DJGraphicsSvgItem::setMatrimoveMode( bool isMatrimoveMode )
{
	m_isMatrimoveMode	= isMatrimoveMode;
}
void DJGraphicsSvgItem::setAlignment( Qt::Alignment alignment )
{
	m_alignment	= alignment;
}
void DJGraphicsSvgItem::setVirtualPos ( const QPointF & vpos )
{
	m_virtualPos	= vpos;
}
void DJGraphicsSvgItem::setExternalScale( qreal externalScale )
{
	m_externalScale	= externalScale;
	qreal totalScale	= m_externalScale * m_internalScale;
	setMatrix( QMatrix(totalScale, 0, 0, totalScale, 0, 0) );
	rotate( m_angleOfRotation );
}
void DJGraphicsSvgItem::setInternalScale( qreal internalScale )
{
	m_internalScale	= internalScale;
	qreal totalScale	= m_externalScale * m_internalScale;
	setMatrix( QMatrix(totalScale, 0, 0, totalScale, 0, 0) );
	rotate( m_angleOfRotation );
}
void DJGraphicsSvgItem::setAngleOfRotation( qreal angleOfRotation )
{
	m_angleOfRotation	= angleOfRotation;
	qreal totalScale	= m_externalScale * m_internalScale;
	setMatrix( QMatrix(totalScale, 0, 0, totalScale, 0, 0) );
	rotate( m_angleOfRotation );
}
void DJGraphicsSvgItem::adjustPos( const QMatrix& m )
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
