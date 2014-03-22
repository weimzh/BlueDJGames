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

#ifndef DJGAMEROOM_H
#define DJGAMEROOM_H

#include <QtCore>
#include "DJBaseGlobal.h"
#include "protocol.h"

class DJ_BASE_EXPORT DJGameRoom
{
public:
    DJGameRoom( const NormalRoomDes *room ,int size );
    DJGameRoom( const DJGameRoom& other);
    virtual ~DJGameRoom();
    DJGameRoom&	operator=( const DJGameRoom& other );
    
	quint8	roomId() const { return m_roomId; }
	quint8	numberOfTables() const { return m_numberOfTables; }
	quint8	numberOfSeats() const { return m_numberOfSeats; }
	quint8	options() const { return m_options; }
	quint16	maxUsers() const { return m_maxUsers; }
	quint16	curUsers() const { return m_curUsers; }
	
	int playerLowestScore() const { return m_playerLowestScore; }
	int playerHighestScore() const { return m_playerHighestScore; }
	quint8 multiple() const { return m_multiple; }
	
	void setCurUsers( quint16 curUsers ) { m_curUsers	= curUsers; }
	const char*	privateRoom() const;
private:
	void init( const DJGameRoom& other );

   	quint8	m_roomId;
	quint8	m_numberOfTables;
	quint8	m_numberOfSeats;
	quint8	m_options;
	qint16	m_maxUsers;
	qint16	m_curUsers;
	
	int		m_playerLowestScore;
	int		m_playerHighestScore;
	quint8	m_multiple;
	QByteArray	m_privateRoom;
};

#endif
