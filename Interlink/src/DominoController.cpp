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

#include "DominoController.h"
#include "DJGameRoom.h"
#include "DJGameUserItem.h"
#include "DominoPanelController.h"
#include "DJGameUser.h"
#include "Utility.h"
#include "dmnprofile.h"

DominoController::DominoController( quint16 gameId, const QString& gameName, const DJHallInterface& hallInterface, QObject *parent )
	: DJGameController( gameId, gameName, hallInterface, parent )
{
}
DominoController::~DominoController()
{
}
QString	DominoController::translatedGameName() const
{
	return tr("Domino");
}
QSize DominoController::desktopSize() const
{
	return QSize( 700, 700 );
}
QString DominoController::roomName( DJGameRoom* normalRoom ) const
{
	PDominoRoom pdmnroom = (PDominoRoom)normalRoom->privateRoom();
	QString name = DJGameController::roomName( normalRoom );
	if(pdmnroom->rule.chCards == DOMINO_DOUBLE9_CARDS)
	{
		name += tr("--Double 9");
	}else 
	{
		name += tr("--Double 6");
	}
	if(normalRoom->numberOfSeats() == 4)
	{
		name += tr("[4p]");
	}
	return name;
}
DJPanelController* DominoController::createPanelController( DJTableController *tc, quint8 seatId )
{
	return new DominoPanelController( tc, seatId );
}


