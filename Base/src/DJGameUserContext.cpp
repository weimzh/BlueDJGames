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

#include "DJGameUserContext.h"
#include "DJGameUser.h"
#include "DJShowView.h"
#include "DJGameController.h"
#include "DJTableController.h"
#include "DJMenuButton.h"
#include "UIAdminCommand.h"
#include "Utility.h"
#include "GameUtility.h"
#include "DJClientRequest.h"
//#include "UIGameCards.h"
#include "DJCardsWidget.h"
#include "DJGameRoom.h"

DJGameUserContext::DJGameUserContext( DJGameController	*gameController, QWidget * parent, Qt::WFlags f )
	:QWidget(parent,f),m_gameController(gameController)
{
	djDebug() << "DJGameUserContext constructor";
	
	m_clickedUserId	= 0;
	
	QHBoxLayout* mainLayout	= new QHBoxLayout(this);
	mainLayout->setMargin( 10 );
	//mainLayout->setSpacing( 0 );
	//mainLayout->setSizeConstraint(QLayout::SetFixedSize);
	
	m_showView	= new DJShowView( 0 );
	QSize size	= m_showView->realSize();
	m_showView->setFixedSize( size );
	mainLayout->addWidget( m_showView );
	
	QVBoxLayout *rightLayout	= new QVBoxLayout;
	rightLayout->setMargin( 10 );
	rightLayout->setSpacing( 0 );
	//rightLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addLayout( rightLayout );
    
    
    m_userInfo		= new QLabel;
    rightLayout->addWidget( m_userInfo, 1 );
    
    //function menu
    m_actLookOn	= new QAction( tr("look on this table"), this );
	connect( m_actLookOn, SIGNAL(triggered()), SLOT(handleLookOn()) );
	m_actChat	= new QAction( tr("chat with this user"), this );
	connect( m_actChat, SIGNAL(triggered()), SLOT(handleChat()) );
	m_actInvite	= new QAction( tr("invite this user to join"), this );
	connect( m_actInvite, SIGNAL(triggered()), SLOT(handleInvite()) );
	m_actForceStandUp	= new QAction( tr("force this user stand up"), this );
	connect( m_actForceStandUp, SIGNAL(triggered()), SLOT(handleForceStandUp()) );
    m_menuFunction	= new QMenu( this );
	m_btnFunction	= new DJMenuButton;
    m_btnFunction->setText( tr("functions") );
    m_btnFunction->setMenu( m_menuFunction );
    rightLayout->addWidget( m_btnFunction, 1 );
    
    //contact menu
    m_actFriend	= new QAction( tr("add to friendlist"), this );
	m_actFriend->setCheckable( true );
	connect( m_actFriend, SIGNAL(triggered(bool)), SLOT(handleContactFriend(bool)) );
	m_actUnwelcome	= new QAction( tr("add to blacklist"), this );
	m_actUnwelcome->setCheckable( true );
	connect( m_actUnwelcome, SIGNAL(triggered(bool)), SLOT(handleContactUnwelcome(bool)) );
    m_menuContact	= new QMenu( this );
    m_menuContact->addAction( m_actFriend );
	m_menuContact->addAction( m_actUnwelcome );
    m_btnContact	= new DJMenuButton;
    m_btnContact->setText( tr("manage contacts") );
    m_btnContact->setMenu( m_menuContact );
    rightLayout->addWidget( m_btnContact, 1 );
    
    //admin menu
    m_actAdminDeltaScore = new QAction( tr("Delta ") + m_gameController->baseSectionName(DJGameController::Score), this );
	connect( m_actAdminDeltaScore, SIGNAL(triggered()), SLOT(handleAdminDeltaScore()) );
    //m_actAdminKick	= new QAction( tr("Kick out"), this );
	//connect( m_actAdminKick, SIGNAL(triggered()), SLOT(handleAdminKick()) );
	m_actAdminShutUp	= new QAction( tr("Shut up"), this );
	connect( m_actAdminShutUp, SIGNAL(triggered()), SLOT(handleAdminShutUp()) );
	m_actAdminDeltaMoney	= new QAction( tr("Delta ") + m_gameController->baseSectionName(DJGameController::Money), this );
	connect( m_actAdminDeltaMoney, SIGNAL(triggered()), SLOT(handleAdminDeltaMoney()) );
	m_actAdminDeltaMagic	= new QAction( tr("Delta ") + m_gameController->baseSectionName(DJGameController::Magic), this );
	connect( m_actAdminDeltaMagic, SIGNAL(triggered()), SLOT(handleAdminDeltaMagic()) );
	m_actAdminDeltaChips	= new QAction( tr("Delta ") + m_gameController->baseSectionName(DJGameController::Chips), this );
	connect( m_actAdminDeltaChips, SIGNAL(triggered()), SLOT(handleAdminDeltaChips()) );
	m_actAdminBanIp	= new QAction( tr("Ban Ip"), this );
	connect( m_actAdminBanIp, SIGNAL(triggered()), SLOT(handleAdminBanIp()) );
	m_actAdminBanAccount	= new QAction( tr("Ban Account"), this );
	connect( m_actAdminBanAccount, SIGNAL(triggered()), SLOT(handleAdminBanAccount()) );
	m_actAdminRobot	= new QAction( tr("Robot"), this );
	connect( m_actAdminRobot, SIGNAL(triggered()), SLOT(handleRobotChat()) );
	
	m_menuAdmin	= new QMenu( tr("Admin Command"), this );
    m_btnAdmin	= new DJMenuButton;
    m_btnAdmin->setText( tr("Admin command") );
    m_btnAdmin->setMenu( m_menuAdmin );
    rightLayout->addWidget( m_btnAdmin, 1 );
    
    rightLayout->addStretch();
	m_labelCards	= new QLabel( tr("cards") );
	rightLayout->addWidget( m_labelCards, 1 );
	m_cardsWidget	= new DJCardsWidget(m_gameController);
	rightLayout->addWidget( m_cardsWidget, 1 );
}
DJGameUserContext::~DJGameUserContext()
{
	djDebug()<<"DJGameUserContext destructor";
}
void DJGameUserContext::moveToAppropriatePos()
{
	QDesktopWidget desktop;
	
	QRect	availableRect	= desktop.availableGeometry();
	QPoint	currentPos	= QCursor::pos();
	QSize	contextSize	= size();
	QRect	contextRect( currentPos, contextSize );	
	
	if ( !availableRect.contains( contextRect ) ) {
		QPoint	rightTop	= QPoint( currentPos.x() + contextSize.width(), currentPos.y() );
		QPoint	leftBottom	= QPoint( currentPos.x(), currentPos.y() + contextSize.height() );
	
		if ( !availableRect.contains(rightTop) ) {
			currentPos.rx()	-= contextSize.width();
		}
		if ( !availableRect.contains(leftBottom) ) {
			currentPos.ry()	-= contextSize.height();
		}
	}
	move( currentPos );
}
void DJGameUserContext::showUserContext( DJGameUser *clickedUser )
{
	djDebug() << "DJGameUserContext::showContext" << clickedUser->userName();
	hideAllWidgets();
	
	DJGameUser* selfUser	= m_gameController->selfUser();
	if ( !selfUser )
		return;
		
	m_clickedUserId	= clickedUser->userId();
	
	createUserInfo( clickedUser );
	int funcMenuItems	= createFunctionMenu( clickedUser, selfUser );
	createContactMenu( clickedUser );
	int adminMenuItems	= createAdminMenu( clickedUser, selfUser );
	
	m_showView->setUser( clickedUser );
	m_showView->show();
		
	m_userInfo->show();
	
	if ( 0 == parent() ) {
		//game hall
		if ( selfUser->userId() != m_clickedUserId ) {
			if ( funcMenuItems )
        		m_btnFunction->show();
        	m_btnContact->show();   	
			if ( adminMenuItems ) {
        		m_btnAdmin->show();
        	}
    	}
	}else {
		//game panel
		if ( //!(m_gameController->selfRoom()->options() & DJGAME_ROOMOPTION_COUNTER)
			m_gameController->selfRoom()->numberOfSeats() > 1
			&& selfUser->userId() == m_clickedUserId ) {
			m_labelCards->show();
			m_cardsWidget->loadCards();
			m_cardsWidget->show();
		}
	}
	adjustSize();
	if ( 0 == parent() ) {
		moveToAppropriatePos();
	}
	show();
}
void DJGameUserContext::handleLookOn()
{
	m_gameController->requestLookOn( m_clickedUserId );
	close();
}
void DJGameUserContext::handleChat()
{
	m_gameController->chatWithUser( m_clickedUserId, false );
	close();
}
void DJGameUserContext::handleRobotChat()
{
	m_gameController->chatWithUser( m_clickedUserId, true );
	close();
}
void DJGameUserContext::handleInvite()
{
	m_gameController->requestInviteUser( m_clickedUserId );
	close();
}
void DJGameUserContext::handleForceStandUp()
{
	m_gameController->requestForceStandUp( m_clickedUserId );
	close();
}
void DJGameUserContext::handleContactFriend( bool checked )
{
	if ( checked ) {
		m_gameController->addContactUserFriend( m_clickedUserId );
	}else {
		m_gameController->removeContactUserFriend( m_clickedUserId );
	}
	close();
}
void DJGameUserContext::handleContactUnwelcome( bool checked )
{
	if ( checked ) {
		m_gameController->addContactUserUnwelcome( m_clickedUserId );
	}else {
		m_gameController->removeContactUserUnwelcome( m_clickedUserId );
	}
	close();
}
/*
void DJGameUserContext::handleAdminKick()
{
	DJGameAdminCommand command;
	setle4( &command.userid, clickedUserId() );
	command.chCommand	= DJGAME_PROTOCOL_ADMIN_SUBTYPE_KICK;
	
	m_gameController->requestAdminCommand( DJGAME_PROTOCOL_TYPE_ADMIN, DJGAME_PROTOCOL_ADMIN_SUBTYPE_KICK, command );
	close();
}
*/
void DJGameUserContext::handleAdminShutUp()
{
	int rankDifference	= 0;
	DJGameUser *selfUser	= m_gameController->selfUser();
	DJGameUser *user	= m_gameController->gameUser( clickedUserId() );
	if ( selfUser && user ) {
		rankDifference	= selfUser->gameRank() - user->gameRank();
	}
	UIAdminCommand adminDlg(user,this,UIAdminCommand::CommandTypeShutUp, rankDifference );
	adminDlg.setWindowTitle( m_actAdminShutUp->text() );
	adminDlg.setHintText(tr("please input minutes :"));
	if ( QDialog::Accepted == adminDlg.exec() ) {
		quint8 shutupTime	= adminDlg.commandParameter().toUInt();
		DJGameAdminCommand command;
		memset( &command, 0, sizeof(DJGameAdminCommand) );
		setle4( &command.userid, clickedUserId() );
		command.chCommand	= DJGAME_PROTOCOL_ADMIN_SUBTYPE_SHUTUP;
		command.chParam1	= shutupTime;
		m_gameController->requestAdminCommand( DJGAME_PROTOCOL_TYPE_ADMIN, DJGAME_PROTOCOL_ADMIN_SUBTYPE_SHUTUP, command );
	}
	close();
}
void DJGameUserContext::handleAdminDeltaMoney()
{
	UIAdminCommand adminDlg(m_gameController->gameUser( clickedUserId() ),this);
	adminDlg.setWindowTitle( m_actAdminDeltaMoney->text() );
	adminDlg.setHintText(tr("please input a number : \npositive means add, negative means subtract"));
	if ( QDialog::Accepted == adminDlg.exec() ) {
		DJGameAdminCommand command;
		memset( &command, 0, sizeof(DJGameAdminCommand) );
		setle4( &command.userid, clickedUserId() );
		command.chCommand	= DJGAME_PROTOCOL_ADMIN_SUBTYPE_TXMONEY;
		qint32 delta	= adminDlg.commandParameter().toInt();
		setle4( command.chBuf, delta );
		m_gameController->requestAdminCommand( DJGAME_PROTOCOL_TYPE_ADMIN, DJGAME_PROTOCOL_ADMIN_SUBTYPE_TXMONEY, command );
	}
	close();
}
void DJGameUserContext::handleAdminDeltaMagic()
{
	UIAdminCommand adminDlg(m_gameController->gameUser( clickedUserId() ),this);
	adminDlg.setWindowTitle( m_actAdminDeltaMagic->text() );
	adminDlg.setHintText(tr("please input a number : \npositive means add, negative means subtract"));
	if ( QDialog::Accepted == adminDlg.exec() ) {
		DJGameAdminCommand command;
		memset( &command, 0, sizeof(DJGameAdminCommand) );
		setle4( &command.userid, clickedUserId() );
		command.chCommand	= DJGAME_PROTOCOL_ADMIN_SUBTYPE_TXMAGIC;
		qint32 delta	= adminDlg.commandParameter().toInt();
		setle4( command.chBuf, delta );
		m_gameController->requestAdminCommand( DJGAME_PROTOCOL_TYPE_ADMIN, DJGAME_PROTOCOL_ADMIN_SUBTYPE_TXMAGIC, command );
	}
	close();
}
void DJGameUserContext::handleAdminDeltaChips()
{
	UIAdminCommand adminDlg(m_gameController->gameUser( clickedUserId() ),this);
	adminDlg.setWindowTitle( m_actAdminDeltaChips->text() );
	adminDlg.setHintText(tr("please input a number : \npositive means add, negative means subtract"));
	if ( QDialog::Accepted == adminDlg.exec() ) {
		DJGameAdminCommand command;
		memset( &command, 0, sizeof(DJGameAdminCommand) );
		setle4( &command.userid, clickedUserId() );
		command.chCommand	= DJGAME_PROTOCOL_ADMIN_SUBTYPE_TXCOUNTER;
		qint32 delta	= adminDlg.commandParameter().toInt();
		setle4( command.chBuf, delta );
		m_gameController->requestAdminCommand( DJGAME_PROTOCOL_TYPE_ADMIN, DJGAME_PROTOCOL_ADMIN_SUBTYPE_TXCOUNTER, command );
	}
	close();
}
void DJGameUserContext::handleAdminBanIp()
{
	UIAdminCommand adminDlg(m_gameController->gameUser( clickedUserId() ),this,UIAdminCommand::CommandTypeBanIp);
	adminDlg.setWindowTitle( m_actAdminBanIp->text() );
	adminDlg.setHintText(tr("WARNING:\nBan IP will cause all users with this same IP can not login"));
	if ( QDialog::Accepted == adminDlg.exec() ) {
		DJGameAdminCommand command;
		memset( &command, 0, sizeof(DJGameAdminCommand) );
		setle4( &command.userid, clickedUserId() );
		command.chCommand	= DJGAME_PROTOCOL_ADMIN_SUBTYPE_BANNEDIP;
	
		m_gameController->requestAdminCommand( DJGAME_PROTOCOL_TYPE_ADMIN, DJGAME_PROTOCOL_ADMIN_SUBTYPE_BANNEDIP, command );
	}
	close();
}
void DJGameUserContext::handleAdminBanAccount()
{
	UIAdminCommand adminDlg(m_gameController->gameUser( clickedUserId() ),this,UIAdminCommand::CommandTypeBanAccount);
	adminDlg.setWindowTitle( m_actAdminBanAccount->text() );
	adminDlg.setHintText(tr("This account will can not login for a while"));
	if ( QDialog::Accepted == adminDlg.exec() ) {
		DJGameAdminCommand command;
		memset( &command, 0, sizeof(DJGameAdminCommand) );
		setle4( &command.userid, clickedUserId() );
		command.chCommand	= DJGAME_PROTOCOL_ADMIN_SUBTYPE_BANNEDACCOUNT;
	
		m_gameController->requestAdminCommand( DJGAME_PROTOCOL_TYPE_ADMIN, DJGAME_PROTOCOL_ADMIN_SUBTYPE_BANNEDACCOUNT, command );
	}
	close();
}
void DJGameUserContext::handleAdminDeltaScore()
{
	UIAdminCommand adminDlg(m_gameController->gameUser( clickedUserId() ),this);
	adminDlg.setWindowTitle( m_actAdminDeltaScore->text() );
	adminDlg.setHintText(tr("please input a number : \npositive means add, negative means subtract"));
	if ( QDialog::Accepted == adminDlg.exec() ) {
		qint16 delta	= adminDlg.commandParameter().toInt();
		DJGameAdminCommand command;
		memset( &command, 0, sizeof(DJGameAdminCommand) );
		setle4( &command.userid, clickedUserId() );
		command.chCommand	= DJGAME_PROTOCOL_SUBTYPE_ADMIN_COMMAND_INCSCORE;
		setle4( &command.chParam1, delta );
		m_gameController->requestAdminCommand( DJGAME_PROTOCOL_TYPE_SUBMIT_GAMEDATA, DJGAME_PROTOCOL_SUBTYPE_ADMIN, command );
	}
	close();
}

void DJGameUserContext::createUserInfo( DJGameUser* clickedUser )
{
	QString info	= m_gameController->userContextInfo( clickedUser );
	m_userInfo->setText( info );
}
int DJGameUserContext::createFunctionMenu( DJGameUser* clickedUser, DJGameUser *selfUser )
{
	m_menuFunction->clear();
	int	count	= 0;
	
	if ( ( selfUser->isDown() || selfUser->isReady() )
		 && !clickedUser->isPlaying()
		 && DJGAME_USER_STATUS_NORMAL == clickedUser->generalStatus() 
		 && clickedUser->tableId() != selfUser->tableId() ) {
        m_menuFunction->addAction( m_actInvite );
        count++;
	}
	if ( 0 != selfUser->tableId()
		&& selfUser->tableId() == clickedUser->tableId()
		&& !selfUser->isPlaying() ) {
        DJTableController*	tc	= m_gameController->tableController( selfUser->tableId() );
        if ( tc && tc->isPreceding( selfUser, clickedUser ) ) {
        	m_menuFunction->addAction( m_actForceStandUp );
        	count++;
        }
	}
	QList<quint8> tableIds	=  m_gameController->allPanelTableIds();
	if ( clickedUser->isPlaying()
		&& !tableIds.contains( clickedUser->tableId()) ) {
        m_menuFunction->addAction( m_actLookOn );
        count++;
    }
    if ( DJ_CONTACT_USER_TYPE_FRIEND == m_gameController->contactUserType(clickedUser->userId()) ) {
        m_menuFunction->addAction( m_actChat );
        count++;
    }
    return count;
}
void DJGameUserContext::createContactMenu( DJGameUser* clickedUser )
{
	m_actFriend->setChecked( false );
	m_actUnwelcome->setChecked( false );
	
	quint32 userId	= clickedUser->userId();
	
	int contactUserType	= m_gameController->contactUserType( userId );
	switch( contactUserType ) {
	case DJ_CONTACT_USER_TYPE_FRIEND:
		m_actFriend->setChecked( true );
		break;
	case DJ_CONTACT_USER_TYPE_UNWELCOME:
		m_actUnwelcome->setChecked( true );
		break;
	case DJ_CONTACT_USER_TYPE_NONE:
		break;
	default:
		m_actFriend->setEnabled( false );
		m_actUnwelcome->setEnabled( false );
		break;
	}
}
int DJGameUserContext::createAdminMenu( DJGameUser* clickedUser, DJGameUser *selfUser )
{
	m_menuAdmin->clear();
	int	count	= 0;
	
	if ( selfUser->gameRank() >= DJGAME_USERTYPE_VIP
		&& selfUser->gameRank() > clickedUser->gameRank() ) {
		m_menuAdmin->addAction( m_actAdminShutUp );
		count++;
	}
	if ( selfUser->gameRank() >= DJGAME_USERTYPE_GAMEADMIN
		&& selfUser->gameRank() > clickedUser->gameRank() ) {
		m_menuAdmin->addAction( m_actAdminDeltaScore );
		count++;
		if ( selfUser->globalRank() >= DJGAME_USERTYPE_GAMEADMIN
			&& selfUser->gameRank() > clickedUser->gameRank() ) {
			m_menuAdmin->addAction( m_actAdminDeltaMoney );
			m_menuAdmin->addAction( m_actAdminDeltaMagic );
			m_menuAdmin->addAction( m_actAdminDeltaChips );
			m_menuAdmin->addAction( m_actAdminBanIp );
			m_menuAdmin->addAction( m_actAdminBanAccount );
			count	+= 5;
			if ( clickedUser->isRobot() ) {
				m_menuAdmin->addAction( m_actAdminRobot );
				count++;
			}
		}
	}
	return count;
}
void DJGameUserContext::hideAllWidgets()
{
	m_userInfo->hide();
	m_btnFunction->hide();
	m_btnContact->hide();
	m_btnAdmin->hide();
	m_labelCards->hide();
	m_cardsWidget->hide();
}

