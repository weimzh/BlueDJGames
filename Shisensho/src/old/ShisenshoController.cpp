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

#include "ShisenshoController.h"
#include "DJGameRoom.h"
#include "shsspanel.h"

ShisenshoController::ShisenshoController( quint16 gameId, const QString& gameName, const DJHallInterface& hallInterface, QObject *parent )
	: DJGameController( gameId, gameName, hallInterface, parent )
{
}
ShisenshoController::~ShisenshoController()
{
}
QString	ShisenshoController::translatedGameName() const
{
	return tr("Shisensho");
}
/*
QString ShisenshoController::roomName( DJGameRoom* normalRoom )
{
	QString name = DJGameController::roomName( normalRoom );
	//name += QString("(%1/%2)").arg(normalRoom->curUsers()).arg(normalRoom->maxUsers());
	return name;
}
*/
QList<QPoint> ShisenshoController::seatOffsets( quint8 numberOfSeats ) const
{
	QList<QPoint>	offsets;
	int offset	= 86;
	if ( 1 == numberOfSeats )
	{	
		offsets << QPoint(0,offset);
	}else
	{
		return DJGameController::seatOffsets(numberOfSeats);
	}
	return offsets;

}
DJGamePanel* ShisenshoController::createGamePanel( DJGameController* gameController, DJGameRoom* normalRoom, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f )
{
	return new SHSSPanel( gameController, normalRoom, userIds, tableId, seatId, parent, f );
}


