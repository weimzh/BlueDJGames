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

//Please visit http://www.bluedj.com for more infomation about us
//and contact us at ggwizard@gmail.com or darkdong@gmail.com
//****************************************************************************/

#ifndef __DJGAME_CLIENT_SHISENSHO_PANEL_H__
#define __DJGAME_CLIENT_SHISENSHO_PANEL_H__

#include<QtGui>
#include <Qt3Support>

#include "DJGamePanel.h"
#include "DJTreeWidget.h"
#include "DJGameRank.h"

#include "protocol.h"
#include "shssrule.h"
#include "shssprofile.h"
#include "panelbase.h"

class SHSSDesktop;
class SHSSPanel;
class DJGamePlayerItem;
class DJMatrixCanvasTimer;

static const int SHSSLifeRTTI = 0x0eab1824;

class SHSSLifeItem:public Q3CanvasLine
{
	public :
		SHSSLifeItem(Q3Canvas * canvas);
		~SHSSLifeItem();
	int rtti () const { return SHSSLifeRTTI; }
};

class SHSSDesktop:public Q3CanvasView
{
        Q_OBJECT
public :
        SHSSDesktop(Q3Canvas * canvas, QWidget* parent,SHSSPanel *panel);
        ~SHSSDesktop();
		void WaitStart( quint16 timeout );
		void WaitDelete( quint16 timeout );
		void WaitContinue( quint16 timeout );
		void AdjustMap(GeneralGameTrace2Head *ptrace);
		void DeleteCard(GeneralGameTrace2Head *ptrace);
		void ClearCards();
		void ClearAllItems();
		void ClearAllLine();
		void RepaintCards();
		
		void RequestAdjust();
		
		int GetFx();
		void Map2Canvas(unsigned char chMapWidth,unsigned char chMapHeight,int *x,int *y,unsigned char chPosType);
		QImage GetCardImage(unsigned char chCard);
		
		SHSSImageItem * GetCardItemByMousePos(int x,int y);
		SHSSImageItem * GetCardItem(int x,int y);
		void ResetCardMark(unsigned char chCard,bool bMark);
		void MarkCard(SHSSImageItem *item,bool bMark);

		void SetACLStatus(bool);
		void SetScore(unsigned int iScore);
		void AddScore(unsigned int	iScore,bool bWah);
		
		void RepaintAvatar();
		void ModifyLifeLine();
		void ClearLifes();
		void RepaintLifes();
public slots:
		void changeBackgroundColor( const QColor& color );		
protected :
	void resizeEvent( QResizeEvent * );
	void contentsMouseReleaseEvent(QMouseEvent* e);
	//void contentsDragMoveEvent ( QDragMoveEvent * e ) ;
	virtual void timerEvent ( QTimerEvent * event );
private slots :
	void ClickStart();
void ClickReset();
	//void HandleTimeout();
	void HandleDeleteTimeout();
private :
	
SHSSImageItem *c_Avatar;
Q3CanvasText	*c_UserName,*c_ScoreText;

	bool			c_bWaitACL;
	Q3Canvas 		*c_Canvas;
	QPushButton		*c_pbStart,*c_pbReset;

	//Q3CanvasPixmapArray *c_paTimer;
	//Q3CanvasSprite *c_itemTimer;
	//Q3CanvasText	*c_TimerText;
	//QTimer		*c_timer;
	QTimer			*c_deleteTimer;
	SHSSPanel			*c_Panel;

	unsigned char c_shSkin;
	unsigned char 	c_chMap[SHISENSHO_MAX_MAP+8];
	ShisenshoDelete		c_CurDel;

	unsigned char c_DelX1,c_DelY1;

	unsigned int  c_iScore;
	
	SHSSLifeItem *c_itemLifeLine;
	Q3CanvasRectangle *c_itemLifeRect;
	
	DJMatrixCanvasTimer	*m_clockItem;

};

class  SHSSPanel : public DJGamePanel
{
	Q_OBJECT
	public :
        SHSSPanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent = 0, Qt::WFlags f = Qt::Window);
        ~SHSSPanel();
		
		virtual void createAccessoryWidgets();
		virtual void playerItemHeaderSections( QList<int> &sectionIds, QStringList &sectionNames );
	
		virtual void userArrived( quint32 userId, quint8 seatId );
        virtual void gameInfo( const QByteArray& buf );
		virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout );
		virtual void gameTrace( const GeneralGameTrace2Head *gameTrace );
		virtual void magicResult( const MagicResult* magicResult );
		virtual void gameOver( const DJGameOver* gameOver );
		
		bool WaittingForDelete();
		unsigned char GetTableStatus();
		quint16 GetWaitTime();
	
		ShisenshoCurrent GetCurrentInfo();
	
protected :
		//void resizeEvent( QResizeEvent * );	
		void closeEvent( QCloseEvent *e);	
	
private :
		DJGameRankTabWidget			*c_Rank;
	
		ShisenshoCurrent	c_Current;

		SHSSDesktop	*c_Desktop;
		Q3Canvas			*c_DesktopCanvas;
	
		quint8			c_chTableStatus;
		quint16 c_shWaitMask;
		int				c_iTimeLin;

};







#endif
