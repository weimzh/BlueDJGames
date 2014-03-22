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

#include "DJGameUserItem.h"
#include "DJGameUser.h"
#include "DJGameController.h"
#include "protocol.h"
#include "Utility.h"

DJGameUserItem::DJGameUserItem( DJGameUser* user, QTreeWidget * parent, int type )
		: QObject(parent),QTreeWidgetItem( parent, type),m_user(user)
{	
	djDebug()<<"DJGameUserItem constructor";
	
	for ( int i = 0; i < parent->columnCount(); i++ ) {
		//if ( m_user->isSelf() ) {
			//setTextColor( i, Qt::red );
		//}
		setTextAlignment( i, Qt::AlignCenter );
	}
}
DJGameUserItem::~DJGameUserItem()
{
	djDebug()<<"DJGameUserItem destructor";
}
DJGameController* DJGameUserItem::gameController() const
{
	return m_user->gameController();
}
void DJGameUserItem::setContactUserType( int contactUserType )
{
	QTreeWidget* parent	= treeWidget();
	switch( contactUserType ) {
	case DJ_CONTACT_USER_TYPE_NONE:
		for ( int i = 0; i < parent->columnCount(); i++ ) {
			setTextColor( i, Qt::black );
		}
		break;
	case DJ_CONTACT_USER_TYPE_FRIEND:
		for ( int i = 0; i < parent->columnCount(); i++ ) {
			setTextColor( i, Qt::red );
		}
		break;
	case DJ_CONTACT_USER_TYPE_UNWELCOME:
		for ( int i = 0; i < parent->columnCount(); i++ ) {
			setTextColor( i, Qt::darkGray );
		}
		break;
	default:
		break;
	}
}
void DJGameUserItem::initialize()
{
	updateAll();
	int index	= gameController()->indexOfSection( DJGameController::UserName );
	if ( -1 != index ) {
		setToolTip( index, QString::number(m_user->userId()) );
	}
	connect( m_user, SIGNAL(trinityChanged(DJGameUser*,quint8,quint8,quint8)), SLOT(updateTableSeat()) );
	connect( m_user, SIGNAL(trinityChanged(DJGameUser*,quint8,quint8,quint8)), SLOT(updateGameStatus()) );
	
	connect( m_user, SIGNAL(generalStatusChanged(DJGameUser*)), SLOT(updateUserName()) );
	connect( m_user, SIGNAL(speedChanged(DJGameUser*)), SLOT(updateSpeed()) );
	connect( m_user, SIGNAL(moneyChanged(DJGameUser*)), SLOT(updateMoney()) );
	connect( m_user, SIGNAL(scoreChanged(DJGameUser*)), SLOT(updateScore()) );
	connect( m_user, SIGNAL(winsChanged(DJGameUser*)), SLOT(updateWins()) );
	connect( m_user, SIGNAL(winsChanged(DJGameUser*)), SLOT(updateWinRate()) );
	connect( m_user, SIGNAL(losesChanged(DJGameUser*)), SLOT(updateLoses()) );
	connect( m_user, SIGNAL(losesChanged(DJGameUser*)), SLOT(updateWinRate()) );
	connect( m_user, SIGNAL(chipsChanged(DJGameUser*)), SLOT(updateChips()) );
	connect( m_user, SIGNAL(magicChanged(DJGameUser*)), SLOT(updateMagic()) );
	connect( m_user, SIGNAL(drawsChanged(DJGameUser*)), SLOT(updateDraws()) );
	connect( m_user, SIGNAL(drawsChanged(DJGameUser*)), SLOT(updateWinRate()) );
	connect( m_user, SIGNAL(unwelcomeChanged(DJGameUser*)), SLOT(updateUnwelcome()) );
	
	connect( m_user, SIGNAL(reloaded(DJGameUser*)), SLOT(updateAll()) );
}
bool DJGameUserItem::operator< ( const QTreeWidgetItem & other ) const
{
	const DJGameUserItem& otherItem	= static_cast<const DJGameUserItem &>(other);
	Qt::SortOrder	order	= treeWidget()->header()->sortIndicatorOrder();
	
	DJGameUser*	thisUser	= user();
	DJGameUser*	otherUser	= otherItem.user();
	
	if ( thisUser->isSelf() || otherUser->isRobot() ) {
		if ( order == Qt::AscendingOrder )
			return true;
		else
			return false;
	}
	if ( otherUser->isSelf() || thisUser->isRobot() ) {
		if ( order == Qt::AscendingOrder )
			return false;
		else
			return true;
	}
	int sortingIndex	= treeWidget()->header()->sortIndicatorSection();
	
	if ( sortingIndex == gameController()->indexOfSection( DJGameController::BreakRate) )
		return thisUser->breakRate()  < otherUser->breakRate();
	else if ( sortingIndex == gameController()->indexOfSection( DJGameController::Score) )
		return thisUser->score()  < otherUser->score();
	else if ( sortingIndex == gameController()->indexOfSection( DJGameController::OnlineTime) )
		return thisUser->onlineTime()  < otherUser->onlineTime();
	else if ( sortingIndex == gameController()->indexOfSection( DJGameController::Speed) )
		return thisUser->speed()  < otherUser->speed();
	else if ( sortingIndex == gameController()->indexOfSection( DJGameController::Money) )
		return thisUser->money()  < otherUser->money();
	else if ( sortingIndex == gameController()->indexOfSection( DJGameController::Magic) )
		return thisUser->magic()  < otherUser->magic();
	else if ( sortingIndex == gameController()->indexOfSection( DJGameController::Chips) )
		return thisUser->chips()  < otherUser->chips();
	else if ( sortingIndex == gameController()->indexOfSection( DJGameController::Wins) )
		return thisUser->wins()  < otherUser->wins();
	else if ( sortingIndex == gameController()->indexOfSection( DJGameController::Loses) )
		return thisUser->loses()  < otherUser->loses();
	else if ( sortingIndex == gameController()->indexOfSection( DJGameController::Draws) )
		return thisUser->draws()  < otherUser->draws();
	else if ( sortingIndex == gameController()->indexOfSection( DJGameController::WinRate) )
		return thisUser->winRate()  < otherUser->winRate();
	else if ( sortingIndex == gameController()->indexOfSection( DJGameController::Unwelcome) )
		return thisUser->unwelcome()  < otherUser->unwelcome();
	else if ( sortingIndex == gameController()->indexOfSection( DJGameController::Rank) )
		return thisUser->gameRank()  < otherUser->gameRank();
	else
		return QTreeWidgetItem::operator< ( other );
}
void DJGameUserItem::setUserName( int index )
{
	switch ( m_user->generalStatus() ) {
	case DJGAME_USER_STATUS_NORMAL:
		setIcon( index, m_user->icon() );
		break;
	case DJGAME_USER_STATUS_BREAK:
		setIcon( index, QIcon( ":/BaseRes/image/hall/break.png") );
		break;
	case DJGAME_USER_STATUS_KICKED:
		setIcon( index, QIcon( ":/BaseRes/image/hall/break.png") );
		break;
	case DJGAME_USER_STATUS_FORBID:
		setIcon( index, QIcon( ":/BaseRes/image/hall/forbidden.png") );
		break;
	default:
		setIcon( index, QIcon() );
		break;
	}
	setText( index, m_user->userName() );
}
void DJGameUserItem::setNickName( int index )
{
	setText( index, m_user->nickName() );
}
void DJGameUserItem::setGameStatus( int index )
{
	switch ( m_user->gameStatus() ) {
	case GAME_STATUS_IDLE:
		setIcon( index, QIcon( ":/BaseRes/image/hall/idle.png") );
		setText( index, tr("idle") );
		break;
	case GAME_STATUS_DOWN:
		setIcon( index, QIcon( ":/BaseRes/image/hall/waiting.png") );
		setText( index, tr("waiting") );
		break;
	case GAME_STATUS_READY:
		setIcon( index, QIcon( ":/BaseRes/image/hall/ready.png") );
		setText( index, tr("ready") );
		break;
	case GAME_STATUS_PLAYING:
		setIcon( index, QIcon( gameController()->gameResDir() + "/image/base/game.png") );
		setText( index, tr("playing") );
		break;
	default:
		setIcon( index, QIcon() );
		setText( index, tr("Unknown Status") );
		break;
	}
}
void DJGameUserItem::setTableSeat( int index )
{
	setText( index, QString("%1-%2").arg( m_user->tableId() ).arg( m_user->seatId() ) );
}
void DJGameUserItem::setPlatform( int index )
{
	switch ( m_user->platform() ) {
	case DJGAME_CLIENT_OS_WINDOWS:
		setText( index, tr("Windows"));
		break;
	case DJGAME_CLIENT_OS_LINUX:
		setText( index, tr("Linux"));
		break;
	case DJGAME_CLIENT_OS_MACOS:
		setText( index, tr("Mac"));
		break;
	case DJGAME_CLIENT_OS_MOBILE:
		setText( index, tr("Mobile"));
		break;
	case DJGAME_CLIENT_OS_TV:
		setText( index, tr("TV") );
		break;
	case DJGAME_CLIENT_OS_BOX:
		setText( index, tr("Box") );
		break;
	case DJGAME_CLIENT_OS_JJETO:
		setText( index, tr("JJeto") );
		break;
	case DJGAME_CLIENT_OS_LOONGSON:
		setText( index, tr("Loongson") );
		break;
	default:
		setText( index, tr("Unknown System"));
		break;
	}
}
void DJGameUserItem::setSpeed( int index )
{	
	setText( index, QString::number( m_user->speed() ) );
}
void DJGameUserItem::setMoney( int index )
{	
	setText( index, QString::number( m_user->money() ) );
}
void DJGameUserItem::setBreakRate( int index )
{	
	setText( index, QString("%1\%").arg(m_user->breakRate() ) );
}
void DJGameUserItem::setOnlineTime( int index )
{	
	quint32 totalSeconds	= m_user->onlineTime();
	int hours,minutes,seconds;
	ConvertSeconds2HMS( totalSeconds, hours, minutes, seconds );
	setText( index, QString("%1:%2:%3").arg(hours).arg(minutes).arg(seconds) );
}
void DJGameUserItem::setRank( int index )
{
	quint8 rank	= m_user->gameRank();
	
	if ( rank >= DJGAME_USERTYPE_SADMIN2 )
		setText( index, tr("super sys admin") );
	else if ( rank >= DJGAME_USERTYPE_ADMIN2 )
		setText( index, tr("advanced sys admin") );
	else if ( rank >= DJGAME_USERTYPE_MANAGER2 )
		setText( index, tr("system admin") );
	else if ( rank >= DJGAME_USERTYPE_GAMEADMIN )
		setText( index, tr("game admin") );
	else if ( rank >= DJGAME_USERTYPE_EMERGENCY )
		setText( index, tr("emergency") );
	else if ( rank >= DJGAME_USERTYPE_SERVICE )
		setText( index, tr("service") );
	else if ( rank >= DJGAME_USERTYPE_STAR ) {
		setText( index, tr("%1star player").arg(rank-DJGAME_USERTYPE_STAR+1) );
	}else if ( rank >= DJGAME_USERTYPE_VIP )
		setText( index, tr("vip") );
	else if ( rank >= DJGAME_USERTYPE_MEMBER )
		setText( index, tr("member") );
	else if ( rank >= DJGAME_USERTYPE_PLAYER )
		setText( index, tr("player") );
	else if ( rank >= DJGAME_USERTYPE_GUEST )
		setText( index, tr("player") );
	else
		setText( index, tr("player") );
}
void DJGameUserItem::setScore( int index )
{	
	setText( index, QString::number( m_user->score() ) );
}
void DJGameUserItem::setWins( int index )
{	
	setText( index, QString::number( m_user->wins() ) );
}
void DJGameUserItem::setLoses( int index )
{	
	setText( index, QString::number( m_user->loses() ) );
}
void DJGameUserItem::setWinRate( int index )
{	
	setText( index, QString("%1\%").arg(m_user->winRate() ) );
}
void DJGameUserItem::setChips( int index )
{
	setText( index, QString::number( m_user->chips() ) );
}
void DJGameUserItem::setMagic( int index )
{	
	setText( index, QString::number( m_user->magic() ) );
}
void DJGameUserItem::setDraws( int index )
{
	setText( index, QString::number( m_user->draws() ) );
}
void DJGameUserItem::setUnwelcome( int index )
{
	setText( index, QString::number( m_user->unwelcome() ) );
}
///////////////////////////////////////////////////
void DJGameUserItem::updateAll()
{
	updateUserName();
	updateNickName();
	updateGameStatus();
	updateTableSeat();
	updatePlatform();
	updateSpeed();
	updateScore();
	updateWins();
	updateLoses();
	updateWinRate();
	updateMoney();
	updateChips();
	updateBreakRate();
	updateOnlineTime();
	updateRank();
	updateMagic();
	updateDraws();
	updateUnwelcome();
}
void DJGameUserItem::updateUserName()
{
	int index	= gameController()->indexOfSection( DJGameController::UserName );
	if ( -1 != index )
		setUserName( index );
}
void DJGameUserItem::updateNickName()
{
	int index	= gameController()->indexOfSection( DJGameController::NickName );
	if ( -1 != index )
		setNickName( index );
}
void DJGameUserItem::updateGameStatus()
{
	int index	= gameController()->indexOfSection( DJGameController::GameStatus );
	if ( -1 != index )
		setGameStatus( index );
}
void DJGameUserItem::updateTableSeat()
{
	int index	= gameController()->indexOfSection( DJGameController::TableSeat );
	if ( -1 != index )
		setTableSeat( index );
}
void DJGameUserItem::updatePlatform()
{
	int index	= gameController()->indexOfSection( DJGameController::Platform );
	if ( -1 != index )
		setPlatform( index );
}
void DJGameUserItem::updateSpeed()
{
	int index	= gameController()->indexOfSection( DJGameController::Speed );
	if ( -1 != index )
		setSpeed( index );
}
void DJGameUserItem::updateScore()
{
	int index	= gameController()->indexOfSection( DJGameController::Score );
	if ( -1 != index )
		setScore( index );
}
void DJGameUserItem::updateWins()
{
	int index	= gameController()->indexOfSection( DJGameController::Wins );
	if ( -1 != index )
		setWins( index );
}
void DJGameUserItem::updateLoses()
{
	int index	= gameController()->indexOfSection( DJGameController::Loses );
	if ( -1 != index )
		setLoses( index );
}
void DJGameUserItem::updateWinRate()
{
	int index	= gameController()->indexOfSection( DJGameController::WinRate );
	if ( -1 != index )
		setWinRate( index );
}
void DJGameUserItem::updateBreakRate()
{
	int index	= gameController()->indexOfSection( DJGameController::BreakRate );
	if ( -1 != index )
		setBreakRate( index );
}
void DJGameUserItem::updateMoney()
{
	int index	= gameController()->indexOfSection( DJGameController::Money );
	if ( -1 != index )
		setMoney( index );
}
void DJGameUserItem::updateChips()
{
	int index	= gameController()->indexOfSection( DJGameController::Chips );
	if ( -1 != index )
		setChips( index );
}
void DJGameUserItem::updateMagic()
{
	int index	= gameController()->indexOfSection( DJGameController::Magic );
	if ( -1 != index )
		setMagic( index );
}
void DJGameUserItem::updateDraws()
{
	int index	= gameController()->indexOfSection( DJGameController::Draws );
	if ( -1 != index )
		setDraws( index );
}
void DJGameUserItem::updateOnlineTime()
{
	int index	 = gameController()->indexOfSection( DJGameController::OnlineTime );
	if ( -1 != index )
		setOnlineTime( index );
}
void DJGameUserItem::updateRank()
{
	int index	= gameController()->indexOfSection( DJGameController::Rank );
	if ( -1 != index )
		setRank( index );
}
void DJGameUserItem::updateUnwelcome()
{
	int index	= gameController()->indexOfSection( DJGameController::Unwelcome );
	if ( -1 != index )
		setUnwelcome( index );
}


