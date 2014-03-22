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

#include "DJUpdateHandler.h"
#include "DJGlobal.h"

static const QFile::Permissions DEFAULT_PERMISSIONS	= QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner
												| QFile::ReadUser | QFile::WriteUser | QFile::ExeUser
												| QFile::ReadGroup | QFile::WriteGroup | QFile::ExeGroup
												| QFile::ReadOther | QFile::ExeOther;

bool DJMoveFile( const QString& sourceFilePath, const QString& targetFilePath )
{
	djDebug() << "DJMoveFile" << sourceFilePath << targetFilePath;
	djDebug() << "target dir = " << targetFilePath.section('/',0,-2);
	QDir targetDir	= targetFilePath.section('/',0,-2);
	targetDir.mkpath(".");
	
	QFile::Permissions permissions	= DEFAULT_PERMISSIONS;
	if ( QFile::exists(targetFilePath) ) {
		permissions	= QFile::permissions(targetFilePath);
		QFile::remove(targetFilePath);
	}
	bool res	= QFile::copy( sourceFilePath, targetFilePath );
	if ( res ) {
		QFile::remove( sourceFilePath );
		QFile::setPermissions( targetFilePath, permissions );
	}
	return res;
}
DJUpdateHandler::DJUpdateHandler( const QStringList& arguments, QObject* parent )
	:QObject(parent)
{
	if ( arguments.size() ) {
		m_hallExecutable	= arguments.at(0);
		m_hallExecutableDir	= QDir( m_hallExecutable.section('/',0,-2) );
		djDebug() << "DJUpdateHandler::constructor" << m_hallExecutable << m_hallExecutableDir.path();
		QTimer::singleShot( 2000, this, SLOT(update()) );
	}else {
		QCoreApplication::quit();
	}
}

DJUpdateHandler::~DJUpdateHandler()
{
}
void DJUpdateHandler::update()
{
	djDebug() <<"DJUpdateHandler::update";
	
	QDir updateHallDir("update/hall");
	QStringList updateHallFiles	= updateHallDir.entryList(QDir::Files);
	QString fileName;
	for( int i = 0; i < updateHallFiles.size(); i++ ) {
		fileName	= updateHallFiles.at(i);
		djDebug() << fileName;
		QString sourceFilePath	= updateHallDir.filePath( fileName );
		QString targetFilePath	= m_hallExecutableDir.filePath( fileName );
		if ( DJMoveFile( sourceFilePath, targetFilePath ) ) {
			;
		}else {
			djDebug() << "move file error" << sourceFilePath << targetFilePath;
		}
	}
	QProcess::startDetached( "\"" + m_hallExecutable + "\"" );
	QCoreApplication::quit();
}
