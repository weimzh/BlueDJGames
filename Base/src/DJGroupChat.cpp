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

#include "DJGroupChat.h"
#include "DJSymbol.h"
#include "DJGameController.h"
#include "DJGameUser.h"
#include "DJClientRequest.h"
#include "DJTextBrowser.h"
#include "Utility.h"
#include "GameUtility.h"
#include "PlayWave.h"
#include "DJToolButton.h"

DJGroupChat::DJGroupChat( DJGameController* gameController, quint32 groupId, Type type, QWidget * parent, Qt::WFlags f )
	:QWidget( parent, f ),m_gameController(gameController),m_groupId(groupId),m_type(type)
{
	djDebug() << "DJGroupChat constructor";
	
	QVBoxLayout* layout	= new QVBoxLayout(this);
	layout->setMargin( 0 );
	layout->setSpacing( 0 );
	
    m_browser	= new DJTextBrowser;
    
    QVBoxLayout* browserLayout	= new QVBoxLayout( m_browser );
    m_symbolWidget	= new DJSymbol( m_browser );
    connect(m_symbolWidget, SIGNAL(playerSymbolSelected(const QString&)),SLOT(insertPlayerSymbol(const QString&)));
    connect(m_symbolWidget, SIGNAL(memberSymbolSelected(const QString&)),SLOT(insertMemberSymbol(const QString&)));
    browserLayout->addWidget( m_symbolWidget );
    m_symbolWidget->hide();
    
    layout->addWidget( m_browser );
    
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
	connect( toolbutton, SIGNAL(clicked()), m_browser, SLOT(clearBrowserContents()) );
	buttonLayout->addWidget( toolbutton );
	
	toolbutton	= new DJToolButton;
#ifdef DJ_TOOL_BUTTON_HAS_TEXT
	toolbutton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
#endif
	toolbutton->setText(tr("symbol"));
	toolbutton->setToolTip( tr("symbol") );
	toolbutton->setIcon( QIcon(":/BaseRes/image/chat/symbol.png") );
	toolbutton->setIconSize( iconSize );
	connect( toolbutton, SIGNAL(clicked()), SLOT(toggleSymbol()) );
	buttonLayout->addWidget( toolbutton );
	
	toolbutton	= new DJToolButton;
#ifdef DJ_TOOL_BUTTON_HAS_TEXT
	toolbutton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
#endif
	toolbutton->setText(tr("send"));
	toolbutton->setToolTip( tr("send") );
	toolbutton->setIcon( QIcon(":/BaseRes/image/chat/send.png") );
	toolbutton->setIconSize( iconSize );
	connect( toolbutton, SIGNAL(clicked()), SLOT(sendChat()) );
	buttonLayout->addWidget( toolbutton );
											
    m_editor	= new QLineEdit;
    m_editor->setMaxLength(200);
    connect(m_editor, SIGNAL(returnPressed()), SLOT(sendChat()) );
    layout->addWidget( m_editor );
        
    QStringList	searchPaths;
    searchPaths << "images" ;
    
	m_browser->setSearchPaths( searchPaths );
	
	DJGameUser* selfUser	= m_gameController->selfUser();
	if ( selfUser && selfUser->gameRank() < DJGAME_USERTYPE_MEMBER ) {
		m_symbolWidget->removeMemberWidget();
	}
    connect(m_browser, SIGNAL(memberImageDone()), m_symbolWidget, SLOT(reloadMemberWidget()) );
}
DJGroupChat::~DJGroupChat()
{
	djDebug() << "DJGroupChat destructor";
}
void DJGroupChat::receiveChat( const ChatContent* chatContent )
{
	djDebug() << "DJGroupChat::receiveChat";
	quint32	speakerId	= letoh4( &chatContent->userid_speaker );
	quint32	listenerId	= letoh4( &chatContent->userid_listener );
	QString sentence	= QString::fromUtf8( chatContent->szSentence );
			
	DJGameUser* selfUser	= m_gameController->selfUser();
	if ( sentence.isEmpty() || 0 == selfUser )
		return;	
	djDebug() << "sentence="<<sentence;
	
	bool isAppend	= false;
	
	QString	paragragh	= QString("[%1]").arg(QTime::currentTime().toString("hh:mm:ss"));
	paragragh.prepend("<font color=#008000 size=3>");
	paragragh.append("</font>");
	if ( speakerId <= 0xFF ) {
		//system msg
		InterpretText( sentence );
		switch( speakerId ) {
		case DJGAME_BLUEDJ_ROOM_ID:
			paragragh.append("<font color=#FF0000 size=3>");
			paragragh.append( tr("room message") );
			break;
		case DJGAME_BLUEDJ_GAME_ID:
			paragragh.append("<font color=#FF0000 size=3>");
			paragragh.append( tr("game panel message") );
			break;
		case DJGAME_AD_ID:
			paragragh.append("<font color=#FF0000 size=3>");
			paragragh.append( tr("system ad") );
			break;
		default:
			paragragh.append("<font color=#FF0000 size=3>");
			paragragh.append( tr("unknown system message") );
			break;	
		}
	}else {
		DJGameUser* speaker	= m_gameController->gameUser( speakerId );
    	if ( 0 == speaker )
    		return;
    	//if ( 10000029 == speakerId )
    		//isAppend	= true;
    		
    	if ( DJGAME_PLAYER_BROADCAST == listenerId ) {
			paragragh.append("<font color=#FF0000 size=3>");
			paragragh.append( speaker->userName() );
			paragragh.append(" ");
			paragragh.append( tr("is broadcasting") );
			isAppend	= true;
		}else {
			//user msg
    		if ( speaker->gameRank() < DJGAME_USERTYPE_MEMBER ) {
    			djDebug() << "remove html tag";
    			RemoveHtmlTag( sentence );
    			RemoveMemberPrefix( sentence );
    		}
    		paragragh.append("<font color=#0000A0 size=3>");
    		paragragh.append( speaker->userName() );
    		
    		bool ring	= InterpretText( sentence );
    		if ( ring && ( GameHall == m_type && (selfUser->gameStatus() < GAME_STATUS_READY) || GamePanel == m_type ) ) {
    			djDebug() << "ring";
    			m_gameController->playWave("wav/call.wav");
    		}
		}
	}
	paragragh.append(" : ");
	paragragh.append("</font>");
	
	//add sentence to paragraph
	//NOTE:add color first,otherwise chaos
	sentence.prepend("<font color=#008080 size=3>");
	sentence.append("</font>");
	
	paragragh	+= (sentence);
	
	if ( isAppend )
		m_browser->appendText( paragragh );
	else
		m_browser->insertText( paragragh );
	
	//QKeyEvent event(QEvent::KeyPress,Qt::Key_End,Qt::NoModifier);
	//QApplication::sendEvent(m_browser, &event);
}
void DJGroupChat::insertText2Editor( const QString& text )
{
	if ( text.isEmpty() )
		return;
	m_editor->insert( text );
	//m_browser->moveCursorToEnd();
	//QKeyEvent event(QEvent::KeyPress,Qt::Key_End,Qt::NoModifier);
	//QApplication::sendEvent(m_browser, &event);
}
void DJGroupChat::insertText2Browser( const QString& text, bool timeStamp )
{
	if ( text.isEmpty() )
		return;
	QString html( text );
	if ( timeStamp ) {
		html.prepend( QString("[%1]").arg(QTime::currentTime().toString("hh:mm:ss")) );
	}
	html.replace("\n","<br>");
	html.prepend("<font color=#808000 size=3>");
	html.append("</font>");
	m_browser->insertText( html );
	
	//QKeyEvent event(QEvent::KeyPress,Qt::Key_End,Qt::NoModifier);
	//QApplication::sendEvent(m_browser, &event);
}
void DJGroupChat::sendChat()
{
	QString text	= m_editor->text();
	djDebug()<<"DJGroupChat::sendChat"<<text;
	DJGameUser* selfUser	= m_gameController->selfUser();
	if ( selfUser && selfUser->gameRank() < DJGAME_USERTYPE_MEMBER ) {
		RemoveHtmlTag( text );
	}
	if ( !text.isEmpty() ) {
		QByteArray utf8	= text.toUtf8();
		
        QByteArray	chatContent( sizeof(ChatContent) + utf8.size() + 1, 0 );
        ChatContent	*pChatContent	= reinterpret_cast<ChatContent *>(chatContent.data());
        setle4( &pChatContent->userid_speaker, m_gameController->selfUserId() );
        if ( DJGroupChat::GameHall == m_type )
        	setle4( &pChatContent->userid_listener, MakeGroupId( m_gameController->gameId(), m_gameController->selfRoomId() ) );
        else
        	setle4( &pChatContent->userid_listener, m_groupId );
        pChatContent->chGameClass = 0xFF;
        pChatContent->chGame = 0xFF;
        qstrcpy( pChatContent->szSentence, utf8.data() );
        
        m_gameController->requestChat( pChatContent, chatContent.size() );
	}
	m_editor->clear();
}

void DJGroupChat::toggleSymbol()
{
	if ( m_symbolWidget->isVisible() ) {
		m_symbolWidget->hide();
	}else {
		m_symbolWidget->show();
	}
	m_browser->moveCursorToEnd();
	//QKeyEvent event(QEvent::KeyPress,Qt::Key_End,Qt::NoModifier);
	//QApplication::sendEvent(m_browser, &event);
	m_editor->setFocus( Qt::OtherFocusReason );
}
void DJGroupChat::insertPlayerSymbol( const QString& name )
{
	djDebug() << "insertPlayerSymbol" << name;
	
	uint	size	= 32;
	Qt::KeyboardModifiers	modifiers	= QApplication::keyboardModifiers();
	if ( Qt::ShiftModifier == modifiers )
		size	= 64;
	else if ( Qt::ControlModifier == modifiers )
		size	= 16;
	m_editor->insert( QString("{{%1,%2}}").arg(name).arg(QString::number(size)) );
	toggleSymbol();
}
void DJGroupChat::insertMemberSymbol( const QString& name )
{
	djDebug() << "insertMemberSymbol" << name;
	m_editor->insert( QString("{{%1}}").arg(name) );
	toggleSymbol();
}

