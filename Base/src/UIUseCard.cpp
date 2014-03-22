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

#include "UIUseCard.h"
#include "protocol.h"
#include "DJGameController.h"
#include "DJGameUser.h"

UIUseCard::UIUseCard( quint8 cardId, const QList<quint32>& userIds, DJGameController *gameController, QWidget *parent ) 
	:QDialog( parent ),m_gameController(gameController),m_id(cardId)
{
	setupUi(this);
	
	foreach( quint32 userId, userIds ) {
		DJGameUser *user	= m_gameController->gameUser( userId );
		if ( user ) {
			cbUsers->addItem( user->userName(), QVariant(userId) );
		}
	}
}
UIUseCard::~UIUseCard()
{
}
quint32 UIUseCard::otherUserId() const
{
	return cbUsers->itemData(cbUsers->currentIndex()).toUInt();
}

