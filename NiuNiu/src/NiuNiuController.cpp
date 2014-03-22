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
#include "DJGameUserItem.h"
#include "NiuNiuPanelController.h"
#include "DJGameUser.h"
#include "Utility.h"
#include "nnprofile.h"

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
QSize NiuNiuController::desktopSize() const
{
	return QSize(850,850);
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
DJPanelController* NiuNiuController::createPanelController( DJTableController *tc, quint8 seatId )
{
	return new NiuNiuPanelController( tc, seatId );
}
