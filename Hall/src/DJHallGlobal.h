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

#ifndef DJHALLGLOBAL_H
#define DJHALLGLOBAL_H

#include "protocol.h"

const	char*	const	ORGANIZATION_NAME	= QT_TRANSLATE_NOOP("OEM","BlueDJ");
const	char*	const	ORGANIZATION_DOMAIN	= "bluedj.com";
const	char*	const	APPLICATION_NAME	= QT_TRANSLATE_NOOP("OEM","BlueDJGames");
const	char*	const	COPYRIGHT_COMPANY	= QT_TRANSLATE_NOOP("OEM","Beijing BlueDJ Tech Ltd.");

const	char*	const	TEMP_DIR	= "BlueDJGames";

const	char*	const	GAME_HOST_IP	= "main.bluedj.com";
const	quint16			GAME_HOST_PORT	= DJGAMESERVER_DEFAULT_PORT;

const	char*	const	AUTH_LOGIN_PATH		=	"/bluedjclientauth.php?username=%1&password=%2";

const	char*	const	UPDATE_INI_PATH			=	"download/update/win/x86/Update.ini";
//const	char*	const	UPDATE_INI_PATH			=	"download/update/mac/ub/Update.ini";
//const	char*	const	UPDATE_INI_PATH			=	"download/update/mac/ppc/Update.ini";
//const	char*	const	UPDATE_INI_PATH			=	"download/update/mac/x86/Update.ini";
//const	char*	const	UPDATE_INI_PATH			=	"download/update/mac/leopard/ub/Update.ini";
//const	char*	const	UPDATE_INI_PATH			=	"download/update/mac/leopard/ppc/Update.ini";
//const	char*	const	UPDATE_INI_PATH			=	"download/update/mac/leopard/x86/Update.ini";
//const	char*	const	UPDATE_INI_PATH			=	"download/update/x11/x86/Update.ini";
//const	char*	const	UPDATE_INI_PATH			=	"download/update/x11/mips/Update.ini";

#endif
