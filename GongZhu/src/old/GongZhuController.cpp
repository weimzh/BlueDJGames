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

#include "GongZhuController.h"
#include "DJGameRoom.h"
#include "gongzhu.h"

GongZhuController::GongZhuController( quint16 gameId, const QString& gameName, const DJHallInterface& hallInterface, QObject *parent )
	: DJGameController( gameId, gameName, hallInterface, parent )
{
}
GongZhuController::~GongZhuController()
{
}
QString	GongZhuController::translatedGameName() const
{
	return tr("GongZhu");
}
QString GongZhuController::roomName( DJGameRoom* normalRoom ) const
{
	QString name = DJGameController::roomName( normalRoom );
	PGongzhuRoom pgozhroom = (PGongzhuRoom)(normalRoom->privateRoom());
	if(pgozhroom->rule.chCardPages == 2)
		name += tr("-[2pig]");
	else
		name += tr("-[1pig]");
	if(pgozhroom->rule.chTeams == 2)
		name +=tr("[Team]");
	return name;
}
DJGamePanel* GongZhuController::createGamePanel( DJGameController* gameController, DJGameRoom* normalRoom, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f )
{
	return new GongzhuPanel( gameController, normalRoom, userIds, tableId, seatId, parent, f );
}
