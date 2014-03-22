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

#include "LaunchUrl.h"

#if defined( Q_WS_MAC )
#include <Carbon/Carbon.h>
#elif defined( Q_WS_WIN )
#include <Windows.h>
#endif

void LaunchURL( const QString &url )
{
#if QT_VERSION >= 0x040200
	QDesktopServices::openUrl( QUrl(url) );
#else
#if defined( Q_WS_MAC )	
	OSStatus err;
    ICInstance inst;
    err = ICStart(&inst, 0 );           // Use your creator code if you have one!
	
    if (err == noErr) {
		Str255  hint;
		c2pstrcpy(hint,"http");
		QByteArray	buf	= url.toLocal8Bit();
		long length = buf.size();
		long startSel = 0;
		long endSel = length;
		err = ICLaunchURL(inst, hint, buf.data(),
						  length, &startSel, &endSel);
        ICStop(inst);
    }
#elif defined( Q_WS_WIN )
	::ShellExecute(0,(const WCHAR*)QString("open").utf16(),(const WCHAR*)url.utf16(),0,0,SW_SHOWNORMAL);
#elif defined( Q_WS_X11 )
	static QProcess browser;
	browser.start( "mozilla", QStringList(url) );
#endif
#endif
}

