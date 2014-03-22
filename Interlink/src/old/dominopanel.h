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
#include "protocol.h"
#include "dmnrule.h"
#include "dmnprofile.h"
#include "panelbase.h"

class DominoPanel;
class DJTreeWidget;
class DominoDesktop;
class DominoHand;
class DJMatrixCanvasTimer;
class DJMatrixCanvasImage;

class DominoHand:public Q3CanvasView
{
	Q_OBJECT
public :
        DominoHand(Q3Canvas * canvas, QWidget* parent,DominoPanel *panel,unsigned char chSeat,DominoDesktop *desktop);
        ~DominoHand();
		void RecvCards(unsigned char *pcards,unsigned char chPages,bool bReset);
		void RepaintCards();
		void RepaintAvatar();
		void PlaceCard();
		void WaitPlace();
		void WaitDraw();
		void ClearCards();
		void ChangeScore(int s);
		void ResetScore();
		void ClearBoneStatus();
		unsigned char GetMarkBone();
		void GameWait(quint16 mask, quint16 timeout);
		bool c_bWaitACL,c_bPlace;
		void RemoveCard(unsigned char chCard,unsigned char chDesktopNumber);
protected :
	void resizeEvent( QResizeEvent * );
	void contentsMouseReleaseEvent(QMouseEvent* e);
	void contentsMouseDoubleClickEvent ( QMouseEvent * e );
private slots :
	void ClickDraw();
	void HandleScoreTimer();
private :
		//Q3CanvasPixmapArray *c_paTimer;
		//Q3CanvasSprite *itemTimer;
		//QTimer			*c_timer,
		//Q3CanvasText	*c_TimerText;
		
	DJMatrixCanvasTimer	*m_clockItem;
	
	QTimer			*c_scoreTimer;
		
	
	int c_Score;

	DominoImageItem			*c_Avatar;
	Q3CanvasText		*c_UserName,*c_textScore;

	Q3Canvas 		*c_Canvas;
	QPushButton *c_pbDraw;

	DominoPanel *c_Panel;
	DominoDesktop *c_Desktop;

	unsigned char c_Seat;
	unsigned char c_Cards[DOMINO_DOUBLE9_CARDS];
	unsigned char c_chPages;
};

class DominoDesktop:public Q3CanvasView
{
        Q_OBJECT
public :
        DominoDesktop(Q3Canvas * canvas, QWidget* parent,DominoPanel *panel);
        ~DominoDesktop();

		bool RepaintLink();
		bool RepaintSingleChain(unsigned char chChainID,int x,int y,unsigned char chOri,QRect & rect);

		void ClearLink(unsigned char chChainID);
		DominoImageItem *FindCard(unsigned char chCard);

		DominoImageItem *  DrawBone(int x,int y,unsigned char chCard,unsigned char chChainID,unsigned char chOri);
		void ClearAllBones();
		void PlayerPlace(unsigned char chSite,unsigned char chChainID,unsigned char chCard);
		void AdjustLink(PDominoChains pchains);
		void WaitStart();
		void WaitPlace();
		PDominoChains GetDominoChains();
		
		void ShowDesktopNumber(unsigned char chNumber);
		void ShowBoneYard(unsigned char chPages);

		void CheckPosition(unsigned char chCard,unsigned char *pchCards,unsigned char chPages,bool bSend=false);
		bool SendPlace(unsigned char chCard,unsigned char chChainID,unsigned char *pchCards,unsigned char chPages);
		bool hasStarted();
		int  c_X,c_Y,c_Fx;
		bool c_bWaitACL,c_bPlace;

		DominoChains		c_chains;

		QString c_strSaveFile;
		DominoPanel *c_Panel;
public slots:
		void changeBackgroundColor( const QColor& color );
protected :
	void resizeEvent( QResizeEvent * );
	void contentsMouseReleaseEvent(QMouseEvent* e);
	void contentsDragMoveEvent ( QDragMoveEvent * e ) ;

private slots :
	void ClickStart();
	void HandleResizeTimeout();
private :
	Q3Canvas 		*c_Canvas;
	QPushButton		*c_pbStart;
	DominoImageItem  		*c_itemFocus,*c_itemTails[5];
	int 							c_TailX[5],c_TailY[5];

	Q3CanvasText	*c_textDN,*c_textYard;
	QTimer *c_resizeTimer;
};



class  DominoPanel : public DJGamePanel
{
	Q_OBJECT
	public :
        DominoPanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent = 0, Qt::WFlags f = Qt::Window);
        ~DominoPanel();
		
		virtual void createAccessoryWidgets();
		
        virtual void userArrived( quint32 userId, quint8 seatId );
        virtual void gameInfo( const QByteArray& buf );
		virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout );
		virtual void gameTrace( const GeneralGameTrace2Head *gameTrace );
		virtual void magicResult( const MagicResult* magicResult );
		virtual void gameOver( const DJGameOver* gameOver );
		bool IsWaitPlace();
		bool IsWaitDraw();
		bool WaittingForMe();
		quint16 GetWaitTime();
		DominoHand * GetSelfHand();
		DominoCurrent	c_Current;
	
	protected :
		//void resizeEvent( QResizeEvent * );	
		void closeEvent( QCloseEvent *e);	
	private :
		//DJTreeWidget		*c_PlayerList;
		//QSplitter *m_leftSplitter;
	
		DominoDesktop	*c_Desktop;
		DominoHand		*c_Hand[5];
		Q3Canvas			*c_DesktopCanvas,*c_HandCanvas[5];
	
		quint8			c_chTableStatus;
		quint16 		c_shWaitMask;
		int				c_iTimeLin;

};



#endif
