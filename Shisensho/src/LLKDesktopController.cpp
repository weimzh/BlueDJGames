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
#include "LLKDesktopController.h"
#include "DJPanelController.h"
#include "DJGraphicsPixmapItem.h"
#include "DJGraphicsTextItem.h"
#include "DJGraphicsChessTimerItem.h"
#include "DJGraphicsLineItem.h"
#include "DJGraphicsRectItem.h"
#include "DJDesktop.h"
#include "DJDesktopScene.h"
#include "DJGameRoom.h"
#include "DJGameUser.h"
#include "DJClientRequest.h"
#include "Utility.h"
#include "DJGroupChat.h"
#include "DJTextBrowser.h"
#include "DJSchemeEvent.h"

#define SHISENSHO_CARD_WIDTH						46
#define SHISENSHO_CARD_HEIGHT						56

#define SHISENSHO_TYPE_CARD							10
#define SHISENSHO_TYPE_LIVES						20

static	int	LLK_SELECT_DELTAHEIGHT			=		10;
static	int	LLK_LIVES_PIXSIZE				=		20;

static	int	LLK_STARTX						=		120;
static	int	LLK_STARTY						=		200;

static	int	LLK_LIFERECT_STARTX				=		160;
static	int	LLK_LIFERECT_STARTY				=		32;
static	int	LLK_LIFERECT_HEIGHT				=		36;
static	int	LLK_LIFERECT_BORDER				=		2;

static int SCORE_TEXT_ITEM_FONT_SIZE		= 		32;

void  HandleAdjustACL( const QByteArray& buf, const QVariant& parameters )
{
	//PDJGameProtocol pacl = (PDJGameProtocol)buf.data();
    LLKDesktopController* dc = static_cast<LLKDesktopController *>(parameters.value<void*>());
    
	dc->setWaitingAdjustACL(false);
}
void  HandleDeleteACL( const QByteArray& buf, const QVariant& parameters )
{
    PDJGameProtocol pacl = (PDJGameProtocol)buf.data();
    LLKDesktopController* dc = static_cast<LLKDesktopController *>(parameters.value<void*>());
    
    if(buf.size() == 0)
    {
        return ;
    }
    if( DJGAME_SUCCESS(pacl->chBuf[0]) )
	{
		;
	}else
	{
		dc->requestAdjust();
	}
}

LLKDesktopController::LLKDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent )
	: DJDesktopController(panelController,size,parent)
{
	djDebug()<<"LLKDesktopController constructor"<<size;
	
	memset(&m_current,0,sizeof(ShisenshoCurrent));
	memset(m_chMap,0,SHISENSHO_MAX_MAP+8);
	m_shSkin = 1;
	m_cardSize	= QSize( SHISENSHO_CARD_WIDTH, SHISENSHO_CARD_HEIGHT );
	m_origin	= QPoint( LLK_STARTX, LLK_STARTY );
	
	m_deleteTimer = new QTimer(this);
    connect(m_deleteTimer,SIGNAL(timeout()),this,SLOT(handleDeleteTimeout()));
    
    memset(&m_del,0,sizeof(ShisenshoDelete));
    setWaitingAdjustACL(false);
    
    m_selectedX = 0;
	m_selectedY = 0;
	
	m_connectLine 	= new DJGraphicsLineItem(0, desktop()->scene()); 
	m_connectLine->setPen(QPen(Qt::red,3));
	m_connectLine->setZValue(2000);
	
	m_btnReset = new QPushButton(tr("Reset"),desktop());
	m_btnReset->adjustSize();
	m_btnReset->hide();
	connect(m_btnReset,SIGNAL(clicked()),this,SLOT(handleResetClicked()));
	
	m_lifeRectBounder	= new DJGraphicsRectItem(0,desktop()->scene());
	m_lifeRectBounder->setRect( LLK_LIFERECT_STARTX, LLK_LIFERECT_STARTY, desktop()->realWidth() - LLK_LIFERECT_STARTX*2, LLK_LIFERECT_HEIGHT );
	m_lifeRectBounder->setPen(QPen(Qt::green,LLK_LIFERECT_BORDER));
	m_lifeRectBounder->setZValue(2000);
	m_lifeRectBounder->adjustPos( desktop()->graphicsMatrix() );
	m_lifeRectBounder->update();
	
	QRectF rect	= m_lifeRectBounder->rect();
	rect.adjust(LLK_LIFERECT_BORDER,LLK_LIFERECT_BORDER,-LLK_LIFERECT_BORDER,-LLK_LIFERECT_BORDER);
	m_lifeRect 	= new DJGraphicsRectItem(0, desktop()->scene()); 
	m_lifeRect->setRect( rect );
	m_lifeRect->setBrush( QBrush(Qt::red) );
	m_lifeRect->setZValue(2001);
	m_lifeRect->adjustPos( desktop()->graphicsMatrix() );
	m_lifeRect->update();
	
	m_timeline = new QTimeLine(1000, this);
 	m_timeline->setCurveShape(QTimeLine::LinearCurve);
 	connect(m_timeline, SIGNAL(frameChanged(int)), SLOT(handleFrameChanged(int)));
 	
 	QFont font;
	font.setPointSize( SCORE_TEXT_ITEM_FONT_SIZE );
	font.setBold( true );
	m_scoreText = new DJGraphicsTextItem( tr("Score:"), 0, desktop()->scene() );
	m_scoreText->setFont( font );
	m_scoreText->setVirtualPos( QPoint(desktop()->realWidth()/2, LLK_LIFERECT_STARTY + LLK_LIFERECT_HEIGHT + 4 ) );
	m_scoreText->setAlignment( Qt::AlignHCenter );
	m_scoreText->adjustPos( desktop()->graphicsMatrix() );
	
	m_score	= 0;
}
LLKDesktopController::~LLKDesktopController()
{
	djDebug()<<"LLKDesktopController destructor";
}
void LLKDesktopController::initCards()
{
	clearItems(SHISENSHO_TYPE_CARD);
	for( int i = 0; i <= SHISENSHO_MAX_MAP; i++ ) {
		for( int j = 0; j <= SHISENSHO_MAX_MAP; j++ ) {
			m_cards[i][j]	= 0;
		}
	}
	int z	= 200;
	for(int i=1;i<=m_current.rule.chWidth;i++)
	{
		for(int j=1;j<=m_current.rule.chHeight;j++)
		{
			m_cards[i][j] = new DJGraphicsPixmapItem(0,desktop()->scene());
			QPen pen( Qt::red );
			pen.setWidth( 2 );
			m_cards[i][j]->setPen( pen );
			m_cards[i][j]->setAlignment( Qt::AlignCenter);
			m_cards[i][j]->setZValue(z);
			z++;
		}
	}
}
void LLKDesktopController::initUnderGameInfo( const QByteArray& buf )
{	
	DJDesktopController::initUnderGameInfo(buf);
	
	memcpy(&m_current,buf.data(),sizeof(ShisenshoCurrent));
	memset(m_chMap,0,SHISENSHO_MAX_MAP+8);
	
	m_score	= letoh4(m_current.iScore);
	
	m_connectLine->hide();
	
	initCards();
	
	repaintLives();
	repaintScore();
}
void LLKDesktopController::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	DJDesktopController::gameWait(mask,status,timeout);
	
	QRectF rect	= m_lifeRectBounder->rect();
	rect.adjust(LLK_LIFERECT_BORDER,LLK_LIFERECT_BORDER,-LLK_LIFERECT_BORDER,-LLK_LIFERECT_BORDER);
	
	switch(status)
	{
	case  DJGAME_TABLE_STATUS_WAITSTART :
		djDebug()<<"DJGAME_TABLE_STATUS_WAITSTART";
		setClockTimeout(0,timeout);
		startButton()->setText(tr("Start"));
		m_lifeRect->setRect( rect );
		m_lifeRect->adjustPos( desktop()->graphicsMatrix() );
		m_lifeRect->update();
		
		m_timeline->stop();
		break;
	case DJGAME_SHISENSHO_TABLE_STATUS_WAIT_DELETE :
		djDebug()<<"DJGAME_SHISENSHO_TABLE_STATUS_WAIT_DELETE";
		m_lifeRect->setRect( rect );
		m_lifeRect->adjustPos( desktop()->graphicsMatrix() );
		m_lifeRect->update();
		
		m_timeline->setDuration( 1000 * timeout );
		m_timeline->setFrameRange(0,timeout);
		m_timeline->setCurrentTime(0);
		m_timeline->start();
		setClockTimeout(0,timeout);
		break;
	case DJGAME_SHISENSHO_TABLE_STATUS_WAIT_CONTINUE :
		djDebug()<<"DJGAME_SHISENSHO_TABLE_STATUS_WAIT_CONTINUE";
		setClockTimeout(0,timeout);
		startButton()->setText(tr("Continue"));
		startButton()->show();
		m_lifeRect->setRect( rect );
		m_lifeRect->adjustPos( desktop()->graphicsMatrix() );
		m_lifeRect->update();
		
		m_timeline->stop();
		break;
	default:
		break;
	}
}
void LLKDesktopController::gameTraceModel( const GeneralGameTrace2Head *gameTrace)
{
	DJDesktopController::gameTraceModel(gameTrace);
	switch( gameTrace->chType ) 
	{
	case SHISENSHO_GAMETRACE_ADJUST :
	{
		PShisenshoAdjust padjust = (PShisenshoAdjust)gameTrace->chBuf;
		ShisenshoRule_SetLines(m_chMap,m_current.rule.chWidth,m_current.rule.chHeight,
		padjust->chStartLine,padjust->chEndLine,padjust->chBuf);
		break;
	}
	case SHISENSHO_GAMETRACE_DELETE :
	{
		m_score	+= m_current.rule.chBaseScore<<1;
		PShisenshoDelete pdel = (PShisenshoDelete)gameTrace->chBuf;
		if(pdel->chWah != 0)
		{
			m_score	+= pdel->chWah*100;
		}
		break;
	}
	case SHISENSHO_GAMETRACE_RESET :
	{
		m_current.rule.chLifes = gameTrace->chBuf[0];
		repaintLives();
		break;
	}
	default:
		break;
	}
}
void LLKDesktopController::gameTraceView( const GeneralGameTrace2Head *gameTrace )
{
	djDebug()<<"LLKDesktopController::gameTraceView";
	DJDesktopController::gameTraceView(gameTrace);
		
	switch( gameTrace->chType ) 
	{
	case SHISENSHO_GAMETRACE_ADJUST :
	{
		initCards();
		playWave("move.wav");
		repaintCards();
		break;
	}
	case SHISENSHO_GAMETRACE_DELETE :
	{
		if(panelController()->isLookingOn())
			deleteCards(gameTrace);
		repaintScore();
		break;
	}
	default:
		break;
	}
}
void LLKDesktopController::locateClockPosition( QPoint& point, Qt::Alignment& align )
{
	djDebug() << "LLKDesktopController::locateClockPosition";
	
    point	= QPoint(1,1);
    align	= 0;
}
void LLKDesktopController::clearItems( int type )
{
	QList<QGraphicsItem*> items	= desktop()->desktopScene()->items();
	foreach ( QGraphicsItem *item, items ) {
		QVariant var0	= item->data(0);
		if ( var0.isValid() ) {
			if ( type == var0.toInt() ) {
				delete item;
			}
		}
	}
}
void LLKDesktopController::clearMarks()
{
	QList<QGraphicsItem*> items	= desktop()->desktopScene()->items();
	foreach ( QGraphicsItem *item, items ) {
		QVariant var0	= item->data(0);
		if ( var0.isValid() ) {
			int type	= var0.toInt();
			if ( SHISENSHO_TYPE_CARD == type ) {
				DJGraphicsPixmapItem *pixItem	= (DJGraphicsPixmapItem*)item;
				markCard(pixItem,false);
			}
		}
	}
}
void LLKDesktopController::markCard( DJGraphicsPixmapItem *item, bool flag )
{
	if ( 0 == item )
		return;
	item->setDJSelected(flag);
	item->adjustPos( desktop()->graphicsMatrix() );
	item->update();
}
void LLKDesktopController::repaintCards()
{
	djDebug()<<"LLKDesktopController::repaintCards";
	for(int i=1;i<=m_current.rule.chWidth;i++)
	{
		for(int j=1;j<=m_current.rule.chHeight;j++)
		{
			if ( 0 != m_cards[i][j] ) {
				unsigned char *pc = ShisenshoRule_GetPoint(m_chMap,m_current.rule.chWidth,m_current.rule.chHeight,i,j);
				if(pc != NULL && *pc != 0)
				{
					QPoint pos	= map2VirtualPos(m_origin,i,j);
					QPixmap pix = GetCardPixmap(*pc);
					m_cards[i][j]->setData(0,SHISENSHO_TYPE_CARD);
					m_cards[i][j]->setData(1,i);
					m_cards[i][j]->setData(2,j);
					m_cards[i][j]->setPixmap(pix);
					m_cards[i][j]->setVirtualPos( pos );
					m_cards[i][j]->setDeltaPos( QPointF(0,-LLK_SELECT_DELTAHEIGHT) );
					m_cards[i][j]->setExternalScale( desktop()->graphicsScale() );
					m_cards[i][j]->adjustPos( desktop()->graphicsMatrix() );
					m_cards[i][j]->show();
				}else {
					m_cards[i][j]->hide();
				}
			}
		}
	}
	m_selectedX = 0;
	m_selectedY = 0;
}
QPixmap LLKDesktopController::GetCardPixmap(unsigned char chCard)
{
	QString name	= QString(":/ShisenshoRes/image/suit%1/%2.png").arg(m_shSkin).arg(chCard);
	QPixmap pix = QPixmap( name );
	return pix;	
}
QPoint LLKDesktopController::map2VirtualPos( const QPoint& origin, quint8 indexx, quint8 indexy )
{
	QPoint result	= origin;
	
	int deltax	= ( indexx - 1 ) * m_cardSize.width();
	int deltay	= ( indexy - 1 ) * m_cardSize.height();
	
	result	+= QPoint(deltax,deltay);
	
	return result;
}
bool LLKDesktopController::isCloseable() const
{
	return true;
}
void LLKDesktopController::handleStartClicked()
{
	if(panelController()->isLookingOn())
		return ;
	startButton()->hide();
	if(gameWaitStatus() == DJGAME_SHISENSHO_TABLE_STATUS_WAIT_CONTINUE)
	{
		sendGameTrace( SHISENSHO_GAMETRACE_CONTINUE );
	}else if(gameWaitStatus() == DJGAME_TABLE_STATUS_WAITSTART)
	{
		memset(m_chMap,0,SHISENSHO_MAX_MAP+8);
		QByteArray buf	= panelController()->makeGameData( DJGAME_PROTOCOL_SUBTYPE_START );
		DJClientRequest request( buf );
		panelController()->sendRequest( request );	
		//c_iScore = 0;
	}
}
void LLKDesktopController::handleMousePressed( QGraphicsSceneMouseEvent *mouseEvent )
{
	djDebug()<<"LLKDesktopController::handleMousePressed";
	if(panelController()->isLookingOn() || gameWaitStatus() != DJGAME_SHISENSHO_TABLE_STATUS_WAIT_DELETE || m_isWaitingAdjustACL )
		return ;
	QPointF scenePos	= mouseEvent->scenePos();
	QPointF pos	= desktop()->graphicsMatrix().inverted().map(scenePos);
	QGraphicsItem *item	= desktop()->desktopScene()->itemAt( scenePos );
	
	if (mouseEvent->button() == Qt::LeftButton && item ) {
		djDebug()<<"left click item";
		if(m_del.chX[0] != 0)
			handleDeleteTimeout();
		unsigned char *p1	= 0;
		unsigned char *p2	= 0;
		p1 = ShisenshoRule_GetPoint(m_chMap,m_current.rule.chWidth,m_current.rule.chHeight,m_selectedX,m_selectedY);
		DJGraphicsPixmapItem *item1	= m_cards[m_selectedX][m_selectedY];
		DJGraphicsPixmapItem *item2	= 0;
		QVariant var	= item->data(0);
		if ( var.isValid() && SHISENSHO_TYPE_CARD == var.toInt() ) {
			item2	= (DJGraphicsPixmapItem*)item;
			p2 = ShisenshoRule_GetPoint(m_chMap,m_current.rule.chWidth,m_current.rule.chHeight,
						item2->data(1).toInt(),item2->data(2).toInt());
		}
		djDebug()<<"item1"<<item1<<"item2"<<item2;
		if( p1 == p2 )
			return;
		if(p1 == 0 || *p1 == 0)
		{
			djDebug()<<"p1 is null";
			clearMarks();
			if(p2 != NULL && *p2 != 0)
			{
				djDebug()<<"p2 is not null";
				m_selectedX = item2->data(1).toInt();
				m_selectedY = item2->data(2).toInt();
				markCard(item2,true);
			}
			return;
		}
		if(p2 == NULL || *p2 == 0)
		{
			djDebug()<<"p2 is null";
			if(item1 != NULL)
			{
				djDebug()<<"item1 is NOT null";
				markCard(item1,false);
			}
			m_selectedX = 0;
			m_selectedY = 0;
			return;
		}
		if(*p1 != *p2)
		{
			djDebug()<<"p1 != p2";
			if(item1 != NULL)
			{
				djDebug()<<"item1 is NOT null";
				markCard(item1,false);
			}
			
			m_selectedX = item2->data(1).toInt();
			m_selectedY = item2->data(2).toInt();
			markCard(item2,true);
			return;
		}
		
		markCard(item2,true);
		int x1	= m_selectedX;
		int y1	= m_selectedY;
		int x2	= item2->data(1).toInt();
		int y2	= item2->data(2).toInt();
				
		ShisenshoDelete	del;
		memset(&del,0,sizeof(ShisenshoDelete));
		del.chX[0] = x1;
		del.chY[0] = y1;
		del.chX[1] = x2;
		del.chY[1] = y2;
		if( ShisenshoRule_SearchPath(m_chMap,m_current.rule.chWidth,m_current.rule.chHeight,del.chX,del.chY) ) {
			djDebug()<<"have path";
			char buffer[255];
			GeneralGameTrace2Head *ptrace	= (GeneralGameTrace2Head *)buffer;
			memcpy( ptrace->chBuf, &del, sizeof(ShisenshoDelete) );
			ptrace->chTable = panelController()->tableId();
			ptrace->chSite = panelController()->seatId();
			ptrace->chType = SHISENSHO_GAMETRACE_DELETE;
			ptrace->chBufLen = sizeof(ShisenshoDelete);					
			QByteArray buf	= panelController()->makeGameData( DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2,QByteArray(buffer,sizeof(GeneralGameTrace2Head)+ptrace->chBufLen) );
			QVariant parameters	= QVariant::fromValue((void*)this);
			DJClientRequest request( buf ,HandleDeleteACL,parameters);
			panelController()->sendRequest( request );
			
			deleteCards(ptrace);
			m_selectedX = 0;
			m_selectedY = 0;
		}else {
			djDebug()<<"no path";
			markCard(item1,false);
			markCard(item2,false);
			m_selectedX = 0;
			m_selectedY = 0;
		}
	}else if ( mouseEvent->button() == Qt::RightButton ) {
		clearMarks();
		m_selectedX = 0;
		m_selectedY = 0;
	}
}
void LLKDesktopController::deleteCards( const GeneralGameTrace2Head *ptrace)
{
	djDebug()<<"LLKDesktopController::deleteCards";
	PShisenshoDelete pdel = (PShisenshoDelete)ptrace->chBuf;
	if(!ShisenshoRule_Check(m_chMap,m_current.rule.chWidth,m_current.rule.chHeight,pdel->chX,pdel->chY) ) {
		requestAdjust();
		return;
	}
	QList<QPointF> points;
	for( int i=0; i<4; i++ ) {
		int	x	= pdel->chX[i];
		int y	= pdel->chY[i];
		if( 0 == x && 0 == y )
			break;
		QPoint pos	= map2VirtualPos(m_origin,x,y);
		//pos	+= QPoint(0,-LLK_SELECT_DELTAHEIGHT);
		points << pos;
	}
	m_connectLine->setPoints( points );
	m_connectLine->adjustPos( desktop()->graphicsMatrix() );
	m_connectLine->show();
	
	if(m_del.chX[0] != 0)
		handleDeleteTimeout();
	memcpy(&m_del,pdel,sizeof(ShisenshoDelete));
	m_deleteTimer->start(300);
}
void LLKDesktopController::handleDeleteTimeout()
{
	djDebug()<<"LLKDesktopController::handleDeleteTimeout";
	int sx,sy,ex,ey;
	unsigned char *p;
	m_deleteTimer->stop();
	m_connectLine->hide();
	
	sx = m_del.chX[0];
	sy = m_del.chY[0];
	markCard(m_cards[sx][sy],false);
	for(int i=1; i<4; i++ )
	{
		p = ShisenshoRule_GetPoint(m_chMap,m_current.rule.chWidth,m_current.rule.chHeight,m_del.chX[i],m_del.chY[i]);
		if(p != NULL && *p != 0)
		{
			ex = m_del.chX[i];
			ey = m_del.chY[i];
			playWave("kill.wav");
			markCard(m_cards[ex][ey],false);
			ShisenshoRule_DeleteCard(m_chMap,m_current.rule.chWidth,m_current.rule.chHeight,
									sx,sy,ex,ey,m_current.rule.chAdjustOri,NULL,NULL);
			break;
		}
	}
	memset(&m_del,0,sizeof(ShisenshoDelete));
	repaintCards();
}
void LLKDesktopController::requestAdjust()
{
	if ( panelController()->isLookingOn() )
		return;
	setWaitingAdjustACL(true);
	QVariant parameters	= QVariant::fromValue((void*)this);
	sendGameTrace( SHISENSHO_GAMETRACE_ADJUST, QByteArray(), HandleAdjustACL, parameters );
	/*
	char buffer[20];
	GeneralGameTrace2Head *ptrace;
	ptrace = (GeneralGameTrace2Head *)buffer;
	ptrace->chTable = panelController()->tableId();
	ptrace->chSite = panelController()->seatId();
	ptrace->chType = SHISENSHO_GAMETRACE_ADJUST;
	ptrace->chBufLen = 0;
	QByteArray buf	= panelController()->makeGameData( DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2,QByteArray(buffer,sizeof(GeneralGameTrace2Head)) );
	QVariant parameters	= QVariant::fromValue((void*)this);
	DJClientRequest request( buf ,HandleAdjustACL,parameters);
	panelController()->sendRequest( request );
	*/
}
void LLKDesktopController::repaintCurrentStatus()
{
	repaintCards();
	repaintScore();
}
void LLKDesktopController::handleResetClicked()
{
	if(panelController()->isLookingOn())
		return;
	if(m_current.rule.chLifes == 0)
	{
		m_btnReset->hide();
		return;
	}
	setWaitingAdjustACL(true);
	QVariant parameters	= QVariant::fromValue((void*)this);
	sendGameTrace( SHISENSHO_GAMETRACE_RESET, QByteArray(), HandleAdjustACL, parameters );
	/*
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
	*/
}
void LLKDesktopController::locateAllNonMatrimoveItems()
{
	djDebug()<<"LLKDesktopController::locateAllNonMatrimoveItems";
	
	DJDesktopController::locateAllNonMatrimoveItems();
	djDebug()<<"reset button width"<<m_btnReset->width()<<"desktop width"<<desktop()->width();
	m_btnReset->move( desktop()->width() - m_btnReset->width(), 10 );
	//m_renshu->move( pos.x() - m_renshu->width(), pos.y() + m_qiuhe->height() + 4 );
}
void LLKDesktopController::repaintLives()
{
	djDebug()<<"LLKDesktopController::repaintLives";
	
	clearItems(SHISENSHO_TYPE_LIVES);
	
	int lives	= m_current.rule.chLifes;
	
	djDebug()<<"lives"<<lives;
	
	if( 0 == lives ) {
		m_btnReset->hide();
		return;
	}
	if(!panelController()->isLookingOn()){
		m_btnReset->show();
	}
		
	QPixmap pix(":/ShisenshoRes/image/base/game.png");
	DJGameUser *user	= panelController()->player(panelController()->seatId());
	if ( user ) {
		pix	= user->avatar();
	}
	pix	= pix.scaled( LLK_LIVES_PIXSIZE, LLK_LIVES_PIXSIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
	int x	= desktop()->viewport()->width();
	for(int i=0;i<lives;i++) {
		DJGraphicsPixmapItem *item = new DJGraphicsPixmapItem(pix,0,desktop()->scene(),false);
		item->setData(0,SHISENSHO_TYPE_LIVES);
		item->setVirtualPos( QPoint(x,48));
		item->setAlignment(Qt::AlignRight);
		item->setZValue(2000);
		//item->setExternalScale( desktop()->graphicsScale() );
		item->adjustPos( desktop()->graphicsMatrix() );
		item->show();
		x	-= LLK_LIVES_PIXSIZE;
	}
}
void LLKDesktopController::repaintAll()
{
	DJDesktopController::repaintAll();
		
	repaintLives();
	repaintScore();
}
void LLKDesktopController::handleMatrixChanged()
{
	DJDesktopController::handleMatrixChanged();
		
	repaintLives();
}
void LLKDesktopController::handleFrameChanged( int frame )
{
	djDebug()<<"LLKDesktopController::handleFrameChanged"<<frame;
	
	qreal timeleft	= (qreal)(m_timeline->endFrame()-frame) / m_timeline->endFrame();
	
	djDebug()<<"endframe"<<m_timeline->endFrame()<<"percent"<<timeleft;
	
	int len	= (desktop()->realWidth() - LLK_LIFERECT_STARTX*2 - LLK_LIFERECT_BORDER) * timeleft;
	
	djDebug()<<"total len"<<desktop()->realWidth() - LLK_LIFERECT_STARTX*2<<"len"<<len;
	
	QRectF rect	= m_lifeRectBounder->rect();
	rect.adjust(LLK_LIFERECT_BORDER,LLK_LIFERECT_BORDER,-LLK_LIFERECT_BORDER,-LLK_LIFERECT_BORDER);
	rect.setWidth( len );
	m_lifeRect->setRect( rect );
	m_lifeRect->adjustPos( desktop()->graphicsMatrix() );
	m_lifeRect->update();
}
void LLKDesktopController::repaintScore()
{
	djDebug()<<"TLJDesktopController::repaintScore";
	
	QString score	= QString::number(m_score);
	
	score.prepend("<font color=#ff0000>");
	score.append("</font>");
	
	score.prepend( tr("Score:") );
	m_scoreText->setHtml( score );
}