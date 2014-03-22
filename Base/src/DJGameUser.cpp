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

#include "DJGameUser.h"
#include "Utility.h"
#include "DJGameController.h"
#include "DJRemoteToLocalFile.h"

DJGameUser::DJGameUser( quint32 userId, DJGameController *gameController )
	:QObject(gameController),m_userId(userId),m_selfSetup(),m_gameController(gameController)
{
	djDebug()<<"DJGameUser constructor";
	
	m_roomId	= 0;
	m_tableId	= 0;
	m_seatId	= 0;
	
	m_userName	= tr("no name");
	m_money		= 0;
	m_magic		= 0;
	m_gender	= 0;
	m_nation	= 0;
	m_languages	= 0;
	m_userType	= 0;
	m_groupId	= 0;
	m_unwelcome	= 0;
	m_breaks	= 0;
	m_total		= 0;
	m_onlineTime= 0;
	m_platform	= 0;
	m_speed		= 0;
	m_generalStatus	= 0;
	
	m_groupDutyId	= 0;
	m_avatarId	= 0;
	m_showId	= 0;
	m_chips		= 0;
	m_nickName	= tr("no name");
	
	m_score		= 0;
	m_wins		= 0;
	m_loses		= 0;
	m_group		= 0;
	m_gameStatus	= GAME_STATUS_IDLE;
	
	m_draws		= 0;
	m_gameUserType	= 0;
	
	m_r2lAvatar	= 0;
}
DJGameUser::DJGameUser( const NormalUserDes *userDsc, DJGameController *gameController )
	:QObject(gameController),m_selfSetup(userDsc),m_gameController(gameController)
{
	djDebug()<<"DJGameUser constructor";
	
	initWithUser( userDsc );
	
	m_r2lAvatar	= 0;
}
DJGameUser::~DJGameUser()
{
	djDebug()<<"DJGameUser destructor";
}
void DJGameUser::reload( const NormalUserDes *userDsc )
{
	quint8 oldTableId	= m_tableId;
	quint8 oldSeatId	= m_seatId;
	quint8 oldGameStatus	= m_gameStatus;
	
	initWithUser( userDsc );
	
	m_selfSetup	= DJGameSetup( userDsc );
	
	emit reloaded( this );
	if ( m_tableId != oldTableId || m_seatId != oldSeatId || m_gameStatus != oldGameStatus ) {
		djDebug() << "emit trinityChanged reload" << oldTableId << oldSeatId << oldGameStatus << "-->" << m_tableId << m_seatId << m_gameStatus;
		emit trinityChanged( this, oldTableId, oldSeatId, oldGameStatus );
	}
}
bool DJGameUser::isSelf() const
{
	return m_userId == m_gameController->selfUserId();
}
bool DJGameUser::isRobot() const
{
	return m_userId >= DJGAME_ROBOT_MINID && m_userId <= DJGAME_ROBOT_MAXID;
}
//general user accessor
QPixmap DJGameUser::avatar()
{
	if ( 0 != m_avatarId ) {
		QString path	= "images/avatars/A1%1.png";
		if ( DJGAME_GENDER_FEMALE == m_gender )
			path	= "images/avatars/A2%1.png";
		QString avatarPath	= path.arg( m_avatarId, 8, 16, QChar('0') );
		if ( QFile::exists( avatarPath ) ) {
			QPixmap pix;
			if ( pix.load( avatarPath ) )
				return pix.scaled( m_gameController->avatarSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation );
		}else {
    		if ( 0 == m_r2lAvatar ) {
    			QUrl	url( HTTP_WWW_HOST + avatarPath );
    			djDebug() << url;
    			m_r2lAvatar	= new DJRemoteToLocalFile( url, avatarPath, false, this );
    			connect( m_r2lAvatar, SIGNAL(localFileDone(const QString&)), SLOT(avatarImageDone()) );
    		}
    	}
	}
	QPixmap	pix;
	if ( DJGAME_GENDER_MALE == m_gender ) {
		pix	= QPixmap( ":/BaseRes/image/hall/AvatarMale.png" );
	}else if ( DJGAME_GENDER_FEMALE == m_gender ) {
		pix	= QPixmap( ":/BaseRes/image/hall/AvatarFemale.png" );
	}else {
		pix	= QPixmap( ":/BaseRes/image/hall/AvatarSecret.png" );
	}
	return pix.scaled( m_gameController->avatarSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation );
}
QIcon DJGameUser::icon() const
{
	if ( DJGAME_GENDER_MALE == m_gender ) {
		return QIcon( ":/BaseRes/image/hall/IconMale.png" );
	}else if (DJGAME_GENDER_FEMALE == m_gender ) {
		return QIcon( ":/BaseRes/image/hall/IconFemale.png" );
	}else {
		return QIcon( ":/BaseRes/image/hall/IconSecret.png" );
	}
}
quint32	DJGameUser::breakRate() const
{
	if ( m_total ) {
		return m_breaks * 100 / m_total;
	}else
		return 0;
}
void DJGameUser::setGeneralStatus( quint8 generalStatus )
{
	if ( m_generalStatus != generalStatus ) {
		m_generalStatus	= generalStatus;
		emit generalStatusChanged( this );
	}
}
void DJGameUser::setMoney( qint32 money )
{
	if ( m_money != money ) {
		m_money	= money;
		emit moneyChanged( this );
	}
}
/*
void DJGameUser::deltaMoney( int delta )
{
	if ( delta ) {
		m_money	+= delta;
		emit moneyChanged( this );
	}
}
*/
void DJGameUser::setMagic( qint32 magic )
{
	if ( m_magic != magic ) {
		m_magic	= magic;
		emit magicChanged( this );
	}
}
/*
void DJGameUser::deltaMagic( int delta )
{
	if ( delta ) {
		m_magic	+= delta;
		emit magicChanged( this );
	}
}
*/
void DJGameUser::setUnwelcome( quint16 unwelcome )
{
	if ( m_unwelcome != unwelcome ) {
		m_unwelcome	= unwelcome;
		emit unwelcomeChanged( this );
	}
}
void DJGameUser::setSpeed( quint8 speed )
{
	if ( m_speed != speed ) {
		m_speed	= speed;
		emit speedChanged( this );
	}
}
void DJGameUser::setChips( qint32 chips )
{
	if ( m_chips != chips ) {
		m_chips	= chips;
		emit chipsChanged( this );
	}
}
void DJGameUser::deltaChips( int delta )
{
	if ( delta ) {
		m_chips	+= delta;
		emit chipsChanged( this );
	}
}
//game normal data accessor
void DJGameUser::setScore( qint32 score )
{
	if ( m_score != score ) {
		m_score	= score;
		emit scoreChanged( this );
	}
}
void DJGameUser::deltaScore( int delta )
{
	if ( delta ) {
		m_score	+= delta;
		emit scoreChanged( this );
	}
}
void DJGameUser::increaseWins()
{
	m_wins++;
	emit winsChanged( this );
}
quint32	DJGameUser::winRate() const
{
	quint32 total	= m_wins + m_loses + m_draws;
	if ( total ) {
		return m_wins * 100 / total;
	}else
		return 0;
}
void DJGameUser::increaseLoses()
{
	m_loses++;
	emit losesChanged( this );
}
void DJGameUser::setRoomId( quint8 roomId )
{
	m_roomId	= roomId;
}
void DJGameUser::increaseDraws()
{
	m_draws++;
	emit drawsChanged( this );
}
void DJGameUser::setTrinity( quint8 tableId, quint8 seatId, quint8 gameStatus )
{
	quint8 oldTableId		= m_tableId;
	quint8 oldSeatId		= m_seatId;
	quint8 oldGameStatus	= m_gameStatus;
	
	m_tableId				= tableId;
	m_seatId				= seatId;
	m_gameStatus			= gameStatus;
	
	if ( m_tableId != oldTableId || m_seatId != oldSeatId || m_gameStatus != oldGameStatus ) {
		djDebug() << "emit trinityChanged setTrinity" << oldTableId << oldSeatId << oldGameStatus << "-->" << m_tableId << m_seatId << m_gameStatus ;
		emit trinityChanged( this, oldTableId, oldSeatId, oldGameStatus );
	}
}
void DJGameUser::orTrinity( quint8 tableId, quint8 seatId, quint8 gameStatus )
{
	/*
	quint8 oldTableId		= m_tableId;
	quint8 oldSeatId		= m_seatId;
	quint8 oldGameStatus	= m_gameStatus;
	
	m_tableId				= tableId;
	m_seatId				= seatId;
	m_gameStatus			|= gameStatus;
	
	if ( m_tableId != oldTableId || m_seatId != oldSeatId || m_gameStatus != oldGameStatus ) {
		djDebug() << "emit trinityChanged orTrinity" << oldTableId << oldSeatId << oldGameStatus <<"-->" << m_tableId << m_seatId << m_gameStatus ;
		emit trinityChanged( this, oldTableId, oldSeatId, oldGameStatus );
	}
	*/
	quint8 oldTableId		= m_tableId;
	quint8 oldSeatId		= m_seatId;
	quint8 oldGameStatus	= m_gameStatus;
	
	m_tableId				= tableId;
	m_seatId				= seatId;
	
	if ( m_tableId != oldTableId || m_seatId != oldSeatId ) {
		m_gameStatus	= gameStatus;
	}else {
		m_gameStatus	|= gameStatus;
	}
	djDebug() << "emit trinityChanged orTrinity" << oldTableId << oldSeatId << oldGameStatus <<"-->" << m_tableId << m_seatId << m_gameStatus ;
	emit trinityChanged( this, oldTableId, oldSeatId, oldGameStatus );
}
void DJGameUser::setSelfSetup( const DJGameSetup& selfSetup )
{
	m_selfSetup	= selfSetup;
}
void DJGameUser::avatarImageDone()
{
	djDebug() << "DJGameUser::avatarImageDone";
	emit reloaded( this );
}
void DJGameUser::initWithUser( const NormalUserDes* userDsc)
{
	const GeneralUserDes	*generalUserDes	= &userDsc->general;
	const GameNormalData	*gameNormalData	= reinterpret_cast<const GameNormalData*>(reinterpret_cast<const char*>(generalUserDes) + userDsc->chGeneralUserDesLen );
	
	m_roomId	= gameNormalData->chRoom;
	m_tableId	= gameNormalData->chTable;
	m_seatId	= gameNormalData->chSite;
	
	m_userId	= letoh4( &generalUserDes->userid );
	m_userName	= QString::fromUtf8( generalUserDes->szUserName );
	m_money		= letoh4( generalUserDes->iMoney );
	m_magic		= letoh4( generalUserDes->iMagic );
	m_gender	= generalUserDes->chGender;
	m_nation	= generalUserDes->chNation;
	m_languages	= generalUserDes->chSupportLanguages;
	m_userType	= generalUserDes->chUserType;
	m_groupId	= letoh4( generalUserDes->uGroupID );
	m_unwelcome	= letoh2( generalUserDes->shUnwelcome );
	m_breaks	= letoh4( generalUserDes->uBreak );
	m_total		= letoh4( generalUserDes->uTotal );
	m_onlineTime= letoh4( generalUserDes->uTotalTime );
	m_platform	= generalUserDes->chOS;
	m_speed		= generalUserDes->chSpeed;
	m_generalStatus	= generalUserDes->chStatus;
	
	m_groupDutyId	= generalUserDes->chGroupDutyID;
	m_avatarId	= letoh4( generalUserDes->uiAvatar );
	m_showId	= letoh4( generalUserDes->uiShow );
	m_lastLoginDate.setTime_t( letoh4( generalUserDes->iLastTime ) );
	m_chips		= letoh4( generalUserDes->iCounter );
	m_nickName	= QString::fromUtf8( generalUserDes->szNickName );
	
	m_score		= letoh4( gameNormalData->iScore );
	m_wins		= letoh4( gameNormalData->uiWins );
	m_loses		= letoh4( gameNormalData->uiLoses );
	m_group		= letoh2( gameNormalData->shGroup );
	
	m_gameStatus	= gameStatusFromServer( gameNormalData->chStatus );
	
	if ( sizeof( GameNormalData ) == userDsc->chGameNormalDataLen ) {
		m_draws		= letoh4( gameNormalData->uiDraws );
		m_gameUserType	= gameNormalData->chUserTypeID;
		m_groupDuty	= gameNormalData->chGroupDuty;
	}else {
		m_draws	= 0;
		m_gameUserType	= 0;
		m_groupDuty	= 0;
	}
}
quint8	DJGameUser::gameStatusFromServer( quint8 serverGameStatus )
{
	switch( serverGameStatus ) {
	case DJGAME_USERGAME_STATUS_DOWN:
		return GAME_STATUS_DOWN;
	case DJGAME_USERGAME_STATUS_READY:
		return GAME_STATUS_READY;
	case DJGAME_USERGAME_STATUS_PLAY:
		return GAME_STATUS_PLAYING;
	default:
		return GAME_STATUS_IDLE;
	}
}
