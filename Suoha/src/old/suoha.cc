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
#include "suoha.h"
#include "shprofile.h"
#include "jdebug.h"
#include "DJOnceSprite.h"
#include "DJGamePlayerItem.h" 
#include "DJGamePokerDesktop.h"
#include "DJGameUserContext.h"
#include "DJMatrixCanvasText.h"
#include "DJMatrixCanvasImage.h"
#include "DJToolButton.h"
#include "DJGlobal.h" 

#define SUOHA_DESKTOP_COUNTER_LAYOUT_SIZE		25

SuohaDesktop::SuohaDesktop(QWidget* parent,SuohaPanel *panel)
					: DJGamePokerDesktop(parent,panel)
{
	memset(&c_current,0,sizeof(SuohaCurrent));
	
	SetCardPictureSize(centerSize(),popupSize(),30,pageLayoutSizeY());
	c_SuohaPanel = panel;
	connect(this,SIGNAL(gameStarted()),this,SLOT(PlayerStarted()));
	
	QSize iconSize( 19 * 2, 19 );
	c_pbAway = new QPushButton(this);
    QPixmap pix = QPixmap(":/BaseRes/image/desktop/pushbutton/FangQi.png");
	c_pbAway->setIcon( pix );
	c_pbAway->setIconSize( iconSize );
	c_pbAway->adjustSize();
	c_pbAway->hide();
	connect(c_pbAway,SIGNAL(clicked()),this,SLOT(ClickAway()));
	
	c_pbKeep = new QPushButton(this);
    pix = QPixmap(":/BaseRes/image/desktop/pushbutton/Gen.png");
	c_pbKeep->setIcon( pix );
	c_pbKeep->setIconSize( iconSize );
	c_pbKeep->adjustSize();
	c_pbKeep->hide();
	connect(c_pbKeep,SIGNAL(clicked()),this,SLOT(ClickKeep()));
	
	c_pbAddup = new QPushButton(this);
    pix = QPixmap(":/BaseRes/image/desktop/pushbutton/JiaZhu.png");
	c_pbAddup->setIcon( pix );
	c_pbAddup->setIconSize( iconSize );
	c_pbAddup->adjustSize();
	c_pbAddup->hide();
	connect(c_pbAddup,SIGNAL(clicked()),this,SLOT(ClickAddup()));
	
	c_pbMost = new QPushButton(this);
    pix = QPixmap(":/BaseRes/image/desktop/pushbutton/SuoLe.png");
	c_pbMost->setIcon( pix );
	c_pbMost->setIconSize( iconSize );
	c_pbMost->adjustSize();
	c_pbMost->hide();
	connect(c_pbMost,SIGNAL(clicked()),this,SLOT(ClickMost()));
	
	QMatrix m = GetCurrentMatrix();
	pix = QPixmap(QString(":/SuohaRes/image/table1.png"));
	QImage img = pix.convertToImage();
	c_CasinoTable = new DJGameImageItem(img,canvas(),m,1,1,DJGAME_POKERDESKTOP_TYPE_TABLE,false);
	c_CasinoTable->setMatrix(m);
	c_CasinoTable->setHAlignment(Qt::AlignHCenter);
	c_CasinoTable->setVAlignment(Qt::AlignVCenter);
	c_CasinoTable->move(GetRealWidth()/2,GetRealHeight()/2);
	c_CasinoTable->setZ(10);
	c_CasinoTable->show();
	
	pix = QPixmap(QString(":/SuohaRes/image/csnfloor1.png"));
	canvas()->setBackgroundPixmap(pix);
	
	DJGameRoom *room = c_SuohaPanel->gameRoom();
	PSuohaRoom  pshroom = (PSuohaRoom)room->privateRoom();
	
	QFont f;
	f.setBold(true);
	f.setPointSize(12);
	
	QString strMin = tr("Min: ");
	strMin += QString("%1").arg(letoh4(pshroom->uMinYazhu));
	c_itemMin = new DJMatrixCanvasText(canvas(),strMin);
	c_itemMin->setFont(f);
	c_itemMin->setZ(100);
	c_itemMin->matrimove(GetRealWidth()/2-187,GetRealHeight()/2-250);
	c_itemMin->show();

	QString strMax = tr("Max: ");
	strMax += QString("%1").arg(letoh4(pshroom->uGameMaxCounter));
	c_itemMax = new DJMatrixCanvasText(canvas(),strMax);
	c_itemMax->setFont(f);
	c_itemMax->setZ(100);
	c_itemMax->matrimove(GetRealWidth()/2-187,GetRealHeight()/2-225);
	c_itemMax->show();
	
	f.setPointSize(22);
	QString strNumber=QString("0");
	for(int i=1;i<=DJGAME_SUOHA_MAX_PLAYERS;i++)
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
	m_firstIsBack	= true;
	
	setHostEnabled( true );
	DJMatrixCanvasImage* host	= hostItem();
	host->setOriginalPixmap( QPixmap(":/SuohaRes/image/master.png") );
	
	StaticInitDesktop();
}
SuohaDesktop::~SuohaDesktop()
{
}

void SuohaDesktop::ClickAway()
{
	PlayerBet(SUOHA_BET_AWAY);
}

void SuohaDesktop::ClickKeep()
{
	PlayerBet(0);
}

void SuohaDesktop::ClickAddup()
{
	DJGameRoom *room = c_SuohaPanel->gameRoom();
	PSuohaRoom  pshroom = (PSuohaRoom)room->privateRoom();
	if(c_CurrentCounters == 0)
	{
		PlayerBet(pshroom->chMaxMulti/2);
	}else
		PlayerBet(pshroom->chMaxMulti);
		
}

void SuohaDesktop::ClickMost()
{
	if(c_chSteps >= 4)
		PlayerBet(SUOHA_BET_MAX);
}

void SuohaDesktop::PlayerBet(unsigned char chCounters)
{
	if(!(c_SuohaPanel->isLookingOn()) && IsWaittingForMe() && GetTableStatus() == DJGAME_SUOHA_TABLE_STATUS_WAIT_BET)
	{
		SuohaBet bet;
		bet.chMapSite = 0;
		bet.chMultiple = chCounters;
		SendGameTrace(SUOHA_GAMETRACE_BET,(char *)&bet,sizeof(SuohaBet));
	}
	c_pbAway->hide();	//
	c_pbAddup->hide();
	c_pbMost->hide();
	c_pbKeep->hide();
}
void SuohaDesktop::userArrived( quint32 userId, quint8 seatId )
{
	djDebug()<<"SuohaDesktop::userArrived"<<userId<<seatId;
	DJGamePokerDesktop::userArrived( userId, seatId );
	
	RepaintCurrentStatus();
}
void SuohaDesktop::gameInfo( const unsigned char *buf )
{
	memcpy(&c_current,buf,sizeof(SuohaCurrent));
	StaticInitDesktop();
}

void SuohaDesktop::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	DJGameDesktop::gameWait(mask,status,timeout);
	switch(status)
	{
		case DJGAME_SUOHA_TABLE_STATUS_WAIT_BET :
		{
			c_pbAway->hide();
			c_pbKeep->hide();
			c_pbAddup->hide();
			c_pbMost->hide();
			if(IsWaittingForMe())
			{
				DJGameRoom *room = c_SuohaPanel->gameRoom();
				PSuohaRoom  pshroom = (PSuohaRoom)room->privateRoom();
				c_pbAway->show();
				c_pbKeep->show();
				if(c_CurrentCounters < pshroom->chMaxMulti)
					c_pbAddup->show();
				if( c_chSteps >= 4 
					&& c_CurrentCounters + c_PlayerPreCounters[c_SuohaPanel->selfSeatId()]
						< (int)letoh2(pshroom->uGameMaxMulti) )
					c_pbMost->show();
			}
			break;
		}
		case DJGAME_TABLE_STATUS_WAITSTART: {
			RepaintCurrentStatus();
			break;
		}
	}
	RepaintSeatCard( c_SuohaPanel->selfSeatId() );		
}
void SuohaDesktop::gameTrace( const GeneralGameTrace2Head *gameTrace )
{
	GeneralGameTrace2Head *ptrace = (GeneralGameTrace2Head *)gameTrace;
	DJGameDesktop::gameTrace(gameTrace);
	StaticGameTrace(ptrace);
	switch(gameTrace->chType)
	{
		case SUOHA_GAMETRACE_PICKUP :
		case SUOHA_GAMETRACE_PICKUP2 :
		case SUOHA_GAMETRACE_SHOWCARDS :
			RepaintSeatCard(gameTrace->chSite);
			break;
		case SUOHA_GAMETRACE_BET :
		{
			PSuohaBet pyazhu = (PSuohaBet)gameTrace->chBuf;
			if(pyazhu->chMultiple > 0)
			{
				RepaintTableCounter(gameTrace->chSite);
				if(gameTrace->chSite == c_SuohaPanel->selfSeatId())
				{
					RepaintHandCounter();
				}
			}
			c_pbAway->hide();	//
			c_pbAddup->hide();
			c_pbMost->hide();
			c_pbKeep->hide();
			break;
		}

		case SUOHA_GAMETRACE_RESULT :
		{
			for(int i=1;i<=4;i++)
			{
				RepaintTableCounter(i);
			}
			break;
			c_pbAway->hide();	//
			c_pbAddup->hide();
			c_pbMost->hide();
			c_pbKeep->hide();
		}
	}
}
void SuohaDesktop::RepaintCurrentStatus()
{
	for(int i=1;i<=4;i++)
	{
		RepaintSeatCard(i);
		RepaintTableCounter(i);
	}
	RepaintHandCounter();
}

QImage SuohaDesktop::GetCounterImage()
{
	DJGameRoom *room = c_SuohaPanel->gameRoom();
	QPixmap pix = QPixmap(QString(":/SuohaRes/image/cm_%1.png").arg(((room->roomId()+1)%3)+1));
	QImage img = pix.convertToImage();
	return img;
}
void SuohaDesktop::RepaintHandCounter()
{
	ClearImage(0,c_SuohaPanel->selfSeatId(),DJGAME_POKERDESKTOP_TYPE_HANDCOUNTER);
	QImage img = GetCounterImage();
	QMatrix m = GetCurrentMatrix();
	DJGamePokerItem *item;
	quint32 x = GetRealWidth();
	quint32 y = GetRealHeight()-img.height()-10;
	
	x -= img.width();
	
	for(int i=0;i<c_HandCounters;i++)
	{
		item = new DJGamePokerItem(img,canvas(),m,i+1,c_SuohaPanel->selfSeatId(),DJGAME_POKERDESKTOP_TYPE_HANDCOUNTER,false);
		item->setZ(1200+i);
		item->move(x,y);
		item->show();
		item->setdy(10);
		item->setMatrix(m);
		x -= SUOHA_DESKTOP_COUNTER_LAYOUT_SIZE;
	}
}

void SuohaDesktop::RepaintTableCounter(quint8 seat)
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

QRect SuohaDesktop::GetTableCounterRect()
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

void SuohaDesktop::RepaintView1TableCounter(quint8 seat)
{
	ClearImage(0,seat,DJGAME_POKERDESKTOP_TYPE_TABLECOUNTER);
	if(c_PlayerCounters[seat] <= 0)
		return ;
	QImage img = GetCounterImage();
	QRect rt = GetTableCounterRect();
	int cx,cy,sh,x,y,dx,lx = SUOHA_DESKTOP_COUNTER_LAYOUT_SIZE;
	GetDesktopCenterPoint(&cx,&cy,&sh);
	QMatrix m = GetCurrentMatrix();
	if(c_PlayerCounters[seat] > 0)
	{
		DJGameRoom *room = c_SuohaPanel->gameRoom();
		PSuohaRoom  pshroom = (PSuohaRoom)room->privateRoom();
		c_itemCounterNumber[1]->setText(QString("%1").arg(letoh4(pshroom->uMinYazhu)*c_PlayerCounters[seat]));
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
			y -= SUOHA_DESKTOP_COUNTER_LAYOUT_SIZE;
		}
		
	}
	if(c_PlayerStatus[seat] != 0)
	{//画标志
		// c_PlayerStatus[seat] == 1 投降
		//c_PlayerStatus[seat] == 2 胜利
		;
	}
	
}
void SuohaDesktop::RepaintView2TableCounter(quint8 seat)
{
	ClearImage(0,seat,DJGAME_POKERDESKTOP_TYPE_TABLECOUNTER);
	if(c_PlayerCounters[seat] <= 0)
		return ;
	QImage img = GetCounterImage();
	QRect rt = GetTableCounterRect();
	int cx,cy,sh,x,y,dy,lx = SUOHA_DESKTOP_COUNTER_LAYOUT_SIZE;
	GetDesktopCenterPoint(&cx,&cy,&sh);
	QMatrix m = GetCurrentMatrix();
	if(c_PlayerCounters[seat] > 0)
	{
		DJGameRoom *room = c_SuohaPanel->gameRoom();
		PSuohaRoom  pshroom = (PSuohaRoom)room->privateRoom();
		c_itemCounterNumber[2]->setText(QString("%1").arg(letoh4(pshroom->uMinYazhu)*c_PlayerCounters[seat]));		c_itemCounterNumber[2]->setVAlignment(Qt::AlignVCenter);
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
			x += SUOHA_DESKTOP_COUNTER_LAYOUT_SIZE;
		}
		
	}
	if(c_PlayerStatus[seat] != 0)
	{//画标志
		// c_PlayerStatus[seat] == 1 投降
		//c_PlayerStatus[seat] == 2 胜利
		;
	}

}
void SuohaDesktop::RepaintView3TableCounter(quint8 seat)
{
	ClearImage(0,seat,DJGAME_POKERDESKTOP_TYPE_TABLECOUNTER);
	if(c_PlayerCounters[seat] <= 0)
		return ;
	QImage img = GetCounterImage();
	QRect rt = GetTableCounterRect();
	int cx,cy,sh,x,y,dx,lx = SUOHA_DESKTOP_COUNTER_LAYOUT_SIZE;
	GetDesktopCenterPoint(&cx,&cy,&sh);
	QMatrix m = GetCurrentMatrix();
	if(c_PlayerCounters[seat] > 0)
	{
		DJGameRoom *room = c_SuohaPanel->gameRoom();
		PSuohaRoom  pshroom = (PSuohaRoom)room->privateRoom();
		c_itemCounterNumber[3]->setText(QString("%1").arg(letoh4(pshroom->uMinYazhu)*c_PlayerCounters[seat]));		c_itemCounterNumber[3]->setHAlignment(Qt::AlignHCenter);
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
			y += SUOHA_DESKTOP_COUNTER_LAYOUT_SIZE;
		}
		
	}
	if(c_PlayerStatus[seat] != 0)
	{//画标志
		// c_PlayerStatus[seat] == 1 投降
		//c_PlayerStatus[seat] == 2 胜利
		;
	}
}
void SuohaDesktop::RepaintView4TableCounter(quint8 seat)
{
	ClearImage(0,seat,DJGAME_POKERDESKTOP_TYPE_TABLECOUNTER);
	if(c_PlayerCounters[seat] <= 0)
		return ;
	QImage img = GetCounterImage();
	QRect rt = GetTableCounterRect();
	int cx,cy,sh,x,y,dy,lx = SUOHA_DESKTOP_COUNTER_LAYOUT_SIZE;
	GetDesktopCenterPoint(&cx,&cy,&sh);
	QMatrix m = GetCurrentMatrix();
	if(c_PlayerCounters[seat] > 0)
	{
		DJGameRoom *room = c_SuohaPanel->gameRoom();
		PSuohaRoom  pshroom = (PSuohaRoom)room->privateRoom();
		c_itemCounterNumber[4]->setText(QString("%1").arg(letoh4(pshroom->uMinYazhu)*c_PlayerCounters[seat]));		c_itemCounterNumber[4]->setVAlignment(Qt::AlignVCenter);
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
			x -= SUOHA_DESKTOP_COUNTER_LAYOUT_SIZE;
		}
		
	}
	if(c_PlayerStatus[seat] != 0)
	{//画标志
		// c_PlayerStatus[seat] == 1 投降
		//c_PlayerStatus[seat] == 2 胜利
		;
	}
}

void SuohaDesktop::StaticInitDesktop()
{
	for(int i=1;i<=DJGAME_SUOHA_MAX_PLAYERS;i++)
	{
		c_itemCounterNumber[i]->hide();
	}
	memset(c_PlayerCards,0,(DJGAME_SUOHA_MAX_PLAYERS+1)*(SUOHA_PLAYER_MAX_CARDS+1));
	DJGameRoom *room = c_SuohaPanel->gameRoom();
	PSuohaRoom  pshroom = (PSuohaRoom)room->privateRoom();

	c_HandCounters = letoh2(pshroom->uGameMaxMulti);
	
	memset(c_PlayerCounters,0,(DJGAME_SUOHA_MAX_PLAYERS+1)*sizeof(qint16));
	memset(c_PlayerStatus,0,DJGAME_SUOHA_MAX_PLAYERS+1);
	memset(c_PlayerPreCounters,0,(DJGAME_SUOHA_MAX_PLAYERS+1)*sizeof(qint16));
	for(int i=1;i<=4;i++)
		ResetPlayerCards(i,NULL,0);
	
	RepaintCurrentStatus();
}
void SuohaDesktop::StaticGameTrace(GeneralGameTrace2Head *gameTrace)
{
	PSuohaPickup ppickup;
	PSuohaShowCards pshows;
	switch(gameTrace->chType)
	{
		case SUOHA_GAMETRACE_PICKUP :
		case SUOHA_GAMETRACE_PICKUP2 :
			ppickup = (PSuohaPickup)gameTrace->chBuf;
			if(ppickup->chCardNumber >0 && ppickup->chCardNumber <=SUOHA_PLAYER_MAX_CARDS)
			{
				c_PlayerCards[gameTrace->chSite][ppickup->chCardNumber-1] = ppickup->chCard;
				ResetPlayerCards(gameTrace->chSite,c_PlayerCards[gameTrace->chSite],ppickup->chCardNumber);
				c_chSteps = ppickup->chCardNumber;
			}
			c_PlayerPreCounters[gameTrace->chSite] = c_PlayerCounters[gameTrace->chSite];
			c_CurrentCounters = 0;
			break;
		case SUOHA_GAMETRACE_SHOWCARDS :
			pshows = (PSuohaShowCards)gameTrace->chBuf;
			if(pshows->chCards >0 && pshows->chCards <=SUOHA_PLAYER_MAX_CARDS)
			{
				memcpy(c_PlayerCards[gameTrace->chSite],pshows->chCard, pshows->chCards);
				ResetPlayerCards(gameTrace->chSite,c_PlayerCards[gameTrace->chSite],pshows->chCards);
				c_chSteps = pshows->chCards;
			}
			break;
		case SUOHA_GAMETRACE_BET :
		{
			PSuohaBet pyazhu = (PSuohaBet)gameTrace->chBuf;
			if(pyazhu->chMultiple == SUOHA_BET_AWAY)
				c_PlayerStatus[gameTrace->chSite] = 1;
			else
			{
				c_PlayerCounters[gameTrace->chSite] = c_PlayerPreCounters[gameTrace->chSite] + pyazhu->chMultiple;
				if(gameTrace->chSite == c_SuohaPanel->selfSeatId())
				{
					if(c_HandCounters > pyazhu->chMultiple)
						c_HandCounters -= pyazhu->chMultiple;
					else
						c_HandCounters = 0;
				}
				c_CurrentCounters = pyazhu->chMultiple;
			}
//			printf("SUOHA_GAMETRACE_BET site %d ,bets=%d,total=%d\n",gameTrace->chSite,pyazhu->chMultiple,c_PlayerCounters[gameTrace->chSite]);

			break;
		}
		/*
		case SUOHA_GAMETRACE_RESULT :
		{
			PSuohaResult presult = (PSuohaResult)gameTrace->chBuf;
			djDebug()<<"SUOHA_GAMETRACE_RESULT";
			djDebug()<<"site"<<gameTrace->chSite<<"result"<<letoh2(presult->shMultiple);
			PSuohaRoom pshroom = (PSuohaRoom)c_SuohaPanel->gameRoom()->privateRoom();
			
			QString result;
			for(int i=1;i<=c_SuohaPanel->numberOfSeats();i++)
			{
				DJGameUser	*user	= c_SuohaPanel->userAtSeat(i);
				if ( user ) {
					QString username	= user->userName();
					if ( c_current.chMaster == user->seatId() )
						username	+= tr("(master)");
    				if(i== gameTrace->chSite) {
    					int value	= letoh2(presult->shMultiple) * letoh4(pshroom->uMinYazhu);
    					c_PlayerCounters[i] += letoh2(presult->shMultiple);
    					c_SuohaPanel->deltaUserData( user, value );
    					//text += QString(" : %1").arg(value);
    					result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( username )
								.arg( value );
    				}else {
    					int value	= c_PlayerCounters[i] * letoh4(pshroom->uMinYazhu);
    					c_PlayerCounters[i] = 0;
    					c_SuohaPanel->deltaUserData( user, -value );
    					//text += QString(" : %1").arg(-value);
    					result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( username )
								.arg( -value );
    				}
				}
			}
			c_SuohaPanel->insertGameResult2Browser( result );
			
			break;
		}
		*/
	}
}

void SuohaDesktop::PlayerStarted()
{
	StaticInitDesktop();
}
void SuohaDesktop::handleMatrixChanged(const QMatrix &m)
{
	DJGamePokerDesktop::handleMatrixChanged(m);
		
	int x,y,iSelfHeight;
	GetDesktopCenterPoint(&x,&y,&iSelfHeight);
	int tx,ty;
	m.map(x,y,&tx,&ty);
	
	c_pbMost->move(tx-c_pbMost->width()/2,ty-c_pbMost->height()-20-c_pbAway->height());
	c_pbAway->move(tx-c_pbAway->width()/2,ty-c_pbAway->height()-10);
	
	c_pbAddup->move(tx-c_pbAddup->width()/2,ty+10);
	c_pbKeep->move(tx-c_pbKeep->width()/2,ty+20+c_pbAddup->height());
	
	c_itemMin->setExternalMatrix(m);
	c_itemMax->setExternalMatrix(m);
}
void SuohaDesktop::contentsMousePressEvent(QMouseEvent* e)
{
	Q3CanvasItemList list=canvas()->collisions(e->pos());
    Q3CanvasItemList::Iterator it=list.begin();
    if (it != list.end())
    {
    	Q3CanvasItem * canvasItem = *it;
    	if ( djgame_imageRTTI == canvasItem->rtti() ) {
			DJGameImageItem *imageItem	= (DJGameImageItem *)canvasItem;
			if ( DJGAME_POKERDESKTOP_TYPE_HANDCARD == imageItem->type()
 				&& imageItem->sid() == GetGamePanel()->selfSeatId() ) {
 				DJGamePokerItem *pokerItem = (DJGamePokerItem*)imageItem;
 				if ( DJGAME_POKER_BACKGROUND == pokerItem->tid() ) {
 					m_firstIsBack	= false;
 					RepaintSeatCard( GetGamePanel()->selfSeatId() );
 				}
 			}
		}
    }
}
void SuohaDesktop::contentsMouseReleaseEvent( QMouseEvent * e )
{
	if ( !m_firstIsBack ) {
		m_firstIsBack	= true;
		RepaintSeatCard( GetGamePanel()->selfSeatId() );
	}
}
void SuohaDesktop::GetPageInterval(quint8 chView,quint8 *pcards,quint8 chPages,int *dx,int *dy)
{
	*dx = 1;
	*dy = 0;
}

unsigned char SuohaDesktop::GetSeatCards(quint8 seat,quint8 *pcards,quint8 chSize)
{
	unsigned char i;
	
	for(i=0;i<=4;i++)
	{
		if(c_PlayerCards[seat][i] != 0) {
			if ( 0 == i ) {
				if ( DJGAME_TABLE_STATUS_WAITSTART == GetTableStatus() )
					pcards[i] = c_PlayerCards[seat][i];
				else {
					if ( m_firstIsBack ) {
						pcards[i]	= DJGAME_POKER_BACKGROUND;
					}else {
						pcards[i] = c_PlayerCards[seat][i];
					}
				}
			}else		
				pcards[i] = c_PlayerCards[seat][i];
		}else
			break;
	}
	return i;
}
QString	SuohaDesktop::playerItemNameSuffix( DJGameUser* user )
{
	if ( c_current.chMaster && c_current.chMaster == user->seatId() )
		return tr("(master)");
	else
		return QString();
}
void SuohaDesktop::repaintHost()
{
	repaintHostItem( c_current.chMaster );
}
quint8 SuohaDesktop::numberOfPlayers() const
{
	quint8 players	= c_current.chPlayers;
	if ( 0 != players )
		return players;
	else
		return DJGameDesktop::numberOfPlayers();
}
QList<quint8> SuohaDesktop::seatsOfPlayers() const
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
//////////////////////////////////////////////////////////////////
SuohaPanel::SuohaPanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f)
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
	SuohaDesktop* gameDesktop = new SuohaDesktop(gameSplitter,this);
	setAccessorySplitter( accessorySplitter );
	setGameDesktop( gameDesktop );
	color()->hide();
}
SuohaPanel::~SuohaPanel()
{
}
