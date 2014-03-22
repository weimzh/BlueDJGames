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

#ifndef DJROOMPLAYERITEM_H
#define DJROOMPLAYERITEM_H

#include <QtGui>
#include "DJBaseGlobal.h"

class DJRoomController;
class DJGameUser;
class DJRoomPlayerAvatarItem;
class DJRoomPlayerNameItem;

class DJ_BASE_EXPORT DJRoomPlayerItem
{
public:
	DJRoomPlayerItem( DJRoomController *roomController, DJGameUser *user, const QFont& font = QFont(), const QColor& color = Qt::black, QGraphicsItem * parent = 0, QGraphicsScene * scene = 0 );
    virtual ~DJRoomPlayerItem();
    
    DJRoomPlayerAvatarItem* avatarItem() const;
    DJRoomPlayerNameItem*	nameItem() const;
    void reload();
    void update();
private:
	DJRoomPlayerAvatarItem		*m_avatarItem;
	DJRoomPlayerNameItem		*m_nameItem;
};

#endif
