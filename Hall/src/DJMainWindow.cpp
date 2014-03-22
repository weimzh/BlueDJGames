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

#include "DJMainWindow.h"
#include "DJGlobal.h"

DJMainWindow::DJMainWindow( QWidget * parent, Qt::WFlags flags )
	: QMainWindow( parent, flags )
{
	m_qtCode		= Qt::Key_Escape;
	m_qtModifiers	= Qt::ShiftModifier;
#if defined( Q_WS_WIN )
	m_code			= VK_ESCAPE;
	m_modifiers		= MOD_SHIFT;
	m_winHotKeyId	= 0x4444;
	bool res = RegisterHotKey( (HWND)winId(), m_winHotKeyId, m_modifiers, m_code );
	djDebug()<<"win register hot key"<<res;
#elif defined (Q_WS_MAC)
	m_code			= 53;
	m_modifiers		= shiftKey;
	m_macHotKeyUPP	= NewEventHandlerUPP(MacHotKeyHandler);
	EventTypeSpec type;
	type.eventClass	= kEventClassKeyboard;
	type.eventKind	= kEventHotKeyPressed;
	InstallApplicationEventHandler( m_macHotKeyUPP, 1, &type, (void*)this, 0 );

	EventHotKeyID	id;
	id.signature	= 'bldj';
	id.id			= 0x4444;
	
	EventTargetRef	target	= GetApplicationEventTarget();
	
	OSStatus res	= RegisterEventHotKey( m_code, m_modifiers, id, target, 0, &m_macHotKeyRef );
	djDebug()<<"mac register hot key"<<res;
#endif	
}
DJMainWindow::~DJMainWindow()
{
#if defined( Q_WS_WIN )
	UnregisterHotKey( (HWND)winId(), m_winHotKeyId );
#elif defined (Q_WS_MAC)
	UnregisterEventHotKey( m_macHotKeyRef );
	DisposeEventHandlerUPP( m_macHotKeyUPP );		
#endif
}
/*
uint DJMainWindow::qtCode2OsCode( Qt::Key qtCode )
{
	uint osCode	= 0;
#if defined( Q_WS_WIN )	
	if ( qtCode >= Qt::Key_0 && qtCode <= Qt::Key_9 ) {
		int delta	= Qt::Key_9 - Qt::Key_0;
		osCode	= 0x30 + delta;
	}else if ( qtCode >= Qt::Key_A && qtCode <= Qt::Key_Z ) {
		int delta	= Qt::Key_Z - Qt::Key_A;
		osCode	= 0x41 + delta;
	}
#elif defined (Q_WS_MAC)
	
#elif defined( Q_WS_X11 )
	
#endif
	return osCode;
}
uint DJMainWindow::qtModifiers2OsModifiers( Qt::KeyboardModifiers qtModifiers )
{
	uint osModifiers	= 0;
#if defined( Q_WS_WIN )	
	if ( qtCode >= Qt::Key_0 && qtCode <= Qt::Key_9 ) {
		int delta	= Qt::Key_9 - Qt::Key_0;
		osCode	= 0x30 + delta;
	}else if ( qtCode >= Qt::Key_A && qtCode <= Qt::Key_Z ) {
		int delta	= Qt::Key_Z - Qt::Key_A;
		osCode	= 0x41 + delta;
	}else {
struct QT_VK_KEYMAP
{
      int key;
      UINT vk;
} qt_vk_table[] = {
      { Qt::Key_Escape,     VK_ESCAPE },
      { Qt::Key_Tab,        VK_TAB },
      { Qt::Key_Backtab,    0 },
      { Qt::Key_Backspace,  VK_BACK },
      { Qt::Key_Return,     VK_RETURN },
      { Qt::Key_Enter,      VK_RETURN },
      { Qt::Key_Insert,     VK_INSERT },
      { Qt::Key_Delete,     VK_DELETE },
      { Qt::Key_Pause,      VK_PAUSE },
      { Qt::Key_Print,      VK_PRINT },
      { Qt::Key_SysReq,     0 },
      { Qt::Key_Clear,      VK_CLEAR },
      { Qt::Key_Home,       VK_HOME },
      { Qt::Key_End,        VK_END },
      { Qt::Key_Left,       VK_LEFT },
      { Qt::Key_Up,         VK_UP },
      { Qt::Key_Right,      VK_RIGHT },
      { Qt::Key_Down,       VK_DOWN },
      { Qt::Key_PageUp,      VK_PRIOR },
      { Qt::Key_PageDown,       VK_NEXT },
      { Qt::Key_Shift,      VK_SHIFT },
      { Qt::Key_Control,    VK_CONTROL },
      { Qt::Key_Meta,       VK_LWIN },
      { Qt::Key_Alt,        VK_MENU },
      { Qt::Key_CapsLock,   VK_CAPITAL },
      { Qt::Key_NumLock,    VK_NUMLOCK },
      { Qt::Key_ScrollLock, VK_SCROLL },
      { Qt::Key_F1,         VK_F1 },
      { Qt::Key_F2,         VK_F2 },
      { Qt::Key_F3,         VK_F3 },
      { Qt::Key_F4,         VK_F4 },
      { Qt::Key_F5,         VK_F5 },
      { Qt::Key_F6,         VK_F6 },
      { Qt::Key_F7,         VK_F7 },
      { Qt::Key_F8,         VK_F8 },
      { Qt::Key_F9,         VK_F9 },
      { Qt::Key_F10,        VK_F10 },
      { Qt::Key_F11,        VK_F11 },
      { Qt::Key_F12,        VK_F12 },
      { Qt::Key_F13,        VK_F13 },
      { Qt::Key_F14,        VK_F14 },
      { Qt::Key_F15,        VK_F15 },
      { Qt::Key_F16,        VK_F16 },
      { Qt::Key_F17,        VK_F17 },
      { Qt::Key_F18,        VK_F18 },
      { Qt::Key_F19,        VK_F19 },
      { Qt::Key_F20,        VK_F20 },
      { Qt::Key_F21,        VK_F21 },
      { Qt::Key_F22,        VK_F22 },
      { Qt::Key_F23,        VK_F23 },
      { Qt::Key_F24,        VK_F24 },
      { Qt::Key_F25,        0 },
      { Qt::Key_F26,        0 },
      { Qt::Key_F27,        0 },
      { Qt::Key_F28,        0 },
      { Qt::Key_F29,        0 },
      { Qt::Key_F30,        0 },
      { Qt::Key_F31,        0 },
      { Qt::Key_F32,        0 },
      { Qt::Key_F33,        0 },
      { Qt::Key_F34,        0 },
      { Qt::Key_F35,        0 },
      { Qt::Key_Super_L,    0 },
      { Qt::Key_Super_R,    0 },
      { Qt::Key_Menu,       0 },
      { Qt::Key_Hyper_L,    0 },
      { Qt::Key_Hyper_R,    0 },
      { Qt::Key_Help,       0 },
      { Qt::Key_Direction_L,0 },
      { Qt::Key_Direction_R,0 },
 
      { Qt::Key_unknown,    0 },
 };
		
	}
#elif defined (Q_WS_MAC)
	
#elif defined( Q_WS_X11 )
	
#endif
}
*/
void DJMainWindow::hotKeyHandler()
{
	djDebug()<<"DJMainWindow::hotKeyHandler";
	if ( isHidden() )
		show();
	else
		hide();
	emit hotKeyPressed();
}
void DJMainWindow::closeEvent( QCloseEvent * e )
{
	djDebug()<<"DJMainWindow::closeEvent";
	Q_UNUSED( e );
	//QMainWindow::closeEvent( e );
	
	//QApplication::quit();
	emit quitHall();
}
#if defined( Q_WS_WIN )
bool DJMainWindow::winEvent( MSG* msg, long* result )
{
	if ( WM_HOTKEY == msg->message ) {
		hotKeyHandler();
	}
	return QMainWindow::winEvent( msg, result );
}
#elif defined (Q_WS_MAC)	
pascal OSStatus DJMainWindow::MacHotKeyHandler(EventHandlerCallRef caller, EventRef theEvent, void *userData)
{
	djDebug()<<"bitch";
	reinterpret_cast<DJMainWindow*>(userData)->hotKeyHandler();
	return CallNextEventHandler( caller, theEvent );
}
#endif
