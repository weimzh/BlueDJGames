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

#include "DJAdminGroupChat.h"
#include "DJGameController.h"
#include "DJAdminBrowser.h"
#include "GameUtility.h"
#include "Utility.h"
#include "DJToolButton.h"

DJAdminGroupChat::DJAdminGroupChat( DJGameController* gameController, quint32 groupId, QWidget * parent )
	: QWidget( parent ),m_gameController(gameController),m_groupId(groupId)
{
	QVBoxLayout* layout	= new QVBoxLayout(this);
	layout->setMargin( 0 );
	layout->setSpacing( 0 );
	
	m_adminBrowser	= new DJAdminBrowser( gameController, groupId );
	layout->addWidget( m_adminBrowser );
	
	QHBoxLayout* buttonLayout	= new QHBoxLayout;
    layout->addLayout( buttonLayout );
    
    QSize	iconSize(32,32);
	
	DJToolButton *toolbutton	= new DJToolButton;
#ifdef DJ_TOOL_BUTTON_HAS_TEXT
	toolbutton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
#endif
	toolbutton->setText(tr("clear"));
	toolbutton->setToolTip( tr("clear") );
	toolbutton->setIcon( QIcon(":/BaseRes/image/chat/clear.png") );
	toolbutton->setIconSize( iconSize );
	connect( toolbutton, SIGNAL(clicked()), m_adminBrowser, SLOT(clear()) );
	buttonLayout->addWidget( toolbutton );
	/*
	QToolBar *bar	= new QToolBar( this );
	bar->setFixedHeight( 32 );
	bar->setIconSize( QSize(32,32) );
	bar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	bar->addAction(QIcon(":/BaseRes/image/chat/clear.png"),
					tr("clear"),
					m_adminBrowser,
					SLOT(clear()) );
	layout->addWidget( bar );
	*/	
	m_editor	= new QLineEdit;
	m_editor->setMaxLength(200);
    connect(m_editor, SIGNAL(returnPressed()), SLOT(sendAdminChat()) );
	layout->addWidget( m_editor );
}
DJAdminGroupChat::~DJAdminGroupChat()
{
}
void DJAdminGroupChat::receiveChat( const ChatContent* chatContent )
{
	m_adminBrowser->receiveChat( chatContent );
}
void DJAdminGroupChat::sendAdminChat()
{
	QString	adminChat	= m_editor->text();
	if ( adminChat.isEmpty() )
		return;
		
	CallAdminInfo	info;
	memset( &info, 0, sizeof(CallAdminInfo) );
	info.callerId	= 0;//ignore
	info.roomId		= 0;//ignore
	info.tableId	= 0;//ignore
	info.reason		= 0;//ignore
	info.eventType	= ADMIN_CHAT_EVENT_TYPE_ADMIN_CHAT;
	
	QString		sentence;
	QTextStream	stream( &sentence, QIODevice::ReadWrite );
	stream <<info.callerId<<" "<<info.roomId<<" "<<info.tableId
			<<" "<<info.reason<<" "<<info.eventType;
	sentence.prepend("{{");
	sentence.append("}}");
	sentence.append( adminChat );
	
	QByteArray	utf8	= sentence.toUtf8();

	QByteArray	chatContent( sizeof(ChatContent) + utf8.size() + 1, 0 );
    ChatContent	*pChatContent	= reinterpret_cast<ChatContent *>(chatContent.data());
    setle4( &pChatContent->userid_speaker, m_gameController->selfUserId() );
    setle4( &pChatContent->userid_listener, m_groupId );
    pChatContent->chGameClass = 0xFF;
    pChatContent->chGame = 0xFF;
    qstrcpy( pChatContent->szSentence, utf8.data() );
    
	m_gameController->requestChat( pChatContent, chatContent.size() );
	
	m_editor->clear();
}


