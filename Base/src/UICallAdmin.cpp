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

#include "UICallAdmin.h"
#include "DJAdminBrowser.h"

UICallAdmin::UICallAdmin( QWidget *parent ) 
	:QDialog( parent )
{
	setupUi(this);
	
	comboReason->addItem( tr("someone is cheating"), QVariant(CALL_ADMIN_REASON_CHEAT) );
	comboReason->addItem( tr("someone is abusing"), QVariant(CALL_ADMIN_REASON_ABUSE) );
	comboReason->addItem( tr("I have a question"), QVariant(CALL_ADMIN_REASON_QUESTION) );
	comboReason->addItem( tr("I have a sugestion"), QVariant(CALL_ADMIN_REASON_SUGGESTION) );
	comboReason->addItem( tr("I'm curious"), QVariant(CALL_ADMIN_REASON_CURIOUS) );
	comboReason->addItem( tr("others reason"), QVariant(CALL_ADMIN_REASON_OTHERS) );
}
UICallAdmin::~UICallAdmin()
{
}
uint UICallAdmin::reason() const
{
	int index	= comboReason->currentIndex();
	return comboReason->itemData( index ).toUInt();
}
QString	UICallAdmin::additionalMsg() const
{
	return leMsg->text();
}
