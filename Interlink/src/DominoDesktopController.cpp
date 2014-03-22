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
#include "DominoDesktopController.h"
#include "DJPanelController.h"
#include "DJGraphicsPixmapItem.h"
#include "DJGraphicsTextItem.h"
#include "DJGraphicsChessTimerItem.h"
#include "DJDesktop.h"
#include "DJDesktopScene.h"
#include "DJGameRoom.h"
#include "DJGameUser.h"
#include "DJClientRequest.h"
#include "Utility.h"
#include "DJDesktopItem.h"

#define DOMINO_CHAIN_LEFT2RIGHT				1
#define DOMINO_CHAIN_BOTTOM2TOP				2
#define DOMINO_CHAIN_RIGHT2LEFT				3
#define DOMINO_CHAIN_TOP2BOTTOM				4
#define DOMINO_CHAIN_ROOT					10
#define DOMINO_CHAIN_VERTICAL				20
#define DOMINO_CHAIN_HORIZONTAL				21

static int TEXT_ITEM_FONT_SIZE		= 	16;

void HandlePlaceACL( const QByteArray& buf, const QVariant& parameters )
{
    PDJGameProtocol pacl = (PDJGameProtocol)buf.data();
    DominoDesktopController* dc = static_cast<DominoDesktopController *>(parameters.value<void*>());    
	
    dc->setWaitingPlaceACL(false);
}

DominoDesktopController::DominoDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent )
	: DJDesktopController(panelController,size,parent)
{
	djDebug()<<"DominoDesktopController constructor"<<size;
		
	QPixmap pix(":/InterlinkRes/image/v/null.png");
	m_cardSize	= pix.size();
	m_cardDistance	= m_cardSize.width() + 2;
	
	m_btnZhua = new QPushButton(desktop());
	m_btnZhua->setText( tr("draw") );
	m_btnZhua->adjustSize();
	connect(m_btnZhua,SIGNAL(clicked()),this,SLOT(clickZhua()));
	m_btnZhua->hide();
	for( int i = 0; i <= panelController->numberOfSeats(); i++) {
		m_score << 0;
	}
	
	QFont font;
	font.setPointSize( TEXT_ITEM_FONT_SIZE );
	font.setBold( true );
	
	m_sumTextItem = new DJGraphicsTextItem( tr("sum:"), 0, desktop()->scene(), false );
	m_sumTextItem->setFont( font );
	
	m_remainTextItem = new DJGraphicsTextItem( tr("remains:"), 0, desktop()->scene(), false );
	m_remainTextItem->setFont( font );
	
	init();
}
DominoDesktopController::~DominoDesktopController()
{
	djDebug()<<"DominoDesktopController destructor";
}
void DominoDesktopController::init()
{
	memset(&m_current,0,sizeof(DominoCurrent));
	memset(&m_chains,DOMINO_CARD_ERRORID,sizeof(DominoChains));
	m_chains.chChainPages = 0;
	m_isWaitingPlaceACL	= false;
	for( int i = 0; i <= panelController()->numberOfSeats(); i++) {
		m_score[i]	= 0;
	}
}
void DominoDesktopController::initUnderGameInfo( const QByteArray& buf )
{	
	DJDesktopController::initUnderGameInfo(buf);
	
	init();
	
	memcpy(&m_current,buf.data(),sizeof(DominoCurrent));
	
	repaintSum(0);
	repaintRemains(0);
}
void DominoDesktopController::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	DJDesktopController::gameWait(mask,status,timeout);
	
	m_btnZhua->hide();
	
	switch(status) {
	case DJGAME_DOMINO_TABLE_STATUS_WAIT_PLACE:
		if( isWaitingForMe() && !panelController()->isLookingOn() )
		{
			m_isWaitingPlaceACL	= false;
		}
		break;
	case DJGAME_DOMINO_TABLE_STATUS_WAIT_DRAW: {
		if( isWaitingForMe() && !panelController()->isLookingOn() )
		{
			m_btnZhua->show();
		}
		break;
	}
	default:
		break;
	}
}
void DominoDesktopController::gameTraceModel( const GeneralGameTrace2Head *gameTrace )
{
	djDebug() << "DominoDesktopController::gameTraceModel";
	
	quint8 gameTraceSeat	= gameTrace->chSite;
	quint8 bufLen	= gameTrace->chBufLen;
	const unsigned char *buf	= gameTrace->chBuf;
	
	djDebug()<<"gameTraceSeat"<<gameTraceSeat<<"bufLen"<<bufLen;
	DebugBuffer( (const char*)buf, bufLen );
	
	switch(gameTrace->chType) {
	case DOMINO_GAMETRACE_PICKUP: {
		djDebug()<<"DOMINO_GAMETRACE_PICKUP"<<"seat"<<gameTraceSeat;
		PDominoPickup ppickup = (PDominoPickup)gameTrace->chBuf;
		if ( DOMINO_PICKUP_PROPERTIES_RESET == ppickup->chProperties ) {
			djDebug()<<"DOMINO_PICKUP_PROPERTIES_RESET";
			clearDesktopItems( gameTraceSeat, DOMINO_TYPE_HANDCARD );
		}
		for ( quint8 i = 0; i < ppickup->chPages; i++ ) {
			if ( DOMINO_CARD_BACK == ppickup->chCards[i]
				&& gameTraceSeat == panelController()->seatId()
				&& !panelController()->isLookingOn() )
			continue;
			djDebug()<<"append item"<<hex<<"card"<<ppickup->chCards[i];
			appendDesktopItem( gameTraceSeat, DOMINO_TYPE_HANDCARD, ppickup->chCards[i] );
		}
		repaintRemains(ppickup->chReserves);
		break;
	}
	case DOMINO_GAMETRACE_PLACE: {
		djDebug()<<"DOMINO_GAMETRACE_PLACE"<<"seat"<<gameTraceSeat;
		PDominoPlace pplace = (PDominoPlace)gameTrace->chBuf;
		djDebug()<<"take item"<<hex<<"card"<<pplace->chCard;
		DJDesktopItem *item	= takeDesktopItem( gameTraceSeat, DOMINO_TYPE_HANDCARD, pplace->chCard );
		djDebug()<<"taken item ="<<item;
		delete item;
		
		m_isWaitingPlaceACL = false;
		if( !DOMINO_ISCARD(m_chains.root))
		{
			m_chains.root = pplace->chCard;
			m_chains.chChainPages = 0;
		}else
		{
	//			printf("%d place chainid=%d card=%02x\n",gameTrace->chSite,pplace->chChainID,pplace->chCard);
			unsigned char chCard = pplace->chCard;
			m_chains.chain[m_chains.chChainPages][DOMINOCHAINS_CHAIN_ID] = pplace->chChainID;
			m_chains.chain[m_chains.chChainPages][DOMINOCHAINS_CHAIN_CARD] = chCard;
			m_chains.chLastCard[pplace->chChainID] = chCard;
			m_chains.chChainPages ++;
		}
		
		if(pplace->chDesktopNumber > 0 && (pplace->chDesktopNumber%5) == 0)
		{
			for(int i=1;i<=panelController()->numberOfSeats();i++)
			{
				if( (i&1) == (gameTraceSeat&1))
					m_score[i]	+= pplace->chDesktopNumber;
				else
					m_score[i]	-= pplace->chDesktopNumber;
			}
		}
		repaintSum(pplace->chDesktopNumber);
		break;
	}
	case DOMINO_GAMETRACE_ADJUST: {
		memcpy(&m_chains,gameTrace->chBuf,sizeof(DominoChains));
		break;
	}
	default:
		break;
	}
}
void DominoDesktopController::gameTraceView( const GeneralGameTrace2Head *gameTrace )
{
	djDebug()<<"DominoDesktopController::gameTraceView";
	
	quint8 gameTraceSeat	= gameTrace->chSite;
	quint8 bufLen	= gameTrace->chBufLen;
	const unsigned char *buf	= gameTrace->chBuf;
	
	switch( gameTrace->chType ) {
	case DOMINO_GAMETRACE_PICKUP:
		repaintHandCards( gameTraceSeat, DOMINO_TYPE_HANDCARD, true, true, 1 );
		break;
	case DOMINO_GAMETRACE_PLACE: {
		playWave("move.wav");
		PDominoPlace pplace = (PDominoPlace)gameTrace->chBuf;
		if(pplace->chDesktopNumber > 0 && (pplace->chDesktopNumber%5) == 0)
		{
			for(int i=1;i<=panelController()->numberOfSeats();i++)
			{
				if ( i != panelController()->seatId() )
					continue;
				if( (i&1) == (gameTraceSeat&1)) {
					playWave("inc.wav");
				}else {
					playWave("dec.wav");
				}
			}
		}
		repaintHandCards( gameTraceSeat, DOMINO_TYPE_HANDCARD, true, true, 1 );
		RepaintLink();
		fillPlayerInfo();
		repaintPlayerInfo();
		break;
	}
	case DOMINO_GAMETRACE_ADJUST:
		RepaintLink();
		break;
	default:
		break;
	}
}
void DominoDesktopController::RepaintLink()
{
	int c_Fx	= 100;
	
	clearDeskCards();
	DJGraphicsPixmapItem *rootitem;
	
	if(!DOMINO_ISCARD(m_chains.root))
	{
		DrawBone(desktop()->realWidth()>>1,desktop()->realHeight()>>1,DOMINO_CARD_NULL,0,DOMINO_CHAIN_ROOT);
		return;
	}
	unsigned char chRootCard = DOMINO_CARD_REVERSE(m_chains.root);
	rootitem = DrawBone(desktop()->realWidth()>>1,desktop()->realHeight()>>1,chRootCard,0,DOMINO_CHAIN_ROOT);
	if(rootitem == NULL)
		return;
	int x,y;
	QRect rect;
	unsigned char i,max=4;
//	printf("repaint link root=%02x, chain1=%02x, chain2=%02x\n",c_chains.root,
//				c_chains.chLastCard[1],c_chains.chLastCard[2]);
	if(DOMINO_CARD_GET1(m_chains.root) != DOMINO_CARD_GET2(m_chains.root) ||
		!DOMINO_ISCARD(m_chains.chLastCard[1]) || !DOMINO_ISCARD(m_chains.chLastCard[2]) )
		max = 2;
	
	for(i=1;i<=max;i++)
	{
		switch(i)
		{
			case 1 :
			{
				x = ((int)rootitem->virtualPos().x())+rootitem->boundingRect().toRect().width();
				y = ((int)rootitem->virtualPos().y())+(rootitem->boundingRect().toRect().height()>>1);
				rect.setLeft(centerPoint().x());
				rect.setTop(availableSpace().top());
				rect.setRight( availableSpace().right() );
				rect.setBottom( centerPoint().y() );
				//rect.setLeft(x+c_Fx);
				//rect.setTop(0);
				//rect.setWidth((availableSpace().width()>>1)-c_Fx/5);
				//rect.setHeight(availableSpace().height()>>1);
				if(!RepaintSingleChain(1,x,y,DOMINO_CHAIN_LEFT2RIGHT,rect) && c_Fx>10)
				{
					c_Fx -= 10;
					clearDeskCards();
					rootitem = DrawBone(desktop()->realWidth()>>1,desktop()->realHeight()>>1,chRootCard,0,DOMINO_CHAIN_ROOT);
					i = 0;
				}
				break;
			}
			case 2 :
			{
				x = ((int)rootitem->virtualPos().x());
				y = ((int)rootitem->virtualPos().y())+(rootitem->boundingRect().toRect().height()>>1);
				rect.setLeft(availableSpace().left());
				rect.setTop(centerPoint().y());
				rect.setRight( centerPoint().x() );
				rect.setBottom( availableSpace().bottom() );
				//rect.setLeft(0);
				//rect.setTop((availableSpace().height()>>1));
				//rect.setWidth((availableSpace().width()>>1)-c_Fx/5);
				//rect.setHeight(availableSpace().height()>>1);
				if(!RepaintSingleChain(2,x,y,DOMINO_CHAIN_RIGHT2LEFT,rect) && c_Fx>10)
				{
					c_Fx -= 10;
					clearDeskCards();
					rootitem = DrawBone(desktop()->realWidth()>>1,desktop()->realHeight()>>1,chRootCard,0,DOMINO_CHAIN_ROOT);
					i = 0;
				}
				break;
			}
			case 3 :
			{
				x = ((int)rootitem->virtualPos().x())+(rootitem->boundingRect().toRect().width()>>1);
				y = ((int)rootitem->virtualPos().y());
				rect.setLeft(availableSpace().left());
				rect.setTop(availableSpace().top());
				rect.setRight(centerPoint().x());
				rect.setBottom(centerPoint().y());
				//rect.setLeft(0);
				//rect.setTop(0);
				//rect.setWidth(availableSpace().width()>>1);
				//rect.setHeight(availableSpace().height()>>1);
				if(!RepaintSingleChain(3,x,y,DOMINO_CHAIN_BOTTOM2TOP,rect) && c_Fx>10)
				{
					c_Fx -= 10;
					clearDeskCards();
					rootitem = DrawBone(desktop()->realWidth()>>1,desktop()->realHeight()>>1,chRootCard,0,DOMINO_CHAIN_ROOT);
					i = 0;
				}
				break;
			}
			case 4 :
			{
				x = ((int)rootitem->virtualPos().x())+(rootitem->boundingRect().toRect().width()>>1);
				y = ((int)rootitem->virtualPos().y())+(rootitem->boundingRect().toRect().height());
				rect.setLeft(centerPoint().x());
				rect.setTop(centerPoint().y());
				rect.setRight(availableSpace().right());
				rect.setBottom(availableSpace().bottom());
				//rect.setLeft(availableSpace().width()>>1);
				//rect.setTop(availableSpace().height()>>1);
				//rect.setWidth(availableSpace().width()>>1);
				//rect.setHeight(availableSpace().height()>>1);
				if(!RepaintSingleChain(4,x,y,DOMINO_CHAIN_TOP2BOTTOM,rect) && c_Fx>10)
				{
					c_Fx -= 10;
					clearDeskCards();
					rootitem = DrawBone(desktop()->realWidth()>>1,desktop()->realHeight()>>1,chRootCard,0,DOMINO_CHAIN_ROOT);
					i = 0;
				}
				break;
			}
		}//switch
	}//for
}
bool DominoDesktopController::RepaintSingleChain(unsigned char chChainID,int x,int y,unsigned char chOri,QRect & rect)
{
	unsigned char chCard,chPages=0;
	char chStart = 0;
	int iCardMaxSize = 42;
	
	DJGraphicsPixmapItem *item;
	do
	{
		chCard = GetCardOnChain(&m_chains,chChainID,1,&chStart);
		if(!DOMINO_ISCARD(chCard))
			break;
		chPages++;
		if(chOri == DOMINO_CHAIN_RIGHT2LEFT || chOri == DOMINO_CHAIN_TOP2BOTTOM)
			chCard = DOMINO_CARD_REVERSE(chCard);
		item = DrawBone(x,y,chCard,chChainID,chOri);
		int w	= item->boundingRect().toRect().width();
		int h	= item->boundingRect().toRect().height();
		switch(chOri)
		{	
			case DOMINO_CHAIN_LEFT2RIGHT :
			{
				if(x+w > rect.right()-iCardMaxSize-(iCardMaxSize>>1))
				{
					chOri = DOMINO_CHAIN_BOTTOM2TOP;
					x += (w-(w>>2));
					y -= (h>>1);
					if(rect.bottom()-rect.top() < (iCardMaxSize<<1))
						return false;
					rect.setBottom(rect.bottom()-(iCardMaxSize>>1));
				}else
					x += w;
				break;
			}
			case DOMINO_CHAIN_RIGHT2LEFT :
			{
				if(x-w < rect.left()+iCardMaxSize+(iCardMaxSize>>1))
				{
					chOri = DOMINO_CHAIN_TOP2BOTTOM;
					x -=(w-(w>>2));
					y += (h>>1);
					if(rect.bottom()-rect.top() < (iCardMaxSize<<1))
						return false;
					rect.setTop(rect.top()+(iCardMaxSize>>1));
				}else
					x -= w;
				break;
			}
			case DOMINO_CHAIN_BOTTOM2TOP :
			{
				if(y-h < rect.top()+iCardMaxSize+(iCardMaxSize>>1))
				{
					chOri = DOMINO_CHAIN_RIGHT2LEFT;
					y -= (h-(h>>2));
					x -= (w>>1);
					if(rect.right()-rect.left() < (iCardMaxSize<<1))
						return false;
					rect.setRight(rect.right()-(iCardMaxSize>>1));
				}else
					y -= h;
				break;
			}
			case DOMINO_CHAIN_TOP2BOTTOM :
			{
				if(y+h > rect.bottom()-iCardMaxSize-(iCardMaxSize>>1))
				{
					chOri = DOMINO_CHAIN_LEFT2RIGHT;
					x += (w>>1);
					y += (h-(h>>2));
					if(rect.right()-rect.left() < (iCardMaxSize<<1))
						return false;
					rect.setLeft(rect.left()+(iCardMaxSize>>1));
				}else
					y += h;
				break;
			}
		}
	}while(true);
	DrawBone(x,y,DOMINO_CARD_NULL,chChainID,chOri);
	return true;
}
QPixmap DominoDesktopController::cardPixmap( quint8 card, quint8 direction )
{
	char cardname[255],vh[2]={0,0};
	switch(direction)
	{
		case DOMINO_CHAIN_LEFT2RIGHT :
		case DOMINO_CHAIN_RIGHT2LEFT :
			if(DOMINO_ISCARD(card) && DOMINO_CARD_GET1(card) == DOMINO_CARD_GET2(card))
				vh[0] ='v';
			else
				vh[0] = 'h';
			break;
		case DOMINO_CHAIN_ROOT :
		{
			if(card == DOMINO_CARD_NULL || DOMINO_CARD_GET1(card) == DOMINO_CARD_GET2(card))
				vh[0] ='v';
			else
				vh[0] ='h';
			break;
		}
		case DOMINO_CHAIN_BOTTOM2TOP :
		case DOMINO_CHAIN_TOP2BOTTOM :
			if(DOMINO_ISCARD(card) && DOMINO_CARD_GET1(card) == DOMINO_CARD_GET2(card))
				vh[0] ='h';
			else
				vh[0] = 'v';
			break;
		case DOMINO_CHAIN_VERTICAL:
			vh[0] ='v';
			break;
 		case DOMINO_CHAIN_HORIZONTAL:
 			vh[0] ='h';
 			break;
 		default:
 			break;
	}
	if(card == DOMINO_CARD_NULL)
		sprintf(cardname,":/InterlinkRes/image/%s/null.png",vh);
	else if( card == DOMINO_CARD_BACK || !DOMINO_ISCARD(card))
		sprintf(cardname,":/InterlinkRes/image/%s/back.png",vh);
	else
		sprintf(cardname,":/InterlinkRes/image/%s/%02x.png",vh,card);
	
	return QPixmap(QString(cardname));
}
DJGraphicsPixmapItem* DominoDesktopController::DrawBone(int x,int y,unsigned char chCard,unsigned char chChainID,unsigned char chOri)
{
	QPixmap pix = cardPixmap(chCard,chOri);
	
	DJGraphicsPixmapItem *item = new DJGraphicsPixmapItem(pix,0,desktop()->scene());
	int w	= item->boundingRect().toRect().width();
	int h	= item->boundingRect().toRect().height();
		
	switch(chOri)
	{	
		case DOMINO_CHAIN_ROOT	:
		{
			x -= w >> 1;
			y -= h >> 1;
			break;
		}
		case DOMINO_CHAIN_LEFT2RIGHT :
			y -= h >> 1;
			break;
		case DOMINO_CHAIN_BOTTOM2TOP :
			y -= h;
			x -= w >> 1;
			break;
		case DOMINO_CHAIN_RIGHT2LEFT :
			y -= h >> 1;
			x -= w;
			break;
		case DOMINO_CHAIN_TOP2BOTTOM :
			x -= w >> 1;
			break;
	}
	item->setVirtualPos( QPoint(x,y) );
	item->setData( KEY_OF_TYPE, DOMINO_TYPE_DESKCARD );
	item->setData( KEY_OF_SEAT, chChainID );
	item->setData( KEY_OF_VALUE, chCard );
	item->setZValue(200);
	if ( chCard == DOMINO_CARD_NULL ) {
		QPen pen( Qt::red );
		pen.setWidth( 2 );
		item->setPen( pen );
	}
	item->setExternalScale( desktop()->graphicsScale() );
	item->adjustPos( desktop()->graphicsMatrix() );
	item->show();
	return item;
}
void DominoDesktopController::clearDeskCards()
{
	djDebug()<<"DominoDesktopController::clearDeskCards";
	QList<QGraphicsItem*> items	= desktop()->desktopScene()->items();
	foreach ( QGraphicsItem *item, items ) {
		QVariant varType	= item->data(KEY_OF_TYPE);
		if ( varType.isValid() ) {
			int type	= varType.toInt();
			if ( DOMINO_TYPE_DESKCARD == type ) {
				delete item;
			}
		}
	}
}
void DominoDesktopController::clearHandCardsMark()
{
	QList<QGraphicsItem*> items	= desktop()->desktopScene()->items();
	foreach ( QGraphicsItem *item, items ) {
		QVariant varSeat	= item->data(KEY_OF_SEAT);
    	QVariant varType	= item->data(KEY_OF_TYPE);
    	if( varSeat.isValid() && varType.isValid() ) {
    		int seat	= varSeat.toInt();
    		int type	= varType.toInt();
    		if ( DOMINO_TYPE_HANDCARD == type &&
    			seat == panelController()->seatId() ) {
    			DJGraphicsPixmapItem *pixItem	= (DJGraphicsPixmapItem*)item;
    			pixItem->setDJSelected( false );
    			pixItem->update();
    		}
    	}
	}
}
quint8 DominoDesktopController::selectedHandCard()
{
	QList<QGraphicsItem*> items	= desktop()->desktopScene()->items();
	foreach ( QGraphicsItem *item, items ) {
		QVariant varSeat	= item->data(KEY_OF_SEAT);
    	QVariant varType	= item->data(KEY_OF_TYPE);
    	if( varSeat.isValid() && varType.isValid() ) {
    		int seat	= varSeat.toInt();
    		int type	= varType.toInt();
    		if ( DOMINO_TYPE_HANDCARD == type &&
    			seat == panelController()->seatId() ) {
    			DJGraphicsPixmapItem *pixItem	= (DJGraphicsPixmapItem*)item;
    			if ( pixItem->isDJSelected() ) {
    				return pixItem->data(KEY_OF_VALUE).toInt();
    			}
    		}
    	}
	}
	return DOMINO_CARD_ERRORID;
}
void DominoDesktopController::repaintSum(int sum)
{
	m_sumTextItem->setPlainText(tr("sum:")+QString::number(sum));
}
void DominoDesktopController::repaintRemains(int remains )
{
	m_remainTextItem->setPlainText(tr("remains:")+QString::number(remains));
}
void DominoDesktopController::repaintCurrentStatus()
{
	RepaintLink();
	for( int seat = 1; seat <= panelController()->numberOfSeats(); seat++) {
		repaintHandCards( seat, DOMINO_TYPE_HANDCARD, true, true, 1 );
	}
}
void DominoDesktopController::repaintHandCards( int seat, int type, bool repaintSize, bool repaintPos, qreal scale )
{
	DesktopItems &items	= desktopItems( seat, type );
	quint8 view = seat2View(seat);
	
	int startx	= 0;
	int starty	= 0;
	qreal z	= 1000;
	Qt::Alignment align	= 0;
	int deltaOfNewline	= 20;
	switch( view ) {
		case 1 :
			startx	= 100;
			starty	= 620;
			z	+= 100;
			repaintCardItems( items, scale, align,
							repaintSize, repaintPos, view,
							startx, starty, z,
							QPoint(1,0), deltaOfNewline, availableSpace().right() );
			break;
		case 2 : {
			startx	= 0;
			starty	= 460;
			repaintCardItems( items, scale, align,
							repaintSize, repaintPos, view,
							startx, starty, z,
							QPoint(0,1), -deltaOfNewline, desktop()->realHeight() );
			break;
		}
		case 3 :
			startx	= 100;
			starty	= 10;
			z	-= 100;
			repaintCardItems( items, scale, align,
							repaintSize, repaintPos, view,
							startx, starty, z,
							QPoint(1,0), deltaOfNewline, availableSpace().right() );
			break;
		case 4 : {
			startx	= 640;
			starty	= 260;
			repaintCardItems( items, scale, align,
							repaintSize, repaintPos, view,
							startx, starty, z,
							QPoint(0,-1), deltaOfNewline, 0 );
			break;
		}
		default:
			break;
	}
}
void DominoDesktopController::repaintCardItems(
	DesktopItems &items, qreal scale, Qt::Alignment align,
	bool repaintSize, bool repaintPos, quint8 view,
	int startx, int starty, qreal z,
	const QPoint& directionOfNextItem, int deltaOfLine, int border )
{
	djDebug()<<"DominoDesktopController::repaintCardItems";
	
	quint8 direction	= DOMINO_CHAIN_VERTICAL;
	if ( 2 == view || 4 == view )
		direction	= DOMINO_CHAIN_HORIZONTAL;
		
	int x	= startx;
	int y	= starty;
	
	QPen pen( Qt::red );
	pen.setWidth( 2 );
			
	DesktopItemsIterator it	= items.begin();
	while( it != items.end() ) {
		DJDesktopItem *desktopItem	= (DJDesktopItem*)*it;
		QGraphicsItem *item	= desktopItem->graphicsItem();
		if ( 0 == item ) {
			quint8 cardValue	= desktopItem->value();
			QPixmap pix	= cardPixmap( cardValue, direction );
			DJGraphicsPixmapItem *imageItem	= new DJGraphicsPixmapItem( pix, 0, desktop()->scene() );	
			desktopItem->setGraphicsItem( imageItem );
			imageItem->setVirtualPos( QPointF(x,y) );
			imageItem->setAlignment( align );
			imageItem->setExternalScale( desktop()->graphicsScale() );
			imageItem->setInternalScale( scale );
			imageItem->setZValue(z++);
			imageItem->setDJSelected( false );
			imageItem->setPen( pen );
			imageItem->adjustPos( desktop()->graphicsMatrix() );
			imageItem->show();
		}else {
			DJGraphicsPixmapItem *imageItem	= (DJGraphicsPixmapItem*)item;
			if ( repaintPos ) {
				imageItem->setVirtualPos( QPointF(x,y) );
				imageItem->setAlignment( align );
			}
			if ( repaintSize ) {
				imageItem->setExternalScale( desktop()->graphicsScale() );
				imageItem->setInternalScale( scale );
			}
			imageItem->setZValue(z++);
			imageItem->setDJSelected( false );
			
			imageItem->adjustPos( desktop()->graphicsMatrix() );
			imageItem->show();
		}
		
		//calculate next item's pos
		DesktopItemsIterator nextIt	= it + 1;
		if ( nextIt == items.end() )
			break;
		
		int nextItemLen	= static_cast<int>( (m_cardDistance + m_cardSize.width()) * scale);
		int newlineDelta	= static_cast<int>( deltaOfLine * scale );
		int distance	= static_cast<int>( m_cardDistance * scale );
		if ( directionOfNextItem.x() > 0 ) {
			if ( x + nextItemLen > border ) {
				x	= startx;
				y	+= newlineDelta;
			}else {
				x	+= distance;
			}
		}else if ( directionOfNextItem.x() < 0 ) {
			if ( x - nextItemLen < border ) {
				x	= startx;
				y	+= newlineDelta;
			}else {
				x	-= distance;
			}
		}else if ( directionOfNextItem.y() > 0 ) {
			if ( y + nextItemLen > border ) {
				x	+= newlineDelta;
				y	= starty;
			}else {
				y	+= distance;
			}
		}else if ( directionOfNextItem.y() < 0 ) {
			if ( y - nextItemLen < border ) {
				x	+= newlineDelta;
				y	= starty;
			}else {
				y	-= distance;
			}
		}
		it++;
	}
}
DJDesktopItem* DominoDesktopController::takeDesktopItem( int seat, int type, int value )
{
	DesktopItems& items	= desktopItems( seat, type );
	DesktopItemsIterator errorItemIt	= items.end();
	DesktopItemsIterator it	= items.begin();
	while( it != items.end() ) {
		DJDesktopItem *item	= *it;
		if ( DOMINO_CARD_ERROR(item->value()) ) {
			errorItemIt	= it;
		}
		if ( value == item->value() || DOMINO_CARD_REVERSE(value) == item->value() ) {
			items.erase(it);
			return item;
		}
		it++;
	}
	if ( errorItemIt != items.end() ) {
		DJDesktopItem *item	= *errorItemIt;
		items.erase( errorItemIt );
		return item;
	}
	return 0;
}
void DominoDesktopController::CheckPosition(unsigned char chCard,unsigned char *pchCards,unsigned char chPages,bool bSend)
{
	djDebug()<<"CheckPosition";
	
	DominoChains chains;
	memcpy(&chains,&m_chains,sizeof(DominoChains));
	
	QList<QGraphicsItem*> items	= desktop()->desktopScene()->items();
	foreach ( QGraphicsItem *item, items ) {
		QVariant varType	= item->data(KEY_OF_TYPE);
		if ( varType.isValid() ) {
			int type	= varType.toInt();
			if ( DOMINO_TYPE_DESKCARD == type ) {
				quint8 chainId	= item->data(KEY_OF_SEAT).toInt();
				quint8 cardId	= item->data(KEY_OF_VALUE).toInt();
				djDebug()<<"deskcard"<<hex<<"chainID"<<chainId<<"cardID"<<cardId;
				DJGraphicsPixmapItem *pixItem	= (DJGraphicsPixmapItem*)item;
				if(DOMINO_CARD_NULL == cardId && AppendCard2Chain(&chains,chainId,chCard,pchCards,chPages,false)) {
					djDebug()<<"got position";
					pixItem->setDJSelected(true);
					memcpy(&chains,&m_chains,sizeof(DominoChains));
					if(bSend) {
						SendPlace(chCard,chainId,pchCards,chPages);
						return;
					}
				}else {
					djDebug()<<"no position";
					pixItem->setDJSelected(false);
				}
				pixItem->update();
			}
		}
	}
}
QByteArray DominoDesktopController::handCardsFromDesktopItems()
{
	QByteArray cards;
	
	int	seat	= panelController()->seatId();
	int type	= DOMINO_TYPE_HANDCARD;
	
	DesktopItems& items	= desktopItems( seat, type );
	foreach( DJDesktopItem *item, items ) {
		cards.append( (char)item->value() );
	}
	return cards;
}
void DominoDesktopController::handleMousePressed( QGraphicsSceneMouseEvent *mouseEvent )
{
	djDebug()<<"DominoDesktopController::handleMousePressed"<<mouseEvent->button()<<mouseEvent->buttons();
	if ( Qt::LeftButton == mouseEvent->button() ) {
		djDebug()<<"left button";
		QPointF scenePos	= mouseEvent->scenePos();
		QGraphicsItem *item	= desktop()->desktopScene()->itemAt( scenePos );
		djDebug()<<"clicked item"<<item;
		if ( 0 == item ) {
			djDebug()<<"no item";
			QRectF r( scenePos, QSize(1,1) );
			QList<QGraphicsItem*> items	= desktop()->desktopScene()->items(r,Qt::IntersectsItemBoundingRect);
			if ( items.isEmpty() ) {
				djDebug()<<"no items too!";
				return;
			}
			item	= items.first();
		}
		QVariant varType	= item->data(KEY_OF_TYPE);
    	if( varType.isValid() ) {
    		djDebug()<<"varType is valid";
    		int type	= varType.toInt();
    		if ( DOMINO_TYPE_HANDCARD == type ) {
    			djDebug()<<"handcard";
    			int seat	= item->data(KEY_OF_SEAT).toInt();
    			if ( seat == panelController()->seatId() ) {
    				//select self hand card
    				playWave("click.wav");
    				quint8 card	= item->data(KEY_OF_VALUE).toInt();
    				djDebug()<<"click my hand"<<hex<<"card"<<card;
    				clearHandCardsMark();
    				DJGraphicsPixmapItem *pixItem	= (DJGraphicsPixmapItem*)item;
    				pixItem->setDJSelected( true );
    				pixItem->update();
    				QByteArray cards	= handCardsFromDesktopItems();
    				CheckPosition(card,(unsigned char*)cards.data(),cards.size(),false);
    			}
    		}else if (DOMINO_TYPE_DESKCARD == type) {
    			djDebug()<<"deskcard";
    			//place card
    			quint8 chainId	= item->data(KEY_OF_SEAT).toInt();
				quint8 cardId	= item->data(KEY_OF_VALUE).toInt();
				
				quint8 selectedCard	= selectedHandCard();
				djDebug()<<hex<<"chaingId"<<chainId<<"cardId"<<cardId<<"selectedCard"<<selectedCard;
				unsigned char cards[2]={DOMINO_CARD_ERRORID,DOMINO_CARD_ERRORID};
				if(!DOMINO_ISCARD(selectedCard)) {
					djDebug()<<"selectedCard is not valid card";
					return;
				}
				cards[0] = selectedCard;
				if(DOMINO_CARD_NULL == cardId ) {
					SendPlace(selectedCard,chainId,cards,1);
				}
    		}
    	}
	}
}
void DominoDesktopController::handleMouseDoubleClicked( QGraphicsSceneMouseEvent *mouseEvent )
{
	djDebug()<<"DominoDesktopController::handleMouseDoubleClicked"<<mouseEvent->button()<<mouseEvent->buttons();
	if ( Qt::LeftButton == mouseEvent->button() ) {
		djDebug()<<"left button";
		QPointF scenePos	= mouseEvent->scenePos();
		QGraphicsItem *item	= desktop()->desktopScene()->itemAt( scenePos );
		djDebug()<<"clicked item"<<item;
		if ( 0 == item ) {
			djDebug()<<"no item";
			QRectF r( scenePos, QSize(1,1) );
			QList<QGraphicsItem*> items	= desktop()->desktopScene()->items(r,Qt::IntersectsItemBoundingRect);
			if ( items.isEmpty() ) {
				djDebug()<<"no items too!";
				return;
			}
			item	= items.first();
		}
		QVariant varType	= item->data(KEY_OF_TYPE);
    	if( varType.isValid() ) {
    		djDebug()<<"varType is valid";
    		int type	= varType.toInt();
    		if ( DOMINO_TYPE_HANDCARD == type ) {
    			djDebug()<<"handcard";
    			int seat	= item->data(KEY_OF_SEAT).toInt();
    			if ( seat == panelController()->seatId() ) {
    				//select self hand card
    				playWave("click.wav");
    				quint8 card	= item->data(KEY_OF_VALUE).toInt();
    				djDebug()<<"click my hand"<<hex<<"card"<<card;
    				clearHandCardsMark();
    				DJGraphicsPixmapItem *pixItem	= (DJGraphicsPixmapItem*)item;
    				pixItem->setDJSelected( true );
    				pixItem->update();
    				QByteArray cards	= handCardsFromDesktopItems();
    				CheckPosition(card,(unsigned char*)cards.data(),cards.size(),true);
    			}
    		}
    	}
	}
}
void DominoDesktopController::SendPlace(unsigned char chCard,unsigned char chChainID,unsigned char *pchCards,unsigned char chPages)
{
	djDebug()<<"DominoDesktopController::SendPlace"<<m_isWaitingPlaceACL;
	if( panelController()->isLookingOn() || m_isWaitingPlaceACL )
		return;
	djDebug()<<"ready";
	DominoChains chains;
	memcpy(&chains,&m_chains,sizeof(DominoChains));
	
	if(AppendCard2Chain(&chains,chChainID,chCard,pchCards,chPages,false))
	{
		djDebug()<<"sendGameTrace DOMINO_GAMETRACE_PLACE";
		m_isWaitingPlaceACL = true;
		
		DominoPlace		place;
		place.chChainID = chChainID;
		place.chCard = chCard;

		QVariant parameters	= QVariant::fromValue((void*)this);
		sendGameTrace( DOMINO_GAMETRACE_PLACE, QByteArray((char *)&place,sizeof(DominoPlace)), HandlePlaceACL, parameters );
	}
}
void DominoDesktopController::locateAvailableSpace( QRect& rect )
{
	QSize s(72,122);
	
	rect.setBottom( desktop()->realHeight() - s.height() );
	rect.setLeft( s.width() );
	rect.setTop( s.height() );
	rect.setRight( desktop()->realWidth() - s.width() );
}
void DominoDesktopController::locateClockPosition( QPoint& point, Qt::Alignment& align )
{
	djDebug() << "DominoDesktopController::locateClockPositions";
	
	QList<quint8> seats	= seatIdsOfPlayingUsers();
	int players	= seats.size();
	
	if ( players > 2 ) {
		point.setX( centerPoint().x() );
    	point.setY( 620 );
    	align	= Qt::AlignCenter;
	}else {
		return DJDesktopController::locateClockPosition(point,align);
	}
}
void DominoDesktopController::fillPlayerInfo()
{
	djDebug()<<"DominoDesktopController::fillPlayerInfo";
	
	for( quint8 seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
		DJGraphicsTextItem *info	= playerInfoItem(seat);
		//int remain	= numberOfPokers( seat, POKER_TYPE_HAND );
		info->setPlainText( tr("%1 fen").arg(m_score[seat]) );
	}
}
void DominoDesktopController::clickZhua()
{
	if( isWaitingForMe() && !panelController()->isLookingOn()
		&& gameWaitStatus() == DJGAME_DOMINO_TABLE_STATUS_WAIT_DRAW ) {
		sendGameTrace( DOMINO_GAMETRACE_DRAW );
	} 
}
void DominoDesktopController::locateAllNonMatrimoveItems()
{
	DJDesktopController::locateAllNonMatrimoveItems();
	
	QPoint pos( 600, 600 );
	QMatrix matrix	= desktop()->graphicsMatrix();
	pos	= matrix.map( pos );
	
	int x	= pos.x();
	int y	= pos.y();
	
	m_btnZhua->move( x-m_btnZhua->width(), y );
	
	x	= 0;
	y	= 0;
	
	m_sumTextItem->setVirtualPos( QPoint(x,y) );
	m_sumTextItem->adjustPos( desktop()->graphicsMatrix() );
	
	y	+= m_sumTextItem->boundingRect().toRect().height();
	m_remainTextItem->setVirtualPos( QPoint(x,y) );
	m_remainTextItem->adjustPos( desktop()->graphicsMatrix() );
}