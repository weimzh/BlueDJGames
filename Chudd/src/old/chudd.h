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
#ifndef __DJGAME_GAMEPANEL_DESKTOP_POKER_CHUDD_H__
#define __DJGAME_GAMEPANEL_DESKTOP_POKER_CHUDD_H__

#include<QtGui>
#include <Qt3Support>
#include "DJBaseGlobal.h"
#include "DJGamePanel.h"
#include "DJGamePokerDesktop.h"

#include "protocol.h"
#include "chuddrule.h"
#include "chuddprof.h"

class ChuddPanel;
class DJToolButton;

#define CHUDD_ARRANGE_STYLE_NUMBER			0
#define CHUDD_ARRANGE_STYLE_DOUBLE			1
#define CHUDD_ARRANGE_STYLE_MAX				1

#define DJGAME_CHUDD_TID_DESCRIBE			0x101
#define DJGAME_CHUDD_TYPE_DESCRIBE		0x101

class ChuddDesktop : public DJGamePokerDesktop
{
	Q_OBJECT
	public :
		ChuddDesktop(QWidget* parent,DJGamePanel *panel);
		virtual ~ChuddDesktop();
	
		virtual void gameInfo( const unsigned char *buf );
		virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
		virtual void gameTrace( const GeneralGameTrace2Head *gameTrace );
		virtual QString bonusTarget( const DJGameBonus* bonus );
		void MoveItem(Q3CanvasItem* item,QPoint &ps,QPoint &pe);
		void RecvThrowACL(bool bFailed);
		void CreateThrowTrace(PGeneralGameTrace2Head ptrace);
		bool IsThrowStep(PGeneralGameTrace2Head ptrace);
		void StaticInitDesktop();
		void StaticGameTrace(GeneralGameTrace2Head *gameTrace);
		void RepaintCurrentStatus();
		void ArrangeCards(quint8 *cards,quint8 chPages, int mode = 1 );
		void GetPageInterval(quint8 chView,quint8 *pcards,quint8 chPages,int *dx,int *dy);
		
		QImage GetCardPicture(quint8 chCard,quint8 chView,qreal rate);
	
		int Compare2Card(quint8 chCard1,quint8 chCard2,PDJGamePokerImage pPokerImage);
		/*
		virtual bool groupGreaterThan( const QList<DJPoker>& group1,
									const QList<DJPoker>& group2 );
		virtual bool shunGreaterThan( const QList<DJPoker>& shun1,
										const QList<DJPoker>& shun2 );
		virtual DJPoker higherSerialPoker( const DJPoker& keyPoker, bool isStrict ) const;
		virtual DJPoker lowerSerialPoker( const DJPoker& keyPoker, bool isStrict ) const;
		virtual DJPoker lowestSerialPoker() const;
		*/
	public slots:
		void ClickPass();
	protected:
		virtual bool handleItemClicked(Qt::MouseButton btn,const QPoint& pos,Q3CanvasItem *item);
		virtual void handleMatrixChanged( const QMatrix& matrix );
	protected slots :
		void PlayerStarted();
		void ClickTip();
		void ClickThrow();
		void ClickArrange();
	private :
		ChuddCurrent c_current;
		ChuddPanel * c_ChuddPanel;

#ifdef DJ_OEM
		DJToolButton 	*m_arrange;
		QPushButton		*m_throw,*m_pass,*m_tip;
#else	
		QWidget			*m_toolbar;
		QToolButton 	*m_throw,*m_arrange,*m_pass,*m_tip;
#endif		
		quint8			c_chArrangeStyle;
	
		quint32  		c_Score;
	
		quint8			c_chThrow[5][15];
		quint8			c_chThrowPages[5];
		quint8			c_chLastThrowSeat;
	
		ChuddRuleCardType	m_tipOtherType,m_tipSelfType;
		QByteArray			m_tipHandCards;
		QList<bool>			m_tipHandIndexes;
		unsigned char		m_tipSelectedCards[15];
};

class  ChuddPanel : public DJGamePanel
{
	Q_OBJECT
	public : 
		ChuddPanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent = 0, Qt::WFlags f = Qt::Window);
        ~ChuddPanel();
};


#endif
