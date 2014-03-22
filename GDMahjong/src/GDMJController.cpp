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

#include "GDMJController.h"
#include "DJGameRoom.h"
#include "GDMJPanelController.h"
#include "DJGameUser.h"
#include "Utility.h"
#include "mj2profile.h"

GDMJController::GDMJController( quint16 gameId, const QString& gameName, const DJHallInterface& hallInterface, QObject *parent )
	: DJGameController( gameId, gameName, hallInterface, parent )
{
}
GDMJController::~GDMJController()
{
}

QString	GDMJController::translatedGameName() const
{
	return tr("GDMahjong");
}
QSize GDMJController::desktopSize() const
{
	return QSize( 1000, 1000 );
}
QString GDMJController::roomName( DJGameRoom* normalRoom ) const
{
	QString name	= DJGameController::roomName( normalRoom );
	
	MahjongRoom *room	= (MahjongRoom *)normalRoom->privateRoom();
		
    if(room->rule.chHURule1 & MAHJONG_RULE_HU1_MUSTDIY)
        name += tr("(hu by yourself)");
    
	return name;
}
DJPanelController* GDMJController::createPanelController( DJTableController *tc, quint8 seatId )
{
	return new GDMJPanelController( tc, seatId );
}
