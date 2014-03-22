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

#include "DJRemoteToLocalFile.h"
#include "Utility.h"

DJRemoteToLocalFile::DJRemoteToLocalFile( const QUrl& remoteUrl, const QString& localFilePath, bool limitFileSize, QObject * parent )
	: QObject(parent),m_localFilePath(localFilePath),m_limitFileSize(limitFileSize)
{
	djDebug() << "DJRemoteToLocalFile constructor" << remoteUrl << localFilePath;
	
	QString urlhost	= remoteUrl.host();
	QString urlpath	= remoteUrl.path();
	djDebug()<<"url"<<urlhost<<urlpath;
	
#if QT_VERSION < 0x040100 && defined( Q_WS_WIN )
	QHostInfo info	= QHostInfo::fromName( urlhost );
	if ( info.addresses().isEmpty() ) {
		m_tempFile	= 0;
		m_http	= 0;
		return;
	}
	
	QString host	= info.addresses().first().toString();
	m_tempFile	= new QTemporaryFile( this );
	m_http	= new QHttp( this );
	
	if ( GetSettingUseProxy() ) {
		int proxyType	= GetSettingProxyType();
		if ( -1 != proxyType ) {
			QList<QVariant> data	= GetSettingProxy( proxyType ).toList();
			QString ip		= data.at(0).toString();
			quint16 port	= data.at(1).toString().toUInt();
			QString username;
			QString password;
			GetSettingProxyAuth( username, password );
			m_http->setProxy( ip, port, username, password );
		}
	}
	QHttpRequestHeader header("GET", urlpath );
    header.setValue("Host", urlhost );

	m_http->setHost( host );
	m_http->request( header, QByteArray(), m_tempFile );
	
	connect(m_http, SIGNAL(responseHeaderReceived (const QHttpResponseHeader &)),SLOT(httpResponseHeaderReceived(const QHttpResponseHeader& )));
	connect(m_http, SIGNAL(done(bool)), SLOT(httpDone(bool)) );
#else
	m_tempFile	= new QTemporaryFile( this );
	m_http	= new QHttp( this );
	
	if ( GetSettingUseProxy() ) {
		int proxyType	= GetSettingProxyType();
		if ( -1 != proxyType ) {
			QList<QVariant> data	= GetSettingProxy( proxyType ).toList();
			QString ip		= data.at(0).toString();
			quint16 port	= data.at(1).toString().toUInt();
			QString username;
			QString password;
			GetSettingProxyAuth( username, password );
			m_http->setProxy( ip, port, username, password );
		}
	}
	m_http->setHost( urlhost );
	m_http->get( urlpath, m_tempFile );
	
	connect(m_http, SIGNAL(responseHeaderReceived (const QHttpResponseHeader &)),SLOT(httpResponseHeaderReceived(const QHttpResponseHeader& )));
	connect(m_http, SIGNAL(done(bool)), SLOT(httpDone(bool)) );
#endif 
}
DJRemoteToLocalFile::~DJRemoteToLocalFile()
{
	djDebug() << "DJRemoteToLocalFile destructor";
}
void DJRemoteToLocalFile::httpResponseHeaderReceived(const QHttpResponseHeader& response )
{
	djDebug() << "DJRemoteToLocalFile::httpResponseHeaderReceived contentlength =" << response.contentLength() << "statuscode=" << response.statusCode() << response.isValid();
	//QStringList	keys	= response.keys();
	//for ( int i = 0; i < keys.size(); i++ ) {
		//djDebug() << keys.at(i) << response.value(keys.at(i));
	//}
	if ( m_limitFileSize ) {
		uint maxSize	= GetSettingMaxImageSize();
		if ( maxSize != 0 && response.contentLength() > (maxSize << 10) ) {
			djDebug()<<"file size is gteater than max size";
			m_http->abort();
		}
	}
}
void DJRemoteToLocalFile::httpDone( bool error )
{
	djDebug() << "DJRemoteToLocalFile::httpDone" << error << m_tempFile->fileName() << m_http->state();
	if ( QHttp::Unconnected == m_http->state() ) {
		QHttpResponseHeader response	= m_http->lastResponse();
		djDebug() << "lastResponse" << response.contentLength() << response.statusCode() 
				<< response.isValid() << m_tempFile->size();
		
		delete m_http;
		m_http	= 0;
		
		djDebug() << "file size=" << m_tempFile->size();
		
		if ( response.isValid() && 0 != response.contentLength() 
			&& 200 == response.statusCode()
			&& 0 != m_tempFile->size() ) {
			djDebug() << "remote to local file done";
			if ( m_localFilePath.isEmpty() ) {
				djDebug() << "emit temp file" << m_tempFile->fileName();
				emit localFileDone( m_tempFile->fileName() );
    		}else {
    			djDebug() << "emit local file" << m_localFilePath;
    			djDebug() << "temp file name" << m_tempFile->fileName();
				//ceate dest dir if it doesn't exist
    			QDir	memberDir( m_localFilePath.section("/",0,-2) );
    			memberDir.mkpath(".");
    			QFile::copy( m_tempFile->fileName(), m_localFilePath );
    			emit localFileDone( m_localFilePath );
    		}
		}
	}else {
		m_http->close();
	}
}
