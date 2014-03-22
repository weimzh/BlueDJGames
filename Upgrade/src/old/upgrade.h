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
#ifndef UPGRADE_H
#define UPGRADE_H

#include<QtGui>
#include <Qt3Support>
#include "DJBaseGlobal.h"
#include "DJGamePanel.h"
#include "DJGamePokerDesktop.h"
#include "DJGameImage.h"

#include "protocol.h"
#include "ug2profile.h"
#include "DJPokerPattern.h"

class UpgradePanel;
class DJToolButton;
	
#define UPGRADE_MAX_SAVE_ROUND			2

#define DJGAME_UPGRADE_TYPE_RESULT		0x101
#define DJGAME_UPGRADE_TID_RESULT		0x101

#define DJGAME_UPGRADE_TYPE_DESCRIBE		0x102
#define DJGAME_UPGRADE_TID_DESCRIBE			0x102

class UpgradeDesktop : public DJGamePokerDesktop
{
	Q_OBJECT
	public :
		UpgradeDesktop(QWidget* parent,DJGamePanel *panel);
		virtual ~UpgradeDesktop();
	
		virtual void userArrived( quint32 userId, quint8 seatId );
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
	
		void sortCards( quint8 *pCards, quint8 len );
		int Compare2Card(quint8 chCard1,quint8 chCard2, PDJGamePokerImage pimage);
		void GetPageInterval(quint8 chView,quint8 *pcards,quint8 chPages,int *dx,int *dy);
	
		void CheckShowStatus();
		void ShowCard(quint8 chCard);
		void DrawOver(bool bRepaint);
		bool SameCardClass(quint8 chCard1,quint8 chCard2);
		
		virtual void RepaintView1Cards(quint8 seat,quint8 *cards,quint8 chPages);
		bool isTrump( const DJPoker& poker ) const;
		int suitOfPoker( const DJPoker& poker ) const;
		QList<DJPoker> pokersOfSuit( quint8 seat, int suit, QList<DJPoker>& leftPokers ) const;
		int findMatchingWidth( QList<DJPokerPattern>& patterns, int width );
		QList<DJPokerPattern>::iterator findMatchingPattern( QList<DJPokerPattern>& patterns, int width, int length, int mode );
		virtual DJPoker higherSerialPoker( const DJPoker& poker, bool isStrict ) const;
		virtual DJPoker lowerSerialPoker( const DJPoker& poker, bool isStrict ) const;
	protected:
		virtual bool handleItemClicked(Qt::MouseButton btn,const QPoint& pos,Q3CanvasItem *item);
		virtual void handleMatrixChanged( const QMatrix& matrix );
	protected slots :
		void PlayerStarted();
		void ClickThrow();
		void ClickTip();
		void ShowHeart();
        void ShowSpade();
        void ShowDiamond();
        void ShowClover();
        void ShowNT();
		void NoShow();
	
		
	
		void PreviousPressed();
		void PreviousRelesed();
		void SubtractReleased();
		void SubtractPressed();
private slots:
#if QT_VERSION >= 0x040200	
	void dealCard(int currentCardFrame);
	void dealCardFinished();
#else	
	void HandleDrawTimeout();
#endif	
	private :
		void ModifyPlayerName();
		void ModifyScoreNumber();
		void ModifyShow(quint8 seat,quint8 chCard);
		void ModifyMaster();
	
		UpgradeCurrent c_current;
		UpgradePanel * c_UpgradePanel;
	
	
	quint8		c_chLastShow;
	quint8		c_chLastShowSeat;
	
	quint8		c_chPlayerCardsBuffer[UPGRADE_MAX_SITES+1][100];
	quint8		c_chPlayerCardPages[UPGRADE_MAX_SITES+1];
	
	bool		c_bFirstPlayer,c_bSelfThrow;
	quint8		c_chFirstPages;
	quint8		c_chFirstClass;
	
	quint8		c_ThrowCards[UPGRADE_MAX_SITES+1][UPGRADE_MAX_SAVE_ROUND+1][60];
	
	quint8		c_ScoreCards[60];
	quint8 		c_ScorePages;
	quint16		c_shCurrentScore;
	quint8		c_chLastScore;
	quint8		c_chLastMulti;
	
	quint8		c_chSubstract[20];
	quint8		c_chSubstractPages;
	quint8 		m_masterSeat;
	
	QWidget		*c_lShow;
#ifdef DJ_OEM
	DJToolButton	*c_bShows[6];
	DJToolButton	*m_previous,*m_subtract;
	QPushButton		*m_tip, *m_throw;
#else
	QWidget		*m_toolbar;
	QToolButton	*c_bShows[6];
	QToolButton	*m_previous,*m_subtract,*m_tip, *m_throw;
#endif
	
	DJGameTextItem	*c_Result;
	
	DJGameTextItem *c_MyTeam,*c_MyTeamLevel;
	DJGameTextItem *c_OtherTeam,*c_OtherLevel;
	DJGameTextItem *c_Master,*c_MasterName;
	DJGameTextItem *c_Shower,*c_ShowerName;
	DJGameTextItem *c_Trump;
	DJGameImageItem *m_trumpImage;
	//DJGameImageItem *c_TrumpIcon[5][4];
	DJGameTextItem *c_ScoreText,*c_ScoreNumber;
	
	QList<DJPoker::Point>		m_series;
		
#if QT_VERSION >= 0x040200
	QTimeLine	*m_timeline;
#else
	QTimer		*c_drawTimer;
#endif
};

class  UpgradePanel : public DJGamePanel
{
	Q_OBJECT
	public : 
		UpgradePanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent = 0, Qt::WFlags f = Qt::Window);
        ~UpgradePanel();
};

#endif
