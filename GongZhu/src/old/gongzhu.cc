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
#include "DJGameRoom.h"
#include "DJTreeWidget.h"
#include "Utility.h"
#include "DJTreeItemDelegate.h"
#include "DJGameImage.h"
#include "DJGameDesktop.h"
#include "DJGamePokerDesktop.h"
#include "gongzhu.h"
#include "gozhrule.h"
#include "gozhprof.h"
#include "DJToolButton.h"
#include "DJGlobal.h" 

/////////////////////////////////////////////////
#ifdef DJ_OEM
static QSize djToolButtonSize(28,25);
#endif
GongzhuDesktop::GongzhuDesktop(QWidget* parent,DJGamePanel *panel)
			: DJGamePokerDesktop(parent,panel)
{
	c_GongzhuPanel = (GongzhuPanel *)panel;
	
	StaticInitDesktop();
	connect(this,SIGNAL(gameStarted()),this,SLOT(PlayerStarted()));
	
	c_pbShow = new QPushButton(this);
	//c_pbShow->setText(tr("Show"));
	QPixmap	pix = QPixmap(":/BaseRes/image/desktop/pushbutton/LiangPai.png");
	c_pbShow->setIcon( pix );
	c_pbShow->setIconSize( pix.size() );
	c_pbShow->adjustSize();
	c_pbShow->hide();
	connect(c_pbShow,SIGNAL(clicked()),this,SLOT(ClickThrow()));
	
	c_pbThrow = new QPushButton(this);
	//c_pbThrow->setText(tr("Throw"));
	pix = QPixmap(":/BaseRes/image/desktop/pushbutton/ChuPai.png");
	c_pbThrow->setIcon( pix );
	c_pbThrow->setIconSize( pix.size() );
	c_pbThrow->adjustSize();
	c_pbThrow->hide();
	connect(c_pbThrow,SIGNAL(clicked()),this,SLOT(ClickThrow()));
	
#ifdef DJ_OEM
	QIcon background;
	background.addPixmap( QPixmap(":/BaseRes/image/desktop/toolbutton/bg.png"), QIcon::Normal, QIcon::Off );
	background.addPixmap( QPixmap(":/BaseRes/image/desktop/toolbutton/bgpressed.png"), QIcon::Normal, QIcon::On );
	
	m_previous = new DJToolButton(this);
	m_previous->setToolTip(tr("look last round"));
	m_previous->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/left.png") );
	m_previous->setIconSize( djToolButtonSize );
	m_previous->setBackgroundIcon( background );
    connect(m_previous,SIGNAL(pressed()),this,SLOT(PreviousPressed()));
    connect(m_previous,SIGNAL(released()),this,SLOT(PreviousRelesed()));
    
    SetFx( getSelfFx(),getOtherFx(),getThrowFx(),0.9 );
#endif	
}

void GongzhuDesktop::ClickThrow()
{
	DJGamePokerDesktop::handleItemClicked(Qt::RightButton,QPoint(),0);
}
#ifdef DJ_OEM
void GongzhuDesktop::PreviousPressed()
{
	RepaintThrowCard(1,&(c_ThrowCards[1][1][1]),c_ThrowCards[1][1][0]);
	RepaintThrowCard(2,&(c_ThrowCards[2][1][1]),c_ThrowCards[2][1][0]);
	RepaintThrowCard(3,&(c_ThrowCards[3][1][1]),c_ThrowCards[3][1][0]);
	RepaintThrowCard(4,&(c_ThrowCards[4][1][1]),c_ThrowCards[4][1][0]);
}
void GongzhuDesktop::PreviousRelesed()
{
	RepaintThrowCard(1,&(c_ThrowCards[1][0][1]),c_ThrowCards[1][0][0]);
	RepaintThrowCard(2,&(c_ThrowCards[2][0][1]),c_ThrowCards[2][0][0]);
	RepaintThrowCard(3,&(c_ThrowCards[3][0][1]),c_ThrowCards[3][0][0]);
	RepaintThrowCard(4,&(c_ThrowCards[4][0][1]),c_ThrowCards[4][0][0]);
}
#endif
GongzhuDesktop::~GongzhuDesktop()
{

}

void GongzhuDesktop::handleMatrixChanged(const QMatrix &m)
{
	DJGamePokerDesktop::handleMatrixChanged(m);
		
	int x,y,iSelfHeight;
	GetDesktopCenterPoint(&x,&y,&iSelfHeight);
	y = GetRealHeight()-iSelfHeight;
	int tx,ty;
	m.map(x,y,&tx,&ty);
	
	c_pbThrow->move(tx - (c_pbThrow->width()>>1),ty);
	c_pbShow->move(tx + (c_pbThrow->width()>>1) + 20,ty);
#ifdef DJ_OEM
	m_previous->move(width()-m_previous->width()-20,ty+40);
#endif	
}
void GongzhuDesktop::PlayerStarted()
{
	//StaticInitDesktop();
	//ClearSeatAttachCard(0);
	//ClearSeatHandCard(0);
	//ClearSeatThrowCard(0);
}

void GongzhuDesktop::StaticInitDesktop()
{
	memset(&c_current,0,sizeof(GongzhuCurrent));
	memset(c_ThrowCards,0,(GONGZHU_MAX_SAVE_ROUND+2)*(GONGZHU_MAX_SITES+1)*6);
	memset(c_ScorePages,0,GONGZHU_MAX_SITES+1);
	
	c_bFirstPlayer = true;
	c_chFirPlayerThrow = 0;
	c_chFirstPages = 0;
	memset(c_chCurrentMax,0,6);
	memset(c_chRounds,0,6);
}

void GongzhuDesktop::StaticGameTrace(GeneralGameTrace2Head *gameTrace)
{
	switch(gameTrace->chType)
	{
		case GONGZHU_GAMETRACE_RESET :
		case GONGZHU_GAMETRACE_PICKUP :
		{
			ResetPlayerCards(gameTrace->chSite,gameTrace->chBuf,gameTrace->chBufLen);
			break;
		}
		case GONGZHU_GAMETRACE_THROW : 
		{
			if(c_bFirstPlayer == true)
			{
				c_bFirstPlayer = false;
				c_chFirPlayerThrow = gameTrace->chBuf[0];
				c_chFirstPages = gameTrace->chBufLen;
				memcpy(c_chCurrentMax,gameTrace->chBuf,gameTrace->chBufLen);
			}
			if ( gameTrace->chSite == c_GongzhuPanel->selfSeatId() )
				c_chRounds[DJGAME_POKER_CLASS(c_chFirPlayerThrow)]++;
			RemovePlayerCards(gameTrace->chSite,gameTrace->chBuf,gameTrace->chBufLen);
			c_ThrowCards[gameTrace->chSite][0][0] = gameTrace->chBufLen;
			memcpy(&(c_ThrowCards[gameTrace->chSite][0][1]),gameTrace->chBuf,gameTrace->chBufLen);
			PlayerThrowCards(gameTrace->chSite,gameTrace->chBuf,gameTrace->chBufLen);
			break;
		}
		case GONGZHU_GAMETRACE_CLEARDESKTOP : 
		{
			DJGameRoom *room= c_GongzhuPanel->gameRoom();
			for(int i= GONGZHU_MAX_SAVE_ROUND;i>= 0;i--)
			{
				for(int j=1;j<=room->numberOfSeats();j++)
				{
					if(i>0)
						memcpy(c_ThrowCards[j][i],c_ThrowCards[j][i-1],6);
					else
						memset(c_ThrowCards[j][0],0,6);
				}
			}
			c_bFirstPlayer = true;
			c_chFirPlayerThrow = 0;
			c_chFirstPages = 0;
			break;
		}
		case GONGZHU_GAMETRACE_ROUNDRESULT :
		{
			//PGongzhuRoundResult pres = (PGongzhuRoundResult)gameTrace->chBuf;
	//		printf("GONGZHU_GAMETRACE_ROUNDRESULT,seat=%d,pages=%d, existpages=%d\n",gameTrace->chSite,
	//						gameTrace->chBufLen,c_ScorePages[gameTrace->chSite]);
			memcpy(&(c_ScoreCards[gameTrace->chSite][c_ScorePages[gameTrace->chSite]]),gameTrace->chBuf,gameTrace->chBufLen);
			c_ScorePages[gameTrace->chSite] += gameTrace->chBufLen;
			break;
		}
		case GONGZHU_GAMETRACE_SHOW :
		{
			PGongzhuShow pshow =(PGongzhuShow)gameTrace->chBuf;
			if(!DJGAME_POKER_ERRORCARD(pshow->chCard) && pshow->chPages > 0)
			{
				quint8 buffer[6];
				for(int i=0;i<pshow->chPages;i++)
					buffer[i] = pshow->chCard;
				RemovePlayerCards(gameTrace->chSite,buffer,pshow->chPages);
				AppendPlayerCards(gameTrace->chSite,buffer,pshow->chPages);
				SetCardStatus(gameTrace->chSite,pshow->chCard,GONGZHU_POKER_SIGN_SHOW);
				memcpy(&(c_ThrowCards[gameTrace->chSite][0][1+c_ThrowCards[gameTrace->chSite][0][0]]),
								buffer,pshow->chPages);
				c_ThrowCards[gameTrace->chSite][0][0] += pshow->chPages;
			}
			break;
		}
	}
}

void GongzhuDesktop::RepaintCurrentStatus()
{
	DJGameRoom *room= c_GongzhuPanel->gameRoom();
	for(int i=1;i<=room->numberOfSeats();i++)
	{
		RepaintSeatCard(i);
		RepaintThrowCard(i,&(c_ThrowCards[i][0][1]),c_ThrowCards[i][0][0]);
//		RepaintScoreCard(i);
	}
}

void GongzhuDesktop::RepaintScoreCard(quint8 seat)
{
}

bool GongzhuDesktop::IsThrowStep(PGeneralGameTrace2Head ptrace)
{
	if(ptrace->chType == GONGZHU_GAMETRACE_THROW)
		return true;
	return false;
}

void GongzhuDesktop::gameInfo( const unsigned char *buf )
{
	StaticInitDesktop();
	memcpy(&c_current,buf,sizeof(GongzhuCurrent));
	ClearSeatAttachCard(0);
	ClearSeatHandCard(0);
	ClearSeatThrowCard(0);
}

void GongzhuDesktop::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	if(GetTableStatus()==DJGAME_GONGZHU_TABLE_STATUS_WAIT_SHOW &&
		status == DJGAME_GONGZHU_TABLE_STATUS_WAIT_THROW)
	{
		ClearSeatThrowCard(0);
	}
	DJGameDesktop::gameWait(mask,status,timeout);
	if(status == DJGAME_TABLE_STATUS_WAITSTART)
	{
		c_pbThrow->hide();
		c_pbShow->hide();
		setWaitingThrow(false);
	}
	c_pbShow->hide();
	switch(status)
	{
		case DJGAME_GONGZHU_TABLE_STATUS_WAIT_THROW :
			c_pbThrow->show();
			if(IsWaittingForMe())
			{
				if(c_ThrowCards[c_GongzhuPanel->selfSeatId()][0][0] != 0)
				{
					c_chFirPlayerThrow = 0;
					c_chFirstPages = 0;
				}
				setWaitingThrow(true);
				setWaitingThrowAcl(false);
				c_pbThrow->show();
			}else
			{
				setWaitingThrow(false);
				c_pbThrow->hide();
			}
			break;
		case DJGAME_GONGZHU_TABLE_STATUS_WAIT_SHOW :
		{
			c_pbThrow->hide();
			c_pbShow->hide();
			quint8 seat = c_GongzhuPanel->selfSeatId();
			PDJGamePokerImage ppokerimg = GetPlayerCardImage(seat);
			if( (ppokerimg->chPage[CARD_ZHU] > 0 && GetCardStatus(seat,CARD_ZHU) == 0)
			 || (ppokerimg->chPage[CARD_YANG] > 0 && GetCardStatus(seat,CARD_YANG) == 0)
			 || (ppokerimg->chPage[CARD_BIAN] > 0 && GetCardStatus(seat,CARD_BIAN) == 0)
			 || (ppokerimg->chPage[CARD_HA] > 0 && GetCardStatus(seat,CARD_HA) == 0) )
			{
				c_pbShow->show();
				setWaitingThrow(true);
				setWaitingThrowAcl(false);
			}
		}
	}
}
void GongzhuDesktop::gameTrace( const GeneralGameTrace2Head *gameTrace )
{
	DJGameDesktop::gameTrace(gameTrace);
	GeneralGameTrace2Head *ptrace = (PGeneralGameTrace2Head)gameTrace;
	StaticGameTrace(ptrace);
	switch(gameTrace->chType)
	{
		case GONGZHU_GAMETRACE_RESET :
		case GONGZHU_GAMETRACE_PICKUP :
		{
			RepaintSeatCard(gameTrace->chSite);
			break;
		}
		case GONGZHU_GAMETRACE_THROW : 
		{
			RepaintSeatCard(gameTrace->chSite);
			RepaintThrowCard(gameTrace->chSite,&(c_ThrowCards[gameTrace->chSite][0][1]),c_ThrowCards[gameTrace->chSite][0][0]);
			c_GongzhuPanel->playWave("out.wav");
			break;
		}
		case GONGZHU_GAMETRACE_CLEARDESKTOP : 
		{
			ClearSeatThrowCard(0);
			break;
		}
		case GONGZHU_GAMETRACE_ROUNDRESULT :
		{
			RepaintAttachCards(gameTrace->chSite,c_ScoreCards[gameTrace->chSite],c_ScorePages[gameTrace->chSite]);
			break;
		}
		case GONGZHU_GAMETRACE_SHOW :
		{
			RepaintSeatCard(gameTrace->chSite);
			RepaintThrowCard(gameTrace->chSite,&(c_ThrowCards[gameTrace->chSite][0][1]),c_ThrowCards[gameTrace->chSite][0][0]);
			break;
		}
	}
}
void GongzhuDesktop::RecvThrowACL(bool bFailed)
{
	DJGamePokerDesktop::RecvThrowACL(bFailed);
	
	/*
	if(GetTableStatus() == DJGAME_GONGZHU_TABLE_STATUS_WAIT_SHOW)
	{
		quint8 seat = c_GongzhuPanel->selfSeatId();
		PDJGamePokerImage ppokerimg = GetPlayerCardImage(seat); 
		if( (ppokerimg->chPage[CARD_ZHU] > 0 && GetCardStatus(seat,CARD_ZHU) == 0)
			 || (ppokerimg->chPage[CARD_YANG] > 0 && GetCardStatus(seat,CARD_YANG) == 0)
			 || (ppokerimg->chPage[CARD_BIAN] > 0 && GetCardStatus(seat,CARD_BIAN) == 0)
			 || (ppokerimg->chPage[CARD_HA] > 0 && GetCardStatus(seat,CARD_HA) == 0) )
		{
			setWaitingThrow(true);
		}
	}
	*/
}

void GongzhuDesktop::CreateThrowTrace(PGeneralGameTrace2Head ptrace)
{
	printf("throw %02x\n",ptrace->chBuf[0]);
	quint8 seat = c_GongzhuPanel->selfSeatId();
	PDJGamePokerImage pokerimg = GetPlayerCardImage(seat);
	if(GetTableStatus() == DJGAME_GONGZHU_TABLE_STATUS_WAIT_THROW)
	{
		quint8 chRound = (c_chFirPlayerThrow == 0)?c_chRounds[DJGAME_POKER_CLASS(ptrace->chBuf[0])]:c_chRounds[DJGAME_POKER_CLASS(c_chFirPlayerThrow)];
		quint8 ret = GongzhuRule_CheckThrow(pokerimg,
					ptrace->chBuf,ptrace->chBufLen,c_chFirPlayerThrow,c_chFirstPages,
					chRound,c_chCurrentMax);
		if(ret != GONGZHU_RULE_FAILED)
			ptrace->chType = GONGZHU_GAMETRACE_THROW;
		else
		{
			printf("failed\n");
			ptrace->chBufLen = 0;
		}
	}else if(GetTableStatus() == DJGAME_GONGZHU_TABLE_STATUS_WAIT_SHOW)
	{
		if(GONGZHU_ZHU(ptrace->chBuf[0]) || GONGZHU_YANG(ptrace->chBuf[0]) ||
			GONGZHU_BIAN(ptrace->chBuf[0]) || GONGZHU_HA(ptrace->chBuf[0]))
		{
			if( (pokerimg->chPage[ptrace->chBuf[0]] > 0 && 
					GetCardStatus(seat,ptrace->chBuf[0]) == 0))
			{
				PGongzhuShow pshow =(PGongzhuShow)ptrace->chBuf;
				pshow->chCard = ptrace->chBuf[0];
				pshow->chPages = pokerimg->chPage[pshow->chCard];
				ptrace->chType = GONGZHU_GAMETRACE_SHOW;
				ptrace->chBufLen = sizeof(GongzhuShow);
			}
		}else
		{
			printf("show failed\n");
			ptrace->chBufLen = 0;
		}
	}
}
void GongzhuDesktop::MoveItem(Q3CanvasItem* item,QPoint &ps,QPoint &pe)
{
	DJGameDesktop::MoveItem(item,ps,pe);
}

bool GongzhuDesktop::handleItemClicked(Qt::MouseButton btn,const QPoint &pos,Q3CanvasItem * item)
{
	bool handled	= DJGamePokerDesktop::handleItemClicked(btn,pos,item);
	if ( !handled )
		return DJGameDesktop::handleItemClicked(btn,pos,item);
	return handled;
}
/////////////////////////////////////////////////
GongzhuPanel::GongzhuPanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f)
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
	GongzhuDesktop	*gameDesktop = new GongzhuDesktop(gameSplitter,this);
	
	setAccessorySplitter( accessorySplitter );
	setGameDesktop( gameDesktop );	
}
GongzhuPanel::~GongzhuPanel()
{
}
