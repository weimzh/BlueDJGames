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

#include "DJTableController.h"
#include "DJRoomController.h"
#include "DJGameController.h"
#include "DJRoomView.h"
#include "DJRoomCellItem.h"
#include "DJRoomTableItem.h"
#include "DJGameUser.h"
#include "DJPanelController.h"

DJTableController::DJTableController( quint8 tableId, DJRoomController* roomController )
	:QObject(roomController),m_tableId(tableId),m_roomController(roomController)
{	
	djDebug() << "DJTableController constructor";
	
	m_cellItem	= new DJRoomCellItem( tableId, roomController, 0, roomController->roomView()->scene() );
	
	m_panelController	= 0;
}

DJTableController::~DJTableController()
{
	djDebug() << "DJTableController destructor";
	
	delete m_cellItem;
	deletePanelController();
}
DJRoomCellItem* DJTableController::cellItem() const
{
	return m_cellItem;
}
void DJTableController::setTableStatus( uint tableStatus )
{
	DJRoomTableItem *tableItem	= m_cellItem->tableItem();
	tableItem->setStatus( tableStatus );
	tableItem->update();
}
void DJTableController::synchronize( uint tableStatus, const QList<DJGameUser*>& tableMapUsers )
{
	 djDebug() << "synchronize status="<<tableStatus;
	 djDebug() <<"tableMapUsers="<<tableMapUsers;
	 djDebug() <<"m_orderedUsers="<<m_orderedUsers;
	setTableStatus( tableStatus );
	//remove user who not in tableMapUsers
	QList<DJGameUser*>::iterator it1 = m_orderedUsers.begin();
	while ( it1 != m_orderedUsers.end() ) {
        DJGameUser*	user	= *it1;
        djDebug() << "m_orderedUsers id = " << user->userId();
        if ( !tableMapUsers.contains( user ) ) {
        	djDebug() << "remove user who not in tableMapUsers" << user->userName();
        	it1	= m_orderedUsers.erase( it1 );
        	//just set user status to idle?
        	user->setTrinity( 0, 0, GAME_STATUS_IDLE );
        }else
        	++it1;
    }
    //add user who not in m_orderedUsers
    QList<DJGameUser*>::const_iterator it2 = tableMapUsers.begin();
	while ( it2 != tableMapUsers.end() ) {
        DJGameUser*	user	= *it2;
        djDebug() << "tableMapUsers id = " << user->userId();
        if ( !m_orderedUsers.contains( user ) ) {
        	djDebug() << "add user who not in m_orderedUsers" << user->userName();
        	m_orderedUsers << user;
        	m_roomController->repaintPlayer( user );
        }
        ++it2;
    }
}
void DJTableController::addUser( DJGameUser* user )
{
	djDebug() <<"DJTableController::addUser"<<user;
	if ( userAtSeat( user->seatId() ) ) {
		//conflict user
		djDebug()<<"conflict user";
		m_roomController->gameController()->requestTableMap( m_tableId, m_tableId );
	}else {
		if ( !m_orderedUsers.contains( user ) ) {
			djDebug()<<"add to orderedUsers";
			m_orderedUsers << user;
		}
	}
}
void DJTableController::removeUser( DJGameUser* user )
{
	djDebug() <<"DJTableController::removeUser"<<user;
	int index	= m_orderedUsers.indexOf( user );
	if ( -1 != index ) {
		djDebug()<<"remove from orderedUsers";
		m_orderedUsers.removeAt( index );
    }
}
DJGameUser* DJTableController::userAtSeat( uint seatId ) const
{
	foreach( DJGameUser* user, m_orderedUsers ) {
		if ( seatId == user->seatId() ) {
			return user;
		}
	}
	return 0;
}
QList<DJGameUser*> DJTableController::allUsers() const
{
	//NOTE: users size is numberOfSeats + 1 
	//userIds[0] is always 0, no use
	//player is from 1 to numberOfSeats for convinence
	QList<DJGameUser*> users;
	for ( int i = 0; i <= m_roomController->numberOfSeats(); i++ ) {
		users << 0;
	}
	foreach( DJGameUser* user, m_orderedUsers ) {
		quint8 seat	= user->seatId();
		if ( seat )
			users[seat]	= user;
	}
	return users;
}
bool DJTableController::isPreceding( DJGameUser *user1, DJGameUser *user2 )
{
	return m_orderedUsers.indexOf(user1) < m_orderedUsers.indexOf(user2);
}
void DJTableController::setPanelController( DJPanelController* panelController )
{
	deletePanelController();
	m_panelController	= panelController;
}
void DJTableController::deletePanelController()
{
	if ( m_panelController ) {
		djDebug()<<"tableController has panelController";
		m_panelController->closeChildDialog();
		m_panelController->removePendingAcl();
		m_panelController->deleteLater();
		m_panelController	= 0;
	}
}
void DJTableController::print() const
{
	djDebug() << "m_orderedUsers size = " << m_orderedUsers.size();
    foreach( DJGameUser* user, m_orderedUsers ) {
		djDebug() << user->userId() << user->userName() << user->tableId() << user->seatId() << user->gameStatus();
	}
}
