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

#ifndef DJDESKTOPSCENE_H
#define DJDESKTOPSCENE_H

#include <QtGui>
#include "DJBaseGlobal.h"

class DJDesktop;

class DJ_BASE_EXPORT DJDesktopScene : public QGraphicsScene
{
    Q_OBJECT 
public:
    DJDesktopScene( QObject * parent = 0 );
    DJDesktopScene( const QRectF & sceneRect, QObject * parent = 0 );
    virtual ~DJDesktopScene();
    
    bool isMouseMoveEventEnabled() const { return m_isMouseMoveEventEnabled; }
    void setMouseMoveEventEnabled( bool isEnabled );
signals:
	void mousePressed( QGraphicsSceneMouseEvent *mouseEvent );
	void mouseReleased( QGraphicsSceneMouseEvent *mouseEvent );
	void mouseDoubleClicked( QGraphicsSceneMouseEvent *mouseEvent );
	void mouseMoving( QGraphicsSceneMouseEvent *mouseEvent );
protected:
	virtual void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
	virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent );
	virtual void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * mouseEvent );
	virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent );
private:
	bool		m_isMouseMoveEventEnabled;
};

#endif
