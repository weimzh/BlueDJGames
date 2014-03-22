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

#include "DJGameRoom.h"

#include "Utility.h"

DJGameRoom::DJGameRoom( const NormalRoomDes *room, int size )
{
	djDebug() << "DJGameRoom constructor";
	
	const GeneralRoomDes	*pGeneralRoomDes	= &room->general;
	
	m_roomId	= pGeneralRoomDes->chRoomID;
	m_numberOfTables	= pGeneralRoomDes->chTables;
	m_numberOfSeats	= pGeneralRoomDes->chSites;
	m_options	= pGeneralRoomDes->chOptions;
	
	m_maxUsers	= pGeneralRoomDes->chMaxUsers;
	m_curUsers	= letoh2( pGeneralRoomDes->shCurUsers );
	
	m_playerLowestScore	= letoh4( pGeneralRoomDes->iPlayerLowestScore );
	m_playerHighestScore	= letoh4( pGeneralRoomDes->iPlayerHighestScore );
	m_multiple	= pGeneralRoomDes->chMultiple;
	
	quint8 generalSize	= pGeneralRoomDes->chSize;
	djDebug() << "room general size = " << generalSize << m_curUsers << m_maxUsers;
	
	m_privateRoom	= QByteArray( reinterpret_cast<const char *>( reinterpret_cast<const char *>(pGeneralRoomDes) + generalSize ), size );
}
DJGameRoom::DJGameRoom( const DJGameRoom& other )
{
	djDebug() << "DJGameRoom constructor";
	
	init( other );
}
DJGameRoom&	DJGameRoom::operator=( const DJGameRoom& other )
{
	init( other );
	return *this;
}
DJGameRoom::~DJGameRoom()
{
	djDebug() << "DJGameRoom destructor";
}
const char* DJGameRoom::privateRoom() const
{
	return m_privateRoom.data();
}
void DJGameRoom::init( const DJGameRoom& other )
{
	m_roomId	= other.m_roomId;
	m_numberOfTables	= other.m_numberOfTables;
	m_numberOfSeats	= other.m_numberOfSeats;
   	m_options	= other.m_options;
   	m_maxUsers	= other.m_maxUsers;
   	m_curUsers	= other.m_curUsers;
   	m_playerLowestScore	= other.m_playerLowestScore;
	m_playerHighestScore	= other.m_playerHighestScore;
	m_multiple	= other.m_multiple;
   	m_privateRoom	= other.m_privateRoom;
}
