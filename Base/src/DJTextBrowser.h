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

#ifndef DJTEXTBROWSER_H
#define DJTEXTBROWSER_H

#include <QtGui>
#include "DJBaseGlobal.h"

struct AppendedSentence
{
	int		len;
	uint	time;
};

class DJBrowserImage;

class DJ_BASE_EXPORT DJTextBrowser : public QTextBrowser
{
	Q_OBJECT
public:
	DJTextBrowser( QWidget * parent = 0 );
	virtual ~DJTextBrowser();
	
	//void description();
	virtual void setSource ( const QUrl & url );
	
	void emitMemberImageDone();
	QList<QUrl> extractImageUrl( const QString& text ) const;
	DJBrowserImage * browserImage( const QUrl& url ) const;
	
	void addBrowserImage( DJBrowserImage* image );
	void insertText( const QString & text );
	void appendText( const QString & text );
	void moveCursorToEnd();
public slots:
	void clearBrowserContents();
signals:
	void memberImageDone();
	void acceptPrivateView( quint32 userId );
	void acceptInvitation( quint8, quint8 );
	void checkInvitation( quint8 );
	void acceptDrawChess();
private slots:
	void updateSentences();
private:
	int positionForInsert();
	void checkOverflow();
	int overflowPosition();
	void increaseImagePosition( int startPos, int len );
	void decreaseImagePosition( int pos );
	
	QList<DJBrowserImage*>			m_images;
	QTimer*							m_timer;
	
	QList<int>						m_insertedSentences;
	QList<AppendedSentence>			m_appendedSentences;
};

#endif
