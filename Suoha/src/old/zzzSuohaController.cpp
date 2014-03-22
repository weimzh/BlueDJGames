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

#include "SuohaController.h"
#include "DJGameRoom.h"
#include "shprofile.h"
#include "suoha.h"
#include "Utility.h"

SuohaController::SuohaController( quint16 gameId, const QString& gameName, const DJHallInterface& hallInterface, QObject *parent )
	: DJGameController( gameId, gameName, hallInterface, parent )
{
}
SuohaController::~SuohaController()
{
}
QString	SuohaController::translatedGameName() const
{
	return tr("Suoha");
}
QColor SuohaController::nameColor() const
{
	return Qt::yellow;
}
QString SuohaController::roomName( DJGameRoom* normalRoom ) const
{
	QString name	= DJGameController::roomName( normalRoom );
	PSuohaRoom pshroom = (PSuohaRoom)normalRoom->privateRoom();
	name += QString("-[%1/%2/%3]").arg(letoh4(pshroom->uMinYazhu)).
							arg(pshroom->chMaxMulti).
							arg(letoh4(pshroom->iMinCounter));

	return name;
}
QString	SuohaController::roomDescription( DJGameRoom* normalRoom ) const
{
	QString chipName	= baseSectionName(Chips);
	PSuohaRoom pshroom = (PSuohaRoom)normalRoom->privateRoom();
	return tr("room info : %1 %4 per wager, max multiple is %2 per wager, require %3 %4 at least")
						.arg(letoh4(pshroom->uMinYazhu))
						.arg(letoh4(pshroom->chMaxMulti))
						.arg(letoh4(pshroom->iMinCounter))
						.arg(chipName);
}
DJGamePanel* SuohaController::createGamePanel( DJGameController* gameController, DJGameRoom* normalRoom, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f )
{
	return new SuohaPanel( gameController, normalRoom, userIds, tableId, seatId, parent, f );
}
