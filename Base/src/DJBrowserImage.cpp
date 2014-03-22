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

#include "DJBrowserImage.h"
#include "DJTextBrowser.h"
#include "DJRemoteToLocalFile.h"

static const int DEFAULT_ANIMATE_PERIOD			= 100;// 0.1 second

//each url corresponds only one DJBrowserImage instance
DJBrowserImage::DJBrowserImage( const QUrl& url, int pos, int len, DJTextBrowser * browser )
	:DJAnimationImage(browser),m_url(url),m_browser(browser)
{
	djDebug() << "DJBrowserImage constructor"<<url;
	
	addPosLen(pos,len);	
	m_r2l	= 0;
	
	QString host	= url.host();
	
	if ( host.isEmpty() ) {
		djDebug()<<"host is empty";
    	setImagePath( url.toString() );
    	displayImage();
    }else {
    	djDebug()<<"host is NOT empty";
    	QString localFilePath;
    	if ( url.host().toLower() == QUrl(HTTP_WWW_HOST).host().toLower() ) {
    		QString path	= url.path();
    		if ( path.startsWith('/') ) {
    			path.remove(0,1);
    		}
    		localFilePath	= path;
    		if ( !QFile::exists(localFilePath) ) {
    			m_r2l	= new DJRemoteToLocalFile( url, localFilePath, true, this );
				connect( m_r2l, SIGNAL(localFileDone(const QString&)), SLOT(handleLocalFileDone(const QString&)) );
    		}else {
    			setImagePath( localFilePath );
    			displayImage();
    		}
    	}else {
    		m_r2l	= new DJRemoteToLocalFile( url, localFilePath, true, this );
			connect( m_r2l, SIGNAL(localFileDone(const QString&)), SLOT(handleLocalFileDone(const QString&)) );
		}
    }
    connect( this, SIGNAL(imageChanged()), SLOT(handleImageChanged()) );
}
DJBrowserImage::~DJBrowserImage()
{
	djDebug() << "DJBrowserImage destructor";
}
void DJBrowserImage::addPosLen( int pos, int len )
{
	QPair<int,int> poslen(pos,len);
	m_poslens << poslen;
}
void DJBrowserImage::increasePosLens( int startPos, int len )
{
	QList< QPair<int,int> >::iterator it	= m_poslens.begin();
	while ( it != m_poslens.end() ) {
		if ( (*it).first >= startPos )
			(*it).first	+= len;
		++it;
	}
}
int DJBrowserImage::decreasePos( int pos )
{
	QList< QPair<int,int> >::iterator it	= m_poslens.begin();
	while ( it != m_poslens.end() ) {
		(*it).first	-= pos;
		if ( (*it).first < 0 )
			it	= m_poslens.erase(it);
		else
			++it;
	}
	return m_poslens.size();
}
void DJBrowserImage::handleLocalFileDone( const QString& localFile )
{
	djDebug()<<"DJBrowserImage::handleLocalFileDone";
	//for self web, local file is images/xxx/yyy.png
	//for other web, local file is temp file
	setImagePath( localFile );
	m_browser->emitMemberImageDone();
	
	displayImage();
	
	m_browser->moveCursorToEnd();
}
void DJBrowserImage::displayImage()
{
	djDebug()<<"DJBrowserImage::displayImage";
	if ( !isValid() )
		return;
	djDebug()<<"is valid image";
	handleImageChanged();
	
	if ( isAnimation() ) {
		djDebug()<<"is animation";
		start();
	}
}
void DJBrowserImage::handleImageChanged()
{	
	m_browser->document()->addResource(QTextDocument::ImageResource, m_url, image() );
	QPair<int,int> poslen;
	foreach( poslen, m_poslens ) {
		m_browser->document()->markContentsDirty( poslen.first, poslen.second );
	}
}
