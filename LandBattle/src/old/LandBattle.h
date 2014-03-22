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

#ifndef __DJGAME_CLIENT_LANDBATTLE_H__
#define __DJGAME_CLIENT_LANDBATTLE_H__

#include<QtGui>
#include <Qt3Support>

#include "DJGamePanel.h"
#include "DJGameDesktop.h"

#include "protocol.h"
#include "4country.h"


class LandBattlePanel;
class DJTreeWidget;
class DJMatrixCanvasTimer;

#define JUNQI_CHESSMAP_RANGE 16
#define LANDBATTLEPANEL_CHESSMAP_MAXSIZE		65535

#define LANDBATTLE_CHIP_WIDTH					50
#define LANDBATTLE_CHIP_HEIGHT					30

#define DESKTOP_IMAGE_TYPE_CHIP				0x110
#define DESKTOP_IMAGE_TYPE_BOARD				0x101
#define DESKTOP_IMAGE_TYPE_PATH				0x102
#define DESKTOP_IMAGE_TYPE_BACKGROUND			0x103
class LandBattleDesktop:public DJGameDesktop
{
        Q_OBJECT
public :
        LandBattleDesktop( QWidget* parent,DJGamePanel *panel,quint32 w,quint32 h);
        ~LandBattleDesktop();

		virtual void userArrived( quint32 userId, quint8 seatId );
        virtual void gameInfo( const unsigned char *buf );
		virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
		virtual void gameTrace( const GeneralGameTrace2Head *gameTrace );
		virtual bool isCloseable();
		void MoveItem(Q3CanvasItem* item,QPoint &ps,QPoint &pe);
		

		void StaticInitDesktop();
		void StaticGameTrace(GeneralGameTrace2Head *gameTrace);
		void RepaintCurrentStatus();
	 	void SetWaitMoveACLStatus(bool b);

		void RepaintMapSiteChips(unsigned char chMapSite);
		void RepaintChessmap(int iContext);

		void DrawPath(PMapNode pcurnode,unsigned char *path);
		void ClearPathLine();
		void ClearSeatChips(quint8 seat);
		void ClearMapSiteChips(quint8 mapsite);
		void ClearNodeChip(PMapNode pnode);
		DJGameImageItem * GetChipOfNode(PMapNode pnode);
		char GetPhysicalBoard(PMapNode pnode);
		void DrawNodeChip(PMapNode pnode,bool bBig);
		
		void PlayerMoveChip( const GeneralGameTrace2Head *gameTrace );
		
		PMapNode	GetNodeOfChip(Q3CanvasItem *item);
		
		void InitMapSiteNodes(quint8 chMapSite,PJunqiTraceInitChip psetchips);
		
		void GetCurrentLayout(PGeneralGameTrace2Head ptrace,unsigned char chMapSite);
		unsigned char GetMapSiteOfSeat(unsigned char seat);
		unsigned char GetSeatOfMapSite(unsigned char chMapSite);
		
		virtual quint8	numberOfPlayers() const;
		virtual QList<quint8> seatsOfPlayers() const;
		
		virtual void locateShowPositions( QList<QPoint>& points, QList<Qt::Alignment>& aligns, QList<int> &modes );
    	virtual void locateNamePositions( QList<QPoint>& points, QList<Qt::Alignment>& aligns, QList<int> &modes );
    	virtual void locateClockPosition( QPoint& point, Qt::Alignment& align, int &mode );
//public slots:
   	//void ClickDuce();
protected :
	virtual bool handleItemClicked(Qt::MouseButton btn,const QPoint &pos,Q3CanvasItem * item);
	virtual void handleMatrixChanged( const QMatrix& matrix );
private slots :
	void ClickSave();
	void ClickLoad();
	void ClickSend();
	void ClickSurrender();
	void ClickDuce();
	void HandleExchangeTimeout();
	void HandleMoveTimeout();

private :
	void layoutButtons();
	
	LandBattlePanel *c_LandBattlePanel;
	JunqiCurrent		c_current;
	 char 		c_chessmap[LANDBATTLEPANEL_CHESSMAP_MAXSIZE];

	QPushButton			*c_pbSave,*c_pbLoad,*c_pbSend,*c_pbDuce,*c_pbSurrender;
	PMapNode 			c_pArrangeNode1,c_pArrangeNode2;
	PMapNode			c_pMoveChipNode;
	QTimer				*c_timerExchange;
	
	PMapNode		c_PlayerMoveNodeStart,c_PlayerMoveNodeEnd;
	unsigned char   c_MoveResult;
	QTimer			*c_MoveTimer;

	bool 			c_bWaitMoveACL;

	unsigned char 		c_chUnion[5],c_chUnions;

	int c_X, c_Y;
	
	DJGameImageItem	*c_itemBoard;

	quint8 c_LastTraceType;
	bool	m_isSelfOver;
	bool	m_hasRequestDraw;
};



class  LandBattlePanel : public DJGamePanel
{
	Q_OBJECT
	public : 
		LandBattlePanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent = 0, Qt::WFlags f = Qt::Window);
        ~LandBattlePanel();
        
        virtual void createAccessoryWidgets();
};
#endif
