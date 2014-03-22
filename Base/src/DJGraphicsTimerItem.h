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

#ifndef	DJGRAPHICSTIMERITEM_H
#define DJGRAPHICSTIMERITEM_H

#include <QtGui>
#include "DJBaseGlobal.h"
#include "DJGraphicsPixmapItem.h"

class DJ_BASE_EXPORT DJGraphicsTimerItem : public QObject, public DJGraphicsPixmapItem
{
public:
	enum { DJGraphicsTimerItemType = UserType + 3 };
	DJGraphicsTimerItem( qreal scale = 1, QGraphicsItem * parent = 0, QGraphicsScene * scene = 0 );
	virtual ~DJGraphicsTimerItem();

	virtual int type() const;
	int seconds() const { return m_seconds; }
	int elapsed() const { return m_elapsed; }
	void setTimeout( const QList<quint8>& views, int seconds );
protected:
    virtual void timerEvent( QTimerEvent * event );
    virtual void paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
private:
	int					m_seconds;
	int					m_elapsed;
	QPixmap				m_clockPanel;
	QList<QPixmap>		m_directions;
	qreal				m_scale;
};
#endif
