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

#ifndef DJGAMEUSERITEM_H
#define DJGAMEUSERITEM_H

#include <QtGui>
#include "DJBaseGlobal.h"

class DJGameUser;
class DJGameController;

class DJ_BASE_EXPORT DJGameUserItem : public QObject, public QTreeWidgetItem
{
	Q_OBJECT
public:
    DJGameUserItem( DJGameUser* user, QTreeWidget * parent, int type = Type );
    virtual ~DJGameUserItem();
    	
	DJGameUser* user() const { return m_user; }
	DJGameController* gameController() const;
	void setContactUserType( int contactUserType );
	
	virtual void initialize();
	virtual bool operator< ( const QTreeWidgetItem & other ) const;
    
	void setUserName( int index );
	void setNickName( int index );
	void setGameStatus( int index );
	void setTableSeat( int index );
	void setPlatform( int index );
	void setSpeed( int index );
	void setScore( int index );
	void setWins( int index );
	void setLoses( int index );
	void setWinRate( int index );
	void setMoney( int index );
	void setBreakRate( int index );
	void setOnlineTime( int index );
	void setRank( int index );
	void setChips( int index );
	void setMagic( int index );
	void setDraws( int index );
	void setUnwelcome( int index );
	
public slots:
	virtual void updateAll();
	
	void updateUserName();
	void updateNickName();
	void updateGameStatus();
	void updateTableSeat();
	void updatePlatform();
	void updateSpeed();
	void updateScore();
	void updateWins();
	void updateLoses();
	void updateWinRate();
	void updateMoney();
	void updateBreakRate();
	void updateOnlineTime();
	void updateRank();
	void updateChips();
	void updateMagic();
	void updateDraws();
	void updateUnwelcome();
private:
   	DJGameUser* 		m_user;
};

#endif
