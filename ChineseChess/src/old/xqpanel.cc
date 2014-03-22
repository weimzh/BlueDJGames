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

#include <QRect>

#include "DJGameController.h"
#include "DJClientRequest.h"
#include "DJGameUser.h"
#include "DJGameRoom.h"
#include "DJTreeWidget.h"
#include "DJTopWidget.h"
#include "Utility.h"
#include "DJTreeItemDelegate.h"
#include "DJGameImage.h"
#include "DJGameDesktop.h"
#include "xqpanel.h"
#include "xqrule.h"
#include "xqprofile.h"
#include "DJGamePlayerItem.h"
#include "DJAutoDisappearText.h"
void  HandleMoveACL( const QByteArray& buf, const QVariant& parameters );
//////////////////////////////////////////////////////////////////////

XiangqiDesktop::XiangqiDesktop(QWidget* parent,DJGamePanel *panel,quint32 w,quint32 h)
			: DJGameDesktop(parent,panel,w,h)
{	
	c_XiangqiPanel = (XiangqiPanel*)panel;
	c_shSkin = 1;
	connect(this,SIGNAL(gameStarted()),this,SLOT(PlayerStarted()));
	
	QMatrix m = GetCurrentMatrix();
	QPixmap pix = QPixmap(QString(":/ChineseChessRes/image/cchessb%1.png").arg(c_shSkin));
	QImage img = pix.convertToImage();
	c_X = 0;
	c_Y = ((h-img.height())>>1);
	c_itemBoard = new DJGameImageItem(img,canvas(),m,DESKTOP_IMAGE_TYPE_BOARD,0,DESKTOP_IMAGE_TYPE_BOARD,false);
	c_itemBoard->move(c_X,c_Y);
	c_itemBoard->setZ(1);
	c_itemBoard->show();
	c_itemBoard->setMatrix(m);
	
	pix = QPixmap(QString(":/ChineseChessRes/image/focus.png"));
	img = pix.convertToImage();
	c_itemFocus = new DJGameImageItem(img,canvas(),m,DESKTOP_IMAGE_TYPE_FOCUS,0,DESKTOP_IMAGE_TYPE_FOCUS,false);
	c_itemFocus->setVAlignment(Qt::AlignVCenter);
	c_itemFocus->setHAlignment(Qt::AlignHCenter);
	c_itemFocus->move(0,0);
	c_itemFocus->setZ(100);
	c_itemFocus->hide();
	c_itemFocus->setMatrix(m);
	
	pix = QPixmap(QString(":/ChineseChessRes/image/start.png"));
	img = pix.convertToImage();
	c_itemStartPos = new DJGameImageItem(img,canvas(),m,DESKTOP_IMAGE_TYPE_FOCUS,0,DESKTOP_IMAGE_TYPE_FOCUS,false);
	c_itemStartPos->setVAlignment(Qt::AlignVCenter);
	c_itemStartPos->setHAlignment(Qt::AlignHCenter);
	c_itemStartPos->move(0,0);
	c_itemStartPos->setZ(100);
	c_itemStartPos->hide();
	c_itemStartPos->setMatrix(m);
	
	PXiangqiRoom pxqroom = (PXiangqiRoom)((c_XiangqiPanel->gameRoom())->privateRoom());
	c_blackTimer = NULL;
	c_redTimer = NULL;
	c_current.chRed = 1;
	c_current.chBlack = 2;
	
	DJGameChessTimer ctimer;
	//象棋的数据结构不符合后来定义的统一的棋类时间描述表结构，因此要适配一下
	ctimer.shBaseTime = pxqroom->rule.shBaseTime;
	ctimer.chTimeAfterBaseTime = pxqroom->rule.chTimeAfterBaseTime;
	ctimer.chTimes = pxqroom->rule.chTimes;
	ctimer.chTimeAfterTimeout = pxqroom->rule.chTimeAfterTimeout;
	ctimer.chSteps = 0;
	
	SetPlayerTimer(DJGAME_XIANGQI_RED,&ctimer);
	SetPlayerTimer(DJGAME_XIANGQI_BLACK,&ctimer);
	
	c_pbDues = new QPushButton(this);
	c_pbDues->setText(tr("Draw"));
	c_pbDues->hide();
	connect(c_pbDues,SIGNAL(released()),this,SLOT(ClickDues()));
	
	c_pbSur = new QPushButton(this);
	c_pbSur->setText(tr("Surrender"));
	c_pbSur->hide();
	connect(c_pbSur,SIGNAL(released()),this,SLOT(ClickSurrender()));
	
	//connect(this,SIGNAL(layoutButtons(const QMatrix &)),this,SLOT(HandleLayoutButtons(const QMatrix &)));
	
	StaticInitDesktop();
}


void XiangqiDesktop::ClickDues()
{
	if(!c_XiangqiPanel->isLookingOn())
		SendGameTrace(XIANGQI_GAMETRACE_REQUESTDUELS,NULL,0,NULL);
	c_pbDues->setEnabled(false);
}
void XiangqiDesktop::ClickSurrender()
{
	if(!c_XiangqiPanel->isLookingOn())
		SendGameTrace(XIANGQI_GAMETRACE_SURRENDER,NULL,0,NULL);
	c_pbSur->setEnabled(false);
}

void XiangqiDesktop::handleMatrixChanged(const QMatrix &m)
{ 
	DJGameDesktop::handleMatrixChanged(m); 
		
	QPoint pa,pn,pt;
	//GetAvatarPosition(1,&pa,&pn,&pt);
	QRect rect	= idealRectOfShow( c_XiangqiPanel->selfSeatId() );
	double tx,ty;
	//m.map(pa.x(),GetRealHeight()>>1,&tx,&ty);
	m.map(rect.left(),GetRealHeight()>>1,&tx,&ty); 
	
	MoveStartButton((int)tx,(int)ty);
	
	c_pbDues->move(((int)tx)+4,((int)ty)+4);
	ty += c_pbDues->height()+4;
	
	c_pbSur->move(((int)tx)+4,((int)ty)+4);
	ty += c_pbSur->height()+4;	
}

void XiangqiDesktop::StaticInitDesktop()
{
	memset(&c_Board,0,sizeof(XiangQiBoard));
	ClearImage(DESKTOP_IMAGE_TYPE_CHIP+DJGAME_XIANGQI_RED,DJGAME_XIANGQI_RED+1,DESKTOP_IMAGE_TYPE_CHIP+DJGAME_XIANGQI_RED);
	ClearImage(DESKTOP_IMAGE_TYPE_CHIP+DJGAME_XIANGQI_BLACK,DJGAME_XIANGQI_BLACK+1,DESKTOP_IMAGE_TYPE_CHIP+DJGAME_XIANGQI_BLACK);
	c_chStartPoint = 0;
	c_bWaitMoveACL = false;
}

void XiangqiDesktop::StaticGameTrace(GeneralGameTrace2Head *gameTrace)
{
	switch(gameTrace->chType)
	{
		case XIANGQI_GAMETRACE_ADJUST:
			memcpy(&c_Board,gameTrace->chBuf,sizeof(XiangQiBoard));
			break;
		case XIANGQI_GAMETRACE_SETRULE :
		{
			DJGameChessTimer ctimer;
			PXiangqiRule prule = (PXiangqiRule)gameTrace->chBuf;
			//象棋的数据结构不符合后来定义的统一的棋类时间描述表结构，因此要适配一下
			ctimer.shBaseTime = prule->shBaseTime;
			ctimer.chTimeAfterBaseTime = prule->chTimeAfterBaseTime;
			ctimer.chTimes = prule->chTimes;
			ctimer.chTimeAfterTimeout = prule->chTimeAfterTimeout;
			ctimer.chSteps = 0;
			
			if(gameTrace->chSite == c_current.chBlack)
				SetPlayerTimer(DJGAME_XIANGQI_BLACK,&ctimer);
			else
				SetPlayerTimer(DJGAME_XIANGQI_RED,&ctimer);
			break;
		}
			
		case XIANGQI_GAMETRACE_MOVE :
		{
			PXiangQiMove pmove = (PXiangQiMove)gameTrace->chBuf;
			unsigned char chStartChip = XiangQi_GetNode(&c_Board,XIANGQI_RULE_GETX(pmove->chStart),XIANGQI_RULE_GETY(pmove->chStart));
			unsigned char chEndChip = XiangQi_GetNode(&c_Board,XIANGQI_RULE_GETX(pmove->chEnd),XIANGQI_RULE_GETY(pmove->chEnd));
		//	printf("%02x->%02x \n",pmove->chStart,pmove->chEnd);
			XiangQi_SetNodeByID(&c_Board,pmove->chStart,0,0);
			XiangQi_SetNodeByID(&c_Board,pmove->chEnd,chStartChip&0x07,chStartChip&0x08);
			if(IsJiangJun(pmove->chEnd) )
				pmove->chBuf[0] = 1;
			else if(chEndChip != 0)
				pmove->chBuf[0] = 2;
			else
				pmove->chBuf[0] = 0;
			break;
		}
	}
}



void XiangqiDesktop::RepaintCurrentStatus()
{
	ClearImage(0,0,DESKTOP_IMAGE_TYPE_RED);
	ClearImage(0,0,DESKTOP_IMAGE_TYPE_BLACK);
	int i,j;
	for(i=1;i<=9;i++)
	{
		for(j=1;j<=10;j++)
		{
			DrawChip(i,j);
		}
	}
}

void XiangqiDesktop::ClearChip(unsigned char x,unsigned char y)
{
	DJGameImageItem *item = GetChipOfNode(x,y);
	if(item != NULL)
		delete item;
}

void XiangqiDesktop::DrawChip(unsigned char x,unsigned char y)
{
	int nx,ny;
	unsigned char chChip;
	char pixname[255];

	chChip = XiangQi_GetNode(&c_Board,x,y);
	if(chChip != 0)
	{
		QPixmap pix;
		QImage img;
		unsigned short sid = XIANGQI_RULE_GETNODEID(x,y);
		unsigned char chOwner = XIANGQI_GETOWNER(chChip);
		unsigned char chSite = (chOwner==DJGAME_XIANGQI_BLACK)?c_current.chBlack:c_current.chRed;
		GetNodeXY(x,y,&nx,&ny);
		sprintf(pixname,":/ChineseChessRes/image/chips%d/%02x.png",c_shSkin,chChip);
		pix = QPixmap(QString(pixname));
		img = pix.convertToImage();
		QMatrix m = GetCurrentMatrix();
		DJGameImageItem *item = new DJGameImageItem(img,canvas(),m,chChip,sid,DESKTOP_IMAGE_TYPE_CHIP+chOwner,false);
		item->setVAlignment(Qt::AlignVCenter);
		item->setHAlignment(Qt::AlignHCenter);
		item->move(nx,ny);
		item->setZ(200);
		item->show();
		item->setMatrix(m);
	}
}

bool XiangqiDesktop::IsJiangJun(unsigned char chEnd)
{
	unsigned char chChip = XiangQi_GetNodeByID(&c_Board,chEnd);
	unsigned char owner = (XIANGQI_GETOWNER(chChip)==DJGAME_XIANGQI_RED)?DJGAME_XIANGQI_BLACK:DJGAME_XIANGQI_RED;
	bool bFind=false;
	unsigned char i=8,x,y,chJiangPos;
	if(owner == DJGAME_XIANGQI_RED)
	{
		i = 1;
	}
	//先找到老将的位置
	for(x=4;x<=6;x++)
	{
		for(y=i;y<=i+2;y++)
		{
			chChip = XiangQi_GetNode(&c_Board,x,y);
			if(chChip == (XIANGQI_CHIP_JIANG | owner) )
			{
				bFind = true;
				break;
			}
		}
		if(bFind)
			break;
	}
	if(bFind)
	{
		//printf("jiang pos %d,%d\n",x,y);
		chJiangPos = XIANGQI_RULE_GETNODEID(x,y);
		chChip = XiangQi_GetNodeByID(&c_Board,chEnd);
		if(XIANGQI_SUCCESS(XiangQi_Move(&c_Board,XIANGQI_GETOWNER(chChip),chEnd,chJiangPos)))
		{
			//printf("jiang!\n");
			return true;
		}
	}
	return false;
	
}

void XiangqiDesktop::PlayerStarted()
{
	//StaticInitDesktop();
}
XiangqiDesktop::~XiangqiDesktop()
{
	delete c_blackTimer;
	delete c_redTimer;
}
QString	XiangqiDesktop::playerItemNameSuffix( DJGameUser* user )
{
	if ( c_current.chRed && c_current.chRed == user->seatId() )
		return tr("(red)");
	else if ( c_current.chBlack && c_current.chBlack == user->seatId() )
		return tr("(black)");
	else
		return QString();
}	
void XiangqiDesktop::gameInfo( const unsigned char *buf )
{
	StaticInitDesktop();
	memcpy(&c_current,buf,sizeof(XiangqiCurrent));
	
	/*
	DJGameChessTimer ctimer;
	//象棋的数据结构不符合后来定义的统一的棋类时间描述表结构，因此要适配一下
	ctimer.shBaseTime = c_current.blackrule.shBaseTime;
	ctimer.chTimeAfterBaseTime = c_current.blackrule.chTimeAfterBaseTime;
	ctimer.chTimes = c_current.blackrule.chTimes;
	ctimer.chTimeAfterTimeout = c_current.blackrule.chTimeAfterTimeout;
	ctimer.chSteps = 0;
	SetPlayerTimer(DJGAME_XIANGQI_BLACK,&ctimer);
	SetPlayerTimer(DJGAME_XIANGQI_RED,&ctimer);
	*/
	
	if(c_XiangqiPanel->isLookingOn())
	{
		c_pbDues->setEnabled(false);
		c_pbSur->setEnabled(false);
	}else
	{
		c_pbDues->setEnabled(true);
		c_pbSur->setEnabled(true);
	}
	c_itemFocus->hide();
	c_itemStartPos->hide();
}
void XiangqiDesktop::ShowAutoDisappearText( const QString& hint )
{
	DJAutoDisappearText* text	= new DJAutoDisappearText(30000, hint, canvas());
	QFont	font	= text->font();
	font.setPointSize( 20 );
	font.setBold( true );
	text->setFont( font );
	text->setColor( Qt::red );
	text->move( width() - text->boundingRect().width() >> 1, 100);
	text->setZ(2000);
	text->show();
}
void XiangqiDesktop::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{

	qDebug() << "XiangqiDesktop::gameWait" << mask << status;
	DJGameDesktop::gameWait(mask,status,timeout);
	if(status == DJGAME_TABLE_STATUS_WAITSTART)
	{
		c_pbSur->hide();
		c_pbDues->hide();
	}else
	{
		c_pbSur->show();
		c_pbDues->show();
	}

	switch(status)
	{
		case DJGAME_XIANGQI_TABLE_STATUS_WAIT_MOVE :
		{
			c_bWaitMoveACL = false;
			if( (1<<(c_current.chBlack-1)) & mask)
			{
				c_blackTimer->startTimer();
				c_redTimer->pauseTimer();
			}else if( (1<<(c_current.chRed-1)) & mask)
			{
				c_redTimer->startTimer();
				c_blackTimer->pauseTimer();
			}
			break;
		}
	}
}
void XiangqiDesktop::gameTrace( const GeneralGameTrace2Head *gameTrace )
{
	DJGameDesktop::gameTrace(gameTrace);
	GeneralGameTrace2Head *ptrace = (PGeneralGameTrace2Head)gameTrace;
	StaticGameTrace(ptrace);
	switch(gameTrace->chType)
	{
		case XIANGQI_GAMETRACE_REQUESTDUELS:
			if(gameTrace->chSite != c_XiangqiPanel->selfSeatId())
			{
				ShowAutoDisappearText(tr("The rival request to draw\nIf you agree,click the draw button"));
			}else
			{
		//		ShowAutoDisappearText(tr("Request to draw\nWaitting for rival....."));
				;
			}
			
			break;
		case XIANGQI_GAMETRACE_ADJUST:
			RepaintCurrentStatus();
			break;
		case XIANGQI_GAMETRACE_MOVE : 
		{
			PXiangQiMove pmove = (PXiangQiMove)gameTrace->chBuf;
		//	RepaintCurrentStatus();
			ClearChip(XIANGQI_RULE_GETX(pmove->chStart),XIANGQI_RULE_GETY(pmove->chStart));
			ClearChip(XIANGQI_RULE_GETX(pmove->chEnd),XIANGQI_RULE_GETY(pmove->chEnd));
			DrawChip(XIANGQI_RULE_GETX(pmove->chEnd),XIANGQI_RULE_GETY(pmove->chEnd));
			int nx,ny;
			GetNodeXY(XIANGQI_RULE_GETX(pmove->chStart),XIANGQI_RULE_GETY(pmove->chStart),&nx,&ny);
			c_itemStartPos->move(nx,ny);
			c_itemStartPos->show();
			GetNodeXY(XIANGQI_RULE_GETX(pmove->chEnd),XIANGQI_RULE_GETY(pmove->chEnd),&nx,&ny);
			c_itemFocus->move(nx,ny);
			c_itemFocus->show();
			
			if(pmove->chBuf[0] == 1)
			{
				c_XiangqiPanel->insertText2Browser(tr("check!"));
				c_XiangqiPanel->playWave("jiang.wav");
			}else if(pmove->chBuf[0] == 2)
				c_XiangqiPanel->playWave("kill.wav");
			else
				c_XiangqiPanel->playWave("move.wav");
			break;
		}
		case XIANGQI_GAMETRACE_REPEAT :
		{
			QString str = tr("The phase repeat again (");
			str += QString("%1").arg(gameTrace->chBuf[0]);
			str += tr(" times)");
			ShowAutoDisappearText(str);
			break;
		}
		default:
			break;
	}
}


void XiangqiDesktop::MoveItem(Q3CanvasItem* item,QPoint &ps,QPoint &pe)
{
}


bool XiangqiDesktop::handleItemClicked(Qt::MouseButton btn,const QPoint& pos,Q3CanvasItem * item)
{
	if ( DJGameDesktop::handleItemClicked(btn,pos,item) )
		return true;

	if(c_XiangqiPanel->isLookingOn())
		return false;
	if(!IsWaittingForMe() || c_bWaitMoveACL)
		return false;
	if(item == NULL || (item == c_itemBoard && c_chStartPoint == 0) )
	{//点击棋盘
		c_itemFocus->hide();
		c_itemStartPos->hide();
		c_chStartPoint = 0;
		return false;
	}
	bool bMove = false;
	unsigned char x,y,chIndex;
	if(item != NULL && item->rtti() == djgame_imageRTTI)
	{
		DJGameImageItem *imageitem = (DJGameImageItem *)item;
		if(DESKTOP_IMAGE_ISCHIP(imageitem->type()) )
		{
			chIndex = (unsigned char)(imageitem->sid());
			x = XIANGQI_RULE_GETX(chIndex);
			y = XIANGQI_RULE_GETY(chIndex);
			unsigned char chChip = XiangQi_GetNode(&c_Board,x,y);
			if(chChip != 0 && ( (XIANGQI_GETOWNER(chChip) == DJGAME_XIANGQI_RED && 
					c_XiangqiPanel->selfSeatId() == c_current.chRed)
				|| (XIANGQI_GETOWNER(chChip) == DJGAME_XIANGQI_BLACK && 
					c_XiangqiPanel->selfSeatId() == c_current.chBlack)) )
			{//自己的棋子
				c_itemStartPos->hide();
				c_chStartPoint = chIndex;
				c_XiangqiPanel->playWave("click.wav");
				int nx,ny;
				GetNodeXY(x,y,&nx,&ny);
				c_itemFocus->move(nx,ny);
				c_itemFocus->show();
				return true;
			}
		}
		
		if(c_chStartPoint != 0)
		{//目标点
			x = (pos.x()-c_X)/(XIANGQI_CHIP_MAX_SIZE)+1;
			y = ((pos.y()-c_Y)/(XIANGQI_CHIP_MAX_SIZE)+1);
			if(x > 9 || y > 10)
			{
				c_itemFocus->hide();
				c_itemStartPos->hide();
				c_chStartPoint = 0;
				return false;
			}
			y = 11-y;
			if(c_XiangqiPanel->selfSeatId() != c_current.chRed)
			{
				x = 10-x;
				y = 11-y;
			}
				unsigned char chEndID = XIANGQI_RULE_GETNODEID(x,y);
				unsigned char owner;
				if(c_XiangqiPanel->selfSeatId() == c_current.chRed)
					owner = DJGAME_XIANGQI_RED;
				else if(c_XiangqiPanel->selfSeatId() == c_current.chBlack)
					owner = DJGAME_XIANGQI_BLACK;
				else
					return false;
				if(XIANGQI_SUCCESS(XiangQi_Move(&c_Board,owner,c_chStartPoint,chEndID)))
				{//发送
					//DEBUG_OUTPUT(("move success\n"));
					unsigned char buffer[30];
					GeneralGameTrace2Head *pmovet;
					PXiangQiMove			pmove;
					pmovet = (GeneralGameTrace2Head *)buffer;
					pmovet->chTable = c_XiangqiPanel->selfTableId();
					pmovet->chSite = c_XiangqiPanel->selfSeatId();
					pmovet->chType = XIANGQI_GAMETRACE_MOVE;
					pmovet->chBufLen = sizeof(XiangQiMove);
					pmove = (PXiangQiMove )pmovet->chBuf;
					pmove->chStart = c_chStartPoint;
					pmove->chEnd = chEndID;
					c_bWaitMoveACL = true;
					QByteArray buf	= c_XiangqiPanel->makeGameData( DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2,QByteArray((char *)buffer,sizeof(GeneralGameTrace2Head)+pmovet->chBufLen) );
					QVariant parameters	= QVariant::fromValue((void*)this);
					DJClientRequest request( buf ,HandleMoveACL,parameters);
					c_itemFocus->hide();
					c_itemStartPos->hide();
					c_XiangqiPanel->sendRequest( request ,false);
				}else
				{
					//DEBUG_OUTPUT(("invalid move\n"));
					;
				}
		}
	}
	return false;
}
DJGameImageItem * XiangqiDesktop::GetChipOfNode(unsigned char x,unsigned char y)
{	
	Q3CanvasItemList list = canvas()->allItems();
    Q3CanvasItemList::Iterator it = list.begin();
	DJGameImageItem *item;
	unsigned short sid = XIANGQI_RULE_GETNODEID(x,y);
    for (; it != list.end(); ++it)
    {
		if(*it && (*it)->rtti() == djgame_imageRTTI)
		{
			item = (DJGameImageItem *)*it;
			if( item->sid() == sid && DESKTOP_IMAGE_ISCHIP(item->type()) )
			{
				return item;
			}	
		}
    }
	return NULL;
}

void XiangqiDesktop::GetNodeXY(unsigned char x,unsigned char y,int *px,int *py)
{
	int nx,ny;
	nx = XIANGQI_CHIP_MAX_SIZE*(x-1)+(XIANGQI_CHIP_MAX_SIZE>>1);
	ny = XIANGQI_CHIP_MAX_SIZE*(10-y)+(XIANGQI_CHIP_MAX_SIZE>>1);
	if(c_XiangqiPanel->selfSeatId() != c_current.chRed)
	{
		nx = XIANGQI_BOARD_MAX_WIDTH-nx;
		ny = XIANGQI_BOARD_MAX_HEIGHT-ny;
	}

	if(nx < 0 || nx > XIANGQI_BOARD_MAX_WIDTH || ny < 0 || ny >XIANGQI_BOARD_MAX_HEIGHT)
	{
		*px = 0;
		*py = 0;
	}else
	{
		*px = nx+c_X;
		*py = ny+c_Y;
	}
	
}

void XiangqiDesktop::SetWaitMoveACLStatus(bool b)
{
	c_bWaitMoveACL = b;
}

void  HandleMoveACL( const QByteArray& buf, const QVariant& parameters )
{
    PDJGameProtocol pacl = (PDJGameProtocol)buf.data();
    XiangqiDesktop* desktop = static_cast<XiangqiDesktop *>(parameters.value<void*>());
        
    if(buf.size() == 0)
    {
        desktop->SetWaitMoveACLStatus(false);
    }
    if( DJGAME_SUCCESS(pacl->chBuf[0]) )
	{
		desktop->SetWaitMoveACLStatus(true);
	}else
	{
		printf("move failed\n");
		desktop->SetWaitMoveACLStatus(false);
	}
}
void XiangqiDesktop::handleLast10Launch(unsigned char countdown)
{
	QString wavName	= QString("number%1.wav").arg(countdown);
	c_XiangqiPanel->playWave(wavName);
}
void XiangqiDesktop::SetPlayerTimer(quint8 chOwner,PDJGameChessTimer rtimer)
{
	QPoint pa,pn,pt;
	quint8 chView,chSite=0;
	DJGameChessTimerItem *curtimer;
	if(chOwner == DJGAME_XIANGQI_BLACK)
	{
		if(c_blackTimer == NULL) {
			c_blackTimer = new DJGameChessTimerItem(canvas(),this,rtimer);
			connect(c_blackTimer, SIGNAL(Last10Launch(unsigned char)), SLOT(handleLast10Launch(unsigned char)));
		}else
			c_blackTimer->setTimer(rtimer);
		curtimer = c_blackTimer;
		chSite = c_current.chBlack;
	}else if(chOwner == DJGAME_XIANGQI_RED)
	{
		if(c_redTimer == NULL) {
			c_redTimer = new DJGameChessTimerItem(canvas(),this,rtimer);
			connect(c_redTimer, SIGNAL(Last10Launch(unsigned char)), SLOT(handleLast10Launch(unsigned char)));
		}else
			c_redTimer->setTimer(rtimer);
		curtimer = c_redTimer;
		chSite = c_current.chRed;
	}
	
	if(chSite != 0)
	{
		chView = MapSeat2View(chSite);

		if(chView == 1)
		{
			QRect showRect	= idealRectOfShow( chSite );
			qDebug() << "showrect" << showRect;
			//QRect rt(pa.x(),(GetRealHeight()>>1),pix.width(),(GetRealHeight()>>1)-pix.height()-10);
			QRect rt( showRect.left(),GetRealHeight()>>1,showRect.width(),(GetRealHeight()>>1)-showRect.height()-10);
			curtimer->move(rt,false);
			curtimer->show();
		}else if(chView == 3)
		{
			//QRect rt(pa.x(),pa.y()+pix.height()+5,pix.width(),(GetRealHeight()>>1)-pix.height()-10);
			QRect showRect	= idealRectOfShow( chSite );
			QRect rt( showRect.left(),showRect.bottom() + 5,showRect.width(),(GetRealHeight()>>1)-showRect.height()-10);
			
			curtimer->move(rt);
			curtimer->show();
		}
	}
}

//////////////////////////////////////////////////////////////////////

XiangqiPanel::XiangqiPanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f)
			: DJGamePanel( gameController, room, userIds, tableId, seatId, parent, f )
{	
	qDebug() << "XiangqiPanel::XiangqiPanel";
	QSplitter* mainSplitter = new QSplitter(Qt::Horizontal , this);
	setCentralWidget( mainSplitter );
	QSplitter	*leftSplitter 	= new QSplitter( Qt::Vertical , mainSplitter );
	QSplitter 	*rightSplitter 	= new QSplitter( Qt::Vertical , mainSplitter );
	
	DJTopWidget*	topWidget	= new DJTopWidget(this, rightSplitter);
	XiangqiDesktop* gameDesktop = new XiangqiDesktop(rightSplitter,this,820,800);
	
	setAccessorySplitter( leftSplitter );
	setGameDesktop( gameDesktop );
	
	connect( topWidget, SIGNAL(changeBackgroundColor(const QColor&)), gameDesktop, SLOT(changeBackgroundColor(const QColor&)) );
}
XiangqiPanel::~XiangqiPanel()
{
}
