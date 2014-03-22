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

#include "LandBattleController.h"
#include "DJGameRoom.h"
#include "DJGameUserItem.h"
#include "LandBattle.h"
#include "4country.h"

LandBattleController::LandBattleController( quint16 gameId, const QString& gameName, const DJHallInterface& hallInterface, QObject *parent )
	: DJGameController( gameId, gameName, hallInterface, parent )
{
}
LandBattleController::~LandBattleController()
{
}
QString LandBattleController::userContextInfo( DJGameUser* user ) const
{
	DJGameUserItem	*userItem	= gameUserItem( user->userId() );
	QString rank;
	if ( userItem ) {
		int index	= indexOfSection( Rank );
		rank	= userItem->text( index );
	}
	QString	info;
	info	+= user->userName() + " : " + rank;
	info	+= "\n";
	info	+= baseSectionName(NickName) + " : " + user->nickName();
	info	+= "\n";
	DJGameRoom	*room	= selfRoom();
	if ( room && room->options() & DJGAME_ROOMOPTION_COUNTER ) {
		info	+= baseSectionName(Money)+ QString(":%1").arg(user->money()) + " ";
		info	+= baseSectionName(Chips)+ QString(":%1").arg(user->chips()) + "\n";
	}else {
		info	+= baseSectionName(Wins)+ QString(":%1").arg(user->wins()) + " ";
		info	+= baseSectionName(Loses)+ QString(":%1").arg(user->loses()) + " ";
		info	+= baseSectionName(Draws)+ QString(":%1").arg(user->draws()) + "\n";
		
		info	+= baseSectionName(Score)+ QString(":%1").arg(user->score()) + "\n";
		
		info	+= baseSectionName(Money)+ QString(":%1").arg(user->money()) + " ";
		info	+= baseSectionName(Chips)+ QString(":%1").arg(user->chips()) + "\n";
	}
	return info;
}
QString	LandBattleController::translatedGameName() const
{
	return tr("LandBattle");
}
QColor LandBattleController::nameColor() const
{
	return QColor(255,128,0);
}
QString LandBattleController::roomName( DJGameRoom* normalRoom ) const
{
	QString roomName=QString("");
	bool bOK=false;
	PJunqiRoom room = (PJunqiRoom)normalRoom->privateRoom();
	
	if(room->chRule == DJGAME_JUNQI_RULE_RAND)
	{
		bOK = true;
		roomName = tr("Normal BattleChess");
	}else if(room->chRule == DJGAME_JUNQI_RULE_LUAN)
	{
		bOK = true;
		roomName = tr("4 Luan");
	}else if(room->chRule & (DJGAME_JUNQI_RULE_4TEAM) )
	{
		if(room->chRule == (DJGAME_JUNQI_RULE_4TEAM | DJGAME_JUNQI_RULE_LUAN) )
		{
			bOK = true;
			roomName = tr("4 HunLuan");
		}else if(room->chRule == DJGAME_JUNQI_RULE_4TEAM ||  room->chRule == (DJGAME_JUNQI_RULE_4TEAM | DJGAME_JUNQI_RULE_RAND))
		{
			bOK = true;
			roomName = tr("4 Hun");
		}
	}
	if(!bOK)
	{
		if(room->chRule & DJGAME_JUNQI_RULE_LUAN)
			roomName += tr("[Luan]");
		if(room->chRule & DJGAME_JUNQI_RULE_4TEAM)
			roomName += tr("[Hun]");
	}
	if(room->chRule & DJGAME_JUNQI_RULE_RAND)
	{
		roomName += tr("[2-4P]");
	}else
	{
		QString str= QString("%1").arg(normalRoom->numberOfSeats());
		roomName += QString("[");
		roomName +=str;
		roomName += tr("P");
		roomName += QString("]");
	}
	if(room->chRule & (DJGAME_JUNQI_RULE_FRIENDVISIBLE) )
	{
		roomName += tr("[FriendVisible] ");
	}
	if(room->chRule & (DJGAME_JUNQI_RULE_FRIENDROW) )
	{
		roomName += tr("[FriendRow] ");
	}
	//roomName += QString(" -- (%1/%2)").arg(normalRoom->curUsers()).arg(normalRoom->maxUsers());
	return roomName;
}
DJGamePanel* LandBattleController::createGamePanel( DJGameController* gameController, DJGameRoom* normalRoom, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f )
{
	return new LandBattlePanel( gameController, normalRoom, userIds, tableId, seatId, parent, f );
}
void LandBattleController::gameUserItemHeaderSections( QList<int> &sectionIds, QStringList &sectionNames )
{
	sectionIds.clear();
	sectionNames.clear();
	
	DJGameRoom	*room	= selfRoom();
	if ( room && room->options() & DJGAME_ROOMOPTION_COUNTER ) {
		sectionIds << UserName << NickName << GameStatus 
				<< TableSeat 
#ifndef DJ_OEM				
				<< Platform
#endif
				<< Speed << Unwelcome
				<< Chips << BreakRate << OnlineTime << Rank;
	}else {
		sectionIds << UserName << NickName << GameStatus 
				<< TableSeat 
#ifndef DJ_OEM				
				<< Platform
#endif 
				<< Speed << Unwelcome
				<< Score << Wins << Loses << Draws << WinRate << Money
				<< Chips << BreakRate << OnlineTime << Rank;
	}
	for ( int i = 0; i < sectionIds.size(); ++i ) {
		sectionNames << baseSectionName( sectionIds.at(i) );
	}
	sectionNames << "-";
}
