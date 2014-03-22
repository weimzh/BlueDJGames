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

#ifndef DJDESKTOPITEM_H
#define DJDESKTOPITEM_H

#include <QtGui>
#include "DJBaseGlobal.h"

#define	KEY_OF_SEAT		0
#define KEY_OF_TYPE		1
#define KEY_OF_VALUE	2

class DJ_BASE_EXPORT DJDesktopItem
{
public:
	DJDesktopItem( int seat, int type, int value );
    virtual ~DJDesktopItem();
    
    int seat() const { return m_seat; }
    int type() const { return m_type; }
    void setType( int type );
    int value() const { return m_value; }
    QGraphicsItem* graphicsItem() const { return m_item; }
    void setGraphicsItem( QGraphicsItem *item );
private:
	int				m_seat;
	int				m_type;
	int				m_value;
	QGraphicsItem	*m_item;
};

#endif
