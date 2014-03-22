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

#include "DJWidget.h"

DJWidget::DJWidget( QWidget * parent, BKMode bkmode )
	: QWidget( parent ),m_mode(bkmode)
{
	djDebug()<<"DJWidget constructor";
	setAutoFillBackground( true );
}
DJWidget::~DJWidget()
{
	djDebug()<<"DJWidget destructor";
}
void DJWidget::setStops( const QGradientStops& stops )
{
	m_stops	= stops;	
}
void DJWidget::setPixmap( const QPixmap& pixmap )
{
	m_pixmap	= pixmap;
	
	QPalette pal	= palette();
	pal.setBrush( backgroundRole(), pixmap );
	setPalette( pal );
}
/*
void DJWidget::paintEvent( QPaintEvent *event )
{
	switch( m_mode ) {
#ifndef Q_WS_MAC		
	case LinearGradient: {
		QLinearGradient linearGradient( rect().topLeft(), rect().topRight() );
		linearGradient.setStops( m_stops );
		
    	QPainter painter(this);
		painter.fillRect( rect(), linearGradient );
		QPalette pal = palette();
		pal.setBrush( backgroundRole(), linearGradient );
		setPalette(pal);
		break;
	}
#endif		
	default:
		break;
	}
	QWidget::paintEvent( event );
}
*/
void DJWidget::resizeEvent ( QResizeEvent * event )
{
	QWidget::resizeEvent( event );

	switch( m_mode ) {
#ifndef Q_WS_MAC		
	case LinearGradient: {
		QLinearGradient linearGradient( rect().topLeft(), rect().topRight() );
		linearGradient.setStops( m_stops );
		
    	QPalette pal = palette();
		pal.setBrush( QPalette::Window, linearGradient );
		setPalette(pal);
		break;
	}
#endif
	case Pixmap: {
		QPixmap pix	= m_pixmap.scaled( event->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
		QPalette pal	= palette();
		pal.setBrush( QPalette::Window, pix );
		setPalette( pal );
		break;
	}	
	default:
		break;
	}
}



