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

//Please visit http://www.bluedj.com for more infomation about us
//and contact us at ggwizard@gmail.com or darkdong@gmail.com
//****************************************************************************/

#include <QtGui>
#include "GZController.h"
#include "Utility.h"
#include "protocol.h"

#if defined( Q_WS_MAC )	
	static const quint32	gameVersion	= 0x9030200;
#elif defined( Q_WS_WIN )
	static const quint32	gameVersion	= 0x9030200;
#elif defined( Q_WS_X11 )
	static const quint32	gameVersion	= 0x9030200;
#endif

static	const quint16		gameId			= DJGAME_GONGZHU;
static	QString				gameName		= "GongZhu";
static 	GZController		*gameController	= 0;

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

extern "C" MY_EXPORT void GetGameInfo( quint16& id, quint32& version, QIcon& icon )
{	
	id		= gameId;
	version	= gameVersion;
	icon	= QIcon( QString(":/%1Res").arg(gameName) + "/image/base/game.png" );
}
extern "C" MY_EXPORT QString GetTranslatedGameName()
{	
	QString locale	= FindSupportedLocaleName();
	
    QTranslator translator;
   	translator.load( QString(":/%1Res").arg(gameName) + "/lang/"  + gameName + "_" + locale );
	return translator.translate( "GZController", gameName.toAscii() );
}
extern "C" MY_EXPORT void CreateGameController( const DJHallInterface &hallInterface )
{
	if ( 0 == gameController )
		gameController	= new GZController( gameId, gameName, hallInterface );
}
extern "C" MY_EXPORT void DeleteGameController()
{
	delete gameController;
	gameController	= 0;
}
extern "C" MY_EXPORT QVariant GameControllerCommand( int command, const QVariant& parameters )
{
	if ( gameController )
		return gameController->gameCommand( command, parameters );
	else
		return QVariant();
}


