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

#include "DJAdminBrowser.h"
#include "DJGameController.h"
#include "DJSchemeEvent.h"
#include "Utility.h"
#include "DJClientRequest.h"

DJAdminBrowser::DJAdminBrowser( DJGameController* gameController, quint32 groupId, QWidget * parent )
	: QTextBrowser( parent ),m_gameController(gameController),m_groupId(groupId)
{
	djDebug() << "DJAdminBrowser constructor";
}
DJAdminBrowser::~DJAdminBrowser()
{
	djDebug() << "DJAdminBrowser destructor";
}
QString DJAdminBrowser::extractCallAdminInfo( const ChatContent* pChatContent, CallAdminInfo& info )
{
	djDebug()<<"DJAdminBrowser::extractCallAdminInfo";
	QString sentence	= QString::fromUtf8( pChatContent->szSentence );
	djDebug()<<"sentence="<<sentence;
	int begin	= sentence.indexOf("{{");
	int	end	= sentence.indexOf("}}");
	if ( -1 != begin && -1 != end ) {
		int	infoLen	= end - begin - 2;
		QString string	= sentence.mid( begin + 2, infoLen );
		djDebug()<<"contents inside {{}}"<<string;
		
		QTextStream	stream( &string, QIODevice::ReadOnly );
		
		stream >> info.callerId;
		stream >> info.roomId;
		stream >> info.tableId;
		stream >> info.reason;
		stream >> info.eventType;
		
		sentence.remove( begin, end - begin + 2 );
	}
	return sentence;
}
void DJAdminBrowser::receiveChat( const ChatContent* pChatContent )
{
	djDebug()<<"DJAdminBrowser::receiveChat";
	CallAdminInfo	info;
	memset( &info, 0, sizeof(CallAdminInfo) );
	QString sentence	= extractCallAdminInfo( pChatContent, info );
	djDebug()<<info.callerId<<info.roomId<<info.tableId
			<<info.reason<<info.eventType<<"extracted sentence="<<sentence;
			
	if ( ADMIN_CHAT_EVENT_TYPE_CALL_ADMIN == info.eventType ) {
		djDebug()<<"call admin";
		QString	text	= tr("user %1 (at room %2 table %3) is calling admin for reason %4")
								.arg( info.callerId )
								.arg( info.roomId )
								.arg( info.tableId )
								.arg( reason(info.reason) );
		//NOTE : must add color before append, otherwise anchor may be corrupted 
		text.prepend("<font color=#808000 size=3>");
		text.append("</font>");
		append( text );
		
		if ( !sentence.isEmpty() ) {
			text	= tr("he/she said : ")+ sentence;
			text.prepend("<font color=#008000 size=3>");
			text.append("</font>");
			append( text );
		}
		
		text	= DJSCHEME_AHREF
					.arg( DJSCHEME )
					.arg( DJSCHEME_EVENT_CALL_ADMIN )
					.arg( info.callerId )
					.arg( DJSCHEME_HOST )
					.arg( tr("pending call") );
		//text.prepend("<font color=#808000 size=3>");
		//text.append("</font>");
		append( text );
		
		//text	= toHtml();
		//djDebug()<<"total text"<<text;
	}else if ( ADMIN_CHAT_EVENT_TYPE_ADMIN_RESPOND == info.eventType ) {
		djDebug()<<"admin respond";
		quint32 speakerId	= letoh4( &pChatContent->userid_speaker );
		handlePendingCall( speakerId, info.callerId );
		if ( speakerId == m_gameController->selfUserId() )
			m_gameController->chatWithUser( info.callerId, true );
	}else if ( ADMIN_CHAT_EVENT_TYPE_ADMIN_CHAT == info.eventType ) {
		djDebug()<<"admin chat";
		quint32 speakerId	= letoh4( &pChatContent->userid_speaker );
		QString	text	= tr("admin %1 : ").arg(speakerId);
		text.append( sentence );
		text.prepend("<font color=#008000 size=3>");
		text.append("</font>");
		append( text );
	}
	if ( toPlainText().size() > 1000 ) {
		QTextCursor	cursor	= textCursor();
		cursor.setPosition( 0 );
		cursor.setPosition( 500, QTextCursor::KeepAnchor );
		cursor.removeSelectedText();
	}
}
void DJAdminBrowser::handlePendingCall( quint32 adminId, quint32 callerId )
{
	QString	adminProcessed	= tr("admin %1 has handled").arg(adminId);
	adminProcessed.prepend("<font color=#FF0000 size=3>");
	adminProcessed.append("</font>");
	
	QString text	= toHtml();
	//djDebug()<<"total text"<<text;
	QString callerAnchor	= DJSCHEME_FIND_AHREF
							.arg( DJSCHEME )
							.arg( DJSCHEME_EVENT_CALL_ADMIN )
							.arg( callerId )
							.arg( DJSCHEME_HOST );
	
	int indexOfBegin;
	while ( -1 != (indexOfBegin = text.indexOf(callerAnchor) ) ) {
		djDebug()<<"indexOfBegin"<<indexOfBegin;
		int indexOfEnd	= text.indexOf( "</a>", indexOfBegin );
		if ( -1 != indexOfEnd ) {
			djDebug()<<"indexOfEnd"<<indexOfEnd;
			int len	= indexOfEnd - indexOfBegin + 4;
			text.replace( indexOfBegin, len, adminProcessed );
			//djDebug()<<"text after remove"<<text;
		}else {
			text.clear();
		}
	}
	setHtml(text);
}
void DJAdminBrowser::setSource ( const QUrl & url )
{
	djDebug() << "DJAdminBrowser::setSource" << url;
	QString scheme	= url.scheme();
	if ( scheme.isEmpty() )
		QTextBrowser::setSource(url);
	else if ( scheme == DJSCHEME ) {
		djDebug() << "DJScheme";
		djDebug()<<"name="<<url.userName()<<"password="<<url.password()
				<<"host="<<url.host()<<"port="<<url.port();
		uint	eventId	= ExtractDJShemeEventID(url);
		if ( eventId == DJSCHEME_EVENT_CALL_ADMIN ) {
			quint32 callerId	= ExtractDJShemeParameter(url).toUInt();
			CallAdminInfo	info;
			memset( &info, 0, sizeof(CallAdminInfo) );
        	info.callerId	= callerId;
        	info.roomId		= 0;//ignore
        	info.tableId	= 0;//ignore
        	info.reason		= 0;//ignore
        	info.eventType	= ADMIN_CHAT_EVENT_TYPE_ADMIN_RESPOND;
        	
        	QString		sentence;
        	QTextStream	stream( &sentence, QIODevice::ReadWrite );
        	stream <<info.callerId<<" "<<info.roomId<<" "<<info.tableId
        			<<" "<<info.reason<<" "<<info.eventType;
        	sentence.prepend("{{");
        	sentence.append("}}");
        	
        	QByteArray	utf8	= sentence.toUtf8();
    	
        	QByteArray	chatContent( sizeof(ChatContent) + utf8.size() + 1, 0 );
            ChatContent	*pChatContent	= reinterpret_cast<ChatContent *>(chatContent.data());
            setle4( &pChatContent->userid_speaker, m_gameController->selfUserId() );
            setle4( &pChatContent->userid_listener, m_groupId );
            pChatContent->chGameClass = 0xFF;
            pChatContent->chGame = 0xFF;
            qstrcpy( pChatContent->szSentence, utf8.data() );
            
        	m_gameController->requestChat( pChatContent, chatContent.size() );
		}
	}
}
QString	DJAdminBrowser::reason( uint reason ) const
{
	static const char *reasons[] = {
       	QT_TR_NOOP("no reason"),
        QT_TR_NOOP("someone is cheating"),
        QT_TR_NOOP("someone is abusing"),
        QT_TR_NOOP("there is a question"),
        QT_TR_NOOP("there is a suggestion"),
        QT_TR_NOOP("curious"),
        QT_TR_NOOP("other reason")
	};
	if ( reason < sizeof(reasons)/sizeof(reasons[0]) )
		return tr(reasons[reason]);
	else
		return tr(reasons[0]);
}
