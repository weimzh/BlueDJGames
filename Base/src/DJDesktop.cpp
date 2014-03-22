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

#include "DJDesktop.h"
#include "DJDesktopScene.h"
#include "DJGameController.h"
#include "DJPanelController.h"
#include "DJDesktopController.h"
#include "Utility.h"

DJDesktop::DJDesktop( DJDesktopController *desktopController, const QSize& realSize, QWidget *parent )
	:QGraphicsView( parent ),m_desktopController(desktopController)
{	
	djDebug() << "DJDesktop constructor";
	
	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	
	m_realWidth	= realSize.width();
	m_realHeight	= realSize.height();
	m_scale	= 1;
	
	DJDesktopScene	*scene	= new DJDesktopScene( QRectF(0,0,realSize.width(),realSize.height()), this );
	setScene( scene );
		
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	/*
	QBrush brush	=  GetSettingGameDesktopBrush (desktopController->panelController()->gameController()->gameName());
	setBackgroundBrush( brush );
	QPalette pal = palette();
	pal.setBrush(QPalette::Background,brush);
	setPalette(pal);
	connect( desktopController->panelController(), SIGNAL(changeDesktopBrush(const QBrush&)), SLOT(setDesktopBrush(const QBrush&)) );
	*/
	
	m_resizeTimer = new QTimer(this);
	m_resizeTimer->setInterval( 500 );
	m_resizeTimer->setSingleShot( true );
	connect(m_resizeTimer,SIGNAL(timeout()), SLOT(calculateMatrix()));
}
DJDesktop::~DJDesktop()
{
	djDebug() << "DJDesktop destructor";
}
DJDesktopScene* DJDesktop::desktopScene() const
{
	return (DJDesktopScene*)scene();
}
/*
void DJDesktop::setDesktopBrush( const QBrush& brush )
{
	setBackgroundBrush( brush );
	QPalette pal = palette();
	pal.setBrush(QPalette::Background,brush);
	setPalette(pal);
	SetSettingGameDesktopBrush ( m_desktopController->panelController()->gameController()->gameName(), brush );
}
*/
void DJDesktop::resizeEvent( QResizeEvent * event )
{
	//djDebug()<<"DJDesktop::resizeEvent"<<event->oldSize()<<event->size();
	QGraphicsView::resizeEvent( event );
	m_resizeTimer->start();
}
void DJDesktop::setRealSize( int realWidth, int realHeight )
{
	m_realWidth	= realWidth;
	m_realHeight	= realHeight;
	calculateMatrix();
}
void DJDesktop::calculateMatrix()
{
	djDebug()<<"DJDesktop::calculateMatrix";
	
	int	w	= viewport()->width();
	int	h	= viewport()->height();
	
	djDebug()<<"viewport"<<"w="<<w<<"h="<<h;
	djDebug()<<"view"<<"width"<<width()<<"height"<<height();
	
	qreal a = 1.0;
	qreal b = 1.0;
	qreal c = 1.0;
	if(m_realWidth != 0 && m_realHeight != 0)
	{
		a = static_cast<qreal>(w)/static_cast<qreal>(m_realWidth);
		b = static_cast<qreal>(h)/static_cast<qreal>(m_realHeight);
		c = qMin(a,b);
	}
	
	qreal dx = (static_cast<qreal>(w) - (m_realWidth*c)) / 2.0;
	qreal dy = (static_cast<qreal>(h) - (m_realHeight*c)) / 2.0;
	
	m_matrix	= QMatrix(c,0,0,c,dx,dy);
	m_scale	= c;
	setSceneRect( 0, 0, w, h );
	emit matrixChanged();
}
