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

//Please visit http://www.bluedj.com for more infomation about us
//and contact us at ggwizard@gmail.com or darkdong@gmail.com
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
#include "shsspanel.h"
#include "shssrule.h"
#include "shssprofile.h"
#include "DJGameRank.h"
#include "DJMatrixCanvasTimer.h"
#include "DJGlobal.h" 

static int	CLOCK_POSITION_X	=	50;
static int	CLOCK_POSITION_Y	=	0;

SHSSLifeItem::SHSSLifeItem( Q3Canvas * canvas) : Q3CanvasLine(canvas)
{
}

SHSSLifeItem::~SHSSLifeItem()
{
}
///////////////////////////////////////////////////////


#define SHISENSHO_CARD_WIDTH						51
#define SHISENSHO_CARD_HEIGHT						52

#define SHISENSHO_POSTYPE_LEFTTOP					1
#define SHISENSHO_POSTYPE_LEFTCENTER				2
#define SHISENSHO_POSTYPE_LEFTBOTTOM				3
#define SHISENSHO_POSTYPE_RIGHTTOP					4
#define SHISENSHO_POSTYPE_RIGHTCENTER				5
#define SHISENSHO_POSTYPE_RIGHTBOTTOM				6
#define SHISENSHO_POSTYPE_CENTERTOP					7
#define SHISENSHO_POSTYPE_CENTER					8
#define SHISENSHO_POSTYPE_CENTERBOTTOM				9

#define SHISENSHO_DESKTOP_HEADSIZE					70

#define SHISENSHO_DESKTOP_IMAGETYPE_CARD			0x101
#define SHISENSHO_DESKTOP_IMAGETYPE_LIFE			0x102

void  HandleDeleteACL( const QByteArray& buf, const QVariant& parameters );
void  HandleAdjustACL( const QByteArray& buf, const QVariant& parameters );

SHSSDesktop::SHSSDesktop(Q3Canvas * canvas, QWidget* parent,SHSSPanel *panel) :
			Q3CanvasView(canvas,parent)
{
	c_Canvas = canvas;
	c_Panel = panel;
	memset(c_chMap,0,SHISENSHO_MAX_MAP+8);
	c_pbStart = new QPushButton(tr("Start"),this);
	connect(c_pbStart,SIGNAL(released()),this,SLOT(ClickStart()));
	c_pbStart->hide();
	
	c_pbReset = new QPushButton(tr("Reset"),this);
	connect(c_pbReset,SIGNAL(released()),this,SLOT(ClickReset()));
	c_pbReset->hide();
	
	c_Canvas->setDoubleBuffering(TRUE);
    c_Canvas->setAdvancePeriod(300);
	setVScrollBarMode(AlwaysOff);
    setHScrollBarMode(AlwaysOff);
	
	/*
	c_paTimer = InitTimerMovie();
	c_itemTimer = new Q3CanvasSprite( c_paTimer,c_Canvas);
	c_itemTimer->setFrameAnimation ( );
	c_itemTimer->setAnimated(TRUE);
	c_itemTimer->setZ(2000);
	c_itemTimer->move(70,20);
	c_itemTimer->show();
	
	c_TimerText = new Q3CanvasText(c_Canvas);
	QFont f = c_TimerText->font();
	f.setBold(true);
	f.setPointSize (12);
	c_TimerText->setFont(f);
	c_TimerText->setColor(QColor(0xFF,0,0));
	c_TimerText->setText(QString("0"));
	c_TimerText->move(102,25);
	c_TimerText->show();
	c_TimerText->setZ(2000);
	*/
	m_clockItem 	= new DJMatrixCanvasTimer( canvas );
	QMatrix m(0.4,0,0,0.4,0,0);
	m_clockItem->setInnerMatrix( m );
	m_clockItem->setZ(3000);
	m_clockItem->move(CLOCK_POSITION_X,CLOCK_POSITION_Y);
	m_clockItem->show();
	
	QFont f;
	f.setBold(true);
	f.setPointSize (18);
	c_ScoreText = new Q3CanvasText(c_Canvas);
	c_ScoreText->move(200,15);
	c_ScoreText->setFont(f);
	c_ScoreText->setColor(QColor(0,0xFF,0));
	c_ScoreText->setZ(2000);
	SetScore(0);
	c_ScoreText->show();
	
	//c_timer = new QTimer(this);
    //connect(c_timer,SIGNAL(timeout()),this,SLOT(HandleTimeout()));
	
	
	c_deleteTimer = new QTimer(this);
    connect(c_deleteTimer,SIGNAL(timeout()),this,SLOT(HandleDeleteTimeout()));
	
	/*
	c_Canvas->setBackgroundColor(QColor(75,130,170));
	QPalette pal = palette();
	pal.setBrush(QPalette::Background,QColor(75,130,170));
	setPalette(pal);
	*/
	QColor color	=  GetSettingGameDesktopColor (c_Panel->gameController()->gameName());
	canvas->setBackgroundColor(color);
	QPalette pal = palette();
	pal.setBrush(QPalette::Background,color);
	setPalette(pal); 
	
	c_shSkin = 1;
	SetACLStatus(false);
	memset(&c_CurDel,0,sizeof(ShisenshoDelete));
	c_iScore = 0;
	
	c_itemLifeLine = new SHSSLifeItem(c_Canvas);
	c_itemLifeLine->setZ(2000);
	c_itemLifeLine->hide();
	c_itemLifeRect = new Q3CanvasRectangle(c_Canvas);
	c_itemLifeRect->setPen(QPen(QColor(0,0,255),2));
	c_itemLifeRect->move(150,50);
	c_itemLifeRect->hide();
	
	c_Avatar = NULL;
	c_UserName = NULL;
	RepaintAvatar();
	
	startTimer(1000);
}
void SHSSDesktop::changeBackgroundColor( const QColor& color )
{
	canvas()->setBackgroundColor( color );
	QPalette pal = palette();
	pal.setBrush(QPalette::Background,color);
	setPalette(pal);
	SetSettingGameDesktopColor ( c_Panel->gameController()->gameName(), color );
}
void SHSSDesktop::ModifyLifeLine()
{
	ShisenshoCurrent current = c_Panel->GetCurrentInfo();
	unsigned short shWait = c_Panel->GetWaitTime();
	unsigned short shTimeout = letoh2(current.rule.timeout);
	if(shTimeout == 0)
	{
		shTimeout = 500;
		shWait = 500;
	}
	unsigned char chG = (255*shWait)/shTimeout;
	int l = ((width()-324)*shWait)/shTimeout;
	c_itemLifeLine->setPen(QPen(QColor(255-chG,chG,0),22));
	c_itemLifeLine->setPoints(162,62,162+l,62);
	c_itemLifeLine->show();
}
void SHSSDesktop::RepaintLifes()
{//SHISENSHO_DESKTOP_IMAGETYPE_LIFE
	quint32 uid;
	DJGameUser *puser;
	QFont f;
	QPixmap pix;
	QImage  img;
	SHSSImageItem *item;
	ShisenshoCurrent current = c_Panel->GetCurrentInfo();
	
	uid = c_Panel->userIdOfSeat(1);
	puser = c_Panel->gameUser(uid);
	if(puser != NULL)
	{
		ClearLifes();
		c_itemLifeRect->setSize(width()-300,24);
		c_itemLifeRect->show();
		ModifyLifeLine();
		
		if(current.rule.chLifes == 0)
		{
			c_pbReset->hide();
			return;
		}
		if(!c_Panel->isLookingOn())
		{
			c_pbReset->move(width()-c_pbReset->width()-10,35);
			c_pbReset->show();
		}
		pix = puser->avatar();
        img = pix.convertToImage();
        img = img.smoothScale(24,24);
		for(int i=0;i<current.rule.chLifes;i++)
		{
			item = new SHSSImageItem(img,c_Canvas,0,i,SHISENSHO_DESKTOP_IMAGETYPE_LIFE);
			item->move(width()-40-i*24,5);
			item->setZ(2000);
			item->show();
		}
	}
		
}

void SHSSDesktop::ClickReset()
{
	if(c_Panel->isLookingOn())
		return;
	ShisenshoCurrent current = c_Panel->GetCurrentInfo();
	if(current.rule.chLifes == 0)
	{
		c_pbReset->hide();
		return;
	}
	SetACLStatus(true);
	char buffer[20];
	GeneralGameTrace2Head *ptrace;
	ptrace = (GeneralGameTrace2Head *)buffer;
	ptrace->chTable = c_Panel->tableId();
	ptrace->chSite = c_Panel->selfSeatId();
	ptrace->chType = SHISENSHO_GAMETRACE_RESET;
	ptrace->chBufLen = 1;
	QByteArray buf	= c_Panel->makeGameData( DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2,QByteArray((char *)buffer,sizeof(GeneralGameTrace2Head)+1) );
	QVariant parameters	= QVariant::fromValue((void*)this);
	DJClientRequest request( buf ,HandleAdjustACL,parameters);
	c_Panel->sendRequest( request ,false);
}
void SHSSDesktop::ClearLifes()
{
	Q3CanvasItemList list = c_Canvas->allItems();
    Q3CanvasItemList::Iterator it = list.begin();
	SHSSImageItem *item;
    while(it != list.end())
    {
    	if((*it)->rtti() == imageRTTI )
		{
			item = (SHSSImageItem *)(*it);
        	if ( item->c_chType == SHISENSHO_DESKTOP_IMAGETYPE_LIFE)
           	 	delete item;
        }
		it++;
    }
}

void SHSSDesktop::RepaintAvatar()
{
	quint32 uid;
	DJGameUser *puser;
	QFont f;
	QPixmap pix;
	QImage  img;
	
	if(c_Avatar != NULL)
		delete c_Avatar;
	if(c_UserName != NULL)
		delete c_UserName;
	
	uid = c_Panel->userIdOfSeat(1);
	puser = c_Panel->gameUser(uid);
	if(puser != NULL)
	{
		pix = puser->avatar();
        img = pix.convertToImage();
        img = img.smoothScale(64,64);
		c_Avatar = new SHSSImageItem(img,c_Canvas,0,1,0);
		c_Avatar->move(5,5);
		c_Avatar->show();
		
		c_UserName = new Q3CanvasText(puser->userName(),c_Canvas);
		c_UserName->setColor(QColor(0,0,0));
		f = c_UserName->font();
		f.setPointSize(14);
		f.setBold(true);
		c_UserName->setFont(f);
		c_UserName->move(75,5);
		c_UserName->show();
	}

}
void SHSSDesktop::SetScore(unsigned int iScore)
{
	c_iScore = iScore;
	QString st=tr("Score : ");
	st += QString("%1").arg(c_iScore);
	c_ScoreText->setText(st);
}

void SHSSDesktop::AddScore(unsigned int iScore,bool bWah)
{
	c_iScore += iScore;
	SetScore(c_iScore);
}

SHSSDesktop::~SHSSDesktop()
{
	delete m_clockItem;
	m_clockItem	= 0;
}

void SHSSDesktop::WaitStart( quint16 timeout )
{
	//c_timer->start(1000);
	m_clockItem->setTimeout( QList<quint8>(), timeout );
	m_clockItem->move(CLOCK_POSITION_X,CLOCK_POSITION_Y);
	c_pbStart->setText(tr("Start"));
	c_pbStart->show();
}
void SHSSDesktop::WaitDelete( quint16 timeout )
{
	//c_timer->start(1000);
	m_clockItem->setTimeout( QList<quint8>(), timeout );
	m_clockItem->move(CLOCK_POSITION_X,CLOCK_POSITION_Y);
	c_pbStart->hide();
}
void SHSSDesktop::WaitContinue( quint16 timeout )
{
	//c_timer->start(1000);
	m_clockItem->setTimeout( QList<quint8>(), timeout );
	m_clockItem->move(CLOCK_POSITION_X,CLOCK_POSITION_Y);
	c_pbStart->setText(tr("Continue"));
	c_pbStart->show();
}
void SHSSDesktop::AdjustMap(GeneralGameTrace2Head *ptrace)
{
	ShisenshoCurrent current = c_Panel->GetCurrentInfo();
	PShisenshoAdjust padjust = (PShisenshoAdjust)ptrace->chBuf;
	ShisenshoRule_SetLines(c_chMap,current.rule.chWidth,current.rule.chHeight,
					padjust->chStartLine,padjust->chEndLine,padjust->chBuf);
	ClearCards();
	c_Panel->playWave("move.wav");
	RepaintCards();
}

void SHSSDesktop::DeleteCard(GeneralGameTrace2Head *ptrace)
{
	PShisenshoDelete pdel = (PShisenshoDelete)ptrace->chBuf;
	ShisenshoCurrent current = c_Panel->GetCurrentInfo();
	if(!ShisenshoRule_Check(c_chMap,current.rule.chWidth,current.rule.chHeight,pdel->chX,pdel->chY) )
	{
		RequestAdjust();
	}else
	{
	//	printf("(%d,%d)->(%d,%d)->(%d,%d)->(%d,%d)\n",
	//				pdel->chX[0],pdel->chY[0] ,
	//				pdel->chX[1],pdel->chY[1],
	//				pdel->chX[2],pdel->chY[2],
	//				pdel->chX[3],pdel->chY[3]);
		
		int i,sx,sy,x,y;
		sx = pdel->chX[0];
		sy = pdel->chY[0];
		for(i=1;i<4;i++)
		{
			if(pdel->chX[i] == 0 && pdel->chY[i] == 0)
				break;
			x = pdel->chX[i];
			y = pdel->chY[i];
			Map2Canvas(current.rule.chWidth,current.rule.chHeight,&sx,&sy,SHISENSHO_POSTYPE_CENTER);
			Map2Canvas(current.rule.chWidth,current.rule.chHeight,&x,&y,SHISENSHO_POSTYPE_CENTER);
			
			Q3CanvasLine *item = new Q3CanvasLine(c_Canvas);
			item->setPoints(sx,sy,x,y);
			item->setPen(QPen(QColor(255,0,0),3));
			item->setZ(2000);
			item->show();
			
			sx = pdel->chX[i];
			sy = pdel->chY[i];
		}
		if(c_CurDel.chX[0] != 0)
			HandleDeleteTimeout();
		memcpy(&c_CurDel,pdel,sizeof(ShisenshoDelete));
		c_deleteTimer->start(300);
	}
		
}

void SHSSDesktop::HandleDeleteTimeout()
{
	ClearAllLine();
	int i;
	ShisenshoCurrent current = c_Panel->GetCurrentInfo();
	int sx,sy,ex,ey;
	unsigned char *p;
	c_deleteTimer->stop();
	sx = c_CurDel.chX[0];
	sy = c_CurDel.chY[0];
	for(i=1;i<4;i++)
	{
		p = ShisenshoRule_GetPoint(c_chMap,current.rule.chWidth,current.rule.chHeight,
					c_CurDel.chX[i],c_CurDel.chY[i]);
		if(p != NULL && *p != 0)
		{
			ex = c_CurDel.chX[i];
			ey = c_CurDel.chY[i];
			c_Panel->playWave("kill.wav");
			ShisenshoRule_DeleteCard(c_chMap,current.rule.chWidth,current.rule.chHeight,
									sx,sy,ex,ey,current.rule.chAdjustOri,NULL,NULL);
			break;
		}
	}
	memset(&c_CurDel,0,sizeof(ShisenshoDelete));
//	if(current.rule.chAdjustOri != 0)
	{
		ClearCards();
		RepaintCards();
	}
}

void SHSSDesktop::ClearAllLine()
{
	Q3CanvasItemList list = c_Canvas->allItems();
    Q3CanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it)
    {
        if ( *it && (*(it))->rtti() == Q3CanvasItem::Rtti_Line )
            delete *it;
    }
}

void SHSSDesktop::ClearAllItems()
{
	Q3CanvasItemList list = c_Canvas->allItems();
    Q3CanvasItemList::Iterator it = list.begin();
    while(it != list.end())
    {
        delete (*it);
		it++;
    }
}

void SHSSDesktop::ClearCards()
{
	Q3CanvasItemList list = c_Canvas->allItems();
    Q3CanvasItemList::Iterator it = list.begin();
	SHSSImageItem *item;
    while(it != list.end())
    {
    	if((*it)->rtti() == imageRTTI )
		{
			item = (SHSSImageItem *)(*it);
        	if ( item->c_chType == SHISENSHO_DESKTOP_IMAGETYPE_CARD)
           	 	delete item;
        }
		it++;
    }
}
void SHSSDesktop::RepaintCards()
{
	ShisenshoCurrent current = c_Panel->GetCurrentInfo();
	unsigned char *pc;
	int x,y,z=200;
	QImage img;
	int i,j;
	for(i=1;i<=current.rule.chWidth;i++)
	{
		for(j=1;j<=current.rule.chHeight;j++)
		{
			pc = ShisenshoRule_GetPoint(c_chMap,current.rule.chWidth,current.rule.chHeight,i,j);
			if(pc != NULL && *pc != 0)
			{
				x = i;
				y = j;
				Map2Canvas(current.rule.chWidth,current.rule.chHeight,&x,&y,SHISENSHO_POSTYPE_LEFTTOP);
				img = GetCardImage(*pc);
				SHSSImageItem *item = new SHSSImageItem(img,c_Canvas,i,j,SHISENSHO_DESKTOP_IMAGETYPE_CARD);
				item->move(x,y);
				item->setZ(z);
				item->show();
				z++;
			}
		}
	}
	c_DelX1 = 0;
	c_DelY1 = 0;
}

QImage SHSSDesktop::GetCardImage(unsigned char chCard)
{
	QPixmap pix;
	QImage img;
	char pixname[255];
	int fx=GetFx();
	sprintf(pixname,":/ShisenshoRes/image/suit%d/%02d.png",c_shSkin,chCard);
	pix = QPixmap(QString(pixname));
	if(pix.isNull())
	{
		sprintf(pixname,":/ShisenshoRes/image/suit1/%02d.png",chCard);
		pix = QPixmap(QString(pixname));
		if(pix.isNull())
		{
			sprintf(pixname,":/ShisenshoRes/image/suit1/card.png");
			pix = QPixmap(QString(pixname));
		}
	}
	img = pix.convertToImage();
    img = img.smoothScale(img.width()*fx/100,img.height()*fx/100);
	
	return img;
	
}


void SHSSDesktop::Map2Canvas(unsigned char chMapWidth,unsigned char chMapHeight,int *x,int *y,unsigned char chPosType)
{
	if(*x < 0 || *y <0 || *x>chMapWidth+1 || *y>chMapHeight+1)
	{
		*x = -200; *y = -200;
		return;
	}
	int Fx = GetFx();
	int w = (SHISENSHO_CARD_WIDTH*Fx/100);
	int h = (SHISENSHO_CARD_HEIGHT*Fx/100);
	int sx,sy;
	
	sx = (width()-(chMapWidth+2)*w)>>1;
	sy = (( (height()-SHISENSHO_DESKTOP_HEADSIZE) - (chMapHeight+2)*h)>>1) + SHISENSHO_DESKTOP_HEADSIZE;
	
	if( chPosType == SHISENSHO_POSTYPE_LEFTTOP || 
			chPosType == SHISENSHO_POSTYPE_LEFTCENTER	||
			chPosType == SHISENSHO_POSTYPE_LEFTBOTTOM)
	{
		*x = (*x)*w+sx;
	}else if( chPosType == SHISENSHO_POSTYPE_CENTERTOP || 
			chPosType == SHISENSHO_POSTYPE_CENTER	||
			chPosType == SHISENSHO_POSTYPE_CENTERBOTTOM)
	{
		*x = (*x)*w+(w>>1)+sx;
	}else 
	{
		*x = (*x)*w+w+sx;
	}
	
	if( chPosType == SHISENSHO_POSTYPE_LEFTTOP || 
			chPosType == SHISENSHO_POSTYPE_RIGHTTOP	||
			chPosType == SHISENSHO_POSTYPE_CENTERTOP)
	{
		*y = (*y)*h+sy;
	}else if( chPosType == SHISENSHO_POSTYPE_LEFTCENTER || 
			chPosType == SHISENSHO_POSTYPE_CENTER	||
			chPosType == SHISENSHO_POSTYPE_RIGHTCENTER)
	{
		*y = (*y)*h+(h>>1)+sy;
	}else 
	{
		*y = (*y)*h+h+sy;
	}
	
}

int SHSSDesktop::GetFx()
{
	ShisenshoCurrent current = c_Panel->GetCurrentInfo();
	if(current.rule.chWidth == 0 || current.rule.chHeight == 0)
		return 100;
	
	int Fx = (width()*100)/(SHISENSHO_CARD_WIDTH*(current.rule.chWidth+2));
	int Fy = ((height()-SHISENSHO_DESKTOP_HEADSIZE)*100)/(SHISENSHO_CARD_HEIGHT*(current.rule.chHeight+2));
	return (Fx<Fy)?Fx:Fy;
}
	


void SHSSDesktop::resizeEvent( QResizeEvent * )
{
	c_Canvas->resize(width(),height());
	c_pbStart->move( (width()-c_pbStart->width())>>1,(height()-c_pbStart->height())>>1);
	ClearCards();
	RepaintCards();
	RepaintLifes();
}

void SHSSDesktop::SetACLStatus(bool b)
{
	c_bWaitACL = b;
}
SHSSImageItem * SHSSDesktop::GetCardItem(int x,int y)
{
	ShisenshoCurrent current = c_Panel->GetCurrentInfo();
	Map2Canvas(current.rule.chWidth,current.rule.chHeight,&x,&y,SHISENSHO_POSTYPE_CENTER);
	return GetCardItemByMousePos(x,y);
}
SHSSImageItem * SHSSDesktop::GetCardItemByMousePos(int x,int y)
{
	QPoint p(x,y);
	SHSSImageItem *item;
	Q3CanvasItemList l=canvas()->collisions(p);
	Q3CanvasItemList::Iterator it=l.begin();
	while(it != l.end())
	{
		if((*it)->rtti() == imageRTTI )
		{
			item = (SHSSImageItem *)(*it);
			if(item->c_chType == SHISENSHO_DESKTOP_IMAGETYPE_CARD)
			{
				return item;
			}
		}
		it ++;
	}
	return NULL;
}

void SHSSDesktop::ResetCardMark(unsigned char chCard,bool bMark)
{
	Q3CanvasItemList list = c_Canvas->allItems();
    Q3CanvasItemList::Iterator it = list.begin();
	SHSSImageItem *item;
	unsigned char *p;
	ShisenshoCurrent current = c_Panel->GetCurrentInfo();
    while(it != list.end())
    {
    	if((*it)->rtti() == imageRTTI )
		{
			item = (SHSSImageItem *)(*it);
        	if ( item->c_chType == SHISENSHO_DESKTOP_IMAGETYPE_CARD && item->GetImageMark() != bMark)
			{
				if(chCard == 0)
					MarkCard(item,bMark);
				else
				{
					p = ShisenshoRule_GetPoint(c_chMap,current.rule.chWidth,current.rule.chHeight,
										item->c_chTable,item->c_chSeat);
					if(p != NULL && *p == chCard)
						MarkCard(item,bMark);
				}
			}
        }
		it++;
    }
	
}

void SHSSDesktop::contentsMouseReleaseEvent(QMouseEvent* e)
{
	if(c_Panel->isLookingOn() || !(c_Panel->WaittingForDelete() ) || c_bWaitACL)
		return ;
	unsigned char *p1,*p2;
	SHSSImageItem *item1,*item2;
	QPoint pp = inverseWorldMatrix().map(e->pos());
	ShisenshoCurrent current = c_Panel->GetCurrentInfo();
	if (e->button() == Qt::LeftButton) 
	{
		if(c_CurDel.chX[0] != 0)
			HandleDeleteTimeout();
		
		p1 = NULL; p2 = NULL;
		p1 = ShisenshoRule_GetPoint(c_chMap,current.rule.chWidth,current.rule.chHeight,c_DelX1,c_DelY1);
		
		item2 = GetCardItemByMousePos(pp.x(),pp.y());
		if(item2 != NULL)
		{
			c_Panel->playWave("click.wav");
			p2 = ShisenshoRule_GetPoint(c_chMap,current.rule.chWidth,current.rule.chHeight,
						item2->c_chTable,item2->c_chSeat);
		}
		if(p1 == p2)
			return;
		if(p1 == NULL || *p1 == 0)
		{//之前无选择
			ResetCardMark(0,false);
			if(p2 != NULL && *p2 != 0)
			{
				c_DelX1 = item2->c_chTable;
				c_DelY1 = item2->c_chSeat;
				MarkCard(item2,true);
			}
			return;
		}
		item1 = GetCardItem(c_DelX1,c_DelY1);
		if(p2 == NULL || *p2 == 0)
		{//当前无选择
			
			if(item1 != NULL)
			{
				MarkCard(item1,false);
			}
			c_DelX1 = 0;
			c_DelY1 = 0;
			return;
		}
		if(*p1 != *p2)
		{//两次选择不同的牌
			if(item1 != NULL)
			{
				MarkCard(item1,false);
			}
			
			c_DelX1 = item2->c_chTable;
			c_DelY1 = item2->c_chSeat;
			MarkCard(item2,true);
			return;
		}
		MarkCard(item2,true);
		char buffer[255];
		GeneralGameTrace2Head *ptrace = (GeneralGameTrace2Head *)buffer;
		PShisenshoDelete		pdel = (PShisenshoDelete)ptrace->chBuf;
		memset(pdel,0,sizeof(ShisenshoDelete));
		pdel->chX[0] = c_DelX1;
		pdel->chY[0] = c_DelY1;
		pdel->chX[1] = item2->c_chTable;
		pdel->chY[1] = item2->c_chSeat;
	//	printf("selected 2 point (%d,%d) (%d,%d)\n",pdel->chX[0],pdel->chY[0],
	//				pdel->chX[1],pdel->chY[1]);
		
		
		if(ShisenshoRule_SearchPath(c_chMap,current.rule.chWidth,current.rule.chHeight,
			pdel->chX,pdel->chY) )
		{
			ptrace->chTable = c_Panel->tableId();
			ptrace->chSite = c_Panel->selfSeatId();
			ptrace->chType = SHISENSHO_GAMETRACE_DELETE;
			ptrace->chBufLen = sizeof(ShisenshoDelete);
			DeleteCard(ptrace);
			c_DelX1 = 0;
			c_DelY1 = 0;
			QByteArray buf	= c_Panel->makeGameData( DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2,QByteArray((char *)buffer,sizeof(GeneralGameTrace2Head)+ptrace->chBufLen) );
			QVariant parameters	= QVariant::fromValue((void*)this);
			DJClientRequest request( buf ,HandleDeleteACL,parameters);
			
			c_Panel->sendRequest( request ,false);
			
		}else
		{
			MarkCard(item1,false);
			MarkCard(item2,false);
			c_DelX1 = 0;
			c_DelY1 = 0;
		}
		
		
	}else if (e->button() == Qt::RightButton) 
	{
		ResetCardMark(0,false);
		c_DelX1 = 0;
		c_DelY1 = 0;
		item2 = GetCardItemByMousePos(pp.x(),pp.y());
		if(item2 != NULL)
		{
			p2 = ShisenshoRule_GetPoint(c_chMap,current.rule.chWidth,current.rule.chHeight,
						item2->c_chTable,item2->c_chSeat);
			if(p2 != NULL && *p2 != 0)
				ResetCardMark(*p2,true);
		}
	}
}
/*
void SHSSDesktop::contentsDragMoveEvent ( QDragMoveEvent * e ) 
{
}
*/
void SHSSDesktop::timerEvent ( QTimerEvent * event )
{
	ModifyLifeLine();
}
void SHSSDesktop::MarkCard(SHSSImageItem *item,bool bMark)
{
	bool bOld = item->GetImageMark();
	item->SetImageMark(bMark);
	if(bMark && !bOld)
	{
		int d = GetFx()*13/100;
		item->move(item->x(),item->y()-d);
	}else if(!bMark && bOld)
	{
		int d = GetFx()*13/100;
		item->move(item->x(),item->y()+d);
	}
}

void SHSSDesktop::ClickStart()
{
	if(c_Panel->isLookingOn())
		return ;
	c_pbStart->hide();
	unsigned char chStatus = c_Panel->GetTableStatus();
	if(chStatus == DJGAME_SHISENSHO_TABLE_STATUS_WAIT_CONTINUE)
	{
		char buffer[20];
		GeneralGameTrace2Head *ptrace =(GeneralGameTrace2Head*)buffer;
		ptrace = (GeneralGameTrace2Head *)buffer;
		ptrace->chTable = c_Panel->tableId();
		ptrace->chSite = c_Panel->selfSeatId();
		ptrace->chType = SHISENSHO_GAMETRACE_CONTINUE;
		ptrace->chBufLen = 0;
		QByteArray buf	= c_Panel->makeGameData( DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2,QByteArray((char *)buffer,sizeof(GeneralGameTrace2Head)) );
		DJClientRequest request( buf);
		c_Panel->sendRequest( request ,false);
	}else if(chStatus == DJGAME_TABLE_STATUS_WAITSTART)
	{
		memset(c_chMap,0,SHISENSHO_MAX_MAP+8);
		//c_wHelp->setText( tr("waiting for other player"));
		QByteArray buf	= c_Panel->makeGameData( DJGAME_PROTOCOL_SUBTYPE_START, QByteArray() );
		DJClientRequest request( buf );
    	c_Panel->sendRequest( request,false );
		c_iScore = 0;
	}
	
}
/*
void SHSSDesktop::HandleTimeout()
{
	c_TimerText->setText(QString("%1").arg(c_Panel->GetWaitTime()));
	ModifyLifeLine();
	c_timer->start(1000);
}
*/
void SHSSDesktop::RequestAdjust()
{
	if ( c_Panel->isLookingOn() )
		return;
	SetACLStatus(true);
	DEBUG_OUTPUT(("delete failed , must ADJUST\n"));
	char buffer[20];
	GeneralGameTrace2Head *ptrace;
	ptrace = (GeneralGameTrace2Head *)buffer;
	ptrace->chTable = c_Panel->tableId();
	ptrace->chSite = c_Panel->selfSeatId();
	ptrace->chType = SHISENSHO_GAMETRACE_ADJUST;
	ptrace->chBufLen = 0;
	QByteArray buf	= c_Panel->makeGameData( DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2,QByteArray((char *)buffer,sizeof(GeneralGameTrace2Head)) );
	QVariant parameters	= QVariant::fromValue((void*)this);
	DJClientRequest request( buf ,HandleAdjustACL,parameters);
	c_Panel->sendRequest( request ,false);
}
void  HandleAdjustACL( const QByteArray& buf, const QVariant& parameters )
{
	PDJGameProtocol pacl = (PDJGameProtocol)buf.data();
    SHSSDesktop* desktop = static_cast<SHSSDesktop *>(parameters.value<void*>());
    
	desktop->SetACLStatus(false);
}
void  HandleDeleteACL( const QByteArray& buf, const QVariant& parameters )
{
    PDJGameProtocol pacl = (PDJGameProtocol)buf.data();
    SHSSDesktop* desktop = static_cast<SHSSDesktop *>(parameters.value<void*>());
    
    if(buf.size() == 0)
    {
        return ;
    }
    if( DJGAME_SUCCESS(pacl->chBuf[0]) )
	{
		;
	}else
	{
		desktop->RequestAdjust();
	}
}


////////////////////////////////////////////////////////////

SHSSPanel::SHSSPanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f)
			: DJGamePanel( gameController, room, userIds, tableId, seatId, parent, f )
{
	djDebug()<<"SHSSPanel constructor";
	//char chDbgFile[60];
	
	//sprintf(chDbgFile,"shss_%d_%d.dbg",tableId,seatId);
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
	createPanelBar( gameSplitter );
	
	c_DesktopCanvas = new  Q3Canvas(this);
	c_DesktopCanvas->resize(100,100);
	c_Desktop = new SHSSDesktop(c_DesktopCanvas,gameSplitter,this);
	connect( this, SIGNAL(changeBackgroundColor(const QColor&)), c_Desktop, SLOT(changeBackgroundColor(const QColor&)) );

	c_chTableStatus = 0;
	c_iTimeLin = 0;
	
	memset(&c_Current,0,sizeof(ShisenshoCurrent));
	/*
	c_Rank = new DJGameRankTabWidget( this, leftSplitter,20);
	
	createChatWidget( leftSplitter );
	showMaximized();
	
	
	QSettings settings;
	if ( settings.contains( KEY_SHISENSHO_MAIN_SPLITTER_SIZES) ) {
    	mainSplitter->restoreState(settings.value( KEY_SHISENSHO_MAIN_SPLITTER_SIZES ).toByteArray());
    }else {
		QList<int> sizes;
    	sizes << 100 << 400;
    	mainSplitter->setSizes( sizes );
	}
	if ( settings.contains( KEY_SHISENSHO_LEFT_SPLITTER_SIZES) ) {
    	leftSplitter->restoreState(settings.value( KEY_SHISENSHO_LEFT_SPLITTER_SIZES ).toByteArray());
    }else {
		QList<int> sizes;
    	sizes << 100 << 100 << 100;
    	leftSplitter->setSizes( sizes );
	}
	*/
}
SHSSPanel::~SHSSPanel()
{
}
void SHSSPanel::createAccessoryWidgets()
{
	QSplitter	*splitter	= accessorySplitter();
	if ( splitter ) {
		createPlayersWidget( splitter );
		c_Rank = new DJGameRankTabWidget( this, splitter,20);
		c_Rank->setMinimumHeight(200);
		createChatWidget( splitter );
	}
}
void SHSSPanel::playerItemHeaderSections( QList<int> &sectionIds, QStringList &sectionNames )
{
	sectionIds.clear();
	sectionNames.clear();
	
	sectionIds << UserName << NickName << Seat;
	for ( int i = 0; i < sectionIds.size(); ++i ) {
		sectionNames << baseSectionName( sectionIds.at(i) );
	}
	sectionNames << "-";
}
/*
DJGamePlayerItem* SHSSPanel::addPlayer( quint32 userId, bool isPlayer )
{
	djDebug() << "DJPlayersTreeWidget::addPlayer" << userId;
	if ( 0 == userId || 0 == playersTreeWidget() )
		return 0;
	DJGameUser*	user	= gameUser( userId,true );
	DJGamePlayerItem* playerItem	= gamePlayerItem( userId );
	if ( 0 == playerItem ) {
		playerItem	= createPlayerItem( userId, playersTreeWidget() );
	}
	if ( isPlayer ) {
		playerItem->setNameAndSuffix( user->userName() );
		playerItem->setSeat( user->seatId() );
	}else {
		playerItem->setNameAndSuffix( user->userName(), tr("(looker)") );
		playerItem->setSeat( 0 );
	}
	return playerItem;
}
QTreeWidgetItem* SHSSPanel::headerOfPlayerItem()
{
	QStringList headers;
	headers << tr("name") << tr("seat")<< "-";
	QTreeWidgetItem* headerItem	= new QTreeWidgetItem;
    for ( int i = 0; i < headers.size(); i++) {
    	headerItem->setTextAlignment( i, Qt::AlignCenter );
    	headerItem->setText( i, headers.at(i) );
    }
    return headerItem;
}
DJGamePlayerItem* SHSSPanel::createPlayerItem( quint32 userId, QTreeWidget* parent, int type )
{
	DJGamePlayerItem*	playerItem	= new ShisenshoPlayerItem( userId, this, parent, type );
	return playerItem;
}
*/
void SHSSPanel::userArrived( quint32 userId, quint8 seatId )
{
	DJGamePanel::userArrived( userId, seatId );
	c_Desktop->RepaintAvatar();
}
void SHSSPanel::gameInfo( const QByteArray& buf )
{
	bool bRepaint=false;
	ShisenshoCurrent *pnc =(ShisenshoCurrent*)buf.data();
	memcpy(&c_Current,buf.data(),sizeof(ShisenshoCurrent));
	c_Desktop->SetScore(letoh4(c_Current.iScore));
	c_Desktop->RepaintLifes();
}

void SHSSPanel::gameWait( quint16 mask, quint8 status ,quint16 timeout )
{
	c_chTableStatus = status;
	c_shWaitMask = mask;
	c_iTimeLin = time(NULL)+timeout;
	DJGameRoom *room = gameRoom();
	switch(status)
	{
		case  DJGAME_TABLE_STATUS_WAITSTART :
			c_Desktop->WaitStart( timeout );
			break;
		case DJGAME_SHISENSHO_TABLE_STATUS_WAIT_DELETE :
			c_Desktop->WaitDelete( timeout );
			break;
		case DJGAME_SHISENSHO_TABLE_STATUS_WAIT_CONTINUE :
			c_Desktop->WaitContinue( timeout );
			break;
	}
}

void SHSSPanel::gameTrace( const GeneralGameTrace2Head *gameTrace )
{
	GeneralGameTrace2Head *trace	= const_cast<GeneralGameTrace2Head *>(gameTrace);
	
	switch( gameTrace->chType ) 
	{
		case SHISENSHO_GAMETRACE_RANK :
		{
			PShisenshoRank prank =(PShisenshoRank)gameTrace->chBuf;
			c_Rank->InsertIntoList(prank->chType,prank->chIndex,QString::fromUtf8(prank->szUserName),letoh4(&prank->userid),letoh4(prank->uiScore),letoh4(prank->timestamp));
			break;
		}
		case SHISENSHO_GAMETRACE_ADJUST :
		{
			c_Desktop->AdjustMap(trace);
			break;
		}
		case SHISENSHO_GAMETRACE_DELETE :
		{
			if(isLookingOn())
				c_Desktop->DeleteCard(trace);
			c_Desktop->AddScore(c_Current.rule.chBaseScore<<1,false);
			PShisenshoDelete pdel = (PShisenshoDelete)trace->chBuf;
			if(pdel->chWah != 0)
			{
				c_Desktop->AddScore(pdel->chWah*100,true);
			}
			break;
		}
		case SHISENSHO_GAMETRACE_RESET :
		{
			c_Current.rule.chLifes = trace->chBuf[0];
			c_Desktop->RepaintLifes();
			break;
		}
	}
}
void SHSSPanel::magicResult( const MagicResult* magicResult )
{
}
void SHSSPanel::gameOver( const DJGameOver* gameOver )
{
	DJGamePanel::gameOver( gameOver );
	/*
	QString res;
	int i,score;
	quint32 uid;
	DJGameUser *puser;

	uid = userIdOfSeat(gameOver->site[0].chSite);
	score = (int)(ltoh4(gameOver->site[0].score));
	//SetUserScore(this,c_PlayerList,uid,score,2,false);
	if(uid != 0)
	{
		puser = gameUser(uid);
		if(puser != NULL)
		{
			res += puser->userName();
			res += QString(" : %1\n").arg(score);
		}
	}
	QMessageBox::information(NULL,tr("game report"),res,QMessageBox::Ok);
	*/
}


///////////////////////////////////////////////////////
bool SHSSPanel::WaittingForDelete()
{
	if(c_chTableStatus != DJGAME_SHISENSHO_TABLE_STATUS_WAIT_DELETE)
		return false;
	return true;
}
quint16 SHSSPanel::GetWaitTime()
{
	int curt = time(NULL);
	if(curt < c_iTimeLin && c_iTimeLin != 0)
		return c_iTimeLin-curt;
	return 0;
}

ShisenshoCurrent SHSSPanel::GetCurrentInfo()
{
	return c_Current;
}

unsigned char SHSSPanel::GetTableStatus()
{
	return c_chTableStatus;
}
/*
void SHSSPanel::resizeEvent( QResizeEvent * )
{
	mainSplitter->setGeometry(0,0,width(),height());
}
*/

void SHSSPanel::closeEvent( QCloseEvent *e)
{
	if ( isLookingOn() ) {
		//look on
		QByteArray data;
		data.append( tableId() );
		QByteArray buf	= makeGameData( DJGAME_PROTOCOL_SUBTYPE_QUITVIEW, data );
		DJClientRequest request( buf );
		sendRequest( request, false );
		e->accept();
		return;
	}
//    if(c_chTableStatus == DJGAME_TABLE_STATUS_WAITSTART)
    {
    	QByteArray buf	= makeGameData( DJGAME_PROTOCOL_SUBTYPE_QUIT, QByteArray() );
		DJClientRequest request( buf );
		sendRequest( request, false );
		e->accept();
    }
			
}



