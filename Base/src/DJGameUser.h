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

#ifndef DJGAMEUSER_H
#define DJGAMEUSER_H

#include <QtGui>
#include "DJBaseGlobal.h"
#include "DJGameSetup.h"
#include "protocol.h"

const quint8	GAME_STATUS_IDLE	= 0;
const quint8	GAME_STATUS_DOWN	= 0x01;
const quint8	GAME_STATUS_READY	= 0x03;
const quint8	GAME_STATUS_PLAYING	= 0x07;

class DJGameController;
class DJRemoteToLocalFile;

class DJ_BASE_EXPORT DJGameUser : public QObject
{
	Q_OBJECT
public:
	DJGameUser( quint32 userId, DJGameController *gameController );
    DJGameUser( const NormalUserDes *userDsc, DJGameController *gameController );
    virtual ~DJGameUser();
    
    DJGameController* gameController() const { return m_gameController; }
    
    virtual void reload( const NormalUserDes *userDsc );
    
    //quint32 selfUserId() const { return m_selfUserId; }
	//void setSelfUserId( quint32 userId );
    bool isSelf() const;
    bool isRobot() const;
    
    bool isIdle() const { return GAME_STATUS_IDLE == m_gameStatus; }
    bool isDown() const { return GAME_STATUS_DOWN == m_gameStatus; }
    bool isReady() const { return GAME_STATUS_READY == m_gameStatus; }
    bool isPlaying() const { return GAME_STATUS_PLAYING == m_gameStatus; }
    
    //general user accessor
    quint32	userId() const { return m_userId; }
    QString	userName() const { return m_userName; }
    qint32	money() const { return m_money; }
    qint32	magic() const { return m_magic; }
    quint8	gender() const { return m_gender; }
    QPixmap avatar();
    QIcon	icon() const;
    //quint8	userType() const { return m_userType; }
    quint8 globalRank() const { return m_userType; }
    quint32 groupId() const { return m_groupId; }
    quint16 unwelcome() const { return m_unwelcome; }
    quint32	breakRate() const;
    quint32	total() const { return m_total; }
    quint32	onlineTime() const { return m_onlineTime; }
    quint8	platform() const { return m_platform; }
    quint8	speed() const { return m_speed; }
    quint8	generalStatus() const { return m_generalStatus; }
    quint32 avatarId() const { return m_avatarId; }
    quint32 showId() const { return m_showId; }
    qint32	chips() const { return m_chips; }
    QDateTime	lastLoginDate() const { return m_lastLoginDate; }
    QString nickName() const { return m_nickName; }
    void	setGeneralStatus( quint8 generalStatus );
    void	setMoney( qint32 money );
    //void 	deltaMoney( int delta );
    void	setMagic( qint32 magic );
    //void 	deltaMagic( int delta );
    void 	setUnwelcome( quint16 unwelcome );
    void	setSpeed( quint8 speed );
    void 	setChips( qint32 chips );
    void 	deltaChips( int delta );
	//game normal data accessor
	qint32	score() const { return m_score; }
	quint32	wins() const { return m_wins; }
	quint32	loses() const { return m_loses; }
	quint32	winRate() const;
	quint8	roomId() const { return m_roomId; }
	quint8	tableId() const { return m_tableId; }
	quint8	seatId() const { return m_seatId; }
	quint8	gameStatus() const { return m_gameStatus; }
	quint32 draws() const { return m_draws; }
	//quint8	gameUserType() const { return m_gameUserType; }
	quint8 gameRank() const { return qMax( m_userType, m_gameUserType ); }
    
	void 	setRoomId( quint8 roomId );
	void 	setTrinity( quint8 tableId, quint8 seatId, quint8 gameStatus );
	void 	orTrinity( quint8 tableId, quint8 seatId, quint8 gameStatus );
	void	setScore( qint32 score );
	void	deltaScore( int delta );
	void 	increaseWins();
	void 	increaseLoses();
	void 	increaseDraws();
	
	//game normal setup accessor
	const DJGameSetup& selfSetup() const { return m_selfSetup; }
	void	setSelfSetup( const DJGameSetup& selfSetup );
	
signals:
	void trinityChanged( DJGameUser* user, quint8 oldTableId, quint8 oldSeatId, quint8 oldGameStatus );
	
	void generalStatusChanged( DJGameUser* user );
	void speedChanged( DJGameUser* user );
	void moneyChanged( DJGameUser* user );
	void magicChanged( DJGameUser* user );
	void chipsChanged( DJGameUser* user );
	
	void scoreChanged( DJGameUser* user );
	void winsChanged( DJGameUser* user );
	void losesChanged( DJGameUser* user );
	void drawsChanged( DJGameUser* user );
	void unwelcomeChanged( DJGameUser* user );
		
	void reloaded( DJGameUser* user );
	
private slots:
	void avatarImageDone();
private:
	void initWithUser( const NormalUserDes *userDsc );
	quint8	gameStatusFromServer( quint8 serverGameStatus );
			
	//general user data
   	quint32	m_userId;
   	QString	m_userName;
   	qint32	m_money;
   	qint32	m_magic;
   	quint8	m_gender;
   	quint8	m_nation;
   	quint8	m_languages;
   	quint8	m_userType;
   	quint32	m_groupId;
   	quint16	m_unwelcome;
   	quint32	m_breaks;
   	quint32	m_total;
   	quint32	m_onlineTime;
   	quint8	m_platform;
   	quint8	m_speed;
   	quint8	m_generalStatus;
   	
   	quint8	m_groupDutyId;
   	quint32	m_avatarId;
   	quint32	m_showId;
   	QDateTime	m_lastLoginDate;
   	qint32	m_chips;
   	QString	m_nickName;
   	//game normal data
   	qint32	m_score;
   	quint32	m_wins;
   	quint32	m_loses;
   	qint16	m_group;
   	quint8	m_gameStatus;
   	quint8	m_roomId;
	quint8	m_tableId;
	quint8	m_seatId;
	quint32	m_draws;
	quint8 	m_gameUserType;
	quint8  m_groupDuty;
	//game normal setup
	DJGameSetup	m_selfSetup;
	
	DJRemoteToLocalFile	*m_r2lAvatar;
	DJGameController *m_gameController;
};

#endif
