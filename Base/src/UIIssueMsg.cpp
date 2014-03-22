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

#include "UIIssueMsg.h"
#include "protocol.h"
#include "DJGameController.h"
#include "Utility.h"

UIIssueMsg::UIIssueMsg( DJGameController *gameController, QWidget *parent ) 
	:QDialog( parent ),m_gameController(gameController)
{
	setupUi(this);
	
	comboMsgType->addItem( tr("room msg"), QVariant(DJGAME_SENDROOMTEXT_OPTION_ROOMWEL) );
	comboMsgType->addItem( tr("game panel msg"), QVariant(DJGAME_SENDROOMTEXT_OPTION_GAMEWEL) );
	comboMsgType->addItem( tr("ad msg"), QVariant(DJGAME_SENDROOMTEXT_OPTION_AD) );
	
	reloadMsg( true );
}
UIIssueMsg::~UIIssueMsg()
{
}
quint8 UIIssueMsg::currentMsgType() const
{
	int index	= comboMsgType->currentIndex();
	return comboMsgType->itemData( index ).toUInt();
}
quint8 UIIssueMsg::currentMsgIndex() const
{
	return comboMsgIndex->currentText().toUInt();
}
QString UIIssueMsg::messageToBeIssued() const
{
	return teMsg->toPlainText();
}
void UIIssueMsg::reloadMsg( bool initMsgIndexCombo )
{
	QStringList	msgs;
	quint8 msgType	= currentMsgType();
	switch( msgType ) {
	case DJGAME_SENDROOMTEXT_OPTION_ROOMWEL:
		msgs	= m_gameController->roomMsgs();
		break;
	case DJGAME_SENDROOMTEXT_OPTION_GAMEWEL:
		msgs	= m_gameController->panelMsgs();
		break;
	case DJGAME_SENDROOMTEXT_OPTION_AD:
		msgs	= m_gameController->adMsgs();
		break;
	default:
		break;
	}
	const QStringList& refMsgs	= msgs;
	if ( initMsgIndexCombo ) {
		comboMsgIndex->clear();
		for( int i = 0; i < refMsgs.size(); ++i ) {
			comboMsgIndex->addItem( QString::number(i+1) );
		}
	}
	quint8	msgIndex	= currentMsgIndex();
	if ( msgIndex ) {
		QString msg	= refMsgs.value( msgIndex - 1, QString() );
		tbMsg->setPlainText( msg );
	}else
		tbMsg->clear();
}
void UIIssueMsg::on_comboMsgType_activated( int index )
{
	djDebug()<<"on_comboMsgType_activated";
	reloadMsg( true );
}
void UIIssueMsg::on_comboMsgIndex_activated( int index )
{
	djDebug()<<"on_comboMsgIndex_activated";
	reloadMsg( false );
}
void UIIssueMsg::on_issueMsgButton_clicked()
{
	djDebug()<<"on_issueMsgButton_clicked";
	quint8 msgType	= currentMsgType();
	quint8 msgIndex	= currentMsgIndex();
	QString msg	= messageToBeIssued();
	djDebug()<<hex<<"msgType="<<msgType<<"msgIndex="<<msgIndex<<"msg="<<msg;
	if ( !msg.isEmpty() ) {		
		DJGameAdminCommand command;
		memset( &command, 0, sizeof(DJGameAdminCommand) );
		setle4( &command.userid, m_gameController->selfUserId() );
		command.chCommand	= DJGAME_PROTOCOL_SUBTYPE_ADMIN_COMMAND_REPLACETEXT;
		command.chParam1	= msgType;
		command.chParam2	= msgIndex;
		m_gameController->requestAdminCommand( DJGAME_PROTOCOL_TYPE_SUBMIT_GAMEDATA, DJGAME_PROTOCOL_SUBTYPE_ADMIN, command, msg );
	}
	accept();
}
void UIIssueMsg::on_removeMsgButton_clicked()
{
	djDebug()<<"on_removeMsgButton_clicked";
	quint8 msgType	= currentMsgType();
	quint8 msgIndex	= currentMsgIndex();
	QString msg;
	djDebug()<<hex<<"msgType="<<msgType<<"msgIndex="<<msgIndex;
	DJGameAdminCommand command;
	memset( &command, 0, sizeof(DJGameAdminCommand) );
	setle4( &command.userid, m_gameController->selfUserId() );
	command.chCommand	= DJGAME_PROTOCOL_SUBTYPE_ADMIN_COMMAND_REPLACETEXT;
	command.chParam1	= msgType;
	command.chParam2	= msgIndex;
	m_gameController->requestAdminCommand( DJGAME_PROTOCOL_TYPE_SUBMIT_GAMEDATA, DJGAME_PROTOCOL_SUBTYPE_ADMIN, command, msg );
	accept();
}
