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

#ifndef DJBASEGLOBAL_H
#define DJBASEGLOBAL_H

#include <QtCore>
#include "DJGlobal.h"

#ifdef Q_WS_WIN
    #if defined(DJ_BUILD_BASE_LIB)
    	#define DJ_BASE_EXPORT Q_DECL_EXPORT
    #elif defined(DJ_BUILD_GAME_LIB)
    	#define DJ_BASE_EXPORT Q_DECL_IMPORT
    #else
    	#define DJ_BASE_EXPORT
    #endif
#else
	#define DJ_BASE_EXPORT
#endif

typedef void (*AclProc)( const QByteArray& content , const QVariant& parameters );

//const definition
const	char*	const KEY_HALL						=	"Hall";

const	char*	const KEY_SETUP						=	"Setup";

const	char*	const KEY_SOUND						=	"Sound";
const	char*	const KEY_LANGUAGE					=	"Language";
const	char*	const KEY_MAX_IMAGE_SIZE			=	"MaxImageSize";
const	char*	const KEY_USE_PROXY					=	"UseProxy";
const	char*	const KEY_PROXY_TYPE				=	"ProxyType";
const	char*	const KEY_SOCKS4_PROXY				=	"Socks4Proxy";
const	char*	const KEY_SOCKS5_PROXY				=	"Socks5Proxy";
const	char*	const KEY_HTTP_PROXY				=	"HttpProxy";
const	char*	const KEY_AUTH_USERNAME				=	"AuthUsername";
const	char*	const KEY_AUTH_PASSWORD				=	"AuthPassword";
const	char*	const KEY_APPLICATION_FONT			=	"ApplicationFont";
const	char*	const KEY_MIN_SENTENCES				=	"MinSentences";
const	char*	const KEY_MAX_SENTENCES				=	"MaxSentences";

const	char*	const KEY_SPEED						=	"Speed";
const	char*	const KEY_REFUSE_SAMEIP				=	"RefuseSameIp";
const	char*	const KEY_BREAK_RATE				=	"BreakRate";
const	char*	const KEY_MIN_SCORE					=	"MinScore";
const	char*	const KEY_MAX_SCORE					=	"MaxScore";
const	char*	const KEY_MAX_UNWELCOME				=	"MaxUnwelcome";

const	char*	const KEY_GAMETAB_SPLITTER_SIZES	=	"GameTabSplitterSizes";
const	char*	const KEY_GAME_DESKTOP_BRUSH		=	"GameDesktopBrush";

const	int		DJ_CONTACT_USER_TYPE_NONE			=	0;
const 	int		DJ_CONTACT_USER_TYPE_FRIEND			=	1;
const 	int		DJ_CONTACT_USER_TYPE_UNWELCOME		=	2;

const	quint32	MIN_USER_ID							=	10000000;

const	char*	const HTTP_WWW_HOST						=	"http://www.bluedj.com/";

#endif
