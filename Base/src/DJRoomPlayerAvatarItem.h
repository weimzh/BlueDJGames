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

#ifndef DJROOMPLAYERAVATARITEM_H
#define DJROOMPLAYERAVATARITEM_H

#include <QtGui>
#include "DJBaseGlobal.h"

class DJRoomController;
class DJGameUser;

class DJ_BASE_EXPORT DJRoomPlayerAvatarItem: public QObject, public QGraphicsPixmapItem
{
	Q_OBJECT
public:
	DJRoomPlayerAvatarItem( DJRoomController *roomController, DJGameUser *user, QGraphicsItem * parent = 0, QGraphicsScene * scene = 0 );
    virtual ~DJRoomPlayerAvatarItem();
    
    DJGameUser* user() const;
	virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
protected:
	virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
	virtual void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event );
signals:
	void playerClicked( DJGameUser* user, Qt::MouseButtons buttons );
	void playerDoubleClicked( DJGameUser* user, Qt::MouseButtons buttons );
private:
	DJRoomController			*m_roomController;
	DJGameUser	*m_user;
};

#endif
