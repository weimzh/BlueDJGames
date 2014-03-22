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

#include "DJDesktopScene.h"
#include "DJDesktop.h"

DJDesktopScene::DJDesktopScene( QObject * parent )
	:QGraphicsScene( parent )
{	
	djDebug() << "DJDesktopScene constructor";
	
	m_isMouseMoveEventEnabled	= false;
}
DJDesktopScene::DJDesktopScene( const QRectF & sceneRect, QObject * parent )
	:QGraphicsScene( sceneRect, parent )
{	
	djDebug() << "DJDesktopScene constructor";
	
	m_isMouseMoveEventEnabled	= false;
}
DJDesktopScene::~DJDesktopScene()
{
	djDebug() << "DJDesktopScene destructor";
}
void DJDesktopScene::setMouseMoveEventEnabled( bool isEnabled )
{
	m_isMouseMoveEventEnabled	= isEnabled;
}
void DJDesktopScene::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
	djDebug()<<"DJDesktopScene::mousePressEvent";
	QGraphicsScene::mousePressEvent( mouseEvent );
	
	/*
	QPointF scenePos	= mouseEvent->scenePos();
	QPointF virtualPos	= m_desktop->totalMatrix().inverted().map(scenePos);
	QGraphicsItem *item	= itemAt( scenePos );
	emit mousePressed( mouseEvent->button(), virtualPos, item );
	*/
	emit mousePressed( mouseEvent );
}
void DJDesktopScene::mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
	djDebug()<<"DJDesktopScene::mouseReleaseEvent";
	QGraphicsScene::mouseReleaseEvent( mouseEvent );
	
	/*
	QPointF scenePos	= mouseEvent->scenePos();
	QPointF virtualPos	= m_desktop->totalMatrix().inverted().map(scenePos);
	QGraphicsItem *item	= itemAt( scenePos );
	emit mouseReleased( mouseEvent->button(), virtualPos, item );
	*/
	emit mouseReleased( mouseEvent );
}
void DJDesktopScene::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
	djDebug()<<"DJDesktopScene::mouseDoubleClickEvent";
	QGraphicsScene::mouseDoubleClickEvent( mouseEvent );
	
	/*
	QPointF scenePos	= mouseEvent->scenePos();
	QPointF virtualPos	= m_desktop->totalMatrix().inverted().map(scenePos);
	QGraphicsItem *item	= itemAt( scenePos );
	emit mouseDoubleClicked( mouseEvent->button(), virtualPos, item );
	*/
	emit mouseDoubleClicked( mouseEvent );
}
void DJDesktopScene::mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
	//djDebug()<<"DJDesktopScene::mouseMoveEvent";
	QGraphicsScene::mouseMoveEvent( mouseEvent );
	
	if ( m_isMouseMoveEventEnabled ) {
		emit mouseMoving( mouseEvent );
	}
}

