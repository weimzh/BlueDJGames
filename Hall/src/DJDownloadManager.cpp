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

#include "DJDownloadManager.h"
#include "Utility.h"
#include "HallUtility.h"
#include "DJHallGlobal.h"
#include "DJGlobal.h"

DJDownloadManager::DJDownloadManager( QWidget* progParent, QObject * parent )
	: QObject( parent )
{
	m_existedFileSize	= 0;
	m_fileType	= 0;
	m_resume	= false;
	m_isDownloading	= false;
	
	m_file	= new QFile( this );
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
	m_progress	= new QProgressDialog(progParent,Qt::WindowMinimizeButtonHint);
	m_progress->setWindowTitle(tr("Download"));
	m_progress->setCancelButtonText(tr("Cancel"));
	
	m_tempDir.setPath( QDir::temp().filePath(TEMP_DIR) );
	m_tempDir.mkpath(".");
	
	connect(m_http, SIGNAL(dataReadProgress(int,int)), SLOT(httpDataReadProgress(int,int)) );
	connect(m_http, SIGNAL(readyRead(const QHttpResponseHeader&)),SLOT(httpReadyRead(const QHttpResponseHeader&)) );
	connect(m_http, SIGNAL(done(bool)), SLOT(httpDone(bool)) );
	connect(m_http, SIGNAL(responseHeaderReceived (const QHttpResponseHeader &)),SLOT(httpResponseHeaderReceived(const QHttpResponseHeader& )));
	connect(m_progress, SIGNAL(canceled()), SLOT(cancelDownload()) );
}
DJDownloadManager::~DJDownloadManager()
{
	djDebug() << "DJDownloadManager destructor";
}
bool DJDownloadManager::isDownloading() const
{
	djDebug() << "DJDownloadManager::isDownloading" << m_isDownloading;
	return m_isDownloading;
	/*
	djDebug() << "m_http->state()" << m_http->state();
	if ( QHttp::Unconnected == m_http->state() )
		return false;
	else
		return true;
		*/
}
void DJDownloadManager::download( const QString& remoteUrl, int fileType, quint16 id, const QString& name, bool resume, bool background, const QString& localFile )
{
	m_isDownloading	= true;
	
	QString host,path;
	ExtractHostAndPath( remoteUrl, host, path );
	djDebug() << "startDownload" << host << path;
	
	m_fileType		= fileType;
	m_id			= id;
	m_name			= name;
	m_resume		= resume;
	m_background	= background;
	m_localFile		= localFile;
	
	sendHttpRequest( m_file, path.section("/",-1), m_http, host, path );
	
	switch(fileType) {
	case Ini:
	case Hall:
	case Library:
	case Base:
	default:
		m_progress->setLabelText( tr("Downloading %1").arg(m_name) );
		break;
	}
	
	if ( background )
		m_progress->hide();
	else {
		m_progress->show();
	}
}
void DJDownloadManager::sendHttpRequest( QFile* file, const QString& fileName, QHttp* http, const QString& host, const QString& path )
{
	djDebug() << "sendHttpRequest";
	m_existedFileSize	= 0;
	file->setFileName( m_tempDir.filePath(fileName) );

#if QT_VERSION < 0x040100 && defined( Q_WS_WIN )
	QString digitalHost;
	QHostInfo info	= QHostInfo::fromName(host);
	if ( !info.addresses().isEmpty() ) {
		digitalHost = info.addresses().first().toString();
	}
	
	QHttpRequestHeader header("GET", path );
	header.setValue( "Host", host );
	if ( file->exists() && m_resume ) {
		djDebug() << "download file exists";
		header.setValue("Range", QString("bytes=%1-").arg(file->size()) );
	}
	
	http->setHost( digitalHost );
	http->request( header, QByteArray() );
#else
	if ( file->exists() && m_resume ) {
		djDebug() << "download file exists";
		QHttpRequestHeader header("GET", path );
		header.setValue( "Host", host );
		header.setValue("Range", QString("bytes=%1-").arg(file->size()) );
		http->setHost( host );
		http->request( header, QByteArray() );
	}else {
		djDebug() << "download file NOT exists";
		http->setHost( host );
		http->get( path );
	}
#endif
}
void DJDownloadManager::cancelDownload()
{
	djDebug() << "cancelDownload";
	m_http->abort();
	m_file->close();
}

void DJDownloadManager::httpResponseHeaderReceived(const QHttpResponseHeader& response )
{
	int status	= response.statusCode();
	djDebug() << "httpResponseHeaderReceived" << status;
	
	QStringList	keys	= response.keys();
	for ( int i = 0; i < keys.size(); i++ ) {
		djDebug() << keys.at(i) << response.value(keys.at(i));
	}
	switch( status ) {
	case 200://normal
		m_file->open( QIODevice::WriteOnly );
		break;
	case 206: {//resume
		m_file->open( QIODevice::WriteOnly | QIODevice::Append );
        QString range	= response.value("content-range");
        //djDebug() << "rang =" << range << range.section("-",0,0);
        range.remove("bytes",Qt::CaseInsensitive).section("-",0,0);
        m_existedFileSize	= range.remove("bytes",Qt::CaseInsensitive).section("-",0,0).toUInt();
		break;
	}
	case 302: {//redirect
		QString url	= QString( response.value("location") );
		QString host,path;
		ExtractHostAndPath( url, host, path );
		//djDebug() << "redirect" << host << path;
		sendHttpRequest( m_file, path.section("/",-1), m_http, host, path );
		break;
	}
	case 416://outof rang,finished
		break;
	default:
		m_file->setFileName("");
		break;
	}
}
void DJDownloadManager::httpDataReadProgress(int bytesRead, int totalBytes)
{
	djDebug() << "httpDataReadProgress" << bytesRead << totalBytes << m_existedFileSize;
	if ( !m_background ) {
    	m_progress->setMaximum( m_existedFileSize + totalBytes);
    	m_progress->setValue( m_existedFileSize + bytesRead );
    }
}
void DJDownloadManager::httpReadyRead ( const QHttpResponseHeader & response )
{
	int status	= response.statusCode();
	djDebug() << "httpReadyRead" << status << m_file->fileName();
	if ( !m_file->fileName().isEmpty() )
		m_file->write( m_http->readAll() );
}
void DJDownloadManager::httpDone( bool error )
{
	djDebug() << "httpDone error = " << error << m_http->state();
	djDebug() << "error string = "<<m_http->errorString();
	if ( error ) {
		m_file->setFileName("");
	}
	m_file->close();
	m_progress->cancel();
	
	if ( QHttp::Unconnected == m_http->state() ) {
		djDebug() << "emit fileFinished";
		QHttpResponseHeader response	= m_http->lastResponse();
		int lastStatusCode	= 0;
		if ( response.isValid() ) {
			lastStatusCode	= response.statusCode();
		}
		m_isDownloading	= false;
		emit fileFinished( m_file, m_localFile, m_fileType, m_id, m_name, lastStatusCode );
	}else {
		djDebug() << "close http";
		m_http->close();
	}
}

