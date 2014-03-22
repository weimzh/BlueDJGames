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

#ifndef	DJGRAPHICSCHESSTIMERITEM_H
#define DJGRAPHICSCHESSTIMERITEM_H

#include <QtGui>
#include "DJBaseGlobal.h"
#include "protocol.h"

class DJGraphicsTextItem;

class DJ_BASE_EXPORT DJGraphicsChessTimerItem : public QObject
{
	Q_OBJECT
public:
	DJGraphicsChessTimerItem( PDJGameChessTimer pchesstimer = 0, QGraphicsItem * parent = 0, QGraphicsScene * scene = 0 );
	virtual ~DJGraphicsChessTimerItem();

	virtual void modifyTimeText();
	
	void setTimer(PDJGameChessTimer pchesstimer);
	void start();
	void pause();
	
	bool isUp2Down() const { return m_isUp2Down; }
	void setUp2Down( bool isUp2Down );
	void setVirtualPos ( const QPointF & vpos );
	void setExternalScale( qreal externalScale );
	void adjustPos( const QMatrix& m = QMatrix() );
signals :
	void countdown(unsigned char chLast);	
private slots :
	void handleTimeout();	
private:
	bool					m_isUp2Down;
	
	DJGameChessTimer 		m_chessTimer;
	QTimer					*m_timer;
	int 					m_iStart,m_iElps;
	
	DJGraphicsTextItem		*m_basictimetext;
	DJGraphicsTextItem		*m_basictime;
	DJGraphicsTextItem		*m_dumiaotext;
	DJGraphicsTextItem		*m_dumiaotime;
	DJGraphicsTextItem		*m_timestext;
	DJGraphicsTextItem		*m_times;
	
	QList<DJGraphicsTextItem*>	m_items;
};
#endif
