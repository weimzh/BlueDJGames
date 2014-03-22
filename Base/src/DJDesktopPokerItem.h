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

#ifndef DJDESKTOPPOKERITEM_H
#define DJDESKTOPPOKERITEM_H

#include <QtGui>
#include "DJBaseGlobal.h"
#include "DJDesktopItem.h"
#include "DJPoker.h"

class DJ_BASE_EXPORT DJDesktopPokerItem : public DJDesktopItem
{
public:
	DJDesktopPokerItem( int seat, int type, int value, bool isBridgeCard );
    virtual ~DJDesktopPokerItem();
    
    const DJPoker& poker() const { return m_poker; }
    
    int distance() const { return m_distance; }
    void setDistance( int distance ) { m_distance	= distance; }
    
    int length() const { return m_length; }
    void setLength( int length ) { m_length	= length; }
    
    bool isBack() const { return m_isBack; }
    void setBack( bool flag ) { m_isBack = flag; }
private:
	DJPoker		m_poker;
	int			m_distance;
	int 		m_length;
	bool		m_isBack;
};

#endif
