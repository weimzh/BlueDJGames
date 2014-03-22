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

#include "DJDesktopItem.h"
#include "DJGraphicsPixmapItem.h"
#include "DJGraphicsSvgItem.h"

DJDesktopItem::DJDesktopItem( int seat, int type, int value )
{
	djDebug()<<"DJDesktopItem constructor"<<seat<<type<<hex<<value;
	
	m_seat	= seat;
	m_type	= type;
	m_value	= value;
	m_item	= 0;
}
DJDesktopItem::~DJDesktopItem()
{
	djDebug()<<"DJDesktopItem destructor"<<m_seat<<m_type<<hex<<m_value;
	
	if ( m_item )
		delete m_item;
	m_item	= 0;
}
void DJDesktopItem::setType( int type )
{
	m_type	= type;
}
void DJDesktopItem::setGraphicsItem( QGraphicsItem *item )
{
	if ( m_item )
		delete m_item;
	m_item	= item;
	if ( item ) {
		item->setData( KEY_OF_SEAT, m_seat );
		item->setData( KEY_OF_TYPE, m_type );
		item->setData( KEY_OF_VALUE, m_value );
	}
}
