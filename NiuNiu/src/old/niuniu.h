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

#ifndef __DJGAME_CLIENT_NIUNIU_H__
#define __DJGAME_CLIENT_NIUNIU_H__

#include<QtGui>
#include <Qt3Support>

#include "DJGamePanel.h"
#include "DJTreeWidget.h"
#include "DJGamePokerDesktop.h"
#include "DJGameImage.h"

#include "protocol.h"
#include "nnprofile.h"


class NiuniuPanel;
class DJOnceSprite;
class DJMatrixCanvasText;	

#define DJGAME_POKERDESKTOP_TYPE_HANDCOUNTER			0x101
#define DJGAME_POKERDESKTOP_TYPE_TABLECOUNTER			0x102
#define DJGAME_POKERDESKTOP_TYPE_MASTER			0x103
#define DJGAME_POKERDESKTOP_TYPE_TABLE			0x110
#define DJGAME_POKERDESKTOP_TYPE_MINMAX			0x111
#define DJGAME_POKERDESKTOP_TYPE_NUMBER			0x112

class NiuniuDesktop : public DJGamePokerDesktop
{
	Q_OBJECT
	public :
		NiuniuDesktop(QWidget* parent,NiuniuPanel *panel);
		~NiuniuDesktop();
	
		//virtual void lookerIn( quint32 userId, bool in );
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
	
		unsigned char GetSeatCards(quint8 seat,quint8 *pcards,quint8 chSize);
		
		virtual QString	playerItemNameSuffix( DJGameUser* user );
		virtual void repaintHost();
		virtual quint8	numberOfPlayers() const;
		virtual QList<quint8> seatsOfPlayers() const;
	protected:
		virtual void handleMatrixChanged( const QMatrix& matrix );
	protected slots :
		void PlayerStarted();
		void ClickAway();
		void ClickAddup();
		void ClickDouble();
		void ClickFollow();
	private :
		NiuniuCurrent c_current;
		NiuniuPanel * c_NiuniuPanel;

		quint8			c_PlayerCards[DJGAME_NIUNIU_MAX_PLAYERS+1][NIUNIU_PLAYER_MAX_CARDS+1];
		qint16			c_PlayerCounters[DJGAME_NIUNIU_MAX_PLAYERS+1];
		qint16			c_PlayerSelfCounters[DJGAME_NIUNIU_MAX_PLAYERS+1];
		quint8			c_HandCounters;
		DJGameImageItem	*c_CasinoTable;
	
		quint8			c_PlayerStatus[DJGAME_NIUNIU_MAX_PLAYERS+1];
	
		QPushButton *c_pbFollow,*c_pbAway,*c_pbAddup,*c_pbDouble;
		//DJGameTextItem 	*c_itemMin,*c_itemMax;
		DJMatrixCanvasText	*c_itemMin,*c_itemMax;
		DJGameTextItem	*c_itemCounterNumber[DJGAME_NIUNIU_MAX_PLAYERS+1];
};


class  NiuniuPanel : public DJGamePanel
{
	Q_OBJECT
	public : 
		NiuniuPanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent = 0, Qt::WFlags f = Qt::Window);
        ~NiuniuPanel();
};




#endif
