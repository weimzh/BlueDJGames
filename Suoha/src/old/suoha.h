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

#ifndef __DJGAME_CLIENT_SUOHA_H__
#define __DJGAME_CLIENT_SUOHA_H__

#include<QtGui>
#include <Qt3Support>

#include "DJGamePanel.h"
#include "DJTreeWidget.h"
#include "DJGamePokerDesktop.h"
#include "DJGameImage.h"

#include "protocol.h"
#include "shprofile.h"


class SuohaPanel;
class DJOnceSprite;
class DJMatrixCanvasText;	

#define DJGAME_POKERDESKTOP_TYPE_HANDCOUNTER			0x101
#define DJGAME_POKERDESKTOP_TYPE_TABLECOUNTER			0x102
#define DJGAME_POKERDESKTOP_TYPE_MASTER			0x103
#define DJGAME_POKERDESKTOP_TYPE_TABLE			0x110
#define DJGAME_POKERDESKTOP_TYPE_MINMAX			0x111
#define DJGAME_POKERDESKTOP_TYPE_NUMBER			0x112

class SuohaDesktop : public DJGamePokerDesktop
{
	Q_OBJECT
	public :
		SuohaDesktop(QWidget* parent,SuohaPanel *panel);
		~SuohaDesktop();
	
		virtual void userArrived( quint32 userId, quint8 seatId );
        virtual void gameInfo( const unsigned char *buf );
		virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
		virtual void gameTrace( const GeneralGameTrace2Head *gameTrace );
		void RepaintCurrentStatus();
	
		void StaticInitDesktop();
		void StaticGameTrace(GeneralGameTrace2Head *gameTrace);
		
		void RepaintHandCounter();
		void GetPageInterval(quint8 chView,quint8 *pcards,quint8 chPages,int *dx,int *dy);
	
	void RepaintView1TableCounter(quint8 seat);
	void RepaintView2TableCounter(quint8 seat);
	void RepaintView3TableCounter(quint8 seat);
	void RepaintView4TableCounter(quint8 seat);
	QImage GetCounterImage();
	QRect GetTableCounterRect();
	void RepaintTableCounter(quint8 seat);
	
	void PlayerBet(unsigned char chCounters);
	
	unsigned char GetSeatCards(quint8 seat,quint8 *pcards,quint8 chSize);
	virtual QString	playerItemNameSuffix( DJGameUser* user );
	virtual void repaintHost();
	virtual quint8	numberOfPlayers() const;
	virtual QList<quint8> seatsOfPlayers() const;
	protected:
		virtual void handleMatrixChanged( const QMatrix& matrix );
		virtual void contentsMousePressEvent(QMouseEvent* e);
		virtual void contentsMouseReleaseEvent( QMouseEvent * e );
	protected slots :
		void PlayerStarted();
		//void HandleLayoutButtons(const QMatrix &m);
		void ClickAway();
		void ClickAddup();
		void ClickMost();
		void ClickKeep();
	private :
		SuohaCurrent c_current;
		SuohaPanel * c_SuohaPanel;
		unsigned char  c_chSteps;
	DJGameImageItem	*c_CasinoTable;
	
		quint8			c_PlayerCards[DJGAME_SUOHA_MAX_PLAYERS+1][SUOHA_PLAYER_MAX_CARDS+1];
		qint16			c_PlayerCounters[DJGAME_SUOHA_MAX_PLAYERS+1];
		qint16			c_PlayerPreCounters[DJGAME_SUOHA_MAX_PLAYERS+1];
		quint8			c_HandCounters;
		quint16			c_CurrentCounters;
	
		quint8			c_PlayerStatus[DJGAME_SUOHA_MAX_PLAYERS+1];
	
		QPushButton *c_pbAway,*c_pbMost,*c_pbAddup,*c_pbKeep;
		//DJGameTextItem 	*c_itemMin,*c_itemMax;
		DJMatrixCanvasText	*c_itemMin,*c_itemMax;
		DJGameTextItem	*c_itemCounterNumber[DJGAME_SUOHA_MAX_PLAYERS+1];
		bool			m_firstIsBack;
};


class  SuohaPanel : public DJGamePanel
{
	Q_OBJECT
	public : 
		SuohaPanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent = 0, Qt::WFlags f = Qt::Window);
        ~SuohaPanel();
};




#endif
