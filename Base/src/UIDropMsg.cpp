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

#include "UIDropMsg.h"
#include "protocol.h"
#include "DJGameController.h"
#include "Utility.h"
#include "GameUtility.h"
#include "DJClientRequest.h"

UIDropMsg::UIDropMsg( DJGameController *gameController, QWidget *parent ) 
	:QWidget( parent ),m_gameController(gameController)
{
	setupUi(this);
}
UIDropMsg::~UIDropMsg()
{
}
void UIDropMsg::on_okButton_clicked()
{
	djDebug()<<"UIDropMsg::on_okButton_clicked";
	
	QString broadcastMsg	= lineEdit->text();
    QByteArray	utf8	= broadcastMsg.toUtf8();
    	
	QByteArray	buf(sizeof(DJGameProtocol)+sizeof(ChatContent) + utf8.size() + 1,0);
    DJGameProtocol *protocol	= reinterpret_cast<DJGameProtocol *>(buf.data());
    protocol->chTotalLen	= buf.size();
    protocol->chType = DJGAME_PROTOCOL_TYPE_CHAT;
    protocol->chLanguage = m_gameController->language();     
        
    ChatContent	*chat	= reinterpret_cast<ChatContent *>(protocol->chBuf);
    setle4( &chat->userid_speaker, m_gameController->selfUserId() );
    setle4( &chat->userid_listener, DJGAME_PLAYER_BROADCAST );
    chat->chGameClass = hibyte(m_gameController->gameId());
    chat->chGame = lobyte(m_gameController->gameId());
    
    qstrcpy( chat->szSentence, utf8.data() );
    
	DJClientRequest request( buf );
	m_gameController->sendRequest( request );
	
	close();
}
void UIDropMsg::on_cancelButton_clicked()
{
	djDebug()<<"UIDropMsg::on_cancelButton_clicked";
	close();
}
