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

#ifndef DJADMINGROUPCHAT_H
#define DJADMINGROUPCHAT_H

#include <QtGui>
#include "DJBaseGlobal.h"
#include "protocol.h"

class DJGameController;
class DJAdminBrowser;

class DJ_BASE_EXPORT DJAdminGroupChat : public QWidget
{
	Q_OBJECT
public:
	DJAdminGroupChat( DJGameController* gameController, quint32 groupId, QWidget * parent = 0 );
	virtual ~DJAdminGroupChat();
	
	DJAdminBrowser* adminBrowser() const { return m_adminBrowser; }
	void receiveChat( const ChatContent* chatContent );
private slots:
	void sendAdminChat();
private:
	DJGameController	*m_gameController;
	quint32				m_groupId;
	DJAdminBrowser		*m_adminBrowser;
	QLineEdit			*m_editor;
};

#endif
