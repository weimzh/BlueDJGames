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

#include "DJGameController.h"
#include "DJClientRequest.h"
#include "DJGameUser.h"
#include "DJTreeWidget.h"
#include "jdebug.h"
#include "Utility.h"
#include "DJTreeItemDelegate.h"
#include "DJGameRoom.h"
#include "blackjack.h"
#include "bjprofile.h"
#include "jdebug.h"
#include "DJOnceSprite.h"
#include "DJGamePlayerItem.h" 
#include "DJGamePokerDesktop.h"
#include "DJMatrixCanvasText.h"
#include "DJMatrixCanvasImage.h"
#include "DJToolButton.h"
#include "DJGlobal.h"
 
#define BLACKJACK_DESKTOP_COUNTER_LAYOUT_SIZE		25

BlackjackDesktop::BlackjackDesktop(QWidget* parent,BlackjackPanel *panel)
					: DJGamePokerDesktop(parent,panel)
{
	memset(&c_current,0,sizeof(BlackjackCurrent));
	
	SetCardPictureSize(centerSize(),popupSize(),30,pageLayoutSizeY());
	c_BlackjackPanel = panel;
	connect(this,SIGNAL(gameStarted()),this,SLOT(PlayerStarted()));
	
	c_pbStand = new QPushButton(this);
    //c_pbStand->setText(tr("Stand"));
    QPixmap	pix = QPixmap(":/BaseRes/image/desktop/pushbutton/TingShou.png");
	c_pbStand->setIcon( pix );
	c_pbStand->setIconSize( pix.size() );
	c_pbStand->adjustSize();
	c_pbStand->hide();
	connect(c_pbStand,SIGNAL(clicked()),this,SLOT(ClickStand()));
	
	c_pbDouble = new QPushButton(this);
    //c_pbDouble->setText(tr("Double"));
    pix = QPixmap(":/BaseRes/image/desktop/pushbutton/JiaBei.png");
	c_pbDouble->setIcon( pix );
	c_pbDouble->setIconSize( pix.size() );
	c_pbDouble->adjustSize();
	c_pbDouble->hide();
	connect(c_pbDouble,SIGNAL(clicked()),this,SLOT(ClickDouble()));
	
	c_pbDraw = new QPushButton(this);
    //c_pbDraw->setText(tr("Draw"));
    pix = QPixmap(":/BaseRes/image/desktop/pushbutton/ZhuaPai.png");
	c_pbDraw->setIcon( pix );
	c_pbDraw->setIconSize( pix.size() );
	c_pbDraw->adjustSize();
	c_pbDraw->hide();
	connect(c_pbDraw,SIGNAL(clicked()),this,SLOT(ClickDraw()));
	
	QMatrix m = GetCurrentMatrix();
	
	pix = QPixmap(QString(":/BlackjackRes/image/table1.png"));
	QImage img = pix.convertToImage();
	c_CasinoTable = new DJGameImageItem(img,canvas(),m,1,1,DJGAME_POKERDESKTOP_TYPE_TABLE,false);
	c_CasinoTable->setMatrix(m);
	c_CasinoTable->setHAlignment(Qt::AlignHCenter);
	c_CasinoTable->setVAlignment(Qt::AlignVCenter);
	c_CasinoTable->move(GetRealWidth()/2,GetRealHeight()/2);
	c_CasinoTable->setZ(10);
	c_CasinoTable->show();
	
	pix = QPixmap(QString(":/BlackjackRes/image/csnfloor1.png"));
	canvas()->setBackgroundPixmap(pix);

	DJGameRoom *room = c_BlackjackPanel->gameRoom();
	PBlackjackRoom  pbjroom = (PBlackjackRoom)room->privateRoom();
	
	/*
	QString strMin = tr("Min: ");
	strMin += QString("%1").arg(pbjroom->uChipFaceValue * pbjroom->chNormalChips);
	c_itemMin = new DJGameTextItem(strMin,canvas(),m,1,1,DJGAME_POKERDESKTOP_TYPE_MINMAX,false);
	QFont f = c_itemMin->font();
	f.setBold(true);
	f.setPointSize(12);
	c_itemMin->setFont(f);
	c_itemMin->setZ(100);
	c_itemMin->setMatrix(m);
	c_itemMin->move(GetRealWidth()/2-177,GetRealHeight()/2-245);
	c_itemMin->show();
	
	QString strMax = tr("Max: ");
	strMax += QString("%1").arg(pbjroom->uGameMaxValues);
	c_itemMax = new DJGameTextItem(strMax,canvas(),m,1,1,DJGAME_POKERDESKTOP_TYPE_MINMAX,false);
	c_itemMax->setFont(f);
	c_itemMax->setZ(100);
	c_itemMax->setMatrix(m);
	c_itemMax->move(GetRealWidth()/2-177,GetRealHeight()/2-225);
	c_itemMax->show();
	*/
	
	QFont f;
	f.setBold(true);
	f.setPointSize(12);
	
	QString strMin = tr("Min: ");
	strMin += QString("%1").arg(letoh4(pbjroom->uChipFaceValue) * pbjroom->chNormalChips);
	c_itemMin = new DJMatrixCanvasText(canvas(),strMin);
	c_itemMin->setFont(f);
	c_itemMin->setZ(100);
	c_itemMin->matrimove(GetRealWidth()/2-177,GetRealHeight()/2-245);
	c_itemMin->show();
	
	QString strMax = tr("Max: ");
	strMax += QString("%1").arg(letoh4(pbjroom->uGameMaxValues));
	c_itemMax = new DJMatrixCanvasText(canvas(),strMax);
	c_itemMax->setFont(f);
	c_itemMax->setZ(100);
	c_itemMax->matrimove(GetRealWidth()/2-177,GetRealHeight()/2-225);
	c_itemMax->show();
	
	f.setPointSize(22);
	QString strNumber=QString("0");
	for(int i=1;i<=DJGAME_BLACKJACK_MAX_PLAYERS;i++)
	{
		c_itemCounterNumber[i] = new DJGameTextItem(strNumber,canvas(),m,1,1,DJGAME_POKERDESKTOP_TYPE_NUMBER,false);
		c_itemCounterNumber[i]->setColor(QColor(0xFF,0,0));
		c_itemCounterNumber[i]->setZ(2000);
		c_itemCounterNumber[i]->setFont(f);
		c_itemCounterNumber[i]->setMatrix(m);
		c_itemCounterNumber[i]->hide();
	}
	for ( quint8 seat = 1; seat <= panel->numberOfSeats(); ++seat) {
		DJMatrixCanvasText* name	= nameItem( seat );
		name->setColor( Qt::yellow );
	}
	setSelectRectEnabled( false );
	
	setHostEnabled( true );
	DJMatrixCanvasImage* host	= hostItem();
	host->setOriginalPixmap( QPixmap(":/BlackjackRes/image/master.png") );
	
	StaticInitDesktop();
}
BlackjackDesktop::~BlackjackDesktop()
{
}

void BlackjackDesktop::ClickStand()
{
	SendGameTrace(BLACKJACK_GAMETRACE_STOP);
	c_pbStand->hide();
	c_pbDouble->hide();
	c_pbDraw->hide();
}

void BlackjackDesktop::ClickDouble()
{
	SendGameTrace(BLACKJACK_GAMETRACE_BET);
	c_pbStand->hide();
	c_pbDouble->hide();
	c_pbDraw->hide();
}

void BlackjackDesktop::ClickDraw()
{
	SendGameTrace(BLACKJACK_GAMETRACE_DRAW);
	c_pbStand->hide();
	c_pbDouble->hide();
	c_pbDraw->hide();
}
QString	BlackjackDesktop::playerItemNameSuffix( DJGameUser* user )
{
	if ( c_current.chDealer && c_current.chDealer == user->seatId() )
		return tr("(dealer)");
	else
		return QString();
}
void BlackjackDesktop::repaintHost()
{
	repaintHostItem( c_current.chDealer );
}
quint8 BlackjackDesktop::numberOfPlayers() const
{
	quint8 players	= c_current.chPlayers;
	if ( 0 != players )
		return players;
	else
		return DJGameDesktop::numberOfPlayers();
}
QList<quint8> BlackjackDesktop::seatsOfPlayers() const
{
	quint8 players	= c_current.chPlayers;
	if ( 0 != players ) {
		QList<quint8>	seatIds;
		for ( int i = 1; i <= players; ++i ) {
			seatIds << c_current.chMapSites[i];
		}
		return seatIds;
	}else {
		return DJGameDesktop::seatsOfPlayers();
	}	
}
void BlackjackDesktop::userArrived( quint32 userId, quint8 seatId )
{
	djDebug()<<"BlackjackDesktop::userArrived"<<userId<<seatId;
	DJGamePokerDesktop::userArrived( userId, seatId );
	
	RepaintCurrentStatus();
}
void BlackjackDesktop::gameInfo( const unsigned char *buf )
{
	memcpy(&c_current,buf,sizeof(BlackjackCurrent));
	StaticInitDesktop();
}

void BlackjackDesktop::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	DJGameDesktop::gameWait(mask,status,timeout);
	switch(status)
	{
		case DJGAME_BLACKJACK_TABLE_STATUS_WAIT_PLAYER :
		{
			c_pbStand->hide();
			c_pbDouble->hide();
			c_pbDraw->hide();
			if(IsWaittingForMe())
			{
				c_pbDraw->show();
				c_pbStand->show();
				if(c_BlackjackPanel->selfSeatId() != c_current.chDealer && 
							c_PlayerDoubles[c_BlackjackPanel->selfSeatId()] > 0)
					c_pbDouble->show();
			}
			break;
		}
	}
			
}
void BlackjackDesktop::gameTrace( const GeneralGameTrace2Head *gameTrace )
{
	GeneralGameTrace2Head *ptrace = (GeneralGameTrace2Head *)gameTrace;
	DJGameDesktop::gameTrace(gameTrace);
	StaticGameTrace(ptrace);
	switch(gameTrace->chType)
	{
		case BLACKJACK_GAMETRACE_PICKUP :
		case BLACKJACK_GAMETRACE_PICKUP2 :
		case BLACKJACK_GAMETRACE_SHOWCARDS :
			RepaintSeatCard(gameTrace->chSite);
			break;
		case BLACKJACK_GAMETRACE_BET :
		{
			RepaintTableCounter(gameTrace->chSite);
			RepaintTableCounter(c_current.chDealer);
			c_pbStand->hide();	//
			c_pbDouble->hide();
			c_pbDraw->hide();
			break;
		}

		case BLACKJACK_GAMETRACE_RESULT :
		{
			RepaintTableCounter(gameTrace->chSite);
			RepaintTableCounter(c_current.chDealer);
			c_pbStand->hide();	//
			c_pbDouble->hide();
			c_pbDraw->hide();
			break;
		}
	}
}
void BlackjackDesktop::RepaintCurrentStatus()
{
	for(int i=1;i<=4;i++)
	{
		RepaintSeatCard(i);
		RepaintTableCounter(i);
	}
}

QImage BlackjackDesktop::GetCounterImage()
{
	DJGameRoom *room = c_BlackjackPanel->gameRoom();
	QPixmap pix = QPixmap(QString(":/BlackjackRes/image/cm_%1.png").arg(((room->roomId()+1)%3)+1));
	QImage img = pix.convertToImage();
	return img;
}


void BlackjackDesktop::RepaintTableCounter(quint8 seat)
{
	quint8 chView = MapSeat2View(seat);
	switch(chView)
	{
		case 1 :
			RepaintView1TableCounter(seat);
			break;
		case 2 :
			RepaintView2TableCounter(seat);
			break;
		case 3 :
			RepaintView3TableCounter(seat);
			break;
		case 4 :
			RepaintView4TableCounter(seat);
			break;
	}
}

QRect BlackjackDesktop::GetTableCounterRect()
{
	QRect rt;
	QImage img2 = GetCardPicture(DJGAME_POKER_BACKGROUND,2,getOtherFx());
	QImage img3 = GetCardPicture(DJGAME_POKER_BACKGROUND,3,getOtherFx());
	rt.setTop(DJGAME_POKER_DESKTOP_AVATAR_MAXHEIGHT+10+img3.height()+10);
	rt.setLeft(DJGAME_POKER_DESKTOP_AVATAR_MAXWIDTH+10+img2.width()+10);
	int x,y,sh;
	GetDesktopCenterPoint(&x,&y,&sh);
	rt.setWidth((x-rt.left())*2);
	rt.setHeight((y-rt.top())*2);
	return rt;
}

void BlackjackDesktop::RepaintView1TableCounter(quint8 seat)
{
	ClearImage(0,seat,DJGAME_POKERDESKTOP_TYPE_TABLECOUNTER);
	if(c_PlayerCounters[seat] <= 0)
		return ;
	QImage img = GetCounterImage();
	QRect rt = GetTableCounterRect();
	int cx,cy,sh,x,y,dx,lx = BLACKJACK_DESKTOP_COUNTER_LAYOUT_SIZE;
	GetDesktopCenterPoint(&cx,&cy,&sh);
	QMatrix m = GetCurrentMatrix();
	if(c_PlayerCounters[seat] > 0)
	{
		DJGameRoom *room = c_BlackjackPanel->gameRoom();
		PBlackjackRoom  pbjroom = (PBlackjackRoom)room->privateRoom();
		c_itemCounterNumber[1]->setText(QString("%1").arg(letoh4(pbjroom->uChipFaceValue)*c_PlayerCounters[seat]));
		c_itemCounterNumber[1]->setHAlignment(Qt::AlignHCenter);
		c_itemCounterNumber[1]->move(cx,rt.bottom()-img.height()+20);
		c_itemCounterNumber[1]->setMatrix(m);
		c_itemCounterNumber[1]->show();
	}else
		c_itemCounterNumber[1]->hide();
	
	int w = lx*(c_PlayerCounters[seat]-1) + img.width();
	while(w > rt.width() && lx>5)
	{
		lx --;
		w = lx*(c_PlayerCounters[seat]-1) + img.width();
	}
		
	x = cx-w/2;
	if(x<rt.left()+img.width())
		x = rt.left()+img.width();
	dx = x;
	y = rt.bottom()-img.height();
	DJGamePokerItem *item;
	for(int i=0;i<c_PlayerCounters[seat];i++)
	{
		item = new DJGamePokerItem(img,canvas(),m,i+1,seat,DJGAME_POKERDESKTOP_TYPE_TABLECOUNTER,false);
		item->setZ(1200+i);
		item->move(x,y);
		item->show();
		item->setMatrix(m);
		x += lx;
		if(x+img.width()>rt.right())
		{
			x = dx;
			y -= BLACKJACK_DESKTOP_COUNTER_LAYOUT_SIZE;
		}
		
	}
		
	if(c_PlayerStatus[seat] != 0)
	{//画标志
		// c_PlayerStatus[seat] == 1 投降
		//c_PlayerStatus[seat] == 2 胜利
		;
	}
	
}
void BlackjackDesktop::RepaintView2TableCounter(quint8 seat)
{
	ClearImage(0,seat,DJGAME_POKERDESKTOP_TYPE_TABLECOUNTER);
	if(c_PlayerCounters[seat] <= 0)
		return ;
	QImage img = GetCounterImage();
	QRect rt = GetTableCounterRect();
	int cx,cy,sh,x,y,dy,lx = BLACKJACK_DESKTOP_COUNTER_LAYOUT_SIZE;
	GetDesktopCenterPoint(&cx,&cy,&sh);
	QMatrix m = GetCurrentMatrix();
	if(c_PlayerCounters[seat] > 0)
	{
		DJGameRoom *room = c_BlackjackPanel->gameRoom();
		PBlackjackRoom  pbjroom = (PBlackjackRoom)room->privateRoom();
		c_itemCounterNumber[2]->setText(QString("%1").arg(letoh4(pbjroom->uChipFaceValue)*c_PlayerCounters[seat]));
		c_itemCounterNumber[2]->setVAlignment(Qt::AlignVCenter);
		c_itemCounterNumber[2]->move(rt.left()+20,cy);
		c_itemCounterNumber[2]->setMatrix(m);
		c_itemCounterNumber[2]->show();
	}else
		c_itemCounterNumber[2]->hide();
	int w = lx*(c_PlayerCounters[seat]-1) + img.width();
	while(w > rt.height() && lx>5)
	{
		lx --;
		w = lx*(c_PlayerCounters[seat]-1) + img.width();
	}
		
	x = rt.left();
	y = cy-w/2;
	if(y < rt.top())
		y = rt.top();
	dy = y;
	DJGamePokerItem *item;
	
	for(int i=0;i<c_PlayerCounters[seat];i++)
	{
		item = new DJGamePokerItem(img,canvas(),m,i+1,seat,DJGAME_POKERDESKTOP_TYPE_TABLECOUNTER,false);
		item->setZ(1200+i);
		item->move(x,y);
		item->show();
		item->setMatrix(m);
		y += lx;
		if(y+img.width()>rt.bottom())
		{
			y = dy;
			x += BLACKJACK_DESKTOP_COUNTER_LAYOUT_SIZE;
		}
		
	}
	if(c_PlayerStatus[seat] != 0)
	{//画标志
		// c_PlayerStatus[seat] == 1 投降
		//c_PlayerStatus[seat] == 2 胜利
		;
	}

}
void BlackjackDesktop::RepaintView3TableCounter(quint8 seat)
{
	ClearImage(0,seat,DJGAME_POKERDESKTOP_TYPE_TABLECOUNTER);
	if(c_PlayerCounters[seat] <= 0)
		return ;
	QImage img = GetCounterImage();
	QRect rt = GetTableCounterRect();
	int cx,cy,sh,x,y,dx,lx = BLACKJACK_DESKTOP_COUNTER_LAYOUT_SIZE;
	GetDesktopCenterPoint(&cx,&cy,&sh);
	QMatrix m = GetCurrentMatrix();
	if(c_PlayerCounters[seat] > 0)
	{
		DJGameRoom *room = c_BlackjackPanel->gameRoom();
		PBlackjackRoom  pbjroom = (PBlackjackRoom)room->privateRoom();
		c_itemCounterNumber[3]->setText(QString("%1").arg(letoh4(pbjroom->uChipFaceValue)*c_PlayerCounters[seat]));
		c_itemCounterNumber[3]->setHAlignment(Qt::AlignHCenter);
		c_itemCounterNumber[3]->move(cx,rt.top()+20);
		c_itemCounterNumber[3]->setMatrix(m);
		c_itemCounterNumber[3]->show();
	}else
		c_itemCounterNumber[3]->hide();
	int w = lx*(c_PlayerCounters[seat]-1)+ img.width();
	while(w > rt.width() && lx>5)
	{
		lx --;
		w = lx*(c_PlayerCounters[seat]-1)+ img.width();
	}
		
	x = cx+w/2-img.width();
	if(x>rt.right()-img.width())
		x = rt.right()-img.width();
	dx = x;
	y = rt.top();
	DJGamePokerItem *item;
	
	for(int i=0;i<c_PlayerCounters[seat];i++)
	{
		item = new DJGamePokerItem(img,canvas(),m,i+1,seat,DJGAME_POKERDESKTOP_TYPE_TABLECOUNTER,false);
		item->setZ(1200+i);
		item->move(x,y);
		item->show();
		item->setMatrix(m);
		x -= lx;
		if(x<rt.left())
		{
			x = dx;
			y += BLACKJACK_DESKTOP_COUNTER_LAYOUT_SIZE;
		}
		
	}
	if(c_PlayerStatus[seat] != 0)
	{//画标志
		// c_PlayerStatus[seat] == 1 投降
		//c_PlayerStatus[seat] == 2 胜利
		;
	}
}
void BlackjackDesktop::RepaintView4TableCounter(quint8 seat)
{
	ClearImage(0,seat,DJGAME_POKERDESKTOP_TYPE_TABLECOUNTER);
	if(c_PlayerCounters[seat] <= 0)
		return ;
	QImage img = GetCounterImage();
	QRect rt = GetTableCounterRect();
	int cx,cy,sh,x,y,dy,lx = BLACKJACK_DESKTOP_COUNTER_LAYOUT_SIZE;
	GetDesktopCenterPoint(&cx,&cy,&sh);
	QMatrix m = GetCurrentMatrix();
	if(c_PlayerCounters[seat] > 0)
	{
		DJGameRoom *room = c_BlackjackPanel->gameRoom();
		PBlackjackRoom  pbjroom = (PBlackjackRoom)room->privateRoom();
		c_itemCounterNumber[4]->setText(QString("%1").arg(letoh4(pbjroom->uChipFaceValue)*c_PlayerCounters[seat]));
		c_itemCounterNumber[4]->setVAlignment(Qt::AlignVCenter);
		c_itemCounterNumber[4]->move(rt.right()-GetTextItemWidth(c_itemCounterNumber[4])-20,cy);
		c_itemCounterNumber[4]->setMatrix(m);
		c_itemCounterNumber[4]->show();
	}else
		c_itemCounterNumber[4]->hide();
	int w = lx*(c_PlayerCounters[seat]-1) + img.width();
	while(w > rt.height() && lx>5)
	{
		lx --;
		w = lx*(c_PlayerCounters[seat]-1) + img.width();
	}
		
	x = rt.right()-img.width();
	y = cy+w/2-img.width();
	if(y+img.width() > rt.bottom())
		y = rt.bottom()-img.width();
	dy = y;
	DJGamePokerItem *item;
	
	for(int i=0;i<c_PlayerCounters[seat];i++)
	{
		item = new DJGamePokerItem(img,canvas(),m,i+1,seat,DJGAME_POKERDESKTOP_TYPE_TABLECOUNTER,false);
		item->setZ(1200+i);
		item->move(x,y);
		item->show();
		item->setMatrix(m);
		y -= lx;
		if(y<rt.top())
		{
			y = dy;
			x -= BLACKJACK_DESKTOP_COUNTER_LAYOUT_SIZE;
		}
		
	}
	if(c_PlayerStatus[seat] != 0)
	{//画标志
		// c_PlayerStatus[seat] == 1 投降
		//c_PlayerStatus[seat] == 2 胜利
		;
	}
}

void BlackjackDesktop::StaticInitDesktop()
{
	for(int i=1;i<=DJGAME_BLACKJACK_MAX_PLAYERS;i++)
	{
		c_itemCounterNumber[i]->hide();
	}
	memset(c_PlayerCards,0,(DJGAME_BLACKJACK_MAX_PLAYERS+1)*(BLACKJACK_PLAYER_MAX_CARDS+1));
	DJGameRoom *room = c_BlackjackPanel->gameRoom();
	PBlackjackRoom  pbjroom = (PBlackjackRoom)room->privateRoom();
	
	memset(c_PlayerCounters,0,(DJGAME_BLACKJACK_MAX_PLAYERS+1)*sizeof(qint16));
	memset(c_PlayerStatus,0,DJGAME_BLACKJACK_MAX_PLAYERS+1);

	for(int i=1;i<=4;i++)
	{
		ResetPlayerCards(i,NULL,0);
		c_PlayerDoubles[i] = pbjroom->chDouble+1;
	}
	
	RepaintCurrentStatus();
}
void BlackjackDesktop::StaticGameTrace(GeneralGameTrace2Head *gameTrace)
{
	PBlackjackPickup ppickup;
	PBlackjackShowCards pshows;
	switch(gameTrace->chType)
	{
		case BLACKJACK_GAMETRACE_PICKUP :
		case BLACKJACK_GAMETRACE_PICKUP2 :
			ppickup = (PBlackjackPickup)gameTrace->chBuf;
			if(ppickup->chCardNumber >0 && ppickup->chCardNumber <=BLACKJACK_PLAYER_MAX_CARDS)
			{
				c_PlayerCards[gameTrace->chSite][ppickup->chCardNumber-1] = ppickup->chCard;
				ResetPlayerCards(gameTrace->chSite,c_PlayerCards[gameTrace->chSite],ppickup->chCardNumber);
			}
			break;
		case BLACKJACK_GAMETRACE_SHOWCARDS :
			pshows = (PBlackjackShowCards)gameTrace->chBuf;
			if(pshows->chCards >0 && pshows->chCards <=BLACKJACK_PLAYER_MAX_CARDS)
			{
				memcpy(c_PlayerCards[gameTrace->chSite],pshows->chCard, pshows->chCards);
				ResetPlayerCards(gameTrace->chSite,c_PlayerCards[gameTrace->chSite],pshows->chCards);
			}
			break;
		case BLACKJACK_GAMETRACE_BET :
		{
			DJGameRoom *room = c_BlackjackPanel->gameRoom();
			PBlackjackRoom  pbjroom = (PBlackjackRoom)room->privateRoom();
			c_PlayerCounters[gameTrace->chSite] += pbjroom->chNormalChips;
			c_PlayerCounters[c_current.chDealer] += pbjroom->chNormalChips;
			c_PlayerDoubles[gameTrace->chSite] --;
			break;
		}
		case BLACKJACK_GAMETRACE_RESULT :
		{
			PBlackjackResult presult = (PBlackjackResult)gameTrace->chBuf;
			djDebug()<<"BLACKJACK_GAMETRACE_RESULT";
			djDebug()<<"site"<<gameTrace->chSite<<"chips"<<presult->chChips;
			PBlackjackRoom  pbjroom = (PBlackjackRoom)c_BlackjackPanel->gameRoom()->privateRoom();
			int value	= presult->chChips * letoh4(pbjroom->uChipFaceValue);
			djDebug()<<"value"<<value;
			DJGameUser	*player	= c_BlackjackPanel->userAtSeat(gameTrace->chSite);
			DJGameUser	*dealer	= c_BlackjackPanel->userAtSeat(c_current.chDealer);
			if( BLACKJACK_GAMERESULT_WIN == presult->chResult )
			{
				djDebug()<<"win";
				
				c_PlayerCounters[gameTrace->chSite] = presult->chChips*2;
				c_PlayerCounters[c_current.chDealer] -= presult->chChips;
				c_BlackjackPanel->deltaUserData( player, value );
				c_BlackjackPanel->deltaUserData( dealer, -value );
				
				QString result;
				if ( dealer ) {
					result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( dealer->userName() + tr("(dealer)") )
								.arg( -value );
					//QString text	= dealer->userName()+ QString(" : %1").arg(-value);
					//c_BlackjackPanel->insertText2Browser( text );
				}
				if ( player ) {
					result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( player->userName() )
								.arg( value );
					//QString text	= player->userName() + QString(" : %1").arg(value);
					//c_BlackjackPanel->insertText2Browser( text );
				}
				c_BlackjackPanel->insertGameResult2Browser( result );
			}else if( BLACKJACK_GAMERESULT_LOST == presult->chResult )
			{
				djDebug()<<"lose";
				c_PlayerCounters[gameTrace->chSite] = 0;
				c_PlayerCounters[c_current.chDealer] += presult->chChips;
				c_BlackjackPanel->deltaUserData( player, -value );
				c_BlackjackPanel->deltaUserData( dealer, value );
				
				QString result;
				if ( dealer ) {
					//QString text	= dealer->userName() + QString(" : %1").arg(value);
					//c_BlackjackPanel->insertText2Browser( text );
					result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( dealer->userName() + tr("(dealer)") )
								.arg( value );
				}
				if ( player ) {
					//QString text	= player->userName()+ QString(" : %1").arg(-value);
					//c_BlackjackPanel->insertText2Browser( text );
					result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( player->userName() )
								.arg( -value );
				}
				c_BlackjackPanel->insertGameResult2Browser( result );
			}else if ( BLACKJACK_GAMERESULT_DRAW == presult->chResult ) {
				QString result;
				if ( dealer ) {
					result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( dealer->userName() + tr("(dealer)") )
								.arg( 0 );
				}
				if ( player ) {
					result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( player->userName() )
								.arg( 0 );
				}
				c_BlackjackPanel->insertGameResult2Browser( result );
			}
			break;
		}
	}
}

void BlackjackDesktop::PlayerStarted()
{
	StaticInitDesktop();
}
void BlackjackDesktop::handleMatrixChanged(const QMatrix &m)
{
	DJGamePokerDesktop::handleMatrixChanged(m); 
		
	int	x	= visibleWidth() * 2 / 3;
	int y	= visibleHeight();
	
	int spacing	= 10;
	
	c_pbDraw->move(x - (c_pbDraw->width()>>1),
					y - c_pbDraw->height() - spacing );
					
	c_pbStand->move(x - (c_pbStand->width()>>1),
					c_pbDraw->pos().y() - c_pbStand->height() - spacing );
	
	c_pbDouble->move(x - (c_pbDouble->width()>>1),
					c_pbStand->pos().y() - c_pbDouble->height() - spacing);
	
	c_itemMin->setExternalMatrix(m);
	c_itemMax->setExternalMatrix(m);
}

void BlackjackDesktop::GetPageInterval(quint8 chView,quint8 *pcards,quint8 chPages,int *dx,int *dy)
{
	*dx = 1;
	*dy = 0;
}

unsigned char BlackjackDesktop::GetSeatCards(quint8 seat,quint8 *pcards,quint8 chSize)
{
	unsigned char i;
	for(i=0;i<BLACKJACK_PLAYER_MAX_CARDS;i++)
	{
		if(c_PlayerCards[seat][i] != 0)
			pcards[i] = c_PlayerCards[seat][i];
		else
			break;
	}
	return i;
}

//////////////////////////////////////////////////////////////////
BlackjackPanel::BlackjackPanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f)
			: DJGamePanel( gameController, room, userIds, tableId, seatId, parent, f )
{	
	QSplitter* mainSplitter = new QSplitter(Qt::Horizontal , this);
	setCentralWidget( mainSplitter );
	QSplitter	*accessorySplitter, *gameSplitter;
	 	
	if ( 0 == layoutMode() ) {
		gameSplitter	= new QSplitter( Qt::Vertical , mainSplitter );
		accessorySplitter 	= new QSplitter( Qt::Vertical , mainSplitter );
	}else {
		accessorySplitter	= new QSplitter( Qt::Vertical , mainSplitter );
		gameSplitter 	= new QSplitter( Qt::Vertical , mainSplitter );
	}
	
	createPanelBar( gameSplitter );
	BlackjackDesktop	*gameDesktop = new BlackjackDesktop(gameSplitter,this);
	setAccessorySplitter( accessorySplitter );
	setGameDesktop( gameDesktop );
	color()->hide();
}
BlackjackPanel::~BlackjackPanel()
{
}
