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

#ifndef DJGAMEINTERFACE_H
#define DJGAMEINTERFACE_H

#include <QtGui>

class DJHallInterface;

enum GameCommand {
	GAME_COMMAND_GAME_DATA,
	GAME_COMMAND_QUIT,
	GAME_COMMAND_RELOAD_SETTING,
	GAME_COMMAND_LOGIN_OK,
	GAME_COMMAND_GAME_CONTEXT,
	GAME_COMMAND_GAMETAB_SELECTED,
	GAME_COMMAND_CONTACT_USER_TYPE_CHANGED,
	GAME_COMMAND_GAME_USER_INFO,
	GAME_COMMAND_DISCONNECTED,
	GAME_COMMAND_RECONNECTING,
	GAME_COMMAND_STATE,
	GAME_COMMAND_REMOVE_GAME
};

typedef	quint32	(*GIGetBaseVersionProc)();

typedef	void	(*GIGetGameInfoProc)( quint16& id, quint32& version, QIcon& icon );
typedef	QString	(*GIGetTranslatedGameNameProc)();

typedef void	(*GICreateGameControllerProc)( const DJHallInterface& hallInterface );
typedef QVariant	(*GIGameControllerCommandProc)( int command, const QVariant& parameters );
typedef void	(*GIDeleteGameControllerProc)();
#endif
