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

#include "DJTextBrowser.h"
#include "DJBrowserImage.h"
#include "LaunchUrl.h"
#include "DJSchemeEvent.h"
#include "Utility.h"

static const int UPDATE_POSITION_PERIOD	= 1000;// 1 second
static const int STAY_DEEP_SECONDS		= 60;//60 second
static const int MAX_ANIMATED_IMAGE		= 8;

DJTextBrowser::DJTextBrowser( QWidget * parent )
	: QTextBrowser( parent )
{
	djDebug() << "DJTextBrowser constructor";
	
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	
	m_timer	= new QTimer(this);
	connect(m_timer,SIGNAL(timeout()),SLOT(updateSentences()));
	m_timer->start(UPDATE_POSITION_PERIOD);		
}
DJTextBrowser::~DJTextBrowser()
{
	djDebug() << "DJTextBrowser destructor";
}
void DJTextBrowser::setSource ( const QUrl & url )
{
	djDebug() << "DJTextBrowser::setSource" << url;
	QString scheme	= url.scheme();
	if ( scheme.isEmpty() )
		QTextBrowser::setSource(url);
	else if ( scheme.toLower() == QString("http").toLower() )
		LaunchURL( url.toString() );
	else if ( scheme.toLower() == QString(DJSCHEME).toLower() ) {
		djDebug() << "DJScheme";
		djDebug()<<"name="<<url.userName()<<"password="<<url.password()
				<<"host="<<url.host()<<"port="<<url.port();
		uint	eventId	= ExtractDJShemeEventID(url);
		switch( eventId ) {
		case DJSCHEME_EVENT_ACCEPT_PRIVATE_VIEW: {
			quint32 userId	= ExtractDJShemeParameter(url).toUInt();
			emit acceptPrivateView( userId );
			break;
		}
		case DJSCHEME_EVENT_ACCEPT_INVITATION: {
			quint8 tableId	= ExtractDJShemeParameter(url).toUInt();
			emit acceptInvitation( tableId, 0 );
			break;
		}
		case DJSCHEME_EVENT_CHECK_INVITATION: {
			quint8 tableId	= ExtractDJShemeParameter(url).toUInt();
			emit checkInvitation( tableId );
			break;
		}
		case DJSCHEME_EVENT_ACCEPT_DRAW_CHESS: {
			emit acceptDrawChess();
			break;
		}
		default:
			break;
		}
	}	
}
void DJTextBrowser::addBrowserImage( DJBrowserImage *image )
{
	djDebug()<<"DJTextBrowser::addBrowserImage"<<m_images.size();
	if ( m_images.size() >= MAX_ANIMATED_IMAGE ) {
		djDebug()<<"m_images size >= MAX_ANIMATED_IMAGE";
		DJBrowserImage *firstImage	= m_images.takeFirst();
		delete firstImage;
	}
	m_images.append( image );
}
void DJTextBrowser::emitMemberImageDone()
{
	emit memberImageDone();
}
QList<QUrl> DJTextBrowser::extractImageUrl( const QString& text ) const
{
	djDebug()<<"DJTextBrowser::extractImageUrl"<<text;
	QList<QUrl> urls;
	int from	= 0;
	while( true ) {
		int indexOfStartImg = text.indexOf( "<img", from, Qt::CaseInsensitive );
		djDebug()<<"indexOfStartImg"<<indexOfStartImg;
		if ( -1 == indexOfStartImg )
			break;
		int indexOfEqual	= text.indexOf( '=', indexOfStartImg, Qt::CaseInsensitive );
		int indexOfEndImg	= text.indexOf( '>', indexOfStartImg, Qt::CaseInsensitive );
		if ( -1 == indexOfEqual || -1 == indexOfEndImg )
			break;
		QString imgUrlString	= text.mid( indexOfEqual+1, indexOfEndImg-indexOfEqual-1);
		djDebug()<<"imgUrlString"<<imgUrlString;
		
		int indexOfSpace	= imgUrlString.indexOf( ' ', 0, Qt::CaseInsensitive );
		if ( -1 != indexOfSpace ) {
			//there are extra width and height, remove them
			imgUrlString	= imgUrlString.mid( 0, indexOfSpace );
		}
		urls << QUrl(imgUrlString);
		from	= indexOfEndImg;
	}
	return urls;
}
DJBrowserImage* DJTextBrowser::browserImage( const QUrl& url ) const
{
	foreach( DJBrowserImage *image, m_images ) {
		if ( image->url().toString().toLower() == url.toString().toLower() )
			return image;
	}
	return 0;
}
void DJTextBrowser::insertText( const QString & text )
{
	djDebug()<<"DJTextBrowser::insertText----------------------BEGIN";
	//description();
	int position	= positionForInsert();
	QTextCursor	cursor	= textCursor();
	cursor.setPosition( position );
	setTextCursor( cursor );
	
	QTextBrowser::insertHtml( text );
	QTextBrowser::insertPlainText("\n");
	
	int len	= textCursor().position() - position;
	m_insertedSentences << len;
	
	increaseImagePosition( position, len );
	
	QList<QUrl> urls	= extractImageUrl( text );
	foreach(QUrl url, urls) {
		djDebug()<<"url="<<url;
		DJBrowserImage	*image	= browserImage( url );
		if ( 0 == image ) {
			djDebug()<<"no browser image found";
			image	= new DJBrowserImage( url, position, len, this );
			addBrowserImage( image );
		}else {
			djDebug()<<"found image";
			image->addPosLen( position, len );
		}
	}
		
	moveCursorToEnd();
	
	checkOverflow();
	
	//description();
	djDebug()<<"DJTextBrowser::insertText----------------------END";
}
void DJTextBrowser::appendText( const QString & text )
{	
	djDebug()<<"DJTextBrowser::appendText----------------------BEGIN";
	//description();
	moveCursorToEnd();
	int pos1	= textCursor().position();
	QTextBrowser::insertHtml( text );
	QTextBrowser::insertPlainText("\n");
	int pos2	= textCursor().position();
	
	int len	= pos2 - pos1;
	djDebug()<<"pos1="<<pos1<<"pos2="<<pos2<<"len="<<len;
	
	QList<QUrl> urls	= extractImageUrl( text );
	foreach(QUrl url, urls) {
		DJBrowserImage	*image	= browserImage( url );
		if ( 0 == image ) {
			djDebug()<<"no browser image found";
			image	= new DJBrowserImage( url, pos1, len, this );
			addBrowserImage( image );
		}else {
			djDebug()<<"found image";
			image->addPosLen( pos1, len );
		}
	}
	
	QDateTime	triggerTime	= QDateTime::currentDateTime().addSecs( STAY_DEEP_SECONDS );
	AppendedSentence	appendedSentence;
	appendedSentence.len	= len;
	appendedSentence.time	= triggerTime.toTime_t();
	
	m_appendedSentences << appendedSentence ;
	
	moveCursorToEnd();
	
	checkOverflow();
	
	//description();	
	djDebug()<<"DJTextBrowser::appendText----------------------END";
}
void DJTextBrowser::moveCursorToEnd()
{
	moveCursor( QTextCursor::End );
}
void DJTextBrowser::clearBrowserContents()
{
	clear();
	qDeleteAll( m_images );
	m_images.clear();
	m_insertedSentences.clear();
	m_appendedSentences.clear();
}
void DJTextBrowser::updateSentences()
{	
	if ( !m_appendedSentences.isEmpty() ) {
		uint	currentTime	= QDateTime::currentDateTime().toTime_t();
		uint	triggerTime	= m_appendedSentences.first().time;
		if ( currentTime >= triggerTime ) {
			djDebug()<<"triggertime"<<currentTime<<triggerTime;
			m_insertedSentences << m_appendedSentences.takeFirst().len;
		}
	}
}
int DJTextBrowser::positionForInsert()
{
	moveCursorToEnd();
	int endPos	= textCursor().position();
	if ( m_appendedSentences.isEmpty() ) {
		return endPos;
	}else {
		int len	= 0;
		QList<AppendedSentence>::iterator it	= m_appendedSentences.begin();
    	while ( it != m_appendedSentences.end() ) {
    		len	+= (*it).len;
    		++it;	
    	}
    	return endPos - len;
	}
}
void DJTextBrowser::checkOverflow()
{
	djDebug()<<"DJTextBrowser::checkOverflow";
	int position	= overflowPosition();
	djDebug()<<"overflowPosition="<<position;
	if ( 0 != position ) {
		QTextCursor	cursor	= textCursor();
		cursor.setPosition( 0 );
		cursor.setPosition( position, QTextCursor::KeepAnchor );
		cursor.removeSelectedText();
		
		decreaseImagePosition( position );
	}
}
int DJTextBrowser::overflowPosition()
{	
	djDebug()<<"DJTextBrowser::overflowPosition";
	int position	= 0;
	int insertedSentences	= m_insertedSentences.size();
	int appendedSentences	= m_appendedSentences.size();
	int minSentences,maxSentences;
	GetSettingMinMaxSentences( minSentences, maxSentences );
	int removeSentences	= maxSentences - minSentences + 1;
	
	djDebug()<<"insertedSentences"<<insertedSentences
			<<"appendedSentences"<<appendedSentences
			<<"removeSentences"<<removeSentences;
	
	if ( insertedSentences > maxSentences ) {
		for ( int i = 0; i < removeSentences; ++i ) {
			position	+= m_insertedSentences.takeFirst();
		}
		djDebug()<<"insertedSentences > MAX_RESERVED_SENTENCES"<<position;
		return position;
	}
	if ( insertedSentences + appendedSentences > maxSentences ) {
		while( !m_insertedSentences.isEmpty() && 0 != removeSentences ) {
			position	+= m_insertedSentences.takeFirst();
			--removeSentences;
		}
		
		for ( int i = 0; i < removeSentences; ++i ) {
			position	+= m_appendedSentences.takeFirst().len;
		}
		djDebug()<<"insertedSentences + appendedSentences > MAX_RESERVED_SENTENCES"<<position;
		return position;
	}
	return 0;
}
void DJTextBrowser::increaseImagePosition( int startPos, int len )
{
	djDebug()<<"DJTextBrowser::reviseImagePosition"<<"startPos"<<startPos<<"len"<<len;
	QList<DJBrowserImage*>::iterator it	= m_images.begin();
	while ( it != m_images.end() ) {
		DJBrowserImage *image	= (*it);
		image->increasePosLens( startPos, len );
		++it;
	}
}
void DJTextBrowser::decreaseImagePosition( int pos )
{
	QList<DJBrowserImage*>::iterator it	= m_images.begin();
	while ( it != m_images.end() ) {
		DJBrowserImage *image	= (*it);
		int numberOfPos	= image->decreasePos( pos );
		if ( 0 == numberOfPos ) {
			//no valid pos in this image, so remove it and delete it;
			it	= m_images.erase(it);
			delete image;
		}else {
			++it;
		}
	}
}
/*
void DJTextBrowser::description()
{
	djDebug()<<"DJTextBrowser::description----begin";
	djDebug()<<"total plain text length="<<toPlainText().length();
	
	foreach(int len, m_insertedSentences) {
		djDebug()<<"InsertedSentence:"
				<<"len"<<len;
	}
	foreach(AppendedSentence as, m_appendedSentences) {
		djDebug()<<"AppendedSentence:"
				<<"len"<<as.len
				<<"time"<<as.time;
	}
	foreach( DJBrowserImage *image, m_images ) {
		djDebug()<< image->url().toString();
		QList< QPair<int,int> > poslens	= image->poslens();
		QPair<int,int> poslen;
		foreach( poslen, poslens ) {
			djDebug()<<"pos"<<poslen.first<<"len"<<poslen.second;
		}
	}
	djDebug()<<"DJTextBrowser::description----end";
}
*/
