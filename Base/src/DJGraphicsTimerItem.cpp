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

#include "DJGraphicsTimerItem.h"
#include "Utility.h"

DJGraphicsTimerItem::DJGraphicsTimerItem( qreal scale, QGraphicsItem * parent, QGraphicsScene * scene )
	:DJGraphicsPixmapItem( parent, scene ),m_scale(scale)
{
	djDebug()<<"DJGraphicsTimerItem constructor";
	m_seconds	= 0;
	m_elapsed	= 0;
	m_clockPanel	= QPixmap(":/BaseRes/image/clock/panel.png");
	m_directions << QPixmap()
					<< QPixmap(":/BaseRes/image/clock/down.png")
					<< QPixmap(":/BaseRes/image/clock/left.png")
					<< QPixmap(":/BaseRes/image/clock/up.png")
					<< QPixmap(":/BaseRes/image/clock/right.png");
	
	ScalePixmap(m_clockPanel,m_scale);
	QList<QPixmap>::iterator it	= m_directions.begin();
	while ( it != m_directions.end() ) {
		QPixmap &pix	= *it;
		ScalePixmap(pix,m_scale);
		++it;
	}
	setPixmap( m_clockPanel );
	startTimer(1000);
}
DJGraphicsTimerItem::~DJGraphicsTimerItem()
{
	djDebug()<<"DJGraphicsTimerItem destructor";
}
int DJGraphicsTimerItem::type() const
{
	return DJGraphicsTimerItemType;
}
void DJGraphicsTimerItem::setTimeout( const QList<quint8>& views, int seconds )
{
	QPixmap	pixmap	= m_clockPanel;
	QPainter painter( &pixmap );
	foreach( quint8 view, views ) {
		painter.drawPixmap( 0, 0, m_directions.at( view ) );
	}
	setPixmap( pixmap );
	m_seconds	= seconds;
	m_elapsed	= 0;
	show();
}
void DJGraphicsTimerItem::timerEvent( QTimerEvent * event )
{
	m_elapsed++;
	if ( m_elapsed > m_seconds )
		m_elapsed	= m_seconds;
	update();
	//scene()->update( sceneBoundingRect() );
}
void DJGraphicsTimerItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{	
	//djDebug()<<"DJGraphicsTimerItem::paint";
	DJGraphicsPixmapItem::paint( painter, option, widget );
	QList<QPixmap> pixmaps;
	 
	int seconds	= m_seconds - m_elapsed;
	int bai	= seconds / 100;
	seconds	-= bai * 100;
	int shi	= seconds / 10;
	seconds	-= shi * 10;
	int num	= 0;
	if ( 0 != bai ) {
		num	= 3;
		pixmaps << QPixmap( QString(":/BaseRes/image/clock/%1.png").arg(bai) )
				<< QPixmap( QString(":/BaseRes/image/clock/%1.png").arg(shi) )
				<< QPixmap( QString(":/BaseRes/image/clock/%1.png").arg(seconds) );
	}else {
		num	= 2;
		pixmaps << QPixmap( QString(":/BaseRes/image/clock/%1.png").arg(shi) )
				<< QPixmap( QString(":/BaseRes/image/clock/%1.png").arg(seconds) );
	}
	//int width	= 34;
	//int height	= 64;
	//QSize size( 34, 64 );

	QPixmap conjointPixmap	= CreateConjointPixmap( pixmaps );
	
	QPoint c	= boundingRect().center().toPoint();
	int w	= static_cast<int>(boundingRect().width()/3);
	int h	= static_cast<int>(boundingRect().height()/3);
	QRect r( c.x()-w/2, c.y()-h/2, w, h );
	ScalePixmap( conjointPixmap, m_scale );
	painter->drawPixmap( r, conjointPixmap );
}
