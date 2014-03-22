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

#ifndef JQDESKTOPCONTROLLER_H
#define JQDESKTOPCONTROLLER_H

#include<QtGui>
#include "DJDesktopController.h"
#include "4country.h"

#define LANDBATTLEPANEL_CHESSMAP_MAXSIZE		65535
#define JQ_TYPE_CHIP							0x60
//#define LANDBATTLE_CHIP_WIDTH					50
//#define LANDBATTLE_CHIP_HEIGHT					32

class DJPanelController;
class DJGraphicsPixmapItem;
class DJGraphicsChessTimerItem;
class DJGraphicsLineItem;

class JQDesktopController : public DJDesktopController
{
	Q_OBJECT
public :
	JQDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent );
	~JQDesktopController();
	
	virtual void initUnderGameInfo( const QByteArray& buf );
	virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
	virtual void gameTraceModel( const GeneralGameTrace2Head *gameTrace );
	virtual void gameTraceView( const GeneralGameTrace2Head *gameTrace );
	virtual void locateAllNonMatrimoveItems();
	virtual void repaintAll();
	virtual void repaintCurrentStatus();
	virtual bool isCloseable() const;
	virtual bool isUserPlaying( quint8 seatId ) const;
	virtual void locateShowPositions( QList<QPoint>& points, QList<Qt::Alignment>& aligns );
	virtual void locateNamePositions( QList<QPoint>& points, QList<Qt::Alignment>& aligns );
	virtual void locateClockPosition( QPoint& point, Qt::Alignment& align );
	void setWaitingMoveACL( bool flag ) { m_isWaitingMoveACL = flag; }
	
public slots :
	void clickSave();
	void clickLoad();
	void clickChuZheng();
	void clickQiuHe();
	void clickRenShu();
	virtual void handleMousePressed( QGraphicsSceneMouseEvent *mouseEvent );
	void handleExchangeTimeout();
	void handleMoveTimeout();
private:
	void init();
	void initMappedSeatNodes(quint8 mappedSeat,PJunqiTraceInitChip psetchips);
	quint8 seat2MappedSeat( quint8 seat ) const;
	quint8 mappedSeat2Seat( quint8 mapSeat ) const;
	//int GetPhysicalBoard(PMapNode pnode);
	void clearAllChips();
	void clearMappedSeatChips( quint8 mapSeat );
	void repaintMappedSeatChips( quint8 mapSeat );
	void clearNodeChip(PMapNode pnode);
	QGraphicsItem* findChip(PMapNode pnode);
	void repaintNodeChip(PMapNode pnode,bool isSelected);
	void drawPathLine(PMapNode pcurnode,unsigned char *path);
	void playerMoveChip( const GeneralGameTrace2Head *gameTrace );
	void GetCurrentLayout(PGeneralGameTrace2Head ptrace,unsigned char chMapSite);
	void repaintBoard();
	quint8 viewOfNode( PMapNode pnode );
	
	JunqiCurrent 	m_current;
	char 			m_chessmap[LANDBATTLEPANEL_CHESSMAP_MAXSIZE];
	
	PMapNode 		m_nodeOfArrange1,m_nodeOfArrange2;
	PMapNode		m_nodeOfStartMove,m_nodeOfEndMove;
	PMapNode		m_nodeOfMoveChip;
	QTimer			*m_exchangeTimer,*m_moveTimer;
	
	unsigned char	m_chUnion[5],m_chUnions;
	
	unsigned char   m_resultOfMove;
	bool 			m_isWaitingMoveACL,m_isSelfOver;
	quint8 			m_lastTraceType;
	QPoint			m_origin;
	DJGraphicsLineItem *m_line;
	DJGraphicsPixmapItem	*m_board;
	QPushButton		*m_btnSave,*m_btnLoad,*m_btnChuZheng,*m_btnQiuHe,*m_btnRenShu;
	QSize			m_chipSize;
	bool			m_hasRequestDraw;
};

#endif
