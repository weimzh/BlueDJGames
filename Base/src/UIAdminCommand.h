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

#ifndef UIADMINCOMMAND_H
#define UIADMINCOMMAND_H

#include <QtGui>
#include "DJBaseGlobal.h"
#include "ui_UIAdminCommand.h"

class DJGameUser;

class DJ_BASE_EXPORT UIAdminCommand : public QDialog, private Ui::adminCommand
{
	Q_OBJECT
public:
	enum CommandType{
		CommandTypeGeneral = 0,
		CommandTypeShutUp,
		CommandTypeBanAccount,
		CommandTypeBanIp,
	};
	UIAdminCommand( DJGameUser *user, QWidget *parent = 0, int type = 0, int option = 0 );
	virtual ~UIAdminCommand();
	
	void setHintText( const QString& text );
	QString commandParameter() const;
private:
	int m_type;
};
#endif
