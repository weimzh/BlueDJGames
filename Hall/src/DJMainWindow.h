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

#ifndef DJMAINWINDOW_H
#define DJMAINWINDOW_H

#include <QtGui>
#if defined( Q_WS_MAC )
	#include <Carbon/Carbon.h>
#elif defined( Q_WS_WIN )
	#include <windows.h>
#endif

class DJMainWindow : public QMainWindow
{
	Q_OBJECT
public:
	DJMainWindow( QWidget * parent = 0, Qt::WFlags flags = 0 );
	virtual ~DJMainWindow();
	
	//uint qtCode2OsCode( Qt::Key qtCode );
	//uint qtModifiers2OsModifiers( Qt::KeyboardModifiers qtModifiers );
	void hotKeyHandler();
signals:
	void quitHall();
	void hotKeyPressed();
protected:
	virtual	void closeEvent( QCloseEvent *e );
#if defined( Q_WS_WIN )	
	virtual bool winEvent( MSG *message, long *result );
#elif defined (Q_WS_MAC)
	static pascal OSStatus MacHotKeyHandler(EventHandlerCallRef caller, EventRef theEvent, void *userData);
#endif
private:
	Qt::Key					m_qtCode;
	Qt::KeyboardModifiers	m_qtModifiers;
	uint					m_code;
	uint					m_modifiers;
#if defined( Q_WS_WIN )
	int	m_winHotKeyId;
#elif defined( Q_WS_MAC )
	EventHandlerUPP	m_macHotKeyUPP;		
	EventHotKeyRef	m_macHotKeyRef;
#endif
};
#endif
