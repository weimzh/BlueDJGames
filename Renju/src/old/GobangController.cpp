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

//Please visit http://www.bluedj.com for more infomation about us
//and contact us at ggwizard@gmail.com or darkdong@gmail.com
//****************************************************************************/

#include "RenjuController.h"
#include "DJGameRoom.h"
#include "DJGameUserItem.h"
#include "Renju.h"
#include "RenjuRule.h"
#include "gobangprofile.h"

RenjuController::RenjuController( quint16 gameId, const QString& gameName, const DJHallInterface& hallInterface, QObject *parent )
	: DJGameController( gameId, gameName, hallInterface, parent )
{
}
RenjuController::~RenjuController()
{
}
QString RenjuController::userContextInfo( DJGameUser* user ) const
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
QString	RenjuController::translatedGameName() const
{
	return tr("Renju");
}
QString RenjuController::roomName( DJGameRoom* normalRoom ) const
{
	QString name;
	
	name = DJGameController::roomName( normalRoom );
	PRenjuRoom pgobangroom = (PRenjuRoom)(normalRoom->privateRoom());
	bool b1 = false;
	bool b2 = false;
	bool b3 = false;
	if(pgobangroom->rule.chRuleSet & RENJU_RULESET_HASFORBID)
	{
		b1 = true;
	}
	if(pgobangroom->rule.chRuleSet & RENJU_RULESET_EXCHANGE)
	{
		b2 = true;
	}
	if(pgobangroom->rule.chRuleSet & RENJU_RULESET_DOUBLE)
	{
		b3 = true;
	}
	name+="-";
	if(!b1 && !b2 && !b3)
		name += tr("No Restriction");
	else if ( b1 && b2 && b3 )
		name += tr("RIF Rules");
	else if ( b1 )
		name += tr("Restrict For Black ");
	else if ( b2 )
		name += tr("3rd is exchangeable ");
	else if ( b3 )
		name += tr("5th can be choosed ");
	return name;
}
DJGamePanel* RenjuController::createGamePanel( DJGameController* gameController, DJGameRoom* normalRoom, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f )
{
	return new RenjuPanel( gameController, normalRoom, userIds, tableId, seatId, parent, f );
}
void RenjuController::gameUserItemHeaderSections( QList<int> &sectionIds, QStringList &sectionNames )
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
