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

#include "DJTreeItemDelegate.h"
#include "DJGlobal.h"

DJTreeItemDelegate::DJTreeItemDelegate( QObject *parent, Direction direction )
    : QItemDelegate(parent),m_direction(direction)
{
	djDebug()<<"DJTreeItemDelegate constructor";
	
	m_selfStops << QGradientStop( 0, QColor(224,238,255) )
				<< QGradientStop( 0.5, QColor(144,194,255) )
				<< QGradientStop( 1, QColor(224,238,255) );
				
    m_otherStops << QGradientStop( 0, Qt::white )
				<< QGradientStop( 1, QColor(225,225,225) );
}
DJTreeItemDelegate::~DJTreeItemDelegate()
{
	djDebug()<<"DJTreeItemDelegate destructor";
}
void DJTreeItemDelegate::setSelfStops( const QGradientStops& stops )
{
	m_selfStops	= stops;
}
void DJTreeItemDelegate::setOtherStops( const QGradientStops& stops )
{
	m_otherStops	= stops;
}
void DJTreeItemDelegate::paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	QPoint	start,final;
	switch( m_direction ) {
	case TopLeft2BottomRight:
		start	= option.rect.topLeft();
		final	= option.rect.bottomRight();
		break;
	case TopRight2BottomLeft:
		start	= option.rect.topRight();
		final	= option.rect.bottomLeft();
		break;
	case TopLeft2BottomLeft:
	default:
		start	= option.rect.topLeft();
		final	= option.rect.bottomLeft();
		break;
	}
	QLinearGradient linearGradient( start, final );
	
	QTreeWidgetItem*	item	= static_cast<QTreeWidgetItem*>(index.internalPointer());
	if ( QTreeWidgetItem::UserType == item->type() ) {
		linearGradient.setStops( m_selfStops );
		painter->fillRect(option.rect,linearGradient);
	}else {
		//linearGradient.setStops( m_otherStops );
	}
	QItemDelegate::paint(painter,option,index);
}
