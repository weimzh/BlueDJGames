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

#include "BlackjackController.h"
#include "DJGameRoom.h"
#include "bjprofile.h"
#include "blackjack.h"
#include "Utility.h"

BlackjackController::BlackjackController( quint16 gameId, const QString& gameName, const DJHallInterface& hallInterface, QObject *parent )
	: DJGameController( gameId, gameName, hallInterface, parent )
{
}
BlackjackController::~BlackjackController()
{
}
QString	BlackjackController::translatedGameName() const
{
	return tr("Blackjack");
}
QColor BlackjackController::nameColor() const
{
	return Qt::yellow;
}
QString BlackjackController::roomName( DJGameRoom* normalRoom ) const
{
	djDebug()<<"BlackjackController::roomName";
	QString name	= DJGameController::roomName( normalRoom );
	PBlackjackRoom pbjroom = (PBlackjackRoom)normalRoom->privateRoom();

	//djDebug()<<letoh4(pbjroom->uChipFaceValue)<<pbjroom->chNormalChips<<pbjroom->chDouble
		//<<ltoh4(pbjroom->uGameMaxValues)<<ltoh4(pbjroom->iMinCounter);
	name += QString("-[%1/%2/%3]").arg(letoh4(pbjroom->uChipFaceValue)*pbjroom->chNormalChips).
							arg(letoh4(pbjroom->uGameMaxValues)).
							arg(letoh4(pbjroom->iMinCounter));

	return name;
}
/*
QString	BlackjackController::roomDescription( DJGameRoom* normalRoom ) const
{
	QString chipName	= baseSectionName(Chips);
	PBlackjackRoom pbjroom = (PBlackjackRoom)normalRoom->privateRoom();
	return tr("room info : %1 %4 per wager, max win/lose is %2 %4 per game, require %3 %4 at least")
						.arg(letoh4(pbjroom->uChipFaceValue)*pbjroom->chNormalChips)
						.arg(letoh4(pbjroom->uGameMaxValues))
						.arg(letoh4(pbjroom->iMinCounter))
						.arg(chipName);
}
*/
DJGamePanel* BlackjackController::createGamePanel( DJGameController* gameController, DJGameRoom* normalRoom, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f )
{
	return new BlackjackPanel( gameController, normalRoom, userIds, tableId, seatId, parent, f );
}
