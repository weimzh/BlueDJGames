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

#include "TLJController.h"
#include "DJGameRoom.h"
#include "TLJPanelController.h"
#include "DJGameUser.h"
#include "Utility.h"
#include "ug2profile.h"

TLJController::TLJController( quint16 gameId, const QString& gameName, const DJHallInterface& hallInterface, QObject *parent )
	: DJGameController( gameId, gameName, hallInterface, parent )
{
}
TLJController::~TLJController()
{
}

QString	TLJController::translatedGameName() const
{
	return tr("Tractor");
}
QSize TLJController::desktopSize() const
{
	return QSize( 850, 850 );
}
QString TLJController::roomName( DJGameRoom* normalRoom ) const
{
	QString name	= DJGameController::roomName( normalRoom );
	UpgradeRoom *room	= (UpgradeRoom *)normalRoom->privateRoom();
	name	+= QString("-%3%4").arg(room->chCards).arg(tr("decks"));
	
	if( 0 != normalRoom->playerLowestScore() )
        name += QString("%1%2").arg(tr("min score")).arg(normalRoom->playerLowestScore());
    
    if( 0 != room->chLowest )
        name += QString(" %1%2").arg(tr("at least over")).arg(room->chLowest);
    
	return name;
}
DJPanelController* TLJController::createPanelController( DJTableController *tc, quint8 seatId )
{
	return new TLJPanelController( tc, seatId );
}
