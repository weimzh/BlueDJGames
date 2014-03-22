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
#include "niuniu.h"
#include "nnprofile.h"
#include "jdebug.h"
#include "DJOnceSprite.h"
#include "DJGamePlayerItem.h" 
#include "DJGamePokerDesktop.h"
#include "DJMatrixCanvasText.h"
#include "DJMatrixCanvasImage.h"
#include "DJMatrixCanvasTimer.h"
#include "DJToolButton.h"
#include "DJGlobal.h" 

#define NIUNIU_DESKTOP_COUNTER_LAYOUT_SIZE		25

NiuniuDesktop::NiuniuDesktop(QWidget* parent,NiuniuPanel *panel)
					: DJGamePokerDesktop(parent,panel)
{
	memset(&c_current,0,sizeof(NiuniuCurrent));
	
	SetCardPictureSize(centerSize(),popupSize(),30,pageLayoutSizeY());
	c_NiuniuPanel = panel;
	connect(this,SIGNAL(gameStarted()),this,SLOT(PlayerStarted()));
	
	QSize iconSize( 19 * 2, 19 );
	
	c_pbAway = new QPushButton(this);
    QPixmap pix = QPixmap(":/BaseRes/image/desktop/pushbutton/FangQi.png");
	c_pbAway->setIcon( pix );
	c_pbAway->setIconSize( iconSize );
	c_pbAway->adjustSize();
	c_pbAway->hide();
	connect(c_pbAway,SIGNAL(clicked()),this,SLOT(ClickAway()));
	
	c_pbAddup = new QPushButton(this);
    pix = QPixmap(":/BaseRes/image/desktop/pushbutton/JiaZhu.png");
	c_pbAddup->setIcon( pix );
	c_pbAddup->setIconSize( iconSize );
	c_pbAddup->adjustSize();
	c_pbAddup->hide();
	connect(c_pbAddup,SIGNAL(clicked()),this,SLOT(ClickAddup()));
	
	c_pbDouble = new QPushButton(this);
    pix = QPixmap(":/BaseRes/image/desktop/pushbutton/JiaBei.png");
	c_pbDouble->setIcon( pix );
	c_pbDouble->setIconSize( iconSize );
	c_pbDouble->adjustSize();
	c_pbDouble->hide();
	connect(c_pbDouble,SIGNAL(clicked()),this,SLOT(ClickDouble()));
	
	c_pbFollow = new QPushButton(this);
	pix = QPixmap(":/BaseRes/image/desktop/pushbutton/Gen.png");
	c_pbFollow->setIcon( pix );
	c_pbFollow->setIconSize( iconSize );
	c_pbFollow->adjustSize();
	c_pbFollow->hide();
	connect(c_pbFollow,SIGNAL(clicked()),this,SLOT(ClickFollow()));
	
	QMatrix m = GetCurrentMatrix();
	
	pix = QPixmap(QString(":/NiuNiuRes/image/table1.png"));
	QImage img = pix.convertToImage();
	c_CasinoTable = new DJGameImageItem(img,canvas(),m,1,1,DJGAME_POKERDESKTOP_TYPE_TABLE,false);
	c_CasinoTable->setMatrix(m);
	c_CasinoTable->setHAlignment(Qt::AlignHCenter);
	c_CasinoTable->setVAlignment(Qt::AlignVCenter);
	c_CasinoTable->move(GetRealWidth()/2,GetRealHeight()/2);
	c_CasinoTable->setZ(10);
	c_CasinoTable->show();
	
	pix = QPixmap(QString(":/NiuNiuRes/image/csnfloor1.png"));
	canvas()->setBackgroundPixmap(pix);
	
	DJGameRoom *room = c_NiuniuPanel->gameRoom();
	PNiuniuRoom  pnnroom = (PNiuniuRoom)room->privateRoom();
	
	QFont f;
	f.setBold(true);
	f.setPointSize(12);
		
	QString strMin = tr("Min: ");
	strMin += QString("%1").arg(letoh4(pnnroom->uMinYazhu));
	c_itemMin = new DJMatrixCanvasText(canvas(),strMin);
	c_itemMin->setFont(f);
	c_itemMin->setZ(100);
	c_itemMin->matrimove(GetRealWidth()/2-187,GetRealHeight()/2-250);
	c_itemMin->show();
	
	QString strMax = tr("Max: ");
	strMax += QString("%1").arg(letoh4(pnnroom->uGameMaxCounter));
	c_itemMax = new DJMatrixCanvasText(canvas(),strMax);
	c_itemMax->setFont(f);
	c_itemMax->setZ(100);
	c_itemMax->matrimove(GetRealWidth()/2-187,GetRealHeight()/2-230);
	c_itemMax->show();
	
	f.setPointSize(22);
	QString strNumber=QString("0");
	for(int i=1;i<=DJGAME_NIUNIU_MAX_PLAYERS;i++)
	{
		c_itemCounterNumber[i] = new DJGameTextItem(strNumber,canvas(),m,1,1,DJGAME_POKERDESKTOP_TYPE_NUMBER,false);
		QColor	color	= Qt::red;
		if ( 0 == panel->gameRoom()->roomId() % 3 )
			color	= Qt::blue;
		c_itemCounterNumber[i]->setColor(color);
		c_itemCounterNumber[i]->setZ(2000);
		c_itemCounterNumber[i]->setFont(f);
		c_itemCounterNumber[i]->setMatrix(m);
		c_itemCounterNumber[i]->hide();
	}
	
	for ( quint8 seat = 1; seat <= panel->numberOfSeats(); ++seat) {
		DJMatrixCanvasText* name	= nameItem( seat );
		name->setColor( Qt::yellow );
	}
	setHostEnabled( true );
	setSelectRectEnabled( false );
	DJMatrixCanvasImage* host	= hostItem();
	host->setOriginalPixmap( QPixmap(":/NiuNiuRes/image/master.png") );
	
	StaticInitDesktop();
}
NiuniuDesktop::~NiuniuDesktop()
{
}

void NiuniuDesktop::ClickAway()
{
	if(!(c_NiuniuPanel->isLookingOn()) && IsWaittingForMe())
	{
		if(GetTableStatus() == DJGAME_NIUNIU_TABLE_STATUS_WAIT_YAZHU)
		{
			NiuniuYazhu ya;
			ya.chMultiple = 0;
			SendGameTrace(NIUNIU_GAMETRACE_YAZHU,(char *)&ya,sizeof(NiuniuYazhu));
		}else if(GetTableStatus() == DJGAME_NIUNIU_TABLE_STATUS_WAIT_MASTER)
		{
			unsigned char ch=0;
			SendGameTrace(NIUNIU_GAMETRACE_MASTER,(char *)&ch,1);
		}
	}
	c_pbAway->hide();
	c_pbAddup->hide();
	c_pbDouble->hide();
	c_pbFollow->hide();
}

void NiuniuDesktop::ClickAddup()
{
	if(!(c_NiuniuPanel->isLookingOn()) && IsWaittingForMe() && GetTableStatus() == DJGAME_NIUNIU_TABLE_STATUS_WAIT_YAZHU)
	{
		DJGameRoom *room = c_NiuniuPanel->gameRoom();
		PNiuniuRoom  pnnroom = (PNiuniuRoom)room->privateRoom();
		NiuniuYazhu ya;
		ya.chMultiple = pnnroom->chMaxMulti/2;
		SendGameTrace(NIUNIU_GAMETRACE_YAZHU,(char *)&ya,sizeof(NiuniuYazhu));
	}
	c_pbAway->hide();
	c_pbAddup->hide();
	c_pbDouble->hide();
	c_pbFollow->hide();
}

void NiuniuDesktop::ClickDouble()
{
	if(!(c_NiuniuPanel->isLookingOn()) && IsWaittingForMe() && GetTableStatus() == DJGAME_NIUNIU_TABLE_STATUS_WAIT_YAZHU)
	{
		DJGameRoom *room = c_NiuniuPanel->gameRoom();
		PNiuniuRoom  pnnroom = (PNiuniuRoom)room->privateRoom();
		NiuniuYazhu ya;
		ya.chMultiple = pnnroom->chMaxMulti;
		SendGameTrace(NIUNIU_GAMETRACE_YAZHU,(char *)&ya,sizeof(NiuniuYazhu));
	}
	c_pbAway->hide();
	c_pbAddup->hide();
	c_pbDouble->hide();
	c_pbFollow->hide();
}

void NiuniuDesktop::ClickFollow()
{
	if(!(c_NiuniuPanel->isLookingOn()) && IsWaittingForMe() && GetTableStatus() == DJGAME_NIUNIU_TABLE_STATUS_WAIT_MASTER)
	{
		unsigned char ch=1;
		SendGameTrace(NIUNIU_GAMETRACE_MASTER,(char *)&ch,1);
	}
	c_pbAway->hide();
	c_pbAddup->hide();
	c_pbDouble->hide();
	c_pbFollow->hide();
}

void NiuniuDesktop::userArrived( quint32 userId, quint8 seatId )
{
	djDebug()<<"NiuniuDesktop::userArrived"<<userId<<seatId;
	DJGamePokerDesktop::userArrived( userId, seatId );
	
	RepaintCurrentStatus();
	/*
	QList<quint8> seatIdsForPlayers	= c_NiuniuPanel->seatIdsForPlayers();
	
	foreach( quint8 seat, seatIdsForPlayers ) {
		RepaintSeatCard( seat );
		RepaintTableCounter( seat );
	}
	RepaintHandCounter();
	*/
}
void NiuniuDesktop::gameInfo( const unsigned char *buf )
{
	memcpy(&c_current,buf,sizeof(NiuniuCurrent));
		
	StaticInitDesktop();
}

void NiuniuDesktop::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	DJGameDesktop::gameWait(mask,status,timeout);
	switch(status)
	{
		case DJGAME_NIUNIU_TABLE_STATUS_WAIT_YAZHU :
		{
			c_pbAway->hide();
			c_pbAddup->hide();
			c_pbDouble->hide();
			c_pbFollow->hide();
			if(IsWaittingForMe())
			{
				c_pbAddup->show();
				c_pbDouble->show();
				c_pbAway->show();
			}
			break;
		}
		case DJGAME_NIUNIU_TABLE_STATUS_WAIT_MASTER :
		{
			c_pbAway->hide();
			c_pbAddup->hide();
			c_pbDouble->hide();
			c_pbFollow->hide();
			if(IsWaittingForMe())
			{
				c_pbFollow->show();
				c_pbAway->show();
			}
			break;
		}
	}
			
}
void NiuniuDesktop::gameTrace( const GeneralGameTrace2Head *gameTrace )
{
	GeneralGameTrace2Head *ptrace = (GeneralGameTrace2Head *)gameTrace;
	DJGameDesktop::gameTrace(gameTrace);
	StaticGameTrace(ptrace);
	c_pbAway->hide();
	c_pbAddup->hide();
	c_pbDouble->hide();
	c_pbFollow->hide();
	
	switch(gameTrace->chType)
	{
		case NIUNIU_GAMETRACE_PICKUP :
		case NIUNIU_GAMETRACE_PICKUP2 :
		case NIUNIU_GAMETRACE_SHOWCARDS :
			RepaintSeatCard(gameTrace->chSite);
			break;
		case NIUNIU_GAMETRACE_YAZHU :
		{
			PNiuniuYazhu pyazhu = (PNiuniuYazhu)gameTrace->chBuf;
			if(pyazhu->chMultiple > 0)
			{
				RepaintTableCounter(gameTrace->chSite);
				if(gameTrace->chSite == c_NiuniuPanel->selfSeatId())
				{
					RepaintHandCounter();
				}else {
					c_pbAway->show();
					c_pbAddup->show();
					c_pbDouble->show();
				}
			}
			break;
		}
		case NIUNIU_GAMETRACE_MASTER :
		{
			if(gameTrace->chBuf[0] != 0)
				RepaintTableCounter(gameTrace->chSite);
			break;
		}
		case NIUNIU_GAMETRACE_RESULT :
		{
			RepaintTableCounter(gameTrace->chSite);
			RepaintTableCounter(c_current.chMaster);
			break;
		}
	}
}
void NiuniuDesktop::RepaintCurrentStatus()
{
	for(int i=1;i<=4;i++)
	{
		RepaintSeatCard(i);
		RepaintTableCounter(i);
	}
	RepaintHandCounter();
}

QImage NiuniuDesktop::GetCounterImage()
{
	DJGameRoom *room = c_NiuniuPanel->gameRoom();
	QPixmap pix = QPixmap(QString(":/NiuNiuRes/image/cm_%1.png").arg((room->roomId()%3)+1));
	QImage img = pix.convertToImage();
	return img;
}
void NiuniuDesktop::RepaintHandCounter()
{
	ClearImage(0,c_NiuniuPanel->selfSeatId(),DJGAME_POKERDESKTOP_TYPE_HANDCOUNTER);
	QImage img = GetCounterImage();
	QMatrix m = GetCurrentMatrix();
	DJGamePokerItem *item;
	quint32 x = GetRealWidth();
	quint32 y = GetRealHeight()-img.height()-10;
	
	x -= img.width();
	
	for(int i=0;i<c_HandCounters;i++)
	{
		item = new DJGamePokerItem(img,canvas(),m,i+1,c_NiuniuPanel->selfSeatId(),DJGAME_POKERDESKTOP_TYPE_HANDCOUNTER,false);
		item->setZ(1200+i);
		item->move(x,y);
		item->show();
		item->setdy(10);
		item->setMatrix(m);
		x -= NIUNIU_DESKTOP_COUNTER_LAYOUT_SIZE;
	}
}

void NiuniuDesktop::RepaintTableCounter(quint8 seat)
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

QRect NiuniuDesktop::GetTableCounterRect()
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

void NiuniuDesktop::RepaintView1TableCounter(quint8 seat)
{
	ClearImage(0,seat,DJGAME_POKERDESKTOP_TYPE_TABLECOUNTER);
	if(c_PlayerCounters[seat] <= 0)
		return ;
	QImage img = GetCounterImage();
	QRect rt = GetTableCounterRect();
	int cx,cy,sh,x,y,dx,lx = NIUNIU_DESKTOP_COUNTER_LAYOUT_SIZE;
	GetDesktopCenterPoint(&cx,&cy,&sh);
	QMatrix m = GetCurrentMatrix();
	if(c_PlayerCounters[seat] > 0)
	{
		DJGameRoom *room = c_NiuniuPanel->gameRoom();
		PNiuniuRoom  pnnroom = (PNiuniuRoom)room->privateRoom();
		c_itemCounterNumber[1]->setText(QString("%1").arg(letoh4(pnnroom->uMinYazhu)*c_PlayerCounters[seat]));
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
		if( i+1 == c_PlayerSelfCounters[seat])
			x += lx;
		if(x+img.width()>rt.right())
		{
			x = dx;
			y -= NIUNIU_DESKTOP_COUNTER_LAYOUT_SIZE;
		}
		
	}
	if(c_PlayerStatus[seat] != 0)
	{//画标志
		// c_PlayerStatus[seat] == 1 投降
		//c_PlayerStatus[seat] == 2 胜利
		;
	}
	
}
void NiuniuDesktop::RepaintView2TableCounter(quint8 seat)
{
	ClearImage(0,seat,DJGAME_POKERDESKTOP_TYPE_TABLECOUNTER);
	if(c_PlayerCounters[seat] <= 0)
		return ;
	QImage img = GetCounterImage();
	QRect rt = GetTableCounterRect();
	int cx,cy,sh,x,y,dy,lx = NIUNIU_DESKTOP_COUNTER_LAYOUT_SIZE;
	GetDesktopCenterPoint(&cx,&cy,&sh);
	QMatrix m = GetCurrentMatrix();
	if(c_PlayerCounters[seat] > 0)
	{
		DJGameRoom *room = c_NiuniuPanel->gameRoom();
		PNiuniuRoom  pnnroom = (PNiuniuRoom)room->privateRoom();
		c_itemCounterNumber[2]->setText(QString("%1").arg(letoh4(pnnroom->uMinYazhu)*c_PlayerCounters[seat]));
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
		if( y+1 == c_PlayerSelfCounters[seat])
			y += lx;
		if(y+img.width()>rt.bottom())
		{
			y = dy;
			x += NIUNIU_DESKTOP_COUNTER_LAYOUT_SIZE;
		}
		
	}
	if(c_PlayerStatus[seat] != 0)
	{//画标志
		// c_PlayerStatus[seat] == 1 投降
		//c_PlayerStatus[seat] == 2 胜利
		;
	}

}
void NiuniuDesktop::RepaintView3TableCounter(quint8 seat)
{
	ClearImage(0,seat,DJGAME_POKERDESKTOP_TYPE_TABLECOUNTER);
	if(c_PlayerCounters[seat] <= 0)
		return ;
	QImage img = GetCounterImage();
	QRect rt = GetTableCounterRect();
	int cx,cy,sh,x,y,dx,lx = NIUNIU_DESKTOP_COUNTER_LAYOUT_SIZE;
	GetDesktopCenterPoint(&cx,&cy,&sh);
	QMatrix m = GetCurrentMatrix();
	if(c_PlayerCounters[seat] > 0)
	{
		DJGameRoom *room = c_NiuniuPanel->gameRoom();
		PNiuniuRoom  pnnroom = (PNiuniuRoom)room->privateRoom();
		c_itemCounterNumber[3]->setText(QString("%1").arg(letoh4(pnnroom->uMinYazhu)*c_PlayerCounters[seat]));
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
		if( i+1 == c_PlayerSelfCounters[seat])
			x -= lx;
		if(x<rt.left())
		{
			x = dx;
			y += NIUNIU_DESKTOP_COUNTER_LAYOUT_SIZE;
		}
		
	}
	if(c_PlayerStatus[seat] != 0)
	{//画标志
		// c_PlayerStatus[seat] == 1 投降
		//c_PlayerStatus[seat] == 2 胜利
		;
	}
}
void NiuniuDesktop::RepaintView4TableCounter(quint8 seat)
{
	ClearImage(0,seat,DJGAME_POKERDESKTOP_TYPE_TABLECOUNTER);
	if(c_PlayerCounters[seat] <= 0)
		return ;
	QImage img = GetCounterImage();
	QRect rt = GetTableCounterRect();
	int cx,cy,sh,x,y,dy,lx = NIUNIU_DESKTOP_COUNTER_LAYOUT_SIZE;
	GetDesktopCenterPoint(&cx,&cy,&sh);
	QMatrix m = GetCurrentMatrix();
	if(c_PlayerCounters[seat] > 0)
	{
		DJGameRoom *room = c_NiuniuPanel->gameRoom();
		PNiuniuRoom  pnnroom = (PNiuniuRoom)room->privateRoom();
		c_itemCounterNumber[4]->setText(QString("%1").arg(letoh4(pnnroom->uMinYazhu)*c_PlayerCounters[seat]));
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
		if( i+1 == c_PlayerSelfCounters[seat])
			y -= lx;
		if(y<rt.top())
		{
			y = dy;
			x -= NIUNIU_DESKTOP_COUNTER_LAYOUT_SIZE;
		}
		
	}
	if(c_PlayerStatus[seat] != 0)
	{//画标志
		// c_PlayerStatus[seat] == 1 投降
		//c_PlayerStatus[seat] == 2 胜利
		;
	}
}

void NiuniuDesktop::StaticInitDesktop()
{
	for(int i=1;i<=DJGAME_NIUNIU_MAX_PLAYERS;i++)
	{
		c_itemCounterNumber[i]->hide();
	}
	memset(c_PlayerCards,0,(DJGAME_NIUNIU_MAX_PLAYERS+1)*(NIUNIU_PLAYER_MAX_CARDS+1));
	DJGameRoom *room = c_NiuniuPanel->gameRoom();
	PNiuniuRoom  pnnroom = (PNiuniuRoom)room->privateRoom();

	c_HandCounters = pnnroom->chMaxMulti *(NIUNIU_PLAYER_MAX_CARDS+1);
	
	memset(c_PlayerCounters,0,(DJGAME_NIUNIU_MAX_PLAYERS+1)*sizeof(qint16));
	memset(c_PlayerSelfCounters,0,(DJGAME_NIUNIU_MAX_PLAYERS+1)*sizeof(qint16));
	
	memset(c_PlayerStatus,0,DJGAME_NIUNIU_MAX_PLAYERS+1);
	
	for(int i=1;i<=4;i++)
		ResetPlayerCards(i,NULL,0);
	
	RepaintCurrentStatus();
}
void NiuniuDesktop::StaticGameTrace(GeneralGameTrace2Head *gameTrace)
{
	PNiuniuPickup ppickup;
	PNiuniuShowCards pshows;
	switch(gameTrace->chType)
	{
		case NIUNIU_GAMETRACE_PICKUP :
		case NIUNIU_GAMETRACE_PICKUP2 :
			ppickup = (PNiuniuPickup)gameTrace->chBuf;
			if(ppickup->chCardNumber >0 && ppickup->chCardNumber <=NIUNIU_PLAYER_MAX_CARDS)
			{
				c_PlayerCards[gameTrace->chSite][ppickup->chCardNumber-1] = ppickup->chCard;
				ResetPlayerCards(gameTrace->chSite,c_PlayerCards[gameTrace->chSite],ppickup->chCardNumber);
			}
			break;
		case NIUNIU_GAMETRACE_SHOWCARDS :
			pshows = (PNiuniuShowCards)gameTrace->chBuf;
			if(pshows->chCards >0 && pshows->chCards <=NIUNIU_PLAYER_MAX_CARDS)
			{
				memcpy(c_PlayerCards[gameTrace->chSite],pshows->chCard, pshows->chCards);
				ResetPlayerCards(gameTrace->chSite,c_PlayerCards[gameTrace->chSite],pshows->chCards);
			}
			break;
		case NIUNIU_GAMETRACE_YAZHU :
		{
			djDebug()<<"NIUNIU_GAMETRACE_YAZHU";
			
			PNiuniuYazhu pyazhu = (PNiuniuYazhu)gameTrace->chBuf;
			if(pyazhu->chMultiple > 0)
			{
				c_PlayerCounters[gameTrace->chSite] += pyazhu->chMultiple;
				c_PlayerSelfCounters[gameTrace->chSite] = c_PlayerCounters[gameTrace->chSite];
				if(gameTrace->chSite == c_NiuniuPanel->selfSeatId())
				{
					if(c_HandCounters > pyazhu->chMultiple)
						c_HandCounters -= pyazhu->chMultiple;
					else
						c_HandCounters = 0;
				}
			}else
				c_PlayerStatus[gameTrace->chSite] = 1;
			break;
		}
		case NIUNIU_GAMETRACE_MASTER :
		{
			djDebug()<<"NIUNIU_GAMETRACE_MASTER";
			if(gameTrace->chBuf[0] != 0)
			{
				c_PlayerCounters[gameTrace->chSite] = 0;
				c_PlayerSelfCounters[gameTrace->chSite] = 0;
				for(int j=1;j<=4;j++)
				{
					if(j != gameTrace->chSite)
						c_PlayerCounters[gameTrace->chSite] += c_PlayerCounters[j];
				}
				c_PlayerSelfCounters[gameTrace->chSite] = c_PlayerCounters[gameTrace->chSite];
			}
			break;
		}
		case NIUNIU_GAMETRACE_RESULT :
		{
			djDebug()<<"NIUNIU_GAMETRACE_RESULT";
			
			PNiuniuResult presult = (PNiuniuResult)gameTrace->chBuf;
			djDebug()<<"site"<<gameTrace->chSite<<"chMultiple"<<presult->chMultiple;
			PNiuniuRoom  pnnroom = (PNiuniuRoom)c_NiuniuPanel->gameRoom()->privateRoom();
			int value	= presult->chMultiple * letoh4(pnnroom->uMinYazhu);
			djDebug()<<"value"<<value;
			DJGameUser	*player	= c_NiuniuPanel->userAtSeat(gameTrace->chSite);
			DJGameUser	*master	= c_NiuniuPanel->userAtSeat(c_current.chMaster);
			if(presult->chResult == NIUNIU_GAMERESULT_WIN)
			{
				djDebug()<<"win";
				c_PlayerCounters[gameTrace->chSite] = presult->chMultiple*2;
				c_PlayerCounters[c_current.chMaster] -= presult->chMultiple;
				c_PlayerSelfCounters[gameTrace->chSite] = presult->chMultiple;
				c_PlayerStatus[gameTrace->chSite] = 2;
				c_NiuniuPanel->deltaUserData( player, value );
				c_NiuniuPanel->deltaUserData( master, -value );
				
				QString result;
				if ( master ) {
					//QString text	= master->userName() + QString(" : %1").arg(-value);
					//c_NiuniuPanel->insertText2Browser( text );
					result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( master->userName() + tr("(master)") )
								.arg( -value );
				}
				if ( player ) {
					//QString text	= player->userName()+ QString(" : %1").arg(value);
					//c_NiuniuPanel->insertText2Browser( text );
					result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( player->userName() )
								.arg( value );
				}
				c_NiuniuPanel->insertGameResult2Browser( result );
			}else
			{
				djDebug()<<"lose";
				c_PlayerCounters[c_current.chMaster] += presult->chMultiple;
				c_PlayerSelfCounters[gameTrace->chSite] = presult->chMultiple;
				c_PlayerCounters[gameTrace->chSite] = 0;
				c_PlayerStatus[gameTrace->chSite] = 1;
				c_NiuniuPanel->deltaUserData( player, -value );
				c_NiuniuPanel->deltaUserData( master, value );
				
				QString result;
				if ( master ) {
					//QString text	= master->userName() + QString(" : %1").arg(value);
					//c_NiuniuPanel->insertText2Browser( text );
					result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( master->userName() + tr("(master)") )
								.arg( value );
				}
				if ( player ) {
					//QString text	= player->userName()+ QString(" : %1").arg(-value);
					//c_NiuniuPanel->insertText2Browser( text );
					result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( player->userName() )
								.arg( -value );
				}
				c_NiuniuPanel->insertGameResult2Browser( result );
			}
			if(gameTrace->chSite == c_NiuniuPanel->selfSeatId())
			{
				//DJGameRoom *room = c_NiuniuPanel->gameRoom();
				//PNiuniuRoom  pnnroom = (PNiuniuRoom)room->privateRoom();
				c_HandCounters = (pnnroom->chMaxMulti *(NIUNIU_PLAYER_MAX_CARDS+1))-c_PlayerSelfCounters[gameTrace->chSite];
			}
			
			if(c_PlayerCounters[c_current.chMaster] >= c_PlayerSelfCounters[c_current.chMaster])
				c_PlayerStatus[c_current.chMaster] = 2;
			else
				c_PlayerStatus[c_current.chMaster] = 1;
			
			break;
		}
	}
}
		

void NiuniuDesktop::PlayerStarted()
{
	StaticInitDesktop();
}
void NiuniuDesktop::handleMatrixChanged(const QMatrix &m)
{
	DJGamePokerDesktop::handleMatrixChanged(m);
		
	int x,y,iSelfHeight;
	GetDesktopCenterPoint(&x,&y,&iSelfHeight);
	x	= GetRealWidth();
	y	+= 120;
	int tx,ty;
	m.map(x,y,&tx,&ty);
	
	//c_pbAway->move(tx-c_pbAway->width()/2,ty-c_pbAway->height()-10);
	//c_pbDouble->move(tx-c_pbDouble->width()/2,ty+10);
	//c_pbAddup->move(tx-c_pbAddup->width()/2,ty+10+c_pbDouble->height());
	//c_pbFollow->move(tx-c_pbFollow->width()/2,ty+10);
	int	spacingY	= 2;
	
	c_pbAway->move(tx-c_pbAway->width(),ty);
	ty	+=	c_pbAway->height()+spacingY;
	c_pbDouble->move(tx-c_pbDouble->width(),ty);
	ty	+=	c_pbDouble->height()+spacingY;
	c_pbAddup->move(tx-c_pbAddup->width(),ty);
	ty	+=	c_pbAddup->height()+spacingY;
	c_pbFollow->move(tx-c_pbFollow->width(),ty);
	
	c_itemMin->setExternalMatrix(m);
	c_itemMax->setExternalMatrix(m);
}

void NiuniuDesktop::GetPageInterval(quint8 chView,quint8 *pcards,quint8 chPages,int *dx,int *dy)
{
	*dx = 1;
	*dy = 0;
}

unsigned char NiuniuDesktop::GetSeatCards(quint8 seat,quint8 *pcards,quint8 chSize)
{
	unsigned char i;
	for(i=0;i<5;i++)
	{
		if(c_PlayerCards[seat][i] != 0)
			pcards[i] = c_PlayerCards[seat][i];
		else
			break;
	}
	return i;
}
QString	NiuniuDesktop::playerItemNameSuffix( DJGameUser* user )
{
	if ( c_current.chMaster && c_current.chMaster == user->seatId() )
		return tr("(master)");
	else
		return QString();
}
void NiuniuDesktop::repaintHost()
{
	repaintHostItem( c_current.chMaster );
}
quint8 NiuniuDesktop::numberOfPlayers() const
{
	quint8 players	= c_current.chPlayers;
	if ( 0 != players )
		return players;
	else
		return DJGameDesktop::numberOfPlayers();
}
QList<quint8> NiuniuDesktop::seatsOfPlayers() const
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
NiuniuPanel::NiuniuPanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f)
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
	NiuniuDesktop* gameDesktop = new NiuniuDesktop(gameSplitter,this);
	setAccessorySplitter( accessorySplitter );
	setGameDesktop( gameDesktop );
	color()->hide();
}
NiuniuPanel::~NiuniuPanel()
{
}
