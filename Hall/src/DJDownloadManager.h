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

#ifndef DJDOWNLOADMANAGER_H
#define DJDOWNLOADMANAGER_H

#include <QtGui>
#include <QtNetwork>

class DJDownloadManager : public QObject
{
	Q_OBJECT
public:
	enum FileType {
		None,
		Ini,
		Hall,
		Library,
		Resource,
		Base,
		HallUpdator
	};
	DJDownloadManager( QWidget* progParent, QObject * parent = 0 );
	virtual ~DJDownloadManager();
	
	bool isDownloading() const;
	void download( const QString& remoteUrl, int fileType, quint16 id, const QString& name, bool resume = true, bool background = false, const QString& localFile=QString() );
signals:
	void fileFinished( QFile* file, const QString& localFile, int fileType, quint16 id, const QString& name, int lastStatusCode );
		
private slots:
	void cancelDownload();
	void httpDataReadProgress( int bytesRead, int totalBytes );
	void httpResponseHeaderReceived( const QHttpResponseHeader& response );
	void httpReadyRead ( const QHttpResponseHeader & response );
	void httpDone( bool error );
private:
	void sendHttpRequest( QFile* file, const QString& fileName, QHttp* http, const QString& host, const QString& path );
private:
	QFile*				m_file;
	QHttp*				m_http;
	QProgressDialog*	m_progress;
	uint				m_existedFileSize;
	QDir				m_tempDir;

	int					m_fileType;
	quint16				m_id;
	QString				m_name;
	bool				m_resume;
	bool				m_background;
	QString				m_localFile;
	bool				m_isDownloading;
};

#endif
