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

#ifndef HALLUTILITY_H
#define HALLUTILITY_H

#include <QtGui>
#include "DJBaseGlobal.h"

void SetDJSetting( const QStringList& keys, const QVariant& value );

/*
bool GetSettingSaveUsername();
void SetSettingSaveUsername( const QVariant& save );
*/
void DeleteAllOldModules();
void ReviseModuleName();
//QStringList GetSettingAllOldModuleNames();
//QString GetSettingModuleName( quint16 gameId );
void SetSettingOldModuleName( quint16 gameId, const QString& oldModuleName );

void SetSettingSound( const QVariant& sound );
void SetSettingLanguage( const QVariant& locale );
void SetSettingMaxImageSize( const QVariant& maxSize );
void SetSettingMinMaxSentences( const QVariant& min, const QVariant& max );

QFont GetSettingApplicationFont();
void SetSettingApplicationFont( const QFont& font );

void SetSettingUseProxy( const QVariant& use );
void SetSettingProxyType( const QVariant& proxyType );
void SetSettingProxy( int proxyType, const QVariant& data );
void SetSettingProxyAuth( const QVariant& username, const QVariant& password );

bool GetSettingSaveChat();
void SetSettingSaveChat( const QVariant& save );

bool GetSettingPopupChat();
void SetSettingPopupChat( const QVariant& popup );

bool GetSettingResume();
void SetSettingResume( const QVariant& resume );

uint GetSettingLastUpdate();
void SetSettingLastUpdate( const QVariant& lastUpdate );

bool GetSettingMultiLogin();
void SetSettingMultiLogin( const QVariant& multiLogin );

//Server.ini
void GetSettingMainServer( QString& ip, quint16& port );
void GetSettingLastServer( QString& ip, quint16& port );
void SetSettingLastServer( const QString& ip, quint16 port );
QString GetSettingUpdateIniUrl();
//Update.ini
quint32 GetSettingHallVersion();
quint32 GetSettingBaseVersion();
quint32 GetSettingGameVersion( quint16 gameId );
quint32 GetSettingVersion( const QString& section );

void GetSettingRemoteFileSizeName( const QString& section, int& size, QString& name );
void GetSettingResource( QHash<QString,QString>& resources );

void ExtractHostAndPath( const QString& url, QString& host, QString& path );
void RelaunchHallForUpdate();
int CheckVersionMatch( quint32 hallVersion, quint32 gameVersion );

inline quint8 FirstVersion( quint32 version )
{
	return (version & 0xFF000000) >> 24;
}
inline quint8 SecondVersion( quint32 version )
{
	return (version & 0x00FF0000) >> 16;
}

bool DJMoveFile( const QString& sourceFilePath, const QString& targetFilePath );

const	char*	const	UPDATE_INI_SECTION_HALL			= "Hall";
const	char*	const	UPDATE_INI_SECTION_BASE			= "Base";
const	char*	const	UPDATE_INI_SECTION_HALL_UPDATOR	= "HallUpdator";
const	char*	const	UPDATE_INI_SECTION_RESOURCE		= "Resource";

const	char*	const	KEY_DOWNLOAD		= "Download";
const	char*	const	KEY_LAST_UPDATE		= "LastUpdate";
const	char*	const	KEY_OLD_MODULE_NAME	= "OldModuleName";

const	char*	const	KEY_MULTI_LOGIN		= "MultiLogin";

const	char*	const	KEY_SAVE_CHAT		= "SaveChat";
const	char*	const	KEY_POPUP_CHAT		= "PopupChat";
const	char*	const	KEY_RESUME			= "Resume";

const	char*	const	LOCALFILE_SERVER_INI	= "Server.ini";
const	char*	const	SERVER_INI_SECTION_MAIN_SERVER	= "MainServer";
const	char*	const	SERVER_INI_SECTION_LAST_SERVER	= "LastServer";
const	char*	const	SERVER_INI_SECTION_UPDATE_INI	= "UpdateIni";
const	char*	const	SERVER_INI_IP				= "Ip";
const	char*	const	SERVER_INI_PORT				= "Port";
const	char*	const	SERVER_INI_URL				= "Url";
	
const	char*	const	LOCALFILE_UPDATE_INI		= "update/Update.ini";
const	char*	const	UPDATE_INI_UPDATE			= "update";
const 	char*	const	UPDATE_INI_UPDATE_URL		= "url";
const 	char*	const	UPDATE_INI_UPDATE_SERVER	= "server";
#endif
