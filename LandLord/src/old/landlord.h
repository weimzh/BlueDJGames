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
#ifndef __DJGAME_GAMEPANEL_DESKTOP_POKER_LANDLORD_H__
#define __DJGAME_GAMEPANEL_DESKTOP_POKER_LANDLORD_H__

#include<QtGui>
#include <Qt3Support>
#include "DJBaseGlobal.h"
#include "DJGamePanel.h"
#include "DJGamePokerDesktop.h"

#include "protocol.h"
#include "ddzrule.h"
#include "ddzprofile.h"

class LandlordPanel;
class LandlordCallButton;
class DJToolButton;

#define DOUDZHU_TYPE_LANDLORD				0x101
#define DOUDZHU_TID_LANDLORD				0x101

#define DJGAME_LANDLORD_TID_DESCRIBE		0x102
#define DJGAME_LANDLORD_TYPE_DESCRIBE		0x102

#define DOUDZHU_ARRANGE_STYLE_NUMBER			0
#define DOUDZHU_ARRANGE_STYLE_DOUBLE			1
#define DOUDZHU_ARRANGE_STYLE_MAX				1


class LandlordDesktop : public DJGamePokerDesktop
{
	Q_OBJECT
	public :
		LandlordDesktop(QWidget* parent,DJGamePanel *panel);
		virtual ~LandlordDesktop();
	
		virtual QString	playerItemNameSuffix( DJGameUser* user );
        virtual void gameInfo( const unsigned char *buf );
		virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
		virtual void gameTrace( const GeneralGameTrace2Head *gameTrace );
		virtual QString bonusTarget( const DJGameBonus* bonus );
    	//virtual void bonusFulfilled( const DJGameBonusFulfill* fulfill );
		void MoveItem(Q3CanvasItem* item,QPoint &ps,QPoint &pe);
		void RecvThrowACL(bool bFailed);
		void CreateThrowTrace(PGeneralGameTrace2Head ptrace);
		bool IsThrowStep(PGeneralGameTrace2Head ptrace);
		void StaticInitDesktop();
		void StaticGameTrace(GeneralGameTrace2Head *gameTrace);
		void RepaintCurrentStatus();
		void GetPageInterval(quint8 chView,quint8 *pcards,quint8 chPages,int *dx,int *dy);
		int Compare2Card(quint8 chCard1,quint8 chCard2,PDJGamePokerImage pimg);
		virtual void RepaintView1Attach(quint8 seat,quint8 *cards,quint8 chPages);
		virtual void RepaintView2Attach(quint8 seat,quint8 *cards,quint8 chPages);
		virtual void RepaintView3Attach(quint8 seat,quint8 *cards,quint8 chPages);
		virtual void RepaintView4Attach(quint8 seat,quint8 *cards,quint8 chPages);
		QImage GetCardPicture(quint8 chCard,quint8 chView,qreal rate);
		void RequestCall(quint8 chScore);
		bool 	IsFriend(quint8 seat1,quint8 seat2);
	
		virtual void repaintHost();
		
		void DisplayBombs();
		void DisplayScore();
		virtual DJPoker higherSerialPoker( const DJPoker& poker, bool isStrict ) const;
		virtual DJPoker lowerSerialPoker( const DJPoker& poker, bool isStrict ) const;
		DJPoker greaterPoker( const DJPoker& poker ) const;
		QList<DJPoker> findMajor( const QHash<DJPoker,int>& countPokers,
									const DJPoker& otherKeyPoker, 
									int width, int length, 
									QSet<DJPoker>& excluder ) const;	
		QList<DJPoker> findMinor( const QHash<DJPoker,int>& countPokers,
									int width, int length, 
									QSet<DJPoker>& excluder ) const;		 
	public slots:
		void ClickPass();
	protected:
		virtual bool handleItemClicked(Qt::MouseButton btn,const QPoint &pos,Q3CanvasItem *item);
		virtual void handleMatrixChanged( const QMatrix& matrix );
	protected slots :
		void PlayerStarted();
		void ClickTip();
		void ClickThrow();
		void ClickArrange();
#ifdef DJ_OEM		
		void PreviousPressed();
		void PreviousRelesed();
#endif		
	private :
		void repaintViewAttach(quint8 seat,quint8 *cards,quint8 chPages);
		
		DoudzhuCurrent c_current;
		LandlordPanel * c_LandlordPanel;
	
		quint8			c_Bombs;
#ifdef DJ_OEM
		QPushButton		*m_throw,*m_pass,*m_tip;
		DJToolButton	*m_arrange,*m_previous;
		quint8			m_previousThrow[5][35];
		quint8			m_previousThrowPages[5];		
#else		
		QWidget			*m_toolbar;
		QToolButton		*m_throw,*m_arrange,*m_pass,*m_tip;
#endif		
		quint8			c_chArrangeStyle;
	
		quint8			c_chLastCall;
		quint8			c_chLastCallSeat;
		quint32  		c_Score;
	
		quint8			c_chThrow[5][35];
		quint8			c_chThrowPages[5];
		quint8			c_chLastThrowSeat;
		quint8			m_lastThrowType;
		quint8			m_lastThrowValue;
		quint8			m_lastThrowSerial;
		
	LandlordCallButton	*llcallb[DOUDZHU_MAX_CALLLEVEL+1];
	//DJGameImageItem *c_itemLandloard;
	
	DJGameTextItem *c_CurrentBombsText,*c_CurrentBombs;
	DJGameTextItem *c_CurrentScoreText,*c_CurrentScore;
	
	QList<DJPoker>		m_tipHandPokers;
	QHash<DJPoker,int>	m_tipCountPokers;
	QSet<DJPoker>		m_tipMajorExcluder;
	QSet<DJPoker>		m_tipBombExcluder;
	bool				m_tipRocket;
};

class  LandlordPanel : public DJGamePanel
{
	Q_OBJECT
	public : 
		LandlordPanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent = 0, Qt::WFlags f = Qt::Window);
        ~LandlordPanel();
};

class LandlordCallButton : public QPushButton
{
	Q_OBJECT
	public:
		LandlordCallButton(QWidget * parent,quint8 chScore);
		~LandlordCallButton();
		quint8 score();
		
		
	private slots :
        void HandleRelesed();
	private :
		quint8 c_chScore;
	LandlordDesktop	*c_Parent;
};

#endif
