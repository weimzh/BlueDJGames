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
#include "XQDesktopController.h"
#include "DJPanelController.h"
#include "DJGraphicsPixmapItem.h"
#include "DJGraphicsChessTimerItem.h"
#include "DJDesktop.h"
#include "DJDesktopScene.h"
#include "DJGameRoom.h"
#include "DJGameUser.h"
#include "DJClientRequest.h"
#include "Utility.h"
#include "xqrule.h"
#include "xqprofile.h"
#include "DJGroupChat.h"
#include "DJTextBrowser.h"
#include "DJSchemeEvent.h"

#define XIANGQI_BOARD_MAX_HEIGHT			750
#define XIANGQI_BOARD_MAX_WIDTH				675
#define XIANGQI_CHIP_MAX_SIZE				75

#define DESKTOP_IMAGE_TYPE_BOARD			0x102

#define DESKTOP_IMAGE_TYPE_CHIP				0x110
#define DESKTOP_IMAGE_TYPE_RED				(DESKTOP_IMAGE_TYPE_CHIP+DJGAME_XIANGQI_RED)
#define DESKTOP_IMAGE_TYPE_BLACK			(DESKTOP_IMAGE_TYPE_CHIP+DJGAME_XIANGQI_BLACK)

#define DESKTOP_IMAGE_TYPE_FOCUS			0x120
#define DESKTOP_IMAGE_ISCHIP(__type)			((__type)== DESKTOP_IMAGE_TYPE_RED || (__type)==DESKTOP_IMAGE_TYPE_BLACK)

void HandleMoveACL( const QByteArray& buf, const QVariant& parameters )
{
    PDJGameProtocol pacl = (PDJGameProtocol)buf.data();
    XQDesktopController* dc = static_cast<XQDesktopController*>(parameters.value<void*>());
        
    dc->setWaitingMoveACL(false);
}
XQDesktopController::XQDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent )
	: DJDesktopController(panelController,size,parent)
{
	djDebug()<<"XQDesktopController constructor"<<size;
	
	init();
	
	m_shSkin = 1;
	
	QPixmap pix = QPixmap(QString(":/ChineseChessRes/image/cchessb%1.png").arg(m_shSkin));
	m_x = 0;
	m_y = ((size.height()-pix.height())>>1);
	m_boardItem = new DJGraphicsPixmapItem(pix,0, desktop()->scene());
	m_boardItem->setVirtualPos( QPoint(m_x,m_y) );
	m_boardItem->setZValue(1);
	m_boardItem->show();
	
	
	pix = QPixmap(QString(":/ChineseChessRes/image/focus.png"));
	m_toPosItem = new DJGraphicsPixmapItem(pix,0, desktop()->scene());
	m_toPosItem->setAlignment(Qt::AlignCenter);
	m_toPosItem->setZValue(100);
	m_toPosItem->hide();
	
	pix = QPixmap(QString(":/ChineseChessRes/image/start.png"));
	m_fromPosItem = new DJGraphicsPixmapItem(pix,0, desktop()->scene());
	m_fromPosItem->setAlignment(Qt::AlignCenter);
	m_fromPosItem->setZValue(100);
	m_fromPosItem->hide();
	
	m_blackTimer = new DJGraphicsChessTimerItem(0,0,desktop()->scene());
	connect(m_blackTimer, SIGNAL(countdown(unsigned char)), SLOT(handleCountdown(unsigned char)));
	m_redTimer = new DJGraphicsChessTimerItem(0,0,desktop()->scene());
	connect(m_redTimer, SIGNAL(countdown(unsigned char)), SLOT(handleCountdown(unsigned char)));
	
	XiangqiRule rule;
	DJGameChessTimer ctimer;
	PXiangqiRoom pxqroom = (PXiangqiRoom)(panelController->gameRoom()->privateRoom());
	memcpy(&rule,&pxqroom->rule,sizeof(XiangqiRule));
	rule2ChessTimer( rule, ctimer );
	SetPlayerTimer(DJGAME_XIANGQI_RED,&ctimer);
	SetPlayerTimer(DJGAME_XIANGQI_BLACK,&ctimer);
	
	m_qiuhe = new QPushButton( desktop() );
	m_qiuhe->setText( tr("Draw") );
	m_qiuhe->adjustSize();
	connect(m_qiuhe,SIGNAL(clicked()),this,SLOT(clickQiuHe()));
	
	m_renshu = new QPushButton( desktop() );
	m_renshu->setText( tr("Surrender") );
	m_renshu->adjustSize();
	connect(m_renshu,SIGNAL(clicked()),this,SLOT(clickRenShu()));
	
	connect( panelController->chatWidget()->textBrowser(), SIGNAL(acceptDrawChess()), SLOT(clickQiuHe()) );
}
XQDesktopController::~XQDesktopController()
{
	djDebug()<<"XQDesktopController destructor";
	
	delete m_blackTimer;
	delete m_redTimer;
}
QString	XQDesktopController::userItemNameSuffix( DJGameUser* user )
{
	if ( m_current.chRed && m_current.chRed == user->seatId() )
		return tr("(red)");
	else if ( m_current.chBlack && m_current.chBlack == user->seatId() )
		return tr("(black)");
	else
		return QString();
}
void XQDesktopController::initUnderGameInfo( const QByteArray& buf )
{	
	DJDesktopController::initUnderGameInfo(buf);
	
	init();
	memcpy(&m_current,buf.data(),sizeof(XiangqiCurrent));
	
	if( panelController()->isLookingOn() ) {
		m_qiuhe->hide();
		m_renshu->hide();
	}else {
		m_qiuhe->show();
		m_renshu->show();
	}
	
	m_fromPosItem->hide();
	m_toPosItem->hide();
}
void XQDesktopController::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	djDebug() << "XQDesktopController::gameWait" << mask << status;
	DJDesktopController::gameWait(mask,status,timeout);
	
	if(status == DJGAME_TABLE_STATUS_WAITSTART)
	{
		m_qiuhe->hide();
		m_renshu->hide();
	}else
	{
		m_qiuhe->show();
		m_renshu->show();
	}
	
	switch(status)
	{
		case DJGAME_XIANGQI_TABLE_STATUS_WAIT_MOVE :
		{
			m_isWaitingMoveACL = false;
			if( (1<<(m_current.chBlack-1)) & mask)
			{
				m_blackTimer->start();
				m_redTimer->pause();
			}else if( (1<<(m_current.chRed-1)) & mask)
			{
				m_redTimer->start();
				m_blackTimer->pause();
			}
			break;
		}
	}
	
}
void XQDesktopController::gameTraceModel( const GeneralGameTrace2Head *gameTrace)
{
	switch(gameTrace->chType)
	{
		case XIANGQI_GAMETRACE_ADJUST:
			memcpy(&m_board,gameTrace->chBuf,sizeof(XiangQiBoard));
			break;
		case XIANGQI_GAMETRACE_SETRULE :
		{
			PXiangqiRule prule = (PXiangqiRule)gameTrace->chBuf;
			XiangqiRule rule	= *prule;
			DJGameChessTimer ctimer;
			rule2ChessTimer( rule, ctimer );
			if(gameTrace->chSite == m_current.chBlack)
				SetPlayerTimer(DJGAME_XIANGQI_BLACK,&ctimer);
			else
				SetPlayerTimer(DJGAME_XIANGQI_RED,&ctimer);
			break;
		}
			
		case XIANGQI_GAMETRACE_MOVE :
		{
			PXiangQiMove pmove = (PXiangQiMove)gameTrace->chBuf;
			unsigned char chStartChip = XiangQi_GetNode(&m_board,XIANGQI_RULE_GETX(pmove->chStart),XIANGQI_RULE_GETY(pmove->chStart));
			unsigned char chEndChip = XiangQi_GetNode(&m_board,XIANGQI_RULE_GETX(pmove->chEnd),XIANGQI_RULE_GETY(pmove->chEnd));
		//	printf("%02x->%02x \n",pmove->chStart,pmove->chEnd);
			XiangQi_SetNodeByID(&m_board,pmove->chStart,0,0);
			XiangQi_SetNodeByID(&m_board,pmove->chEnd,chStartChip&0x07,chStartChip&0x08);
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
void XQDesktopController::gameTraceView( const GeneralGameTrace2Head *gameTrace )
{
	djDebug()<<"XQDesktopController::gameTraceView";
	
	switch(gameTrace->chType)
	{
		case XIANGQI_GAMETRACE_REQUESTDUELS: {
			
			DJGameUser *puser = panelController()->player(gameTrace->chSite);
			if( puser ) {
    			QString text	= puser->userName();
    			if ( m_hasRequestDraw ) {
    				text	+= tr(" accept draw");
    			}else {
    				text	+= tr(" request draw");
    			}
    			panelController()->insertText2Browser(text);
				//ShowAutoDisappearText(text);
				
				if ( gameTrace->chSite != panelController()->seatId()
					&& !m_hasRequestDraw ) {
					text	= DJSCHEME_AHREF
								.arg( DJSCHEME )
								.arg( DJSCHEME_EVENT_ACCEPT_DRAW_CHESS )
								.arg( puser->userId() )
								.arg( DJSCHEME_HOST )
								.arg( tr("Accept") );
					panelController()->insertText2Browser( text );
				}
				m_hasRequestDraw	= true;
			}
			break;
		}
		case XIANGQI_GAMETRACE_ADJUST:
			repaintCurrentStatus();
			break;
		case XIANGQI_GAMETRACE_MOVE : 
		{
			PXiangQiMove pmove = (PXiangQiMove)gameTrace->chBuf;
			ClearChip(XIANGQI_RULE_GETX(pmove->chStart),XIANGQI_RULE_GETY(pmove->chStart));
			ClearChip(XIANGQI_RULE_GETX(pmove->chEnd),XIANGQI_RULE_GETY(pmove->chEnd));
			DrawChip(XIANGQI_RULE_GETX(pmove->chEnd),XIANGQI_RULE_GETY(pmove->chEnd));
			int nx,ny;
			GetNodeXY(XIANGQI_RULE_GETX(pmove->chStart),XIANGQI_RULE_GETY(pmove->chStart),&nx,&ny);
			m_fromPosItem->setVirtualPos( QPoint(nx,ny) );
			m_fromPosItem->adjustPos( desktop()->graphicsMatrix() );
			m_fromPosItem->show();
			GetNodeXY(XIANGQI_RULE_GETX(pmove->chEnd),XIANGQI_RULE_GETY(pmove->chEnd),&nx,&ny);
			m_toPosItem->setVirtualPos( QPoint(nx,ny) );
			m_toPosItem->adjustPos( desktop()->graphicsMatrix() );
			m_toPosItem->show();
			
			if(pmove->chBuf[0] == 1)
			{
				panelController()->insertText2Browser(tr("check!"));
				playWave("jiang.wav");
			}else if(pmove->chBuf[0] == 2)
				playWave("kill.wav");
			else
				playWave("move.wav");
			break;
		}
		case XIANGQI_GAMETRACE_REPEAT :
		{
			/*
			QString str = tr("The phase repeat again (");
			str += QString("%1").arg(gameTrace->chBuf[0]);
			str += tr(" times)");
			ShowAutoDisappearText(str);
			*/
			break;
		}
		default:
			break;
	}
}

void XQDesktopController::repaintCurrentStatus()
{
	djDebug()<<"XQDesktopController::repaintCurrentStatus";
	
	qDeleteAll( m_chipItems );
	m_chipItems.clear();
	
	int i,j;
	for(i=1;i<=9;i++)
	{
		for(j=1;j<=10;j++)
		{
			DrawChip(i,j);
		}
	}
}
void XQDesktopController::init()
{
	memset( &m_board, 0, sizeof(XiangQiBoard) );
	memset( &m_current, 0, sizeof(XiangqiCurrent) );
	m_current.chRed = 1;
	m_current.chBlack = 2;
	
	qDeleteAll( m_chipItems );
	m_chipItems.clear();
	
	m_chStartPoint = 0;
	m_isWaitingMoveACL = false;
	m_hasRequestDraw 	= false;
}
void XQDesktopController::handleMatrixChanged()
{
	djDebug()<<"XQDesktopController::handleMatrixChanged";
	DJDesktopController::handleMatrixChanged();
}
void XQDesktopController::rule2ChessTimer( const XiangqiRule& rule, DJGameChessTimer& ctimer )
{
	ctimer.shBaseTime = rule.shBaseTime;
	ctimer.chTimeAfterBaseTime = rule.chTimeAfterBaseTime;
	ctimer.chTimes = rule.chTimes;
	ctimer.chTimeAfterTimeout = rule.chTimeAfterTimeout;
	ctimer.chSteps = 0;
}
void XQDesktopController::handleMousePressed( QGraphicsSceneMouseEvent *mouseEvent )
{
	djDebug()<<"XQDesktopController::handleMousePressed";
	
	if( panelController()->isLookingOn() )
		return;
	
	QPointF scenePos	= mouseEvent->scenePos();
	QPointF pos	= desktop()->graphicsMatrix().inverted().map(scenePos);
	QGraphicsItem *item	= desktop()->desktopScene()->itemAt( scenePos );
	
	if( !isWaitingForMe() || m_isWaitingMoveACL )
		return;
	if( 0 == item || (item == m_boardItem && 0 == m_chStartPoint) )
	{//点击棋盘
		djDebug()<<"click board";
		m_fromPosItem->hide();
		m_toPosItem->hide();
		m_chStartPoint = 0;
		return;
	}
	//bool bMove = false;
	unsigned char x,y,chIndex;
	if( item && DJGraphicsPixmapItem::DJGraphicsPixmapItemType == item->type() )
	{
		DJGraphicsPixmapItem *pixItem = (DJGraphicsPixmapItem *)item;
		QVariant pixType	= pixItem->data(0);
		if( pixType.isValid() && DESKTOP_IMAGE_ISCHIP(pixType.toInt()) )
		{
			djDebug()<<"click chip";
			chIndex = (unsigned char)(pixItem->data(2).toInt());
			x = XIANGQI_RULE_GETX(chIndex);
			y = XIANGQI_RULE_GETY(chIndex);
			unsigned char chChip = XiangQi_GetNode(&m_board,x,y);
			if(chChip != 0 && ( (XIANGQI_GETOWNER(chChip) == DJGAME_XIANGQI_RED && 
					panelController()->seatId() == m_current.chRed)
				|| (XIANGQI_GETOWNER(chChip) == DJGAME_XIANGQI_BLACK && 
					panelController()->seatId() == m_current.chBlack)) )
			{//自己的棋子
				djDebug()<<"self chip";
				m_fromPosItem->hide();
				m_chStartPoint = chIndex;
				playWave("click.wav");
				int nx,ny;
				GetNodeXY(x,y,&nx,&ny);
				m_toPosItem->setVirtualPos( QPoint(nx,ny) );
				m_toPosItem->adjustPos( desktop()->graphicsMatrix() );
				m_toPosItem->show();
				return;
			}
		}
		
		if(m_chStartPoint != 0)
		{//目标点
			x = (pos.x()-m_x)/(XIANGQI_CHIP_MAX_SIZE)+1;
			y = ((pos.y()-m_y)/(XIANGQI_CHIP_MAX_SIZE)+1);
			djDebug()<<"move chip"<<x<<y<<m_chStartPoint;
			if(x > 9 || y > 10)
			{
				m_fromPosItem->hide();
				m_toPosItem->hide();
				m_chStartPoint = 0;
				return;
			}
			y = 11-y;
			if( panelController()->seatId() != m_current.chRed )
			{
				x = 10-x;
				y = 11-y;
			}
			unsigned char chEndID = XIANGQI_RULE_GETNODEID(x,y);
			unsigned char owner;
			if( panelController()->seatId() == m_current.chRed )
				owner = DJGAME_XIANGQI_RED;
			else if( panelController()->seatId() == m_current.chBlack )
				owner = DJGAME_XIANGQI_BLACK;
			else
				return;
			djDebug()<<"check dest point"<<owner<<"startpoint"<<m_chStartPoint<<"endId"<<chEndID;
			if(XIANGQI_SUCCESS(XiangQi_Move(&m_board,owner,m_chStartPoint,chEndID)))
			{//发送
				djDebug()<<"send request";
				unsigned char buffer[30];
				GeneralGameTrace2Head *pmovet;
				PXiangQiMove			pmove;
				pmovet = (GeneralGameTrace2Head *)buffer;
				pmovet->chTable = panelController()->tableId();
				pmovet->chSite = panelController()->seatId();
				pmovet->chType = XIANGQI_GAMETRACE_MOVE;
				pmovet->chBufLen = sizeof(XiangQiMove);
				pmove = (PXiangQiMove )pmovet->chBuf;
				pmove->chStart = m_chStartPoint;
				pmove->chEnd = chEndID;
				m_isWaitingMoveACL = true;
				QByteArray buf	= panelController()->makeGameData( DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2,QByteArray((char *)buffer,sizeof(GeneralGameTrace2Head)+pmovet->chBufLen) );
				QVariant parameters	= QVariant::fromValue((void*)this);
				DJClientRequest request( buf ,HandleMoveACL,parameters);
				m_fromPosItem->hide();
				m_toPosItem->hide();
				panelController()->sendRequest( request );
			}else
			{
				djDebug()<<"dest is not valid point";
			}
		}
	}
}
void XQDesktopController::GetNodeXY(unsigned char x,unsigned char y,int *px,int *py)
{
	int nx,ny;
	nx = XIANGQI_CHIP_MAX_SIZE*(x-1)+(XIANGQI_CHIP_MAX_SIZE>>1);
	ny = XIANGQI_CHIP_MAX_SIZE*(10-y)+(XIANGQI_CHIP_MAX_SIZE>>1);
	if(panelController()->seatId() != m_current.chRed)
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
		*px = nx+m_x;
		*py = ny+m_y;
	}
	
}
void XQDesktopController::DrawChip(unsigned char x,unsigned char y)
{	
	int nx,ny;
	unsigned char chChip;
	char pixname[255];
	
	chChip = XiangQi_GetNode(&m_board,x,y);
	if(chChip != 0)
	{
		QPixmap pix;
		unsigned short sid = XIANGQI_RULE_GETNODEID(x,y);
		unsigned char chOwner = XIANGQI_GETOWNER(chChip);
		GetNodeXY(x,y,&nx,&ny);
		djDebug()<<"x="<<x<<"y="<<y<<"nx="<<nx<<"ny="<<ny;
		sprintf(pixname,":/ChineseChessRes/image/chips%d/%02x.png",m_shSkin,chChip);
		pix = QPixmap( QString(pixname) );
		DJGraphicsPixmapItem *item = new DJGraphicsPixmapItem(pix,0, desktop()->scene());
		item->setData( 0, DESKTOP_IMAGE_TYPE_CHIP+chOwner );
		item->setData( 1, chChip );
		item->setData( 2, sid );
		m_chipItems.insert( makeint16(x,y), item );
		item->setAlignment(Qt::AlignCenter);
		item->setVirtualPos( QPoint(nx,ny) );
		item->setExternalScale( desktop()->graphicsScale() );
		item->adjustPos( desktop()->graphicsMatrix() );
		item->setZValue(200);
		item->show();
	}
}
void XQDesktopController::ClearChip(unsigned char x,unsigned char y)
{	
	quint16 node	= makeint16(x,y);
	DJGraphicsPixmapItem *item = m_chipItems.value( node,0);
	if ( item ) {
		m_chipItems.remove( node );
		delete item;
	}
}
bool XQDesktopController::IsJiangJun(unsigned char chEnd)
{
	unsigned char chChip = XiangQi_GetNodeByID(&m_board,chEnd);
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
			chChip = XiangQi_GetNode(&m_board,x,y);
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
		chChip = XiangQi_GetNodeByID(&m_board,chEnd);
		if(XIANGQI_SUCCESS(XiangQi_Move(&m_board,XIANGQI_GETOWNER(chChip),chEnd,chJiangPos)))
		{
			//printf("jiang!\n");
			return true;
		}
	}
	return false;
	
}
void XQDesktopController::SetPlayerTimer(quint8 chOwner,PDJGameChessTimer rtimer)
{	
	djDebug()<<"SetPlayerTimer"<<chOwner<<rtimer;
	
	QPoint pa,pn,pt;
	quint8 chView,chSite=0;
	DJGraphicsChessTimerItem *curtimer=0;
	if(chOwner == DJGAME_XIANGQI_BLACK)
	{
		m_blackTimer->setTimer(rtimer);
		curtimer = m_blackTimer;
		chSite = m_current.chBlack;
	}else if(chOwner == DJGAME_XIANGQI_RED)
	{
		m_redTimer->setTimer(rtimer);
		curtimer = m_redTimer;
		chSite = m_current.chRed;
	}
	
	if(chSite != 0 )
	{
		djDebug()<<"chSite"<<chSite;
		chView = seat2View(chSite);

		if(chView == 1)
		{
			QRect showRect	= rectOfSeatShow( chSite );
			djDebug() << "view 1 showrect" << showRect;
			curtimer->setUp2Down( false );
			curtimer->setVirtualPos( QPointF(showRect.center().x(), showRect.top()) );
			curtimer->setExternalScale( desktop()->graphicsScale() );
			curtimer->adjustPos( desktop()->graphicsMatrix() );
		}else if(chView == 3)
		{
			QRect showRect	= rectOfSeatShow( chSite );
			djDebug() << "view 3 showrect" << showRect;
			curtimer->setUp2Down( true );
			curtimer->setVirtualPos( QPointF(showRect.center().x(), showRect.bottom()) );
			curtimer->setExternalScale( desktop()->graphicsScale() );
			curtimer->adjustPos( desktop()->graphicsMatrix() );
		}
	}
}
void XQDesktopController::handleCountdown(unsigned char countdown)
{
	QString wavName	= QString("number%1.wav").arg(countdown);
	playWave(wavName);
}
void XQDesktopController::clickQiuHe()
{
	if( panelController()->isLookingOn() )
		return;
	sendGameTrace(XIANGQI_GAMETRACE_REQUESTDUELS);
}
void XQDesktopController::clickRenShu()
{
	if( panelController()->isLookingOn() )
		return;
	sendGameTrace(XIANGQI_GAMETRACE_SURRENDER);
}
void XQDesktopController::locateAllNonMatrimoveItems()
{
	DJDesktopController::locateAllNonMatrimoveItems();
	
	QPoint pos( desktop()->realWidth(), (desktop()->realHeight() >> 1) + 60 );
	QMatrix matrix	= desktop()->graphicsMatrix();
	pos	= matrix.map( pos );
	//pos.rx()	-= m_btnStart->width()>>1;
	m_qiuhe->move( pos.x() - m_qiuhe->width(), pos.y() );
	m_renshu->move( pos.x() - m_renshu->width(), pos.y() + m_qiuhe->height() + 4 );
}
void XQDesktopController::locateStartButton()
{
	QMatrix matrix	= desktop()->graphicsMatrix();
	QPixmap pix = QPixmap(QString(":/ChineseChessRes/image/cchessb%1.png").arg(m_shSkin));
	int x = pix.width()>>1;
	int y = (desktop()->realHeight()>>1);
	QPoint pos(x,y);
	pos	= matrix.map( pos );
	pos.rx()	-= startButton()->width()>>1;
	startButton()->move( pos );
}