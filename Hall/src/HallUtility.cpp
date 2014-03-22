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

#include "HallUtility.h"
#include "Utility.h"
#include "DJHallGlobal.h"
#include "DJGlobal.h"

static const QFile::Permissions DEFAULT_PERMISSIONS	= QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner
												| QFile::ReadUser | QFile::WriteUser | QFile::ExeUser
												| QFile::ReadGroup | QFile::WriteGroup | QFile::ExeGroup
												| QFile::ReadOther | QFile::ExeOther;

void SetDJSetting( const QStringList& keys, const QVariant& value )
{
	QSettings settings;
	QString key	= keys.join("/");
	settings.setValue( key, value );
}

void DeleteAllOldModules()
{
	djDebug() << "DeleteAllOldModule";
	QSettings settings;
	
	settings.beginGroup( KEY_HALL );
	settings.beginGroup( KEY_OLD_MODULE_NAME );
	
	QStringList keys	= settings.allKeys();
	foreach( QString key, keys ) {
		QString oldModuleName	= settings.value( key ).toString();
		djDebug() << "oldModuleName" << oldModuleName;
		QFile::remove( oldModuleName );
		settings.remove( key );
	}

	settings.endGroup();
	settings.endGroup();
}

void ReviseModuleName()
{
	djDebug() << "ReviseModuleName";
	
	QString suffix;
#if defined( Q_WS_MAC )	
	suffix = ".dylib";
#elif defined( Q_WS_WIN )
	suffix = ".dll";
#elif defined( Q_WS_X11 )
	suffix = ".so";
#endif

	QDir moduleDir("modules");
	QStringList moduleNames	= moduleDir.entryList(QDir::Files);
	foreach( QString moduleName, moduleNames ) {
		QString originalModuleName	= moduleName;
		djDebug() << moduleName;
		while ( !moduleName.isEmpty() && !moduleName.endsWith( suffix, Qt::CaseInsensitive ) ) {
			//cut the last section 
			moduleName	= moduleName.section('.',0,-2);
		}
		if ( !moduleName.isEmpty() && moduleName != originalModuleName ) {
			QFile::remove( moduleDir.filePath( moduleName ) );
			QFile::rename( moduleDir.filePath( originalModuleName ), moduleDir.filePath( moduleName ));
		}
	}
}
void SetSettingOldModuleName( quint16 gameId, const QString& oldModuleName )
{
	QStringList keys;
	keys << KEY_HALL << KEY_OLD_MODULE_NAME << QString::number(gameId);
	SetDJSetting( keys, oldModuleName );
}
void SetSettingSound( const QVariant& sound )
{
	QStringList keys;
	keys << KEY_HALL << KEY_SETUP << KEY_SOUND;
	SetDJSetting( keys, sound );
}
void SetSettingLanguage( const QVariant& locale )
{
	QStringList keys;
	keys << KEY_HALL << KEY_SETUP << KEY_LANGUAGE;
	SetDJSetting( keys, locale );
}
void SetSettingMaxImageSize( const QVariant& maxSize )
{
	QStringList keys;
	keys << KEY_HALL << KEY_SETUP << KEY_MAX_IMAGE_SIZE;
	SetDJSetting( keys, maxSize );
}
void SetSettingMinMaxSentences( const QVariant& min, const QVariant& max )
{
	QStringList keys;
	keys << KEY_HALL << KEY_SETUP << KEY_MIN_SENTENCES;
	SetDJSetting( keys, min );
	
	keys.clear();
	keys << KEY_HALL << KEY_SETUP << KEY_MAX_SENTENCES;
	SetDJSetting( keys, max );
}
QFont GetSettingApplicationFont()
{
	QSettings settings;
	
	QStringList keys;
	keys << KEY_HALL << KEY_SETUP << KEY_APPLICATION_FONT;
	QString key	= keys.join("/");
	
	if ( settings.contains( key ) ) {
		QVariant var	= settings.value( key );
		QFont	font	= var.value<QFont>();
		return font;
	}else {
		return QFont();
		//QFontDatabase	fontDatabase;
		//QStringList fontFamilies	= base.families( QFontDatabase::SimplifiedChinese );
	}
}
void SetSettingApplicationFont( const QFont& font )
{
	QStringList keys;
	keys << KEY_HALL << KEY_SETUP << KEY_APPLICATION_FONT;
	SetDJSetting( keys, font );
}

void SetSettingUseProxy( const QVariant& use )
{
	QStringList keys;
	keys << KEY_HALL << KEY_SETUP << KEY_USE_PROXY;
	SetDJSetting( keys, use );
}

void SetSettingProxyType( const QVariant& proxyType )
{
	QStringList keys;
	keys << KEY_HALL << KEY_SETUP << KEY_PROXY_TYPE;
	SetDJSetting( keys, proxyType );
}
void SetSettingProxy( int proxyType, const QVariant& data )
{
	QStringList keys;
	switch( proxyType ) {
	case 0://socks5
		keys << KEY_HALL << KEY_SETUP << KEY_SOCKS5_PROXY;
		break;
	case 1://socks4
		keys << KEY_HALL << KEY_SETUP << KEY_SOCKS4_PROXY;
		break;
	case 2://http
		keys << KEY_HALL << KEY_SETUP << KEY_HTTP_PROXY;
		break;
	default:
		return;
	}
	SetDJSetting( keys, data );
}
void SetSettingProxyAuth( const QVariant& username, const QVariant& password )
{
	QStringList keys;
	
	keys << KEY_HALL << KEY_SETUP << KEY_AUTH_USERNAME;
	SetDJSetting( keys, username );
	
	keys.clear();
	keys << KEY_HALL << KEY_SETUP << KEY_AUTH_PASSWORD;
	SetDJSetting( keys, crypt(password.toString().toUtf8()) );
}
bool GetSettingSaveChat()
{
	QSettings settings;
	
	QStringList keys;
	keys << KEY_HALL << KEY_SETUP << KEY_SAVE_CHAT;
	QString key	= keys.join("/");
	
    if ( settings.contains( key ) ) {
    	return settings.value( key ).toBool();
    }else {
    	return true;
    }
}
void SetSettingSaveChat( const QVariant& save )
{
	QStringList keys;
	keys << KEY_HALL << KEY_SETUP << KEY_SAVE_CHAT;
	SetDJSetting( keys, save );
}
bool GetSettingPopupChat()
{
	QSettings settings;
	
	QStringList keys;
	keys << KEY_HALL << KEY_SETUP << KEY_POPUP_CHAT;
	QString key	= keys.join("/");
	
	if ( settings.contains( key ) ) {
    	return settings.value( key ).toBool();
    }else {
    	return true;
    }
}
void SetSettingPopupChat( const QVariant& popup )
{
	QStringList keys;
	keys << KEY_HALL << KEY_SETUP << KEY_POPUP_CHAT;
	SetDJSetting( keys, popup );
}
bool GetSettingResume()
{
	QSettings settings;
	
	QStringList keys;
	keys << KEY_HALL << KEY_DOWNLOAD << KEY_RESUME;
	QString key	= keys.join("/");
	
    if ( settings.contains( key ) ) {
    	return settings.value( key ).toBool();
    }else {
    	return true;
    }
}
void SetSettingResume( const QVariant& resume )
{
	QStringList keys;
	keys << KEY_HALL << KEY_DOWNLOAD << KEY_RESUME;
	SetDJSetting( keys, resume );
}

uint GetSettingLastUpdate()
{
	QSettings settings;
	
	QStringList keys;
	keys << KEY_HALL << KEY_LAST_UPDATE;
	QString key	= keys.join("/");
	
    return settings.value( key ).toUInt();
}
void SetSettingLastUpdate( const QVariant& lastUpdate )
{
	QStringList keys;
	keys << KEY_HALL << KEY_LAST_UPDATE;
	SetDJSetting( keys, lastUpdate );
}
bool GetSettingMultiLogin()
{
	QSettings settings;
	
	QStringList keys;
	keys << KEY_HALL << KEY_SETUP << KEY_MULTI_LOGIN;
	QString key	= keys.join("/");
	
    if ( settings.contains( key ) ) {
    	return settings.value( key ).toBool();
    }else {
    	return false;
    }
}
void SetSettingMultiLogin( const QVariant& multiLogin )
{
	QStringList keys;
	keys << KEY_HALL << KEY_SETUP << KEY_MULTI_LOGIN;
	SetDJSetting( keys, multiLogin );
}
//Server.ini
void GetSettingMainServer( QString& ip, quint16& port )
{
	QSettings serverSettings( LOCALFILE_SERVER_INI, QSettings::IniFormat );
	serverSettings.beginGroup( SERVER_INI_SECTION_MAIN_SERVER );
	ip	= serverSettings.value(SERVER_INI_IP).toString();
	port	= serverSettings.value(SERVER_INI_PORT).toInt();
	
	if ( ip.isEmpty() || 0 == port ) {
		ip		= GAME_HOST_IP;
		port	= GAME_HOST_PORT;
		
		serverSettings.setValue( SERVER_INI_IP, ip );
		serverSettings.setValue( SERVER_INI_PORT, port );
	}
	serverSettings.endGroup();
	djDebug() << "GetSettingMainServer" << ip << port;
}
void GetSettingLastServer( QString& ip, quint16& port )
{
	QSettings serverSettings( LOCALFILE_SERVER_INI, QSettings::IniFormat );
	serverSettings.beginGroup( SERVER_INI_SECTION_LAST_SERVER );
	ip	= serverSettings.value(SERVER_INI_IP).toString();
	port	= serverSettings.value(SERVER_INI_PORT).toInt();
	
	if ( ip.isEmpty() || 0 == port ) {
		GetSettingMainServer( ip, port );
		
		serverSettings.setValue( SERVER_INI_IP, ip );
		serverSettings.setValue( SERVER_INI_PORT, port );
	}
	serverSettings.endGroup();
	djDebug() << "GetSettingLastServer" << ip << port;
}
void SetSettingLastServer( const QString& ip, quint16 port )
{
	QSettings serverSettings( LOCALFILE_SERVER_INI, QSettings::IniFormat );
	serverSettings.beginGroup( SERVER_INI_SECTION_LAST_SERVER );
		
	serverSettings.setValue( SERVER_INI_IP, ip );
	serverSettings.setValue( SERVER_INI_PORT, port );
	
	serverSettings.endGroup();
}
QString GetSettingUpdateIniUrl()
{
	QSettings serverSettings( LOCALFILE_SERVER_INI, QSettings::IniFormat );
	serverSettings.beginGroup( SERVER_INI_SECTION_UPDATE_INI );
	QString remoteIni	= serverSettings.value(SERVER_INI_URL).toString();
	QUrl	url(remoteIni);
	if ( !url.isValid() ) {
		remoteIni = QString(HTTP_WWW_HOST) + UPDATE_INI_PATH;
		serverSettings.setValue( SERVER_INI_URL, remoteIni );
	}
	serverSettings.endGroup();
	return remoteIni;
}
//end server.ini
//Update.ini
quint32 GetSettingHallVersion()
{
	return GetSettingVersion( UPDATE_INI_SECTION_HALL );
}
quint32 GetSettingBaseVersion()
{
	return GetSettingVersion( UPDATE_INI_SECTION_BASE );
}
quint32 GetSettingGameVersion( quint16 gameId )
{
	return GetSettingVersion( QString::number(gameId,16) );
}
quint32 GetSettingVersion( const QString& section )
{
	quint32 version	= 0;
	
	QSettings updateSettings( LOCALFILE_UPDATE_INI, QSettings::IniFormat );
	updateSettings.beginGroup( section );	
	
	int size = updateSettings.beginReadArray( UPDATE_INI_UPDATE );
	if ( size ) {
    	updateSettings.setArrayIndex(0);
    	QString	fileName	= updateSettings.value(UPDATE_INI_UPDATE_URL).toString().section('/',-1);
    	bool ok;
    	version	= fileName.section('.',-1).toUInt( &ok, 16 );
    	if ( !ok )
    		version	= 0;
	}
	updateSettings.endArray();
	updateSettings.endGroup();
	return version;
}
void GetSettingRemoteFileSizeName( const QString& section, int& size, QString& name )
{
	size	= 0;
	name	= QString();
	
	QSettings updateSettings( LOCALFILE_UPDATE_INI, QSettings::IniFormat );
	updateSettings.beginGroup( section );
	
	int arraySize = updateSettings.beginReadArray( UPDATE_INI_UPDATE );
	if ( arraySize ) {
		//get the first
    	updateSettings.setArrayIndex(0);
    	QString	fileName	= updateSettings.value(UPDATE_INI_UPDATE_URL).toString().section('/',-1);
    	bool ok;
    	size	= fileName.section('.',-1).toUInt( &ok );
    	if ( ok ) {
    		name	= fileName.section('.',0,-2);
    	}else
    		size	= 0;
	}
	updateSettings.endArray();
	updateSettings.endGroup();
}
void GetSettingResource( QHash<QString,QString>& resources )
{
	resources.clear();
	
	QSettings updateSettings( LOCALFILE_UPDATE_INI, QSettings::IniFormat );
	updateSettings.beginGroup( UPDATE_INI_SECTION_RESOURCE );
	QStringList	keys	= updateSettings.allKeys();
	//djDebug() << "keys size =" << keys.size();
	for ( int i = 0; i < keys.size(); i++ ) {
		QString key	= keys.at(i);
		QString value	= updateSettings.value(key).toString();
		//djDebug() << key << value;
		resources.insert( key, value );
	}
	updateSettings.endGroup();
}
//end update.ini
void ExtractHostAndPath( const QString& url, QString& host, QString& path )
{
	QString theUrl	= url;
	if ( theUrl.startsWith( "http://", Qt::CaseInsensitive ) ) {
		theUrl.remove( "http://", Qt::CaseInsensitive );
	}
	host	= theUrl.section( "/",0,0 );
	path	= theUrl.remove( host, Qt::CaseInsensitive );
}
void RelaunchHallForUpdate()
{
	QString appFilePath	= QApplication::applicationFilePath();
	djDebug() << "RelaunchHallForUpdate" << appFilePath;
			
#if defined( Q_WS_MAC )	
	QProcess::startDetached( "./UpdateHall.app/Contents/MacOS/UpdateHall", QStringList(appFilePath) );
#elif defined( Q_WS_WIN )
	QProcess::startDetached( "./UpdateHall", QStringList(appFilePath) );
#elif defined( Q_WS_X11 )
	QProcess *process	= new QProcess;
	process->start( "./UpdateHall",QStringList(appFilePath) );
#endif
	QApplication::quit();
}
int CheckVersionMatch( quint32 version1, quint32 version2 )
{		
	quint32 v1,v2;
	
	v1	= version1 & 0xFF000000;
	v2	= version2 & 0xFF000000;
		
	if ( v1 > v2 )
		return 4;
	if ( v1 < v2 ) 
		return -4;
	
	v1	= version1 & 0x00FF0000;
	v2	= version2 & 0x00FF0000;
	if ( v1 > v2 )
		return 3;
	if ( v1 < v2 ) 
		return -3;
	
	v1	= version1 & 0x0000FF00;
	v2	= version2 & 0x0000FF00;
	if ( v1 > v2 )
		return 2;
	if ( v1 < v2 ) 
		return -2; 
	
	v1	= version1 & 0x000000FF;
	v2	= version2 & 0x000000FF;
	if ( v1 > v2 )
		return 1;
	if ( v1 < v2 ) 
		return -1;
		
	return 0;
}
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
