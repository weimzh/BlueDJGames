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

#include "GameUtility.h"

//make game data
DJ_BASE_EXPORT QByteArray MakeGameData( quint16 gameId, quint8 language, quint8 subType, const QByteArray& gameData, quint8 type )
{
	QByteArray	buf( sizeof(DJGameProtocol) + sizeof(GameDataHead) + gameData.size(), 0 );
	DJGameProtocol *protocol	= reinterpret_cast<DJGameProtocol *>(buf.data());
	protocol->chTotalLen	= buf.size();
	protocol->chType	= type;
	protocol->chSubType	= subType;
	protocol->chLanguage= language;
	GameDataHead	*gameHead	= reinterpret_cast<GameDataHead *>(protocol->chBuf);
	gameHead->chGameClass	= gameId >> 8;
	gameHead->chGame		= gameId & 0xFF;
	memcpy( gameHead->chBuf, gameData.data(), gameData.size() );
	return buf;
}
DJ_BASE_EXPORT bool GroupIsTable( quint32 groupId )
{
	quint8 gameClassId	= ExtractGameClassId(groupId);
	quint8 gameGameId	= ExtractGameGameId(groupId);
	quint8 roomId	= ExtractRoomId(groupId);
	quint8 tableId	= ExtractTableId( groupId );
	if ( tableId > DJGAME_MAX_TABLES || 0 == tableId )
		return false;
	return  gameClassId && gameGameId && roomId && tableId;
}
DJ_BASE_EXPORT bool GroupIsRoom( quint32 groupId )
{
	quint8 gameClassId	= ExtractGameClassId(groupId);
	quint8 gameGameId	= ExtractGameGameId(groupId);
	quint8 roomId	= ExtractRoomId(groupId);
	quint8 tableId	= ExtractTableId( groupId );
	if ( 0 != tableId )
		return false;
	return gameClassId && gameGameId && roomId;
}
DJ_BASE_EXPORT bool GroupIsAdmin( quint32 groupId )
{
	quint8 gameClassId	= ExtractGameClassId(groupId);
	quint8 gameGameId	= ExtractGameGameId(groupId);
	quint8 roomId	= ExtractRoomId(groupId);
	quint8 tableId	= ExtractTableId( groupId );
	if ( DJGAME_ADMINISTRATOR_TABLEGROUP == tableId
		&& 0 == roomId ) {
		return gameClassId && gameGameId;
	}
	return false;
}


