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
#ifndef __DJGAME_CLIENT_RENJU_PANEL_H__
#define __DJGAME_CLIENT_RENJU_PANEL_H__

#include<QtGui>
#include <Qt3Support>

#include "DJGamePanel.h"
#include "DJGameDesktop.h"

#include "protocol.h"
#include "RenjuRule.h"
#include "gobangprofile.h"

class  RenjuPanel;

#define RENJU_DESKTOP_TID_BOARD		(DJGAME_DESKTOP_TID_GAMEDEFINE+1)
#define RENJU_DESKTOP_TID_CHIP			(DJGAME_DESKTOP_TID_GAMEDEFINE+2)
#define RENJU_DESKTOP_TID_CHIPNUMBER	(DJGAME_DESKTOP_TID_GAMEDEFINE+3)
#define RENJU_DESKTOP_TID_CHIPCURRENT	(DJGAME_DESKTOP_TID_GAMEDEFINE+4)
#define RENJU_DESKTOP_TID_BUTTON_SIGNYN	(DJGAME_DESKTOP_TID_GAMEDEFINE+5)
#define RENJU_DESKTOP_TID_BUTTON_T0T1	(DJGAME_DESKTOP_TID_GAMEDEFINE+6)
//#define RENJU_DESKTOP_TID_AXIS	(DJGAME_DESKTOP_TID_GAMEDEFINE+7)

#define RENJU_DESKTOP_TYPE_BOARD		(DJGAME_DESKTOP_TYPE_GAMEDEFINE+1)
#define RENJU_DESKTOP_TYPE_CHIP		(DJGAME_DESKTOP_TYPE_GAMEDEFINE+2)
#define RENJU_DESKTOP_TYPE_BUTTON		(DJGAME_DESKTOP_TYPE_GAMEDEFINE+3)
#define RENJU_DESKTOP_TYPE_AXIS		(DJGAME_DESKTOP_TYPE_GAMEDEFINE+4)

class RenjuDesktop;
	
class RenjuChip : public QObject
{
	public :
		RenjuChip(Q3Canvas * cv,RenjuDesktop *desktop,quint8 chOwner,int x,int y,quint16 chStep);
		~RenjuChip();
		
		void show();
		void hide();
		void setNumberBase(quint16 base);
		quint16 number();
		quint16 	currentNumber();
		quint8	color();
		void setDisplayNumber(bool b);
		void setCurrentChip(bool b);
		bool isMe(Q3CanvasItem *item);
		bool isCurrent();
		void changeCurrentImage(quint8 ch);
	
	private :
		Q3Canvas		*c_Canvas;
	
		DJGameImageItem	*c_itemChip;
		DJGameTextItem	*c_itemNumber;
		quint8			c_chOwner;
		
		DJGameImageItem  *c_itemCurrent;
		bool	c_bCurrent;
	
		bool 	c_bNumber;
		quint16	c_NumberBase;
		quint16	c_Number;
		RenjuDesktop *c_Desktop;
};
		
class RenjuDesktop : public DJGameDesktop
{
	Q_OBJECT
	public :
		RenjuDesktop( QWidget* parent,DJGamePanel *panel,quint32 w,quint32 h);
		virtual ~RenjuDesktop();
	
		virtual QString	playerItemNameSuffix( DJGameUser* user );
        virtual void gameInfo( const unsigned char *buf );
		virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
		virtual void gameTrace( const GeneralGameTrace2Head *gameTrace );
		virtual bool isCloseable();
		void SetPlayerTimer(quint8 chOwner,PDJGameChessTimer rtimer);
		void MoveItem(Q3CanvasItem* item,QPoint &ps,QPoint &pe);
		//void HitItem(Qt::MouseButton,QPoint &pos,Q3CanvasItem *);
		
		void StaticInitDesktop();
		void StaticGameTrace(GeneralGameTrace2Head *gameTrace);
		void RepaintCurrentStatus();
	
		void SetWaitMoveACLStatus(bool b);
	
		RenjuChip * isChip(Q3CanvasItem *);
		void SetChipBaseNumber(quint16 base);
		void SignChips(bool b);
		void ClearChip(quint8 x,quint8 y);
		void CreateChip(quint8 chOwner,quint8 x,quint8 y,quint8 chStep);
	public slots:
		void ClickSurrender();
	protected:
		virtual bool handleItemClicked(Qt::MouseButton btn,const QPoint &pos,Q3CanvasItem *item);
		virtual void handleMatrixChanged( const QMatrix& matrix );
	protected slots :
		void PlayerStarted();
		void ClickForbid();
		void ClickExchange();
		void ClickReqDraw();
	private slots:
		void handleLast10Launch(unsigned char countdown);
	private :
		RenjuCurrent c_current;
		RenjuBoard		c_board;
		DJGameImageItem	*c_itemBoard;
		DJGameChessTimerItem	*c_blackTimer;
		DJGameChessTimerItem	*c_whiteTimer;
		RenjuPanel				*c_RenjuPanel;
		bool 					c_bWaitMoveACL;
	
		DJGameImageItem			*c_itemSignYN;
		DJGameImageItem			*c_itemSignT0T1;
		bool 					c_bSignYN;
		quint8					c_chSignT0;
	
		quint8 			c_chAutoForbid;
		QPushButton		*c_pbForbid;
		QPushButton		*c_pbExchange;
		QPushButton		*c_pbReqDraw;
		QPushButton		*c_pbSurrender;
		bool			c_b25Info;
		
		//QToolBar		*m_toolbar;
		//QToolButton		*c_pbForbid, *c_pbExchange, *c_pbReqDraw, *c_pbSurrender;
		
		RenjuStepDescribe	c_describe;
		
		quint16			c_shSteps;
		quint8			c_LastOwner;
		quint16 		c_iNumberBase;
		quint8			c_StepNumbers;
		quint8			c_Steps[300][2];

		RenjuChip 	*c_chips[DJGAME_RENJU_WIDTH+1][DJGAME_RENJU_WIDTH+1];
		char 		extar[100];
		bool		m_isRIFRules;
		
		DJGameTextItem	*m_axisX[15],*m_axisY[15];
		bool m_hasRequestDraw;
};

class  RenjuPanel : public DJGamePanel
{
	Q_OBJECT
public : 
		RenjuPanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent = 0, Qt::WFlags f = Qt::Window);
        ~RenjuPanel();
    virtual void createAccessoryWidgets();
protected :
	virtual void closeEvent( QCloseEvent *e );
};

#endif
