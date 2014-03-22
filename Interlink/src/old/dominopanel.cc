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
#include "jdebug.h"
#include "panelbase.h"
#include "Utility.h"
#include "DJTreeItemDelegate.h"
#include "dominopanel.h"
#include "dmnrule.h"
#include "dmnprofile.h"
#include "DJMatrixCanvasTimer.h"
#include "DJToolButton.h"
#include "DJGlobal.h" 

void  HandlePlaceACL( const QByteArray& buf, const QVariant& parameters );

void  HandlePlaceACL( const QByteArray& buf, const QVariant& parameters )
{
    PDJGameProtocol pacl = (PDJGameProtocol)buf.data();
    DominoDesktop* desktop = static_cast<DominoDesktop *>(parameters.value<void*>());
    
	desktop->c_bWaitACL = false;
    if(buf.size() == 0)
    {
        return ;
    }
    if( DJGAME_SUCCESS(pacl->chBuf[0]) )
	{
		desktop->c_bPlace = false;
	}
}


///////////////////////////////////////////////////////////////////////////

#define DOMINO_HAND_IMAGE_AVATAR			0x101
#define DOMINO_HAND_IMAGE_CARD			0x102
#define DOMINO_HAND_CARD_HEIGHT					45
#define DOMINO_HAND_CARD_WIDTH					25

DominoHand::DominoHand(Q3Canvas * canvas, QWidget* parent,DominoPanel *panel,unsigned char chSeat,DominoDesktop *desktop)
					 : Q3CanvasView(canvas,parent)
{
	c_Canvas = canvas;
	c_Seat = chSeat;
	c_Desktop = desktop;
	c_Panel = panel;
	
	c_Canvas->setDoubleBuffering(TRUE);
    c_Canvas->setAdvancePeriod(300);
	setHScrollBarMode(Q3ScrollView::AlwaysOff);
	setVScrollBarMode(Q3ScrollView::AlwaysOff);
	setMinimumHeight(85);
	
	memset(c_Cards,DOMINO_CARD_ERRORID,DOMINO_DOUBLE9_CARDS);
	
 	if(c_Seat == c_Panel->selfSeatId())
	{//只有自己的窗口才有draw button
		c_pbDraw = new QPushButton(this);
		QPixmap	pix = QPixmap(":/BaseRes/image/desktop/pushbutton/ZhuaPai.png");
		c_pbDraw->setIcon( pix );
		c_pbDraw->setIconSize( pix.size() );
		c_pbDraw->adjustSize();
		connect(c_pbDraw,SIGNAL(clicked()),this,SLOT(ClickDraw()));
		c_pbDraw->hide();
	}else
		c_pbDraw = NULL;
	
	c_Canvas->setBackgroundColor(QColor(75,130,170));
	QPalette pal = palette();
	pal.setBrush(QPalette::Background,QColor(75,130,170));
	setPalette(pal);
	
	c_UserName = NULL;
	c_Avatar = NULL;
	c_Score = 0;
	c_chPages = 0;
	RepaintAvatar();
	
	c_textScore = new Q3CanvasText("0",c_Canvas);
	c_textScore->setColor(QColor(0,0xFF,0));
	QFont f = c_textScore->font();
	f.setPointSize(32);
	f.setBold(true);
	c_textScore->setFont(f);
	c_textScore->hide();
	c_textScore->setZ(5000);
	c_scoreTimer = new QTimer(this);
    connect(c_scoreTimer,SIGNAL(timeout()),this,SLOT(HandleScoreTimer()));
     
    m_clockItem 	= new DJMatrixCanvasTimer( canvas );
	QMatrix m(0.3,0,0,0.3,0,0);
	m_clockItem->setInnerMatrix( m );
	m_clockItem->setZ(3000);
	m_clockItem->show();
}

void DominoHand::HandleScoreTimer()
{
	c_textScore->hide();
	c_scoreTimer->stop();
}
DominoHand::~DominoHand()
{
}

void DominoHand::GameWait(quint16 mask, quint16 timeout)
{
	quint16 shMask = 1<<(c_Seat-1);
	if( shMask & mask )
	{
		m_clockItem->setTimeout( QList<quint8>(), timeout );
		m_clockItem->move( visibleWidth() - m_clockItem->realSize().width(),
						(visibleHeight() - m_clockItem->realSize().height()) >> 1 );
 	}else
	{
		m_clockItem-> hide();
	}
}
void DominoHand::RecvCards(unsigned char *pcards,unsigned char chPages,bool bReset)
{
	if(*pcards == DOMINO_CARD_BACK && c_Seat == c_Panel->selfSeatId() && !c_Panel->isLookingOn())
		return;
	if(bReset)
	{
		c_chPages = chPages;
		memset(c_Cards,DOMINO_CARD_ERRORID,DOMINO_DOUBLE9_CARDS);
		if(c_chPages > 0)
			memcpy(c_Cards,pcards,chPages);
	}else
	{
		memcpy(&(c_Cards[c_chPages]),pcards,chPages);
		c_chPages += chPages;
	}
	RepaintCards();
}

void DominoHand::RepaintCards()
{
	ClearCards();
	if(c_chPages > 0)
	{
		int i,x,y;
		//DominoImageItem *item;
		QPixmap pix;
		QImage img;
		x = 70;
		int w = width();
		if(w<71+DOMINO_HAND_CARD_WIDTH)
			w = 71+DOMINO_HAND_CARD_WIDTH;
		i = c_chPages/((w-70)/DOMINO_HAND_CARD_WIDTH);
		if(c_chPages%((w-70)/DOMINO_HAND_CARD_WIDTH) != 0)
			i++;
		y = i*DOMINO_HAND_CARD_HEIGHT;
		if(y > c_Canvas->height())
			c_Canvas->resize(c_Canvas->width(),y+10);
		
		y = (c_Canvas->height()-y)>>1;
		char pixname[255];
		for(i=0;i<c_chPages;i++)
		{
			if( c_Cards[i] == DOMINO_CARD_BACK || !DOMINO_ISCARD(c_Cards[i]))
				sprintf(pixname,":/InterlinkRes/image/v/back.png");
			else
				sprintf(pixname,":/InterlinkRes/image/v/%02x.png",c_Cards[i]);
			pix = QPixmap(QString(pixname));
			img = pix.convertToImage();
			DominoImageItem *item = new DominoImageItem(img,c_Canvas,c_Cards[i],y,DOMINO_HAND_IMAGE_CARD);
			item->move(x,y);
			item->setZ(200);
			item->show();
			x += DOMINO_HAND_CARD_WIDTH;
			if(x +DOMINO_HAND_CARD_WIDTH > c_Canvas->width())
			{
				x = 70;
				y += DOMINO_HAND_CARD_HEIGHT;
			}
		}
	}
	c_Canvas->update();
}

void DominoHand::RemoveCard(unsigned char chCard,unsigned char chDesktopNumber)
{
	if(c_pbDraw != NULL)
		c_pbDraw->hide();
	c_bPlace = false;
	if(c_chPages == 0)
		return ;
	if(c_Seat != c_Panel->selfSeatId())
	{
		c_chPages --;
		c_Cards[c_chPages] = DOMINO_CARD_ERRORID;
		RepaintCards();
	}else
	{
		int i;
//		printf("RemoveCard %02x reverse=%02x\n",chCard,DOMINO_CARD_REVERSE(chCard));
		for(i=0;i<c_chPages;i++)
		{
			if( c_Cards[i] == chCard || c_Cards[i] == DOMINO_CARD_REVERSE(chCard))
			{
				c_Cards[i] = c_Cards[c_chPages-1];
				c_Cards[c_chPages-1] = DOMINO_CARD_ERRORID;
				c_chPages --;
				RepaintCards();
				return;
			}
		}
	}
}
void DominoHand::ClearCards()
{
	Q3CanvasItemList list = c_Canvas->allItems();
    Q3CanvasItemList::Iterator it = list.begin();
	DominoImageItem *item;
    while(it != list.end())
    {
    	if((*it)->rtti() == imageRTTI )
		{
			item = (DominoImageItem *)(*it);
        	if ( item->c_chType == DOMINO_HAND_IMAGE_CARD)
           	 	delete item;
        }
		it++;
    }
}

void DominoHand::RepaintAvatar()
{
	quint32 uid;
	DJGameUser *puser;
	QPixmap pix;
	QImage  img;
	QFont f;
	
	if(c_Avatar != NULL)
		delete c_Avatar;
	if(c_UserName != NULL)
		delete c_UserName;
	
	c_Avatar = NULL;
	c_UserName = NULL;
	uid = c_Panel->userIdOfSeat(c_Seat);
	puser = c_Panel->gameUser(uid);
	
	if(puser != NULL)
	{
		c_UserName = new Q3CanvasText(puser->userName(),c_Canvas);
		c_UserName->setColor(QColor(0,0,0xFF));
		f = c_UserName->font();
		f.setPointSize(11);
		f.setBold(false);
		c_UserName->setFont(f);
		c_UserName->move(1,65);
		c_UserName->show();
		pix = puser->avatar();
        img = pix.convertToImage();
        img = img.smoothScale(64,64);
        c_Avatar = new DominoImageItem(img,c_Canvas,0,0,DOMINO_HAND_IMAGE_AVATAR);
		c_Avatar->move(1,1);
		c_Avatar->show();
	}
}

void DominoHand::PlaceCard()
{
}

void DominoHand::WaitPlace()
{
	if(c_pbDraw != NULL)
		c_pbDraw->hide();
	if(c_Seat == c_Panel->selfSeatId() && c_Panel->IsWaitPlace())
		c_bPlace = true;
	else
		c_bPlace = false;
	ClearBoneStatus();
}

unsigned char DominoHand::GetMarkBone()
{
	Q3CanvasItemList list = c_Canvas->allItems();
    Q3CanvasItemList::Iterator it = list.begin();
	DominoImageItem *item;
    while(it != list.end())
    {
    	if((*it)->rtti() == imageRTTI )
		{
			item = (DominoImageItem *)(*it);
        	if ( item->c_chType == DOMINO_HAND_IMAGE_CARD && item->c_bMark)
			{
				return (unsigned char)item->c_chTable;
			}
        }
		it++;
    }
	return DOMINO_CARD_ERRORID;
}
void DominoHand::ClearBoneStatus()
{
	Q3CanvasItemList list = c_Canvas->allItems();
    Q3CanvasItemList::Iterator it = list.begin();
	DominoImageItem *item;
    while(it != list.end())
    {
    	if((*it)->rtti() == imageRTTI )
		{
			item = (DominoImageItem *)(*it);
        	if ( item->c_chType == DOMINO_HAND_IMAGE_CARD)
			{
				item->SetImageMark(false);
			}
        }
		it++;
    }
	c_Desktop->CheckPosition(DOMINO_CARD_ERRORID,NULL,0);
	c_Canvas->update();
}
void DominoHand::WaitDraw()
{
	if(c_Seat == c_Panel->selfSeatId() && c_Panel->IsWaitDraw() && c_pbDraw!=NULL)
		c_pbDraw->show();
}
void DominoHand::resizeEvent( QResizeEvent *e )
{
	djDebug()<<"DominoHand::resizeEvent";
	Q3CanvasView ::resizeEvent( e );
	
	int w=width()-2;
	int h;
	if(w < (70+DOMINO_HAND_CARD_WIDTH+1))
		w = (70+DOMINO_HAND_CARD_WIDTH+1);
	h = (c_chPages/((w-70)/DOMINO_HAND_CARD_WIDTH)) * DOMINO_HAND_CARD_HEIGHT;
	if(h<height()-4)
		h = height()-4;
	if(h < DOMINO_HAND_CARD_HEIGHT+1)
		h = DOMINO_HAND_CARD_HEIGHT+1;
	c_Canvas->resize(w,h);
	
	RepaintCards();
	
	//don't know why can't move here
	//m_clockItem->move( w - m_clockItem->realSize().width(),
		//				(h - m_clockItem->realSize().height()) >> 1 );
	if(c_pbDraw != NULL) {
		//c_pbDraw->setGeometry((width()>>1)-30,(height()>>1)-13,60,26);
		c_pbDraw->move((width()>>1)-30,(height()>>1)-13);
	}
}

void DominoHand::ResetScore()
{
	c_Score = 0;
	ChangeScore(0);
}
void DominoHand::ChangeScore(int s)
{
	c_Score += s;
	quint32 uid;
	QFont f;
	DJGameUser *puser;
	uid = c_Panel->userIdOfSeat(c_Seat);
	puser = c_Panel->gameUser(uid);
	
	if(puser != NULL)
	{
		if(c_UserName != NULL)
			delete c_UserName;
		c_UserName = new Q3CanvasText(puser->userName()+QString("(%1)").arg(c_Score),c_Canvas);
		c_UserName->setColor(QColor(0,0,0xFF));
		f = c_UserName->font();
		f.setPointSize(11);
		f.setBold(false);
		c_UserName->setFont(f);
		c_UserName->move(1,65);
		c_UserName->show();
	}
	if(s == 0)
		return;
	if(s<0)
	{
		if(c_Seat == c_Panel->selfSeatId())
			c_Panel->playWave( QString("dec.wav"));
		c_textScore->setText(QString("%1").arg(s));
		c_textScore->setColor(QColor(0xFF,0,0));
	}else
	{
		if(c_Seat == c_Panel->selfSeatId())
			c_Panel->playWave( QString("inc.wav"));
		c_textScore->setText(QString("+%1").arg(s));
		c_textScore->setColor(QColor(0,0xFF,0));
	}
	QRect rect = c_textScore->boundingRect();
	//int x = (width()-rect.width())>>1;
	//int y = (height()-rect.height())>>1;
	c_textScore->move( (width()-rect.width())>>1, (height()-rect.height())>>1);
	
	c_textScore->show();
	c_scoreTimer->start(3000,false);
	
}

void DominoHand::contentsMouseReleaseEvent(QMouseEvent* e)
{
	if(c_Panel->isLookingOn())
		return;
	if(c_Seat == c_Panel->selfSeatId() && c_chPages>0)
	{
		ClearBoneStatus();
		QPoint pp = inverseWorldMatrix().map(e->pos());
		Q3CanvasItemList l=canvas()->collisions(pp);
		Q3CanvasItemList::Iterator it=l.begin();
		if (it != l.end() && (*it)->rtti() == imageRTTI )
		{
			DominoImageItem *item=(DominoImageItem *)(*it);
        	if ( item->c_chType == DOMINO_HAND_IMAGE_CARD)
			{
				c_Panel->playWave( QString("click.wav"));
				unsigned char chcards[55];
				memcpy(chcards,c_Cards,c_chPages);
				item->SetImageMark(true);
				c_Desktop->CheckPosition((unsigned char)item->c_chTable,chcards,c_chPages,false);
				return;
			}
		}
	}
}

void DominoHand::contentsMouseDoubleClickEvent ( QMouseEvent * e )
{
	if(c_Panel->isLookingOn())
		return;
	if(c_Seat == c_Panel->selfSeatId() && c_chPages>0)
	{
		ClearBoneStatus();
		QPoint pp = inverseWorldMatrix().map(e->pos());
		Q3CanvasItemList l=canvas()->collisions(pp);
		Q3CanvasItemList::Iterator it=l.begin();
		if (it != l.end() && (*it)->rtti() == imageRTTI )
		{
			DominoImageItem *item=(DominoImageItem *)(*it);
        	if ( item->c_chType == DOMINO_HAND_IMAGE_CARD)
			{
				unsigned char chcards[55];
				memcpy(chcards,c_Cards,c_chPages);
				item->SetImageMark(true);
				c_Desktop->CheckPosition((unsigned char)item->c_chTable,chcards,c_chPages,true);
				return;
			}
		}
	}
}

void DominoHand::ClickDraw()
{
	c_pbDraw->hide();
	GeneralGameTrace2Head draw;
	draw.chTable = c_Panel->tableId();
	draw.chType = DOMINO_GAMETRACE_DRAW;
	draw.chSite = c_Panel->selfSeatId();
	draw.chBufLen = 0;
	QByteArray buf	= c_Panel->makeGameData( DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2,QByteArray((char *)&draw,sizeof(GeneralGameTrace2Head)) );
	DJClientRequest request( buf);
	c_Panel->sendRequest( request );
	
}
/*
void DominoHand::HandleTimeout()
{
	c_TimerText->setText(QString("%1").arg(c_Panel->GetWaitTime()));
	itemTimer->show();
	c_TimerText->show();
	c_timer->start(1000,false);
} 
*/
/////////////////////////////////////////////////////////////////////////////
#define DOMINO_CHAIN_LEFT2RIGHT				1
#define DOMINO_CHAIN_BOTTOM2TOP				2
#define DOMINO_CHAIN_RIGHT2LEFT				3
#define DOMINO_CHAIN_TOP2BOTTOM				4
#define DOMINO_CHAIN_ROOT					10

#define DOMINO_DESKTOP_IMAGE_CARD			0x102

DominoDesktop::DominoDesktop(Q3Canvas * canvas, QWidget* parent,DominoPanel *panel)
					: Q3CanvasView(canvas,parent)
{
	c_Panel= panel;
	c_Canvas = canvas;
	
	c_Canvas->setDoubleBuffering(TRUE);
    c_Canvas->setAdvancePeriod(100);
	setHScrollBarMode(Q3ScrollView::AlwaysOff);
	setVScrollBarMode(Q3ScrollView::AlwaysOff);
	
	c_pbStart = new QPushButton(this);
	QPixmap	pix = QPixmap(":/BaseRes/image/desktop/pushbutton/KaiShi.png");
	c_pbStart->setIcon( pix );
	c_pbStart->setIconSize( pix.size() );
	c_pbStart->adjustSize();
	connect(c_pbStart,SIGNAL(clicked()),this,SLOT(ClickStart()));
	c_pbStart->hide();
	
	c_Fx = 100;
	
	c_resizeTimer = new QTimer(this);
	c_resizeTimer->setInterval( 500 );
	c_resizeTimer->setSingleShot( true );
	connect(c_resizeTimer,SIGNAL(timeout()),this,SLOT(HandleResizeTimeout())); 
	
	QColor color	=  GetSettingGameDesktopColor (c_Panel->gameController()->gameName());
	canvas->setBackgroundColor(color);
	QPalette pal = palette();
	pal.setBrush(QPalette::Background,color);
	setPalette(pal); 
	
	memset(&c_chains,DOMINO_CARD_ERRORID,sizeof(DominoChains));
	c_chains.chChainPages = 0;
	c_textDN = NULL;
	c_textYard = NULL;
}
DominoDesktop::~DominoDesktop()
{
}
void DominoDesktop::HandleResizeTimeout()
{
	QPixmap pix( ":/InterlinkRes/image/base/background.png");
	if ( !pix.isNull() ) {
		pix	= pix.scaled( visibleWidth(),visibleHeight(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
		canvas()->setBackgroundPixmap( pix );
	}
}
void DominoDesktop::CheckPosition(unsigned char chCard,unsigned char *pchCards,unsigned char chPages,bool bSend)
{
	DominoChains chains;
	memcpy(&chains,&c_chains,sizeof(DominoChains));
	
	Q3CanvasItemList list = c_Canvas->allItems();
    Q3CanvasItemList::Iterator it = list.begin();
	DominoImageItem *item;
    while(it != list.end())
    {
    	if((*it)->rtti() == imageRTTI )
		{
			item = (DominoImageItem *)(*it);
        	if ( item->c_chType >= DOMINO_DESKTOP_IMAGE_CARD && item->c_chType <= DOMINO_DESKTOP_IMAGE_CARD + 4
					&& item->c_chTable == DOMINO_CARD_NULL  )
			{
				unsigned char chChainID = item->c_chType-DOMINO_DESKTOP_IMAGE_CARD;
				if(AppendCard2Chain(&chains,chChainID,chCard,pchCards,chPages,false))
				{
					item->SetImageMark(true);
					memcpy(&chains,&c_chains,sizeof(DominoChains));
					if(bSend)
					{
						SendPlace(chCard,chChainID,pchCards,chPages);
					}
				}else
				{
					item->SetImageMark(false);
				}
			}
        }
		it++;
    }
}

bool DominoDesktop::SendPlace(unsigned char chCard,unsigned char chChainID,unsigned char *pchCards,unsigned char chPages)
{
	DominoChains chains;
	if(c_bWaitACL || !c_bPlace)
		return true;
	memcpy(&chains,&c_chains,sizeof(DominoChains));
	
	if(AppendCard2Chain(&chains,chChainID,chCard,pchCards,chPages,false))
	{
		c_bWaitACL = true;
		
		unsigned char buffer[50];
		GeneralGameTrace2Head *ptrace;
		PDominoPlace			pplace;
		ptrace = (GeneralGameTrace2Head *)buffer;
		ptrace->chTable = c_Panel->tableId();
		ptrace->chSite = c_Panel->selfSeatId();
		ptrace->chType = DOMINO_GAMETRACE_PLACE;
		ptrace->chBufLen = sizeof(DominoPlace);
		pplace = (PDominoPlace )ptrace->chBuf;
		pplace->chChainID = chChainID;
		pplace->chCard = chCard;

		QByteArray buf	= c_Panel->makeGameData( DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2,QByteArray((char *)buffer,sizeof(GeneralGameTrace2Head)+ptrace->chBufLen) );
		QVariant parameters	= QVariant::fromValue((void*)this);
		DJClientRequest request( buf ,HandlePlaceACL,parameters);
		c_Panel->sendRequest( request );
		return true;
	}
	return false;
}

bool DominoDesktop::RepaintLink()
{
	ClearLink(0);
	DominoImageItem *rootitem;
	
	if(!DOMINO_ISCARD(c_chains.root))
	{
		DrawBone(c_Canvas->width()>>1,c_Canvas->height()>>1,DOMINO_CARD_NULL,0,DOMINO_CHAIN_ROOT);
		return true;
	}
	unsigned char chRootCard = DOMINO_CARD_REVERSE(c_chains.root);
	rootitem = DrawBone(c_Canvas->width()>>1,c_Canvas->height()>>1,chRootCard,0,DOMINO_CHAIN_ROOT);
	if(rootitem == NULL)
		return false;
	int x,y;
	QRect rect;
	unsigned char i,max=4;
//	printf("repaint link root=%02x, chain1=%02x, chain2=%02x\n",c_chains.root,
//				c_chains.chLastCard[1],c_chains.chLastCard[2]);
	if(DOMINO_CARD_GET1(c_chains.root) != DOMINO_CARD_GET2(c_chains.root) ||
		!DOMINO_ISCARD(c_chains.chLastCard[1]) || !DOMINO_ISCARD(c_chains.chLastCard[2]) )
		max = 2;
	
	for(i=1;i<=max;i++)
	{
		switch(i)
		{
			case 1 :
			{
				x = ((int)rootitem->x())+rootitem->width();
				y = ((int)rootitem->y())+(rootitem->height()>>1);
				rect.setLeft(x+c_Fx/5);
				rect.setTop(0);
				rect.setWidth((width()>>1)-c_Fx/5);
				rect.setHeight(height()>>1);
				if(!RepaintSingleChain(1,x,y,DOMINO_CHAIN_LEFT2RIGHT,rect) && c_Fx>10)
				{
					c_Fx -= 10;
					ClearLink(0);
					rootitem = DrawBone(c_Canvas->width()>>1,c_Canvas->height()>>1,chRootCard,0,DOMINO_CHAIN_ROOT);

					i = 0;
				}
				break;
			}
			case 2 :
			{
				x = ((int)rootitem->x());
				y = ((int)rootitem->y())+(rootitem->height()>>1);
				rect.setLeft(0);
				rect.setTop((height()>>1));
				rect.setWidth((width()>>1)-c_Fx/5);
				rect.setHeight(height()>>1);
				if(!RepaintSingleChain(2,x,y,DOMINO_CHAIN_RIGHT2LEFT,rect) && c_Fx>10)
				{
					c_Fx -= 10;
					ClearLink(0);
					rootitem = DrawBone(c_Canvas->width()>>1,c_Canvas->height()>>1,chRootCard,0,DOMINO_CHAIN_ROOT);

					i = 0;
				}
				break;
			}
			case 3 :
			{
				x = ((int)rootitem->x())+(rootitem->width()>>1);
				y = ((int)rootitem->y());
				rect.setLeft(0);
				rect.setTop(0);
				rect.setWidth(width()>>1);
				rect.setHeight(height()>>1);
				if(!RepaintSingleChain(3,x,y,DOMINO_CHAIN_BOTTOM2TOP,rect) && c_Fx>10)
				{
					c_Fx -= 10;
					ClearLink(0);
					rootitem = DrawBone(c_Canvas->width()>>1,c_Canvas->height()>>1,chRootCard,0,DOMINO_CHAIN_ROOT);

					i = 0;
				}
				break;
			}
			case 4 :
			{
				x = ((int)rootitem->x())+(rootitem->width()>>1);
				y = ((int)rootitem->y())+(rootitem->height());
				rect.setLeft(width()>>1);
				rect.setTop(height()>>1);
				rect.setWidth(width()>>1);
				rect.setHeight(height()>>1);
				if(!RepaintSingleChain(4,x,y,DOMINO_CHAIN_TOP2BOTTOM,rect) && c_Fx>10)
				{
					c_Fx -= 10;
					ClearLink(0);
					rootitem = DrawBone(c_Canvas->width()>>1,c_Canvas->height()>>1,chRootCard,0,DOMINO_CHAIN_ROOT);

					i = 0;
				}
				break;
			}
		}//switch
	}//for
	c_Canvas->update();
	return true;
}

void DominoDesktop::WaitPlace()
{
	if(c_Panel->IsWaitPlace())
		c_bPlace = true;
	else
		c_bPlace = false;
	c_bWaitACL = false;
}

bool DominoDesktop::RepaintSingleChain(unsigned char chChainID,int x,int y,unsigned char chOri,QRect & rect)
{
	unsigned char chCard,chPages=0;
	char chStart = 0;
	int iCardMaxSize = (42*c_Fx/100);
//	iCardMaxSize += (iCardMaxSize>>1);
	
	DominoImageItem *item;
	do
	{
		chCard = GetCardOnChain(&c_chains,chChainID,1,&chStart);
		if(!DOMINO_ISCARD(chCard))
			break;
		chPages++;
		if(chOri == DOMINO_CHAIN_RIGHT2LEFT || chOri == DOMINO_CHAIN_TOP2BOTTOM)
			chCard = DOMINO_CARD_REVERSE(chCard);
		item = DrawBone(x,y,chCard,chChainID,chOri);
		switch(chOri)
		{
			case DOMINO_CHAIN_LEFT2RIGHT :
			{
				if(x+item->width() > rect.right()-iCardMaxSize-(iCardMaxSize>>1))
				{
					chOri = DOMINO_CHAIN_BOTTOM2TOP;
					x += (item->width()-(item->width()>>2));
					y -= (item->height()>>1);
					if(rect.bottom()-rect.top() < (iCardMaxSize<<1))
						return false;
					rect.setBottom(rect.bottom()-(iCardMaxSize>>1));
				}else
					x += item->width();
				break;
			}
			case DOMINO_CHAIN_RIGHT2LEFT :
			{
				if(x-item->width() < rect.left()+iCardMaxSize+(iCardMaxSize>>1))
				{
					chOri = DOMINO_CHAIN_TOP2BOTTOM;
					x -=(item->width()-(item->width()>>2));
					y += (item->height()>>1);
					if(rect.bottom()-rect.top() < (iCardMaxSize<<1))
						return false;
					rect.setTop(rect.top()+(iCardMaxSize>>1));
				}else
					x -= item->width();
				break;
			}
			case DOMINO_CHAIN_BOTTOM2TOP :
			{
				if(y-item->height() < rect.top()+iCardMaxSize+(iCardMaxSize>>1))
				{
					chOri = DOMINO_CHAIN_RIGHT2LEFT;
					y -= (item->height()-(item->height()>>2));
					x -= (item->width()>>1);
					if(rect.right()-rect.left() < (iCardMaxSize<<1))
						return false;
					rect.setRight(rect.right()-(iCardMaxSize>>1));
				}else
					y -= item->height();
				break;
			}
			case DOMINO_CHAIN_TOP2BOTTOM :
			{
				if(y+item->height() > rect.bottom()-iCardMaxSize-(iCardMaxSize>>1))
				{
					chOri = DOMINO_CHAIN_LEFT2RIGHT;
					x += (item->width()>>1);
					y += (item->height()-(item->height()>>2));
					if(rect.right()-rect.left() < (iCardMaxSize<<1))
						return false;
					rect.setLeft(rect.left()+(iCardMaxSize>>1));
				}else
					y += item->height();
				break;
			}
		}
	}while(true);
	DrawBone(x,y,DOMINO_CARD_NULL,chChainID,chOri);
	return true;
}

DominoImageItem * DominoDesktop::FindCard(unsigned char chCard)
{
	Q3CanvasItemList list = c_Canvas->allItems();
    Q3CanvasItemList::Iterator it = list.begin();
	DominoImageItem *item;
    while(it != list.end())
    {
    	if((*it)->rtti() == imageRTTI )
		{
			item = (DominoImageItem *)(*it);
        	if ( item->c_chType >= DOMINO_DESKTOP_IMAGE_CARD && item->c_chType <= DOMINO_DESKTOP_IMAGE_CARD + 4
					&& (item->c_chTable == chCard || item->c_chTable == DOMINO_CARD_REVERSE(chCard) ) )
				return item;
        }
		it++;
    }
	return NULL;
}

void DominoDesktop::ClearLink(unsigned char chChainID)
{
	Q3CanvasItemList list = c_Canvas->allItems();
    Q3CanvasItemList::Iterator it = list.begin();
	DominoImageItem *item;
    while(it != list.end())
    {
    	if((*it)->rtti() == imageRTTI )
		{
			item = (DominoImageItem *)(*it);
        	if ( ( (chChainID==0 || chChainID>4) && item->c_chType >= DOMINO_DESKTOP_IMAGE_CARD && item->c_chType <= DOMINO_DESKTOP_IMAGE_CARD + 4) 
						|| item->c_chType == DOMINO_DESKTOP_IMAGE_CARD+chChainID)
           	 	delete item;
        }
		it++;
    }
}
DominoImageItem * DominoDesktop::DrawBone(int x,int y,unsigned char chCard,unsigned char chChainID,unsigned char chOri)
{
	char cardname[255],vh[2]={0,0};
	switch(chOri)
	{
		case DOMINO_CHAIN_LEFT2RIGHT :
		case DOMINO_CHAIN_RIGHT2LEFT :
			if(DOMINO_ISCARD(chCard) && DOMINO_CARD_GET1(chCard) == DOMINO_CARD_GET2(chCard))
				vh[0] ='v';
			else
				vh[0] = 'h';
			break;
		case DOMINO_CHAIN_ROOT :
		{
			if(chCard == DOMINO_CARD_NULL || DOMINO_CARD_GET1(chCard) == DOMINO_CARD_GET2(chCard))
				vh[0] ='v';
			else
				vh[0] ='h';
			break;
		}
		case DOMINO_CHAIN_BOTTOM2TOP :
		case DOMINO_CHAIN_TOP2BOTTOM :
			if(DOMINO_ISCARD(chCard) && DOMINO_CARD_GET1(chCard) == DOMINO_CARD_GET2(chCard))
				vh[0] ='h';
			else
				vh[0] = 'v';
			break;
	}
	if(chCard == DOMINO_CARD_NULL)
		sprintf(cardname,":/InterlinkRes/image/%s/null.png",vh);
	else if( chCard == DOMINO_CARD_BACK || !DOMINO_ISCARD(chCard))
		sprintf(cardname,":/InterlinkRes/image/%s/back.png",vh);
	else
		sprintf(cardname,":/InterlinkRes/image/%s/%02x.png",vh,chCard);
	
	QPixmap pix = QPixmap(QString(cardname));
	QImage	img = pix.convertToImage();
	img = img.smoothScale(img.width()*c_Fx/100,img.height()*c_Fx/100);
	DominoImageItem *item = new DominoImageItem(img,c_Canvas,chCard,y,DOMINO_DESKTOP_IMAGE_CARD+chChainID);
	switch(chOri)
	{
		case DOMINO_CHAIN_ROOT	:
		{
			x -= (item->width()>>1);
			y -= (item->height()>>1);
			break;
		}
		case DOMINO_CHAIN_LEFT2RIGHT :
			y -= (item->height()>>1);
			break;
		case DOMINO_CHAIN_BOTTOM2TOP :
			y -= item->height();
			x -= (item->width()>>1);
			break;
		case DOMINO_CHAIN_RIGHT2LEFT :
			y -= (item->height()>>1);
			x -= item->width();
			break;
		case DOMINO_CHAIN_TOP2BOTTOM :
			x -= (item->width()>>1);
			break;
	}
	item->move(x,y);
	item->setZ(200);
	item->show();
	return item;
}
void DominoDesktop::ClearAllBones()
{
}
void DominoDesktop::PlayerPlace(unsigned char chSite,unsigned char chChainID,unsigned char chCard)
{
}
void DominoDesktop::AdjustLink(PDominoChains pchains)
{
}
void DominoDesktop::WaitStart()
{
	if(c_Panel->WaittingForMe())
		c_pbStart->show();
}

PDominoChains DominoDesktop::GetDominoChains()
{
	return &c_chains;
}

void DominoDesktop::ClickStart()
{
	memset(&c_chains,DOMINO_CARD_ERRORID,sizeof(DominoChains));
	c_chains.chChainPages = 0;
	
	c_pbStart->hide();
	//c_wHelp->setText( tr("waiting for other player"));
	QByteArray buf	= c_Panel->makeGameData( DJGAME_PROTOCOL_SUBTYPE_START, QByteArray() );
	DJClientRequest request( buf );
		
    c_Panel->sendRequest( request );

}
bool DominoDesktop::hasStarted()
{
	return !c_pbStart->isVisible();
}
void DominoDesktop::resizeEvent( QResizeEvent * )
{
	int w,h;
	w = width();
	h = height();
	c_Canvas->resize(width(),height());
	c_Fx = 100;
	RepaintLink();
	c_pbStart->move((width()>>1)-40,(height()>>1)-13);
	c_resizeTimer->start();
}
void DominoDesktop::contentsMouseReleaseEvent(QMouseEvent* e)
{
	if(c_Panel->isLookingOn())
		return;
	if(c_Panel->IsWaitPlace() && c_bPlace && !c_bWaitACL)
	{
		unsigned char chCard = (c_Panel->GetSelfHand())->GetMarkBone();
		unsigned char cards[2]={DOMINO_CARD_ERRORID,DOMINO_CARD_ERRORID};
		if(!DOMINO_ISCARD(chCard))
			return;
		cards[0] = chCard;
		QPoint pp = inverseWorldMatrix().map(e->pos());
		Q3CanvasItemList l=canvas()->collisions(pp);
		Q3CanvasItemList::Iterator it=l.begin();
		if (it != l.end() && (*it)->rtti() == imageRTTI )
		{
			DominoImageItem *item=(DominoImageItem *)(*it);
        	if ( item->c_chType >= DOMINO_DESKTOP_IMAGE_CARD && item->c_chType <= DOMINO_DESKTOP_IMAGE_CARD + 4
					&& item->c_chTable == DOMINO_CARD_NULL  )
			{
				unsigned char chChainID = item->c_chType-DOMINO_DESKTOP_IMAGE_CARD;
				SendPlace(chCard,chChainID,cards,1);
			}
		}
	}
}

void DominoDesktop::contentsDragMoveEvent ( QDragMoveEvent * e )
{
}

void DominoDesktop::ShowDesktopNumber(unsigned char chNumber)
{
	QFont f;
	if(c_textDN != NULL)
		delete c_textDN;

	c_textDN = new Q3CanvasText(tr("Desktop Number : ")+QString("%1").arg(chNumber),c_Canvas);
	c_textDN->setColor(QColor(0xFF,0,0));
	f = c_textDN->font();
	f.setPointSize(13);
	f.setBold(true);
	c_textDN->setFont(f);
	c_textDN->move(1,1);
	c_textDN->show();
}

void DominoDesktop::ShowBoneYard(unsigned char chYard)
{
	QFont f;
	if(c_textYard != NULL)
		delete c_textYard;

	c_textYard = new Q3CanvasText(QString("%1").arg(chYard)+tr(" Bones in bonesyard"),c_Canvas);
	c_textYard->setColor(QColor(0xFF,0,0));
	f = c_textYard->font();
	f.setPointSize(13);
	f.setBold(true);
	c_textYard->setFont(f);
	QRect rect=c_textYard->boundingRect();
	c_textYard->move(width()-rect.width()-5,1);
	c_textYard->show();
}
void DominoDesktop::changeBackgroundColor( const QColor& color )
{
	canvas()->setBackgroundColor( color );
	QPalette pal = palette();
	pal.setBrush(QPalette::Background,color);
	setPalette(pal);
	SetSettingGameDesktopColor ( c_Panel->gameController()->gameName(), color );
}
///////////////////////////////////////////////////////////////////////////

DominoPanel::DominoPanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f)
			: DJGamePanel( gameController, room, userIds, tableId, seatId, parent, f )
{
	//char chDbgFile[60];
	//sprintf(chDbgFile,"Xiangqi_%d_%d.dbg",tableId,seatId);
	//DEBUG_INITFILE((chDbgFile));
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
	
	setAccessorySplitter( accessorySplitter ); 
	
	createPanelBar(gameSplitter); 
#ifdef DJ_OEM
	color()->hide();
#endif	
	int i;
	for(i=0;i<5;i++)
	{
		c_HandCanvas[i] = NULL;
		c_Hand[i] = NULL;
	}
	//s3->setHandleWidth(1);
	QSplitter *s4 = new QSplitter(Qt::Horizontal , gameSplitter);
	
	c_DesktopCanvas = new  Q3Canvas(this);
	c_DesktopCanvas->resize(100,100);
	c_Desktop = new DominoDesktop(c_DesktopCanvas,gameSplitter,this);
	connect( this, SIGNAL(changeBackgroundColor(const QColor&)), c_Desktop, SLOT(changeBackgroundColor(const QColor&)) );

	i = selfSeatId();
	s4->setHandleWidth(1);
	c_HandCanvas[i] = new  Q3Canvas(this);
	c_Hand[i] = new DominoHand(c_HandCanvas[i],s4,this,i,c_Desktop);
//	printf("Create %d hand\n",i);
	if(i<= 2 )
	{
		i = (i==2)?1:2;
	}else
	{
		i = (i==3)?4:3;
	}
	c_HandCanvas[i] = new  Q3Canvas(this);
	c_Hand[i] = new DominoHand(c_HandCanvas[i],s4,this,i,c_Desktop);
//	printf("Create %d hand\n",i);
	
	if(room->numberOfSeats() == 4)
	{
		QSplitter *s5 = new QSplitter(Qt::Horizontal , gameSplitter);
		s5->setHandleWidth(1);
		i = (i<3)?3:1;
		c_HandCanvas[i] = new  Q3Canvas(this);
		c_Hand[i] = new DominoHand(c_HandCanvas[i],s5,this,i,c_Desktop);
		printf("Create %d hand\n",i);
		i =(i==1)?2:4;
		c_HandCanvas[i] = new  Q3Canvas(this);
		c_Hand[i] = new DominoHand(c_HandCanvas[i],s5,this,i,c_Desktop);
		printf("Create %d hand\n",i);
	}
	
	memset(&c_Current,0,sizeof(DominoCurrent));

	c_chTableStatus = 0;
	c_shWaitMask 	= 0; 
	c_iTimeLin 	= 0;
}
void DominoPanel::createAccessoryWidgets()
{
	QSplitter	*splitter	= accessorySplitter();
	if ( splitter ) {
		createPlayersWidget( splitter );
		createChatWidget( splitter );
	}
}
void DominoPanel::userArrived( quint32 userId, quint8 seatId )
{
	DJGamePanel::userArrived( userId, seatId );
	c_Hand[seatId]->RepaintAvatar();
}
void DominoPanel::gameInfo( const QByteArray& buf )
{
	//bool bRepaint=false;
	//DominoCurrent *pnc =(DominoCurrent*)buf.data();

	memcpy(&c_Current,buf.data(),sizeof(DominoCurrent));
	
}

void DominoPanel::gameWait( quint16 mask, quint8 status ,quint16 timeout )
{
	c_chTableStatus = status;
	c_shWaitMask = mask;
	c_iTimeLin = time(NULL)+timeout;
//	c_Desktop->GameWait();
	DJGameRoom *room = gameRoom();
	int i;
	for(i=1;i<=room->numberOfSeats();i++)
	{
		c_Hand[i]->GameWait(mask,timeout);
	}
	switch(status)
	{
		case  DJGAME_TABLE_STATUS_WAITSTART :
			c_Desktop->WaitStart();
			break;
		case DJGAME_DOMINO_TABLE_STATUS_WAIT_PLACE :
		{
			c_Desktop->WaitPlace();
			for(i=1;i<=room->numberOfSeats();i++)
			{
				c_Hand[i]->WaitPlace();
			}
			break;
		}
		case DJGAME_DOMINO_TABLE_STATUS_WAIT_DRAW :
		{
			for(i=1;i<=room->numberOfSeats();i++)
			{
				c_Hand[i]->WaitDraw();
			}
			break;
		}
	}
}
quint16 DominoPanel::GetWaitTime()
{
	int curt = time(NULL);
	if(curt < c_iTimeLin)
		return c_iTimeLin-curt;
	return 0;
}
void DominoPanel::gameTrace( const GeneralGameTrace2Head *gameTrace )
{
	//GeneralGameTrace2Head *trace	= const_cast<GeneralGameTrace2Head *>(gameTrace);
	
	switch( gameTrace->chType ) 
	{
		case DOMINO_GAMETRACE_PICKUP :
		{
			PDominoPickup ppickup = (PDominoPickup)gameTrace->chBuf;
			c_Desktop->ShowBoneYard(ppickup->chReserves);
			c_Hand[gameTrace->chSite]->RecvCards(ppickup->chCards,ppickup->chPages,!(ppickup->chProperties&DOMINO_PICKUP_PROPERTIES_APPEND));
			break;
		}
		case DOMINO_GAMETRACE_PLACE :
		{
			playWave( QString("move.wav"));
			PDominoPlace pplace = (PDominoPlace)gameTrace->chBuf;
			c_Hand[gameTrace->chSite]->RemoveCard(pplace->chCard,pplace->chDesktopNumber);
			PDominoChains pchains = c_Desktop->GetDominoChains();
			c_Desktop->c_bPlace = false;
			if( !DOMINO_ISCARD(pchains->root))
			{
				pchains->root = pplace->chCard;
				pchains->chChainPages = 0;
			}else
			{
	//			printf("%d place chainid=%d card=%02x\n",gameTrace->chSite,pplace->chChainID,pplace->chCard);
				unsigned char chCard = pplace->chCard;
				pchains->chain[pchains->chChainPages][DOMINOCHAINS_CHAIN_ID] = pplace->chChainID;
				pchains->chain[pchains->chChainPages][DOMINOCHAINS_CHAIN_CARD] = chCard;
				pchains->chLastCard[pplace->chChainID] = chCard;
				pchains->chChainPages ++;
			}
			c_Desktop->RepaintLink();
			if(pplace->chDesktopNumber > 0 && (pplace->chDesktopNumber%5) == 0)
			{
				DJGameRoom *room = gameRoom();
				int i,s=pplace->chDesktopNumber;
				for(i=1;i<=room->numberOfSeats();i++)
				{
					if( (i&1) == (gameTrace->chSite&1))
						c_Hand[i]->ChangeScore(s);
					else
						c_Hand[i]->ChangeScore(-s);
				}
			}
			c_Desktop->ShowDesktopNumber(pplace->chDesktopNumber);
			break;
		}
		case DOMINO_GAMETRACE_ADJUST :
		{
			PDominoChains pchains = c_Desktop->GetDominoChains();
			memcpy(pchains,gameTrace->chBuf,sizeof(DominoChains));
			c_Desktop->RepaintLink();
			break;
		}
	}
}
void DominoPanel::magicResult( const MagicResult* magicResult )
{
}
void DominoPanel::gameOver( const DJGameOver* gameOver )
{
	QString result;
	
//	if(gameOver->chSites == 2 && 
//		gameOver->site[0].score == gameOver->site[1].score)
//	{
//		res = tr("dues.");
//	}else
	//{
		for( int i=0;i<gameOver->chSites;i++)
		{
			DJGameUser *puser = userAtSeat(gameOver->site[i].chSite);
			int score = (int)(letoh4(gameOver->site[i].score));
			if(puser)
			{
				deltaUserData( puser, score );
				c_Hand[gameOver->site[i].chSite]->ResetScore();
				result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( puser->userName() )
								.arg( score );
			}
		}
	//}
	insertGameResult2Browser( result );
	c_Desktop->ShowDesktopNumber(0);
}

DominoHand * DominoPanel::GetSelfHand()
{
	return c_Hand[selfSeatId()];
}
bool DominoPanel::IsWaitPlace()
{
	if(!WaittingForMe())
		return false;
	return (c_chTableStatus == DJGAME_DOMINO_TABLE_STATUS_WAIT_PLACE);
}

bool DominoPanel::IsWaitDraw()
{
	if(!WaittingForMe())
		return false;
	return (c_chTableStatus == DJGAME_DOMINO_TABLE_STATUS_WAIT_DRAW);
}

///////////////////////////////////////////////////////
bool DominoPanel::WaittingForMe()
{
	if(c_chTableStatus != DJGAME_TABLE_STATUS_WAITSTART &&
			c_chTableStatus != DJGAME_DOMINO_TABLE_STATUS_WAIT_PLACE &&
			c_chTableStatus != DJGAME_DOMINO_TABLE_STATUS_WAIT_DRAW )
		return false;
	quint16 shMyMask = 1<<(selfSeatId()-1);
	return (shMyMask & c_shWaitMask);
}
void DominoPanel::closeEvent( QCloseEvent *e)
{
	if ( isLookingOn() ) {
		//look on
		QByteArray data;
		data.append( tableId() );
		QByteArray buf	= makeGameData( DJGAME_PROTOCOL_SUBTYPE_QUITVIEW, data );
		DJClientRequest request( buf );
		sendRequest( request );
		e->accept();
		return;
	}
    if(c_chTableStatus == DJGAME_TABLE_STATUS_WAITSTART && !c_Desktop->hasStarted() )
    {
    	QByteArray buf	= makeGameData( DJGAME_PROTOCOL_SUBTYPE_QUIT, QByteArray() );
		DJClientRequest request( buf );
		sendRequest( request );
		e->accept();
    }else {
    	e->ignore();
    }
			
}
	
DominoPanel::~DominoPanel()
{
}
