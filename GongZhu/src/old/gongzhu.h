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
#ifndef __DJGAME_GAMEPANEL_DESKTOP_POKER_GONGZHU_H__
#define __DJGAME_GAMEPANEL_DESKTOP_POKER_GONGZHU_H__

#include<QtGui>
#include <Qt3Support>
#include "DJBaseGlobal.h"
#include "DJGamePanel.h"
#include "DJGamePokerDesktop.h"

#include "protocol.h"
#include "gozhrule.h"
#include "gozhprof.h"

class GongzhuPanel;
class DJToolButton;
	
#define GONGZHU_MAX_SAVE_ROUND			2

class GongzhuDesktop : public DJGamePokerDesktop
{
	Q_OBJECT
	public :
		GongzhuDesktop(QWidget* parent,DJGamePanel *panel);
		~GongzhuDesktop();
	
        virtual void gameInfo( const unsigned char *buf );
		virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
		virtual void gameTrace( const GeneralGameTrace2Head *gameTrace );
		void MoveItem(Q3CanvasItem* item,QPoint &ps,QPoint &pe);
		void RecvThrowACL(bool bFailed);
		void CreateThrowTrace(PGeneralGameTrace2Head ptrace);
		bool IsThrowStep(PGeneralGameTrace2Head ptrace);
		void StaticInitDesktop();
		void StaticGameTrace(GeneralGameTrace2Head *gameTrace);
		void RepaintCurrentStatus();
		void RepaintScoreCard(quint8 seat);
	protected:
		virtual bool handleItemClicked(Qt::MouseButton btn,const QPoint &pos,Q3CanvasItem *item);
		virtual void handleMatrixChanged( const QMatrix& matrix );
	protected slots :
		void PlayerStarted();
		void ClickThrow();
#ifdef DJ_OEM	
		void PreviousPressed();
		void PreviousRelesed();
#endif		
	private :
		GongzhuCurrent c_current;
		GongzhuPanel * c_GongzhuPanel;
	
		quint8			c_ThrowCards[GONGZHU_MAX_SITES+1][GONGZHU_MAX_SAVE_ROUND+2][6];
		quint8			c_ScoreCards[GONGZHU_MAX_SITES+1][50];
		quint8			c_ScorePages[GONGZHU_MAX_SITES+1];
	
		bool c_bFirstPlayer;
		quint8 c_chFirPlayerThrow,c_chFirstPages;
		quint8 c_chCurrentMax[6],c_chRounds[6];
		QPushButton *c_pbShow,*c_pbThrow;
#ifdef DJ_OEM			
		DJToolButton	*m_previous;
#endif		
};

class  GongzhuPanel : public DJGamePanel
{
	Q_OBJECT
	public : 
		GongzhuPanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent = 0, Qt::WFlags f = Qt::Window);
        ~GongzhuPanel();
};

#endif
