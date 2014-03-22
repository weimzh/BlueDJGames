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

#include "UIAdminCommand.h"
#include "DJGameUser.h"

UIAdminCommand::UIAdminCommand( DJGameUser *user, QWidget *parent, int type, int option ) 
	:QDialog( parent ),m_type(type)
{
	setupUi(this);
	
	if ( user ) {
		cbUser->addItem(user->userName()+"("+user->nickName()+")");
	}else {
		cbUser->addItem(DJGameUser::tr("no name"));
	}
		
	labelHint->setMinimumWidth( 120 );
	
	switch( m_type ) {
	case CommandTypeShutUp:
		cbInput->setEditable( false );
		for ( int minutes	= 4; minutes <= 64 && option > 0; minutes *= 2, option-- ) {
			cbInput->addItem( tr("%1 minutes").arg(minutes), QVariant(minutes) );
		}
		break;
	case CommandTypeBanAccount:
	case CommandTypeBanIp:
		cbInput->hide();
		break;
	default:
		break;
	}
}
UIAdminCommand::~UIAdminCommand()
{
}
void UIAdminCommand::setHintText( const QString& text )
{
	labelHint->setText( text );
}
QString UIAdminCommand::commandParameter() const
{
	if ( CommandTypeShutUp == m_type ) {
		int index	= cbInput->currentIndex();
		return cbInput->itemData( index ).toString();
	}else {
		return cbInput->currentText();
	}
}

