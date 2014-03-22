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

#include "DJChat.h"
#include "DJContact.h"
#include "DJSymbol.h"
#include "DJTextBrowser.h"
#include "Utility.h"
#include "PlayWave.h"
#include "DJToolButton.h" 
#include "DJGlobal.h"
 
DJTextEdit::DJTextEdit( QWidget * parent )
	:QTextEdit(parent)
{
}
DJTextEdit::~DJTextEdit()
{
}
void DJTextEdit::keyPressEvent ( QKeyEvent * e )
{
	int key	= e->key();
	if ( Qt::Key_Return == key || Qt::Key_Enter == key ) {
		djDebug() << "returnPressed";
		emit returnPressed();
	}else {
		QTextEdit::keyPressEvent( e );
	}
}
DJChat::DJChat( quint32 selfUserId, QString selfUserName, quint32 otherUserId, QString otherUserName, DJContact* contact, QWidget* parent, Qt::WFlags f )
	:QWidget( parent, f ),m_selfUserId(selfUserId),m_selfUserName(selfUserName),m_otherUserId(otherUserId),m_otherUserName(otherUserName),m_contact(contact)
{
	djDebug() << "DJChat constructor";
    djDebug()<<m_selfUserId <<m_selfUserName <<m_otherUserId <<m_otherUserName;
    
    m_gameId	= 0;
 	m_selfChatDir	= QString("chat/") + QString::number(m_selfUserId);
	
	m_chatFileName	= QString::number(m_otherUserId) + ".html";
		
	setWindowTitle(m_selfUserName + "--" + m_otherUserName);
	QStringList	searchPaths;
    searchPaths << m_selfChatDir.path() << "images" ;
    
	QVBoxLayout* layout	= new QVBoxLayout( this );
	layout->setMargin( 0 );
	layout->setSpacing( 0 ); 
	
    m_browser	= new DJTextBrowser;
    m_browser->setSearchPaths( searchPaths );
    layout->addWidget( m_browser, 2 );
    
    QHBoxLayout* buttonLayout	= new QHBoxLayout;
    layout->addLayout( buttonLayout );
     
    QSize	iconSize(32,32);
	djDebug()<<"add history";
	DJToolButton *toolbutton	= new DJToolButton;
#ifdef DJ_TOOL_BUTTON_HAS_TEXT
	toolbutton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
#endif
	toolbutton->setText(tr("history"));
	toolbutton->setToolTip( tr("history") );
	toolbutton->setIcon( QIcon(":/HallRes/image/chat/history.png") );
	toolbutton->setIconSize( iconSize );
	QMenu *menu	= new QMenu( this );
	menu->addAction( tr("view/close"), this, SLOT(toggleHistory()));
	menu->addAction( tr("clear"), this, SLOT(clearHistory()));
    toolbutton->setMenu( menu );
    toolbutton->setPopupMode( QToolButton::InstantPopup );
	buttonLayout->addWidget( toolbutton );
	
	djDebug()<<"add symbol";
	m_symbol	= new DJToolButton;
#ifdef DJ_TOOL_BUTTON_HAS_TEXT
	m_symbol->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
#endif
	m_symbol->setText(tr("symbol"));
	m_symbol->setToolTip( tr("symbol") );
	m_symbol->setIcon( QIcon(":/HallRes/image/chat/symbol.png") );
	m_symbol->setIconSize( iconSize );
	connect( m_symbol, SIGNAL(clicked()), SLOT(toggleSymbol()) );
	buttonLayout->addWidget( m_symbol );
	
	djDebug()<<"add send";
	m_send	= new DJToolButton;
#ifdef DJ_TOOL_BUTTON_HAS_TEXT
	m_send->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
#endif
	m_send->setText(tr("send"));
	m_send->setToolTip( tr("send") );
	m_send->setIcon( QIcon(":/HallRes/image/chat/send.png") );
	m_send->setIconSize( iconSize );
	connect( m_send, SIGNAL(clicked()), SLOT(sendChat()) );
	buttonLayout->addWidget( m_send );
	
    m_editor	= new DJTextEdit;
    connect(m_editor, SIGNAL(returnPressed()), SLOT(sendChat()));
    QVBoxLayout* editorLayout	= new QVBoxLayout( m_editor );
    
    m_symbolWidget	= new DJSymbol;
    connect(m_symbolWidget, SIGNAL(playerSymbolSelected(const QString&)),SLOT(insertPlayerSymbol(const QString&)));
    connect(m_symbolWidget, SIGNAL(memberSymbolSelected(const QString&)),SLOT(insertMemberSymbol(const QString&)));
    editorLayout->addWidget( m_symbolWidget );
    m_symbolWidget->hide();
    
    layout->addWidget( m_editor, 1 );
    
    m_history	= new DJTextBrowser( this ); 
    m_history->setSearchPaths( searchPaths );
    m_history->setSource( QUrl(m_chatFileName) );
    m_history->hide();
    
   	connect(m_browser, SIGNAL(memberImageDone()), m_symbolWidget, SLOT(reloadMemberWidget()) );
}
DJChat::~DJChat()
{
	djDebug() << "DJChat destructor";
}
void DJChat::closeEvent( QCloseEvent * e )
{
	djDebug()<<"DJChat::closeEvent";
	m_browser->clearBrowserContents();
	QWidget::closeEvent( e );
}
void DJChat::receiveChat( quint32 speakerId, const QString& contents )
{
	djDebug() << "DJChat::receiveChat" << contents.size();
	if ( contents.isEmpty() )
		return;
	QString text( contents );
	djDebug() << text;
	
	if ( InterpretText( text ) )
		PlayWave("wav/call.wav");
	
	QString	sentence;
	if ( speakerId == m_selfUserId ) {
		sentence	= m_selfUserName;
		sentence.prepend("<font color=#008080 size=3>");
	}else if ( speakerId == m_otherUserId ) {
		sentence	= m_otherUserName;
		sentence.prepend("<font color=#0000A0 size=3>");
	}else {
		sentence	= tr("stranger");
		sentence.prepend("<font color=#000000 size=3>");
	}	
	sentence	= sentence.append(" : ").append(text);
	sentence.append("</font>");
	
	//m_browser->append( sentence );
	//m_browser->moveCursorToEnd();
	m_browser->appendText( sentence );
	
	if ( m_contact->saveChat() )
		saveToFile( sentence.toUtf8() );
	if ( m_history->isVisible() )
		m_history->reload();
}
void DJChat::saveToFile( const QByteArray& contents )
{
	djDebug() << "appendToFile";
	QString fileName	=  m_selfChatDir.filePath( m_chatFileName );
	QFile	file( fileName );
	if ( file.exists() ) {
		djDebug() << "file exists";
		if ( !file.open( QIODevice::WriteOnly | QIODevice::Append ) )
			return;
	}else {
		djDebug() << "file not exists";
		if ( !file.open( QIODevice::WriteOnly ) ) 
			return;
		file.write( QByteArray("<head><meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\"></head><br>\n") );
	}
	file.write( contents );
	file.write( QByteArray("<br>\n") );
	file.close();
}
void DJChat::setChatEnabled( bool isEnabled )
{ 
	//admin chat (gameid!=0)is always enabled
	if ( 0 != m_gameId )
 		isEnabled 	= true;
 	m_editor->setEnabled( isEnabled );
	m_symbol->setEnabled( isEnabled );
	m_send->setEnabled( isEnabled );
}
void DJChat::showChatWidget()
{ 
	m_contact->removePendingChat( m_otherUserId );
	show();
}
void DJChat::setGameId( quint16 gameId )
{
	m_gameId	= gameId;
}
void DJChat::setOtherUserName( const QString& otherUserName )
{
	m_otherUserName 	= otherUserName;
	setWindowTitle(m_selfUserName + "--" + m_otherUserName);
}
void DJChat::sendChat()
{
	djDebug()<<"DJChat::sendChat";
	QString text	= m_editor->toPlainText();
	if ( text.isEmpty() )
		return;
	
	m_contact->sendChat( m_gameId, m_otherUserId, text );
	m_editor->clear();
}
void DJChat::toggleHistory()
{
	if ( m_history->isVisible() ) {
		layout()->removeWidget(m_history);
		adjustSize();
		m_history->hide();
	}else {
		layout()->addWidget(m_history);
		adjustSize();
		m_history->show();
		m_history->reload();
	}
}
void DJChat::clearHistory()
{
	QString fileName	=  m_selfChatDir.filePath( m_chatFileName );
	QFile	file( fileName );
	file.remove();
	m_history->reload();
}
void DJChat::toggleSymbol()
{
	if ( m_symbolWidget->isVisible() ) {
		m_symbolWidget->hide();
	}else {
		m_symbolWidget->show();
	}
	m_browser->moveCursorToEnd();
	m_editor->setFocus( Qt::OtherFocusReason );
}
void DJChat::insertPlayerSymbol( const QString& name )
{
	djDebug() << "insertSymbol" << name;
	uint	size	= 32;
	Qt::KeyboardModifiers	modifiers	= QApplication::keyboardModifiers();
	if ( Qt::ShiftModifier == modifiers )
		size	= 64;
	else if ( Qt::ControlModifier == modifiers )
		size	= 16;
	m_editor->insertPlainText( QString("{{%1,%2}}").arg(name).arg(size) );
	toggleSymbol();
}
void DJChat::insertMemberSymbol( const QString& name )
{
	djDebug() << "insertMemberSymbol" << name;
	m_editor->insertPlainText( QString("{{%1}}").arg(name) );
	toggleSymbol();
}
