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

#include "XQController.h"
#include "DJGameRoom.h"
#include "DJGameUserItem.h"
#include "XQPanelController.h"
#include "DJGameUser.h"
#include "Utility.h"
#include "xqprofile.h"

XQController::XQController( quint16 gameId, const QString& gameName, const DJHallInterface& hallInterface, QObject *parent )
	: DJGameController( gameId, gameName, hallInterface, parent )
{
}
XQController::~XQController()
{
}
QString XQController::userContextInfo( DJGameUser* user ) const
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
QString	XQController::translatedGameName() const
{
	return tr("ChineseChess");
}
QSize XQController::desktopSize() const
{
	return QSize( 820, 800 );
}
QString XQController::roomName( DJGameRoom* normalRoom ) const
{
	QString name	= DJGameController::roomName( normalRoom );
	PXiangqiRoom pxqroom = (PXiangqiRoom)normalRoom->privateRoom();
	if(pxqroom->rule.chRuleMask == 0)
	{
		if(letoh4(pxqroom->rule.shBaseTime) < 60)
			name +=tr("--Fastest");	//速战速决
		else
		{
			name += QString("--%1 ").arg(letoh2(pxqroom->rule.shBaseTime)/60);
			name += tr("min. room");//X分钟快棋 (比如10分钟快棋)
			name += tr("(%1 times preserved)").arg(pxqroom->rule.chTimes);
		}
	}else
	{
		name += tr("--Free time");//标准房间
	}
	return name;
}
DJPanelController* XQController::createPanelController( DJTableController *tc, quint8 seatId )
{
	return new XQPanelController( tc, seatId );
}
void XQController::gameUserItemHeaderSections( QList<int> &sectionIds, QStringList &sectionNames )
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
