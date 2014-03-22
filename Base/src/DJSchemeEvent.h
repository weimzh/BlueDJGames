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

#ifndef DJSCHEMEEVENT_H
#define DJSCHEMEEVENT_H

static QString 	DJSCHEME_AHREF			= 	QString("<a href=\"%1://%2:%3@%4\">%5</a>");
static QString	DJSCHEME_FIND_AHREF	=	QString("<a href=\"%1://%2:%3@%4\">");

//keep all string with lowercase, otherwise system will auto cast to lowercase
//define anchor event scheme
const 	char*	const DJSCHEME		=	"djscheme";

//url.name() is event id
enum DJSchemeEvent {
	DJSCHEME_EVENT_ACCEPT_PRIVATE_VIEW	=	1,
	DJSCHEME_EVENT_ACCEPT_INVITATION,
	DJSCHEME_EVENT_CHECK_INVITATION,
	DJSCHEME_EVENT_CALL_ADMIN,
	DJSCHEME_EVENT_ACCEPT_DRAW_CHESS,
};
//url.password() is parameter

//url.host() must be a string, otherwise djschem will be chaos
const 	char*	const DJSCHEME_HOST	=	"djhost";

//url.port() is reserved, port must be great than 10, or port will be combined with host
inline uint ExtractDJShemeEventID( const QUrl& url )
{
	return url.userName().toUInt();
}
inline QString ExtractDJShemeParameter( const QUrl& url )
{
	return url.password();
}
#endif
