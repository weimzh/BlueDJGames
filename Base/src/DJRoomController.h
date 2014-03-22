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

#ifndef DJROOMCONTROLLER_H
#define DJROOMCONTROLLER_H

#include <QtGui>
#include "DJBaseGlobal.h"

class DJGameController;
class DJRoomView;
class DJRoomCellItem;
class DJGameUser;
class DJRoomPlayerItem;
class DJRoomReadyItem;
class DJTableController;

class DJ_BASE_EXPORT DJRoomController : public QObject
{
    Q_OBJECT 
public:
    DJRoomController( DJGameController* gameController, QWidget *container );
    virtual ~DJRoomController();

	void createRoomItems( const QSize& cellSize, const QBrush& backgroundBrush,
				const QPixmap& readyImage, const QPixmap& tablePlayingPix,
    			uint numberOfTables,const QPixmap& tablePix, 
    			const QFont& tableFont, const QColor& tableColor,
				const QFont& playerFont, const QColor& playerColor,
				QList<QPixmap> seatPixs, const QList<QPoint>& seatOffsets );
	void clearRoomItems();
	void repaintAll();
	void addUser( DJGameUser* user );
	void removeUser( DJGameUser* user );
	void repaintPlayer( DJGameUser* user, DJRoomPlayerItem *playerItem );
	const QSize& cellSize() const { return m_cellSize; }
	const QPixmap&	readyPix() const { return m_readyPix; }
	const QPixmap& tablePlayingPix() const;
	const QPixmap& tablePix() const;
	const QFont& tableFont() const;
	const QColor& tableColor() const;
	const QList<QPixmap>& seatPixs() const;
	const QList<QPoint>& seatOffsets() const;
	DJRoomView *roomView() const;
	DJTableController* tableController( quint8 tableId ) const;
	DJGameController* gameController() const;
	int numberOfSeats() const { return m_seatPixs.size(); }
	qreal roomViewScale() const;
	void setRoomViewScale( qreal scale );
public slots:
	void repaintPlayer( DJGameUser* user );
	void handleSeatClicked( quint8 tableId, quint8 seatId, QGraphicsSceneMouseEvent *mouseEvent );
	void handleReady();
	void handlePlayerClicked( DJGameUser *user, Qt::MouseButtons buttons );
	void handlePlayerDoubleClicked( DJGameUser *user, Qt::MouseButtons buttons );
	void changeTableUsers( DJGameUser* user, quint8 oldTableId = 0, quint8 oldSeatId = 0, quint8 oldGameStatus = 0 );
	void reloadUser( DJGameUser* user );
	void setFocusTable( quint8 tableId );
	//void showRoomViewContext(const QPoint&);
	//void zoomIn();
	//void zoomOut();
	//void revert();
	void handleScaleChanged( int value );
private:
	int calculateColumns(int totalWidth, int cellWidth, int interval ) const;
private:
	QSize						m_cellSize;
	QPixmap						m_readyPix;
	QPixmap						m_tablePlayingPix;
	QPixmap						m_tablePix;
	QFont						m_tableFont;
	QColor						m_tableColor;
	QList<QPixmap>				m_seatPixs;
	QList<QPoint> 				m_seatOffsets;
	QFont						m_playerFont;
	QColor						m_playerColor;
	DJGameController			*m_gameController;
	
	DJRoomView					*m_roomView;
	QList<QPoint>				m_cellOrigins;
	
	QHash<DJGameUser*,DJRoomPlayerItem*>	m_playerItems;
	DJRoomReadyItem				*m_selfReadyItem;
	QGraphicsRectItem			*m_focusRectItem;
	QList<DJTableController*>	m_tableControllers;
	
	//QMenu*							m_roomViewContextMenu;
	//QAction*						m_actZoomIn;
	//QAction*						m_actRevert;
	//QAction*						m_actZoomOut;
};

#endif
