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

#ifndef DJGAMEUSERCONTEXT_H
#define DJGAMEUSERCONTEXT_H

#include <QtGui>
#include "DJBaseGlobal.h"
#include "protocol.h"

class DJGameController;
class DJGameUser;
class DJShowView;
class DJMenuButton;
class DJCardsWidget;

class DJ_BASE_EXPORT DJGameUserContext : public QWidget
{
	Q_OBJECT
public:
	DJGameUserContext( DJGameController	*gameController, QWidget * parent = 0, Qt::WFlags f = 0 );
	virtual ~DJGameUserContext();
	
	DJShowView *showView() const { return m_showView; }
	DJCardsWidget *cardsWidget() const { return m_cardsWidget; }
	quint32 clickedUserId() const { return m_clickedUserId; }
	void moveToAppropriatePos();
	void showUserContext( DJGameUser *clickedUser );
public slots:
	void createUserInfo( DJGameUser* clickedUser );	
private slots:
	void handleLookOn();
	void handleChat();
	void handleRobotChat();
	void handleInvite();
	void handleForceStandUp();
	void handleContactFriend( bool checked );
	void handleContactUnwelcome( bool checked );
	
	//void handleAdminKick();
	void handleAdminShutUp();
	void handleAdminDeltaMoney();
	void handleAdminDeltaMagic();
	void handleAdminDeltaChips();
	void handleAdminBanIp();
	void handleAdminBanAccount();
	
	void handleAdminDeltaScore();
private:
	int createFunctionMenu( DJGameUser* clickedUser, DJGameUser* selfUser );
	void createContactMenu( DJGameUser* clickedUser );
	int createAdminMenu( DJGameUser* clickedUser, DJGameUser* selfUser );
	
	void hideAllWidgets();
	
	DJGameController	*m_gameController;
	quint32 			m_clickedUserId;
	DJShowView			*m_showView;
	
	QLabel				*m_userInfo;
	
	QMenu				*m_menuFunction,*m_menuContact,*m_menuAdmin;
	DJMenuButton		*m_btnFunction,*m_btnContact,*m_btnAdmin;
	QAction				*m_actLookOn,*m_actChat,*m_actInvite,*m_actForceStandUp;
	QAction				*m_actFriend,*m_actUnwelcome;
	QAction				*m_actAdminDeltaScore,*m_actAdminShutUp,
						*m_actAdminDeltaMoney,*m_actAdminDeltaMagic,*m_actAdminDeltaChips,
						*m_actAdminBanIp,*m_actAdminBanAccount,*m_actAdminRobot;
	
	QLabel				*m_labelCards;
	DJCardsWidget		*m_cardsWidget;
};

#endif
