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

#ifndef GAMEUTILITY_H
#define GAMEUTILITY_H

#include <QtCore>
#include "DJBaseGlobal.h"
#include "protocol.h"

DJ_BASE_EXPORT QByteArray MakeGameData( quint16 gameId, quint8 language, quint8 subType, const QByteArray& gameData = QByteArray(), quint8 type = DJGAME_PROTOCOL_TYPE_SUBMIT_GAMEDATA );
DJ_BASE_EXPORT inline quint32 MakeGroupId( quint16 gameId, quint8 roomId = 0, quint8 tableId = 0 )
{
	return gameId << 16 | roomId << 8 | tableId;
}
DJ_BASE_EXPORT inline quint8 ExtractGameClassId( quint32 groupId )
{ 
	return (groupId & 0x0FF000000) >> 24;
}
DJ_BASE_EXPORT inline quint8 ExtractGameGameId( quint32 groupId )
{ 
	return (groupId & 0x00FF0000) >> 16;
}
DJ_BASE_EXPORT inline quint8 ExtractRoomId( quint32 groupId )
{ 
	return (groupId & 0x0000FF00) >> 8;
}
DJ_BASE_EXPORT inline quint8 ExtractTableId( quint32 groupId )
{
	return groupId & 0x000000FF;
}
DJ_BASE_EXPORT bool GroupIsTable( quint32 groupId );
DJ_BASE_EXPORT bool GroupIsRoom( quint32 groupId );
DJ_BASE_EXPORT bool GroupIsAdmin( quint32 groupId );
#endif
