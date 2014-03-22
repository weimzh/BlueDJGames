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

#ifndef	DJGRAPHICSTEXTITEM_H
#define DJGRAPHICSTEXTITEM_H

#include <QtGui>
#include "DJBaseGlobal.h"

class DJ_BASE_EXPORT DJGraphicsTextItem : public QGraphicsTextItem
{
public:
	enum { DJGraphicsTextItemType = UserType + 1 };
	DJGraphicsTextItem( QGraphicsItem * parent = 0, QGraphicsScene * scene = 0, bool isMatrimove = true );
	DJGraphicsTextItem( const QString & text, QGraphicsItem * parent = 0, QGraphicsScene * scene = 0, bool isMatrimove = true );
	virtual ~DJGraphicsTextItem();

	virtual int type() const;
	
	bool isMatrimoveMode() const { return m_isMatrimoveMode; }
	void setMatrimoveMode( bool isMatrimoveMode );
	Qt::Alignment	alignment() const { return m_alignment; }
	void setAlignment( Qt::Alignment alignment );
	QPointF virtualPos() const { return m_virtualPos; }
	void setVirtualPos( const QPointF &vpos );
	qreal externalScale() const { return m_externalScale; }
	void setExternalScale( qreal externalScale );
	qreal internalScale() const { return m_internalScale; }
	void setInternalScale( qreal internalScale );
	qreal angleOfRotation() const { return m_angleOfRotation; }
	void setAngleOfRotation( qreal angleOfRotation );
	void adjustPos( const QMatrix& m = QMatrix() );
private:
	void init();
	
	bool			m_isMatrimoveMode;
	Qt::Alignment	m_alignment;
	QPointF			m_virtualPos;
	qreal 			m_externalScale;
	qreal			m_internalScale;
	qreal			m_angleOfRotation;
};
#endif
