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

#include "WinWebWidget.h"
#include "DJGlobal.h"

WinWebWidget::WinWebWidget( const QString& url, QWidget * parent )
	: QWidget( parent )
{
#ifdef MYWEBPAGE	
	QLibrary lib("webpage.dll");
#else
	QLibrary lib("cwebpage.dll");
#endif

	m_embedWebPageProc		= (EmbedWebPageProc)lib.resolve(EmbedWebPageProcName);
	m_unEmbedWebPageProc	= (UnEmbedWebPageProc)lib.resolve(UnEmbedWebPageProcName);
	m_resizeWebPageProc		= (ResizeWebPageProc)lib.resolve(ResizeWebPageProcName);
	m_doWebPageActionProc	= (DoWebPageActionProc)lib.resolve(DoWebPageActionProcName);
	
#ifdef MYWEBPAGE
	if ( m_embedWebPageProc )
		m_embedWebPageProc( (HWND)winId(), (LPCTSTR)url.utf16() );
#else
	m_displayHTMLPageProc	= (DisplayHTMLPageProc)lib.resolve(DisplayHTMLPageProcName);
	if ( m_embedWebPageProc && m_displayHTMLPageProc ) {
		m_embedWebPageProc( (HWND)winId() );
		m_displayHTMLPageProc( (HWND)winId(), (LPCTSTR)url.utf16() );
	}
#endif
}
WinWebWidget::~WinWebWidget()
{
	if ( m_unEmbedWebPageProc )
		m_unEmbedWebPageProc( (HWND)winId() );
}
void WinWebWidget::doWebPageAction( uint action )
{
	if ( m_doWebPageActionProc )
		m_doWebPageActionProc( (HWND)winId(), action );
}
void WinWebWidget::resizeEvent( QResizeEvent * event )
{
	QWidget::resizeEvent( event );
	if ( m_resizeWebPageProc ) {
		m_resizeWebPageProc( (HWND)winId(), (DWORD)width(), (DWORD)height() );
	}
}



