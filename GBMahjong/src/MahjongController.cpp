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

#include "MahjongController.h"
#include "DJGameRoom.h"
#include "mj2profile.h"
#include "GBMahjong.h"
//#include "MahjongUserItem.h"

MahjongController::MahjongController( quint16 gameId, const QString& gameName, const DJHallInterface& hallInterface, QObject *parent )
	: DJGameController( gameId, gameName, hallInterface, parent )
{
	tr("Mahjong");
}
MahjongController::~MahjongController()
{
}
QString	MahjongController::translatedGameName() const
{
	return tr("Mahjong");
}
QString MahjongController::roomName( DJGameRoom* normalRoom ) const
{
	QString name	= DJGameController::roomName( normalRoom );
	
	MahjongRoom *room	= (MahjongRoom *)normalRoom->privateRoom();
	
	if( MAHJONG_RULE_HU_MINTERM_FAN == room->rule.chHUMinTerm && room->rule.chHUMinTermFAN > 0) {
		name += QString("--%1").arg(room->rule.chHUMinTermFAN);
		name += tr("fan");
	}
    if(room->rule.chHURule1 & MAHJONG_RULE_HU1_MUSTDIY)
        name += tr("(hu by yourself)");
            
	return name;
}
QString MahjongController::roomTitle() const
{
	return tr("mahjong room");
}
QString MahjongController::roomDescription( DJGameRoom* normalRoom ) const
{
	MahjongRoom *room	= (MahjongRoom *)normalRoom->privateRoom();
	
	QString description	= tr("This is GB Mahjong room");
	
	if( MAHJONG_RULE_HU_MINTERM_FAN == room->rule.chHUMinTerm && room->rule.chHUMinTermFAN > 0) {
		description	+= tr(", Hu needs %1 fan at least").arg(room->rule.chHUMinTermFAN);
	}
	description	+= ".";
	return description;
}
DJGamePanel* MahjongController::createGamePanel( DJGameController* gameController, DJGameRoom* normalRoom, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f )
{
	return new MahjongPanel (gameController, normalRoom, userIds, tableId, seatId, parent, f );
}

