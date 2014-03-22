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

#include "DJGraphicsChessTimerItem.h"
#include "DJGraphicsTextItem.h"
#include "DJDesktop.h"
#include "Utility.h"

DJGraphicsChessTimerItem::DJGraphicsChessTimerItem( PDJGameChessTimer pchesstimer, QGraphicsItem * parent, QGraphicsScene * scene )
{
	djDebug()<<"DJGraphicsChessTimerItem constructor";
	
	m_isUp2Down	= true;
	
	if(pchesstimer != NULL)
	{
		memcpy(&m_chessTimer,pchesstimer,sizeof(DJGameChessTimer));
		m_chessTimer.shBaseTime = letoh2(pchesstimer->shBaseTime);
	}else
		memset(&m_chessTimer,0,sizeof(DJGameChessTimer));

	m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),SLOT(handleTimeout()));
    
    QString str;
	str=tr("Basic Time");
	m_basictimetext = new DJGraphicsTextItem(str,parent,scene);
	m_items << m_basictimetext;
	str	= "Basic Time";
	m_basictime = new DJGraphicsTextItem(str,parent,scene);
	m_items << m_basictime;
	str = tr("Fast");
	m_dumiaotext = new DJGraphicsTextItem(str,parent,scene);
	m_items << m_dumiaotext;
	str=QString("0");
	m_dumiaotime = new DJGraphicsTextItem(str,parent,scene);
	m_items << m_dumiaotime;
	str = tr("Reserve");
	m_timestext = new DJGraphicsTextItem(str,parent,scene);
	m_items << m_timestext;
	str=QString("0");
	m_times = new DJGraphicsTextItem(str,parent,scene);
	m_items << m_times;
}
DJGraphicsChessTimerItem::~DJGraphicsChessTimerItem()
{
	djDebug()<<"DJGraphicsChessTimerItem destructor";
}
void DJGraphicsChessTimerItem::setUp2Down( bool isUp2Down )
{
	m_isUp2Down	= isUp2Down;
	
	Qt::Alignment align	= Qt::AlignHCenter;
	if ( !isUp2Down )
		align	|= Qt::AlignBottom; 
	foreach( DJGraphicsTextItem* item, m_items ) {
		item->setAlignment( align );
	}
}
void DJGraphicsChessTimerItem::setVirtualPos ( const QPointF & vpos )
{
	qreal	x	= vpos.x();
	qreal	y	= vpos.y();
	if ( m_isUp2Down ) {
		foreach( DJGraphicsTextItem* item, m_items ) {
			item->setVirtualPos( QPointF(x,y) );
			y	+= item->boundingRect().height();
		}
	}else {
		for( int i = m_items.size() - 1; i >= 0; i-- ) {
			DJGraphicsTextItem* item	= m_items[i];
			item->setVirtualPos( QPointF(x,y) );
			y	-= item->boundingRect().height();
		}
	}
}
void DJGraphicsChessTimerItem::setExternalScale( qreal externalScale )
{
	foreach( DJGraphicsTextItem* item, m_items ) {
		item->setExternalScale( externalScale );
	}
}
void DJGraphicsChessTimerItem::adjustPos( const QMatrix& m )
{
	foreach( DJGraphicsTextItem* item, m_items ) {
		item->adjustPos( m );
	}
}
void DJGraphicsChessTimerItem::setTimer( PDJGameChessTimer pchesstimer )
{
	if(pchesstimer != NULL)
	{
		memcpy(&m_chessTimer,pchesstimer,sizeof(DJGameChessTimer));
		m_chessTimer.shBaseTime = letoh2(pchesstimer->shBaseTime);
	}else
		memset(&m_chessTimer,0,sizeof(DJGameChessTimer));

	modifyTimeText();
}
void DJGraphicsChessTimerItem::start()
{
	m_iStart = time(NULL);
	m_iElps = 0;
	m_timer->start(1000);
}
void DJGraphicsChessTimerItem::pause()
{
	m_iStart = 0;
	m_iElps = 0;
	m_timer->stop();
}
void DJGraphicsChessTimerItem::handleTimeout()
{
	djDebug()<<"DJGraphicsChessTimerItem::handleTimeout";
	int ispace = time(NULL)-m_iStart;
	if(ispace > m_iElps)
		ispace = ispace-m_iElps;
	else
		return;
	m_iElps += ispace;
	if(m_chessTimer.shBaseTime > ispace)
	{
		m_chessTimer.shBaseTime -= ispace;
	}else
	{
		if(m_chessTimer.shBaseTime != 0)
		{
			ispace -= m_chessTimer.shBaseTime;
			m_chessTimer.shBaseTime = 0;
		}
		if(m_chessTimer.chTimeAfterBaseTime > ispace)
		{
			m_chessTimer.chTimeAfterBaseTime -= ispace;
			if ( m_chessTimer.chTimeAfterBaseTime <= 10 )
				emit countdown(m_chessTimer.chTimeAfterBaseTime);
		}else if(m_chessTimer.chTimes > 0)
		{
			ispace -= m_chessTimer.chTimeAfterBaseTime;
			m_chessTimer.chTimeAfterBaseTime = 0;
			m_chessTimer.chTimes --;
			m_chessTimer.chTimeAfterBaseTime += (m_chessTimer.chTimeAfterTimeout-ispace);
		}
	}
	modifyTimeText();
}
void DJGraphicsChessTimerItem::modifyTimeText()
{	
	char buffer[100];
		
	sprintf(buffer,"%d:%d:%d",m_chessTimer.shBaseTime/3600,
				(m_chessTimer.shBaseTime%3600)/60,(m_chessTimer.shBaseTime%3600)%60);
	m_basictime->setPlainText(QString(buffer));
	
	sprintf(buffer,"%d:%d",m_chessTimer.chTimeAfterBaseTime/60,
				m_chessTimer.chTimeAfterBaseTime%60);
	m_dumiaotime->setPlainText(QString(buffer));
	
	sprintf(buffer,"%d-%d:%d",m_chessTimer.chTimes,
				m_chessTimer.chTimeAfterTimeout/60,m_chessTimer.chTimeAfterTimeout%60);
	QString str=QString(buffer)+tr("times");
	m_times->setPlainText(QString(buffer));
}
