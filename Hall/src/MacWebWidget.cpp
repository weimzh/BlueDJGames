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

#include "MacWebWidget.h"
#include "MacWebPage.h"

MacWebWidget::MacWebWidget( const QString& url, QWidget* container, QWidget * parent )
	: QWidget( parent )
{
	setAttribute(Qt::WA_OpaquePaintEvent);
	//setAttribute(Qt::WA_NoSystemBackground);
	
	m_hiWebView	= 0;
	m_container	= container;
	
	CFStringRef strUrl	= CFStringCreateWithCharacters(NULL,url.utf16(),url.size());
	EmbedWebPage( strUrl, (HIViewRef)container->winId(), m_hiWebView );
	CFRelease(strUrl);
}
MacWebWidget::~MacWebWidget()
{
}
void MacWebWidget::resizeEvent( QResizeEvent *event )
{
	QWidget::resizeEvent( event );
	
	if ( m_hiWebView && m_container )  {
		setWebViewFrame();
	}
	
}
void MacWebWidget::setWebViewFrame()
{
	if ( m_hiWebView && m_container)  {
		QPoint globalOrigin	= mapToGlobal( QPoint(0,0) );
		
		QPoint containerOrigin	= m_container->mapFromGlobal(globalOrigin);
		
		HIRect          bounds;
		bounds.origin.x	= containerOrigin.x();
		bounds.origin.y	= containerOrigin.y();
		bounds.size.width	= width();
		bounds.size.height	= height();
		
		HIViewSetFrame( m_hiWebView, &bounds );
	}
}
void MacWebWidget::showEvent ( QShowEvent * event )
{
	QWidget::showEvent(event);
	if ( m_hiWebView && m_container)  {
		setWebViewFrame();
		HIViewAddSubview( (HIViewRef)m_container->winId(), m_hiWebView );
		HIViewSetVisible( m_hiWebView, true );
	}
}
void MacWebWidget::hideEvent ( QHideEvent * event )
{
	QWidget::hideEvent(event);
	if ( m_hiWebView && m_container)  {
		HIViewRemoveFromSuperview( m_hiWebView );
		HIViewSetVisible( m_hiWebView, false );
	}
}


