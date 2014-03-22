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

#include "ChineseChessController.h"
#include "DJGameRoom.h"
#include "xqpanel.h"
#include "Utility.h"
ChineseChessController::ChineseChessController( quint16 gameId, const QString& gameName, const DJHallInterface& hallInterface, QObject *parent )
	: DJGameController( gameId, gameName, hallInterface, parent )
{
}
ChineseChessController::~ChineseChessController()
{
}
QString	ChineseChessController::translatedGameName() const
{
	return tr("ChineseChess");
}
QSize ChineseChessController::cellSize( quint8 numberOfSeats ) const
{
	return QSize( 240, 160 );
}
QString ChineseChessController::roomName( DJGameRoom* normalRoom ) const
{
	PXiangqiRoom pxqroom = (PXiangqiRoom)normalRoom->privateRoom();
	QString name = DJGameController::roomName( normalRoom );
	if(pxqroom->rule.chRuleMask == 0)
	{
		if(ltoh4(pxqroom->rule.shBaseTime) < 60)
			name +=tr("--Fastest");	//速战速决
		else
		{
			name += QString("--%1 ").arg(ltoh2(pxqroom->rule.shBaseTime)/60);
			name += tr("min. room");//X分钟快棋 (比如10分钟快棋)
			name += tr("(%1 times preserved)").arg(pxqroom->rule.chTimes);
		}
	}else
	{
		name += tr("--Free time");//标准房间
	}
	//name += QString("(%1/%2)").arg(normalRoom->curUsers()).arg(normalRoom->maxUsers());
	return name;
}
DJGamePanel* ChineseChessController::createGamePanel( DJGameController* gameController, DJGameRoom* normalRoom, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f )
{
	return new XiangqiPanel( gameController, normalRoom, userIds, tableId, seatId, parent, f );
}
