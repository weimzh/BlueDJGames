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

#ifndef __DJGAME_CLIENT_XIANGQI_PANEL_H__
#define __DJGAME_CLIENT_XIANGQI_PANEL_H__

#include<QtGui>
#include <Qt3Support>

#include "DJGamePanel.h"
#include "DJTopWidget.h"
#include "DJGameDesktop.h"

#include "protocol.h"
#include "xqrule.h"
#include "xqprofile.h"

class XiangqiPanel;
class DJTreeWidget;
class XiangqiDesktop;
class DJGamePlayerItem;

#define XIANGQI_BOARD_MAX_HEIGHT			750
#define XIANGQI_BOARD_MAX_WIDTH				675
#define XIANGQI_CHIP_MAX_SIZE				75

#define DESKTOP_IMAGE_TYPE_BOARD			0x102

#define DESKTOP_IMAGE_TYPE_CHIP				0x110
#define DESKTOP_IMAGE_TYPE_RED				(DESKTOP_IMAGE_TYPE_CHIP+DJGAME_XIANGQI_RED)
#define DESKTOP_IMAGE_TYPE_BLACK			(DESKTOP_IMAGE_TYPE_CHIP+DJGAME_XIANGQI_BLACK)

#define DESKTOP_IMAGE_TYPE_FOCUS			0x120
#define DESKTOP_IMAGE_ISCHIP(__type)			((__type)== DESKTOP_IMAGE_TYPE_RED || (__type)==DESKTOP_IMAGE_TYPE_BLACK)


class XiangqiDesktop:public DJGameDesktop
{
        Q_OBJECT
	public :
        XiangqiDesktop( QWidget* parent,DJGamePanel *panel,quint32 w,quint32 h);
        ~XiangqiDesktop();
		
		virtual QString	playerItemNameSuffix( DJGameUser* user );
        virtual void gameInfo( const unsigned char *buf );
		virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
		virtual void gameTrace( const GeneralGameTrace2Head *gameTrace );
		void MoveItem(Q3CanvasItem* item,QPoint &ps,QPoint &pe);
		

		void StaticInitDesktop();
		void StaticGameTrace(GeneralGameTrace2Head *gameTrace);
		void RepaintCurrentStatus();
	 	void SetWaitMoveACLStatus(bool b);

		void GetNodeXY(unsigned char x,unsigned char y,int *px,int *py);
		DJGameImageItem * GetChipOfNode(unsigned char x,unsigned char y);
		void DrawChip(unsigned char x,unsigned char y);
		void ClearChip(unsigned char x,unsigned char y);

		void SetPlayerTimer(quint8 chOwner,PDJGameChessTimer rtimer);
		void ShowAutoDisappearText( const QString& hint );
		
	protected :
		virtual bool handleItemClicked(Qt::MouseButton btn,const QPoint& pos,Q3CanvasItem * item);
		virtual void handleMatrixChanged( const QMatrix& matrix );	
	private slots :
		void PlayerStarted();
		void ClickSurrender();
		void ClickDues();
		//void HandleLayoutButtons(const QMatrix &m);
		void handleLast10Launch(unsigned char countdown);
	private :
		bool	IsJiangJun(unsigned char chEnd);

		XiangqiCurrent c_current;
		XiangQiBoard		c_Board;
		DJGameImageItem	*c_itemBoard;
		DJGameImageItem	*c_itemFocus,*c_itemStartPos;

		DJGameChessTimerItem	*c_blackTimer;
		DJGameChessTimerItem	*c_redTimer;
		XiangqiPanel				*c_XiangqiPanel;
		bool 					c_bWaitMoveACL;

		QPushButton		*c_pbSur,*c_pbDues;

		int 			c_X,c_Y;
		unsigned char c_chStartPoint;

		unsigned char c_shSkin;
};



class  XiangqiPanel : public DJGamePanel
{
	Q_OBJECT
	public : 
		XiangqiPanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent = 0, Qt::WFlags f = Qt::Window);
        ~XiangqiPanel();
};

#endif
