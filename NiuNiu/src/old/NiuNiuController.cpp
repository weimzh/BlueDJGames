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

#include "NiuNiuController.h"
#include "DJGameRoom.h"
#include "nnprofile.h"
#include "niuniu.h"
#include "Utility.h"

NiuNiuController::NiuNiuController( quint16 gameId, const QString& gameName, const DJHallInterface& hallInterface, QObject *parent )
	: DJGameController( gameId, gameName, hallInterface, parent )
{
}
NiuNiuController::~NiuNiuController()
{
}
QString	NiuNiuController::translatedGameName() const
{
	return tr("NiuNiu");
}
QColor NiuNiuController::nameColor() const
{
	return Qt::yellow;
}
QString NiuNiuController::roomName( DJGameRoom* normalRoom ) const
{
	QString name	= DJGameController::roomName( normalRoom );
	PNiuniuRoom pnnroom = (PNiuniuRoom)normalRoom->privateRoom();
	name += QString("-[%1/%2/%3]").arg(letoh4(pnnroom->uMinYazhu)).
							arg(pnnroom->chMaxMulti).
							arg(letoh4(pnnroom->iMinCounter));

	return name;
}
/*
QString	NiuNiuController::roomDescription( DJGameRoom* normalRoom ) const
{
	QString chipName	= baseSectionName(Chips);
	PNiuniuRoom pnnroom = (PNiuniuRoom)normalRoom->privateRoom();
	return tr("room info : %1 %4 per wager, max multiple is %2 per wager, require %3 %4 at least")
						.arg(letoh4(pnnroom->uMinYazhu))
						.arg(letoh4(pnnroom->chMaxMulti))
						.arg(letoh4(pnnroom->iMinCounter))
						.arg(chipName);
}
*/
DJGamePanel* NiuNiuController::createGamePanel( DJGameController* gameController, DJGameRoom* normalRoom, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f )
{
	return new NiuniuPanel( gameController, normalRoom, userIds, tableId, seatId, parent, f );
}
