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

#ifndef	DJGRAPHICSPIXMAPITEM_H
#define DJGRAPHICSPIXMAPITEM_H

#include <QtGui>
#include "DJBaseGlobal.h"

class DJ_BASE_EXPORT DJGraphicsPixmapItem : public QGraphicsPixmapItem
{
public:
	enum { DJGraphicsPixmapItemType = UserType + 2 };
	
	DJGraphicsPixmapItem( QGraphicsItem * parent = 0, QGraphicsScene * scene = 0, bool isMatrimoveMode = true );
	DJGraphicsPixmapItem( const QPixmap & pixmap, QGraphicsItem * parent = 0, QGraphicsScene * scene = 0, bool isMatrimoveMode = true );
	virtual ~DJGraphicsPixmapItem();

	virtual int type() const;
	virtual void paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
	
	QPointF deltaPos() const { return m_deltaPos; }
	void setDeltaPos( const QPointF & deltaPos );
	void setPen( const QPen& pen );
	bool isDJSelected() const { return m_selected; }
	void setDJSelected( bool selected );
	
	bool isMatrimoveMode() const { return m_isMatrimoveMode; }
	void setMatrimoveMode( bool isMatrimoveMode );
	Qt::Alignment	alignment() const { return m_alignment; }
	void setAlignment( Qt::Alignment alignment );
	QPointF virtualPos() const { return m_virtualPos; }
	void setVirtualPos ( const QPointF & pos );
	qreal externalScale() const { return m_externalScale; }
	void setExternalScale( qreal externalScale );
	qreal internalScale() const { return m_internalScale; }
	void setInternalScale( qreal internalScale );
	qreal angleOfRotation() const { return m_angleOfRotation; }
	void setAngleOfRotation( qreal angleOfRotation );
	void adjustPos( const QMatrix& m = QMatrix() );
private:
	void init();
	
	QPointF			m_deltaPos;
	QPen			m_pen;
	bool			m_selected;
	
	bool			m_isMatrimoveMode;
	Qt::Alignment	m_alignment;
	QPointF			m_virtualPos;
	qreal 			m_externalScale;
	qreal			m_internalScale;
	qreal			m_angleOfRotation;
};
#endif
