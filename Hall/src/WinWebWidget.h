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

#ifndef WINWEBWIDGET_H
#define WINWEBWIDGET_H

#include <QtGui>

#include <windows.h>
#include <exdisp.h>

#define MYWEBPAGE

#define WEBPAGE_GOBACK		0
#define WEBPAGE_GOFORWARD	1
#define WEBPAGE_GOHOME		2
#define WEBPAGE_SEARCH		3
#define WEBPAGE_REFRESH		4
#define WEBPAGE_STOP		5


#ifdef MYWEBPAGE
typedef void (*EmbedWebPageProc)(HWND hwnd, LPCTSTR url);
typedef void (*UnEmbedWebPageProc)(HWND hwnd);
typedef void (*ResizeWebPageProc)(HWND hwnd, DWORD width,DWORD height);
typedef void (*DoWebPageActionProc)(HWND hwnd, DWORD action);

#define EmbedWebPageProcName	"EmbedWebPage"
#define UnEmbedWebPageProcName	"UnEmbedWebPage"
#define ResizeWebPageProcName	"ResizeWebPage"
#define DoWebPageActionProcName	"DoWebPageAction"
#else
typedef long WINAPI (*EmbedWebPageProc)(HWND hwnd);
typedef void WINAPI (*UnEmbedWebPageProc)(HWND hwnd);
typedef long WINAPI (*DisplayHTMLPageProc)(HWND hwnd, LPCTSTR url);
typedef void WINAPI (*ResizeWebPageProc)(HWND hwnd, DWORD width, DWORD height);
typedef void WINAPI (*DoWebPageActionProc)(HWND hwnd, DWORD action);

#define EmbedWebPageProcName	"EmbedBrowserObject"
#define UnEmbedWebPageProcName	"UnEmbedBrowserObject"
#define DisplayHTMLPageProcName	"DisplayHTMLPage"
#define ResizeWebPageProcName	"ResizeBrowser"
#define DoWebPageActionProcName	"DoPageAction"
#endif
class WinWebWidget : public QWidget
{
	Q_OBJECT
public:
	WinWebWidget( const QString& url, QWidget * parent = 0 );
	virtual ~WinWebWidget();
	
	void doWebPageAction( uint action );
protected:
	virtual void resizeEvent( QResizeEvent * event );
private:
	EmbedWebPageProc			m_embedWebPageProc;
	UnEmbedWebPageProc			m_unEmbedWebPageProc;
	ResizeWebPageProc			m_resizeWebPageProc;
	DoWebPageActionProc			m_doWebPageActionProc;
#ifndef MYWEBPAGE
	DisplayHTMLPageProc			m_displayHTMLPageProc;
#endif	
};

#endif
