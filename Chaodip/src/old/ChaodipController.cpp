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

#include "ChaodipController.h"
#include "DJGameRoom.h"
#include "ug2profile.h"
#include "Chaodip.h"

ChaodipController::ChaodipController( quint16 gameId, const QString& gameName, const DJHallInterface& hallInterface, QObject *parent )
	: DJGameController( gameId, gameName, hallInterface, parent )
{
}
ChaodipController::~ChaodipController()
{
}
QString	ChaodipController::translatedGameName() const
{
	return tr("Chaodip");
}
QString ChaodipController::roomName( DJGameRoom* normalRoom ) const
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
DJGamePanel* ChaodipController::createGamePanel( DJGameController* gameController, DJGameRoom* normalRoom, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f )
{
	return new ChaodipPanel( gameController, normalRoom, userIds, tableId, seatId, parent, f );
}
