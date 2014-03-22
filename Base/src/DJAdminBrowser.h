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

#ifndef DJADMINBROWSER_H
#define DJADMINBROWSER_H

#include <QtGui>
#include "DJBaseGlobal.h"
#include "protocol.h"

class DJGameController;

enum AdminChatEventType {
	ADMIN_CHAT_EVENT_TYPE_CALL_ADMIN	= 1,
	ADMIN_CHAT_EVENT_TYPE_ADMIN_RESPOND,
	ADMIN_CHAT_EVENT_TYPE_ADMIN_CHAT,
};
enum CallAdminReason {
	CALL_ADMIN_REASON_CHEAT	=	1,
	CALL_ADMIN_REASON_ABUSE,
	CALL_ADMIN_REASON_QUESTION,
	CALL_ADMIN_REASON_SUGGESTION,
	CALL_ADMIN_REASON_CURIOUS,
	CALL_ADMIN_REASON_OTHERS
};
struct CallAdminInfo
{
	int	callerId;
	int	roomId;
	int	tableId;
	int	reason;
	int	eventType;
};

class DJ_BASE_EXPORT DJAdminBrowser : public QTextBrowser
{
	Q_OBJECT
public:
	DJAdminBrowser( DJGameController* gameController, quint32 groupId, QWidget * parent = 0 );
	virtual ~DJAdminBrowser();
	
	QString extractCallAdminInfo( const ChatContent* pChatContent, CallAdminInfo& callAdminInfo );
	void receiveChat( const ChatContent* pChatContent );
	void handlePendingCall( quint32 adminId, quint32 callerId );
	virtual void setSource ( const QUrl & url );
	QString	reason( uint reason ) const;
private:
	DJGameController	*m_gameController;
	quint32				m_groupId;
};

#endif
