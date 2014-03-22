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

#include "DJApplication.h"
#include "DJMainWindow.h"
#include "DJGlobal.h"

#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
	
DJApplication::DJApplication( int & argc, char ** argv )
	: QApplication( argc, argv )
{
	m_mainWindow	= 0;
	
	m_qtCode		= Qt::Key_Escape;
	m_qtModifiers	= Qt::ShiftModifier;
	m_code		= XKeysymToKeycode( QX11Info::display(),XK_Escape );
	m_modifiers	= ShiftMask;
		
	XGrabKey( QX11Info::display(), m_code, m_modifiers, QX11Info::appRootWindow(), false, GrabModeAsync, GrabModeAsync );	
}
DJApplication::~DJApplication()
{
	XUngrabKey( QX11Info::display(), m_code, m_modifiers, QX11Info::appRootWindow() );
}
void DJApplication::setMainWindow( DJMainWindow* mainWindow )
{
	m_mainWindow	= mainWindow;
}
bool DJApplication::x11EventFilter ( XEvent * event )
{
	if ( KeyPress == event->type ) {
		XKeyEvent	keyEvent	= event->xkey;
		if ( m_code == keyEvent.keycode
			&& m_modifiers == keyEvent.state
			&& m_mainWindow ) {
			m_mainWindow->hotKeyHandler();
			return true;
		}
	}
	return false;
}	