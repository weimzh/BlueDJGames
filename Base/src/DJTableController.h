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

#ifndef DJTABLECONTROLLER_H
#define DJTABLECONTROLLER_H

#include <QtGui>
#include "DJBaseGlobal.h"

class DJRoomController;
class DJRoomCellItem;
class DJGameUser;
class DJPanelController;
class DJGameController;
class DJGameRoom;

class DJ_BASE_EXPORT DJTableController : public QObject
{
    Q_OBJECT 
public:
    DJTableController( quint8 tableId, DJRoomController* roomController );
    virtual ~DJTableController();
	
	quint8 tableId() const { return m_tableId; }
	DJRoomCellItem* cellItem() const;
	void setTableStatus( uint tableStatus );
	void synchronize( uint tableStatus, const QList<DJGameUser*>& tableMapUsers = QList<DJGameUser*>() );
	void addUser( DJGameUser* user );
	void removeUser( DJGameUser* user );
	DJGameUser* userAtSeat( uint seatId ) const;
	QList<DJGameUser*> allUsers() const;
	bool isPreceding( DJGameUser *user1, DJGameUser *user2 );
		
	DJRoomController* roomController() const { return m_roomController; }
	DJPanelController* panelController() const { return m_panelController; }
	void setPanelController( DJPanelController* panelController );
private:	
	void deletePanelController();
	void print() const;
private:
	quint8					m_tableId;
	DJRoomController		*m_roomController;
	DJRoomCellItem			*m_cellItem;
	QList<DJGameUser*>		m_orderedUsers;
	DJPanelController		*m_panelController;
};

#endif
