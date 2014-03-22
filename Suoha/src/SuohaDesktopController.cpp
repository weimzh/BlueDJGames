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
#include "SuohaDesktopController.h"
#include "DJPanelController.h"
#include "DJGraphicsPixmapItem.h"
#include "DJGraphicsTextItem.h"
#include "DJDesktop.h"
#include "DJDesktopScene.h"
#include "DJGameRoom.h"
#include "DJGameUser.h"
#include "DJClientRequest.h"
#include "Utility.h"
#include "DJGroupChat.h"
#include "DJTextBrowser.h"
#include "DJSchemeEvent.h"
#include "DJDesktopPokerItem.h"


static	const	int 	WAGER_TEXT_ITEM_FONT_SIZE		= 	24;
static	const	int 	MINMAX_TEXT_ITEM_FONT_SIZE		= 	10;

SuohaDesktopController::SuohaDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent )
	: DJDesktopPokerController(panelController,size,parent)
{
	djDebug()<<"SuohaDesktopController constructor"<<size;
	
	memset(&m_current,0,sizeof(SuohaCurrent));
	
	setSelectRectEnabled(false);
	setSelectEnabled(false);
	
	QPixmap pix(":/SuohaRes/image/casinodesk.png");
	DJGraphicsPixmapItem *pixItem = new DJGraphicsPixmapItem(pix,0,desktop()->scene());
	pixItem->setVirtualPos( QPoint(desktop()->realWidth()>>1,desktop()->realHeight()>>1) );
	pixItem->setAlignment(Qt::AlignCenter);
	pixItem->setZValue(10);
	pixItem->adjustPos( desktop()->graphicsMatrix() );
	pixItem->show();
	
	QFont font;
	font.setPointSize( WAGER_TEXT_ITEM_FONT_SIZE );
	font.setBold( true );
	
	for ( int view = 0; view <= 4; ++view ) {
		setScaleOfHand( view, 1 );
		DJGraphicsTextItem *item	= new DJGraphicsTextItem( 0, desktop()->scene() );
		item->setDefaultTextColor( Qt::red );
		item->setFont( font );
		item->setZValue( 3000 );
		m_wagerTextItems << item;
	}
	
	font.setPointSize( MINMAX_TEXT_ITEM_FONT_SIZE );
	font.setBold( false );
	
	PSuohaRoom  pshroom = (PSuohaRoom)panelController->gameRoom()->privateRoom();
	int minWager	= letoh4(pshroom->uMinYazhu);
	QString text	= tr("Min:")+QString::number(minWager);
	
	DJGraphicsTextItem *textItem	= new DJGraphicsTextItem( 0, desktop()->scene() );
	textItem->setFont( font );
	textItem->setPlainText( text );
	textItem->setZValue( 20 );
	textItem->setVirtualPos( QPoint(desktop()->realWidth()/2-187,desktop()->realHeight()/2-250) );
	
	int maxWager	= letoh4(pshroom->uGameMaxCounter);
	text	= tr("Max:")+QString::number(maxWager);
	textItem	= new DJGraphicsTextItem( 0, desktop()->scene() );
	textItem->setFont( font );
	textItem->setPlainText( text );
	textItem->setZValue( 20 );
	textItem->setVirtualPos( QPoint(desktop()->realWidth()/2-187,desktop()->realHeight()/2-225) );
		
	m_btnQi = new QPushButton( desktop() );
	m_btnQi->setText( tr("Away") );
	m_btnQi->adjustSize();
	m_btnQi->hide();
	connect(m_btnQi,SIGNAL(clicked()),this,SLOT(clickQi()));
	
	m_btnSuo = new QPushButton( desktop() );
	m_btnSuo->setText( tr("All") );
	m_btnSuo->adjustSize();
	m_btnSuo->hide();
	connect(m_btnSuo,SIGNAL(clicked()),this,SLOT(clickSuo()));
	
	m_btnJia = new QPushButton( desktop() );
	m_btnJia->setText( tr("Add") );
	m_btnJia->adjustSize();
	m_btnJia->hide();
	connect(m_btnJia,SIGNAL(clicked()),this,SLOT(clickJia()));
	
	m_btnGen = new QPushButton( desktop() );
	m_btnGen->setText( tr("Keep") );
	m_btnGen->adjustSize();
	m_btnGen->hide();
	connect(m_btnGen,SIGNAL(clicked()),this,SLOT(clickGen()));
}
SuohaDesktopController::~SuohaDesktopController()
{
	djDebug()<<"SuohaDesktopController destructor";
}
void SuohaDesktopController::initUnderGameInfo( const QByteArray& buf )
{
	djDebug()<<"SuohaDesktopController::initUnderGameInfo";
	
	DJDesktopPokerController::initUnderGameInfo( buf );
		
	memcpy(&m_current,buf.data(),sizeof(SuohaCurrent));
	memset(c_PlayerCounters,0,(DJGAME_SUOHA_MAX_PLAYERS+1)*sizeof(qint16));
	memset(c_PlayerPreCounters,0,(DJGAME_SUOHA_MAX_PLAYERS+1)*sizeof(qint16));
	
	m_steps	= 0;
	c_CurrentCounters	= 0;
	
	foreach( DJGraphicsTextItem *item, m_wagerTextItems ) {
		item->hide();
	}
	
	PSuohaRoom  pshroom = (PSuohaRoom)panelController()->gameRoom()->privateRoom();
	c_HandCounters = letoh2(pshroom->uGameMaxMulti);
	
	repaintHandChips();
}
void SuohaDesktopController::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	DJDesktopPokerController::gameWait(mask,status,timeout);
	
	m_btnQi->hide();
	m_btnSuo->hide();
	m_btnJia->hide();
	m_btnGen->hide();
	
	switch(status) {
	case DJGAME_SUOHA_TABLE_STATUS_WAIT_BET : {
		
		if( isWaitingForMe() && !panelController()->isLookingOn() )
		{
			djDebug()<<"DJGAME_SUOHA_TABLE_STATUS_WAIT_BET";
			PSuohaRoom  pshroom = (PSuohaRoom)panelController()->gameRoom()->privateRoom();
			m_btnQi->show();
			m_btnGen->show();
			
			if ( c_CurrentCounters < pshroom->chMaxMulti )
				m_btnJia->show();
			
			
			int selfseat	= panelController()->seatId();
			
			if( m_steps >= 4 
				&& c_CurrentCounters + c_PlayerPreCounters[selfseat]
				< (int)letoh2(pshroom->uGameMaxMulti) )
				m_btnSuo->show();		
			if ( isLeader(selfseat) )
				m_btnGen->setText(tr("Stay"));
			else
				m_btnGen->setText(tr("Keep"));
			m_btnGen->adjustSize();
		}
		break;
	}
	default:
		break;
	}
	locateButtons();
}
void SuohaDesktopController::gameTraceModel( const GeneralGameTrace2Head *gameTrace )
{
	djDebug() << "SuohaDesktopController::gameTraceModel";
	
	quint8 gameTraceSeat	= gameTrace->chSite;
	quint8 bufLen	= gameTrace->chBufLen;
	const unsigned char *buf	= gameTrace->chBuf;
	
	djDebug()<<"gameTraceSeat"<<gameTraceSeat<<"bufLen"<<bufLen;
	DebugBuffer( (const char*)buf, bufLen );
	
	switch(gameTrace->chType) {
	case SUOHA_GAMETRACE_PICKUP :
	case SUOHA_GAMETRACE_PICKUP2 : {
		PSuohaPickup ppickup = (PSuohaPickup)gameTrace->chBuf;
		djDebug()<<hex<<"mapsite"<<ppickup->chMapSite<<"chCardNumber"<<ppickup->chCardNumber<<"chCard"<<ppickup->chCard;
		if(ppickup->chCardNumber >0 && ppickup->chCardNumber <=SUOHA_PLAYER_MAX_CARDS)
		{
			DJDesktopItem *item	= createDesktopItem( gameTraceSeat, POKER_TYPE_HAND, ppickup->chCard );
			DJDesktopPokerItem *pokerItem	= (DJDesktopPokerItem*)item;
			if ( gameTraceSeat == panelController()->seatId() && 1 == ppickup->chCardNumber ) {
				pokerItem->setBack( true );
			}
			replaceDesktopItem( gameTraceSeat, POKER_TYPE_HAND, ppickup->chCardNumber-1, item );
			m_steps	= ppickup->chCardNumber;
		}
		c_PlayerPreCounters[gameTrace->chSite] = c_PlayerCounters[gameTrace->chSite];
		c_CurrentCounters = 0;
		break;
	}
	case SUOHA_GAMETRACE_SHOWCARDS : {
		djDebug()<<"SUOHA_GAMETRACE_SHOWCARDS";
		clearDesktopItems( gameTraceSeat, POKER_TYPE_HAND );
		PSuohaShowCards pshows = (PSuohaShowCards)gameTrace->chBuf;
		if(pshows->chCards >0 && pshows->chCards <=SUOHA_PLAYER_MAX_CARDS)
		{
			djDebug()<<"pshows->chCards"<<pshows->chCards;
			for ( quint8 i = 0; i < pshows->chCards; i++ ) {
				if ( 0 != pshows->chCard[i] )
					appendDesktopItem( gameTraceSeat, POKER_TYPE_HAND, pshows->chCard[i] );
			}
			m_steps = pshows->chCards;
		}
		break;
	}
	case SUOHA_GAMETRACE_BET :
	{
		PSuohaBet pyazhu = (PSuohaBet)gameTrace->chBuf;
		djDebug()<<"SUOHA_GAMETRACE_BET"<<"seat"<<gameTraceSeat<<"multi"<<pyazhu->chMultiple;
		
		if(pyazhu->chMultiple == SUOHA_BET_AWAY) {
			djDebug()<<"SUOHA_BET_AWAY";
		}else
		{
			djDebug()<<"yazhu chMultiple"<<pyazhu->chMultiple;
			c_PlayerCounters[gameTraceSeat] = c_PlayerPreCounters[gameTraceSeat] + pyazhu->chMultiple;
			c_CurrentCounters = pyazhu->chMultiple;
			if(gameTraceSeat == panelController()->seatId())
			{
				if(c_HandCounters > pyazhu->chMultiple)
					c_HandCounters -= pyazhu->chMultiple;
				else
					c_HandCounters = 0;
			}
				
			clearDesktopItems( gameTraceSeat, CHIP_TYPE_WAGER );
			for ( quint8 i = 0; i < c_PlayerCounters[gameTraceSeat]; i++ ) {
				DJDesktopItem *item	= DJDesktopController::createDesktopItem( gameTraceSeat, CHIP_TYPE_WAGER, CHIP_BLUE1 );
				appendDesktopItem( gameTraceSeat, CHIP_TYPE_WAGER, item );			
			}
		}
		break;
	}
	default:
		break;
	}
}
void SuohaDesktopController::gameTraceView( const GeneralGameTrace2Head *gameTrace )
{
	djDebug()<<"SuohaDesktopController::gameTraceView";
	
	quint8 gameTraceSeat	= gameTrace->chSite;
	quint8 bufLen	= gameTrace->chBufLen;
	const unsigned char *buf	= gameTrace->chBuf;
	
	switch( gameTrace->chType ) {
	case SUOHA_GAMETRACE_PICKUP :
	case SUOHA_GAMETRACE_PICKUP2 :
	case SUOHA_GAMETRACE_SHOWCARDS :
		repaintHandPokers( gameTraceSeat, POKER_TYPE_HAND, true, true, false );
		break;
	case SUOHA_GAMETRACE_BET : {
		PSuohaRoom  pshroom = (PSuohaRoom)panelController()->gameRoom()->privateRoom();
		int chipValue	= letoh4(pshroom->uMinYazhu);
		DJGraphicsTextItem *textItem	= m_wagerTextItems[gameTraceSeat];
		repaintWagerChips( gameTraceSeat, CHIP_TYPE_WAGER, textItem, chipValue, true, true, false );
		repaintHandChips();
		break;
	}
	default:
		break;
	}
}
void SuohaDesktopController::handleMousePressed( QGraphicsSceneMouseEvent *mouseEvent )
{
	djDebug()<<"SuohaDesktopController::handleMousePressed"<<mouseEvent->button()<<mouseEvent->buttons();
	if ( Qt::LeftButton == mouseEvent->button() ) {
		djDebug()<<"left button";
		QPointF scenePos	= mouseEvent->scenePos();
		QGraphicsItem *item	= desktop()->desktopScene()->itemAt( scenePos );
		djDebug()<<"clicked item"<<item;
		if ( 0 == item ) {
			djDebug()<<"no item";
			return;
		}
		QVariant var1	= item->data(KEY_OF_SEAT);
    	QVariant var2	= item->data(KEY_OF_TYPE);
    	if( var1.isValid() && var2.isValid() ) {
    		djDebug()<<"var1 and var2 is valid";
    		int seat	= var1.toInt();
    		int type	= var2.toInt();
    		if ( POKER_TYPE_HAND == type &&
    			seat == panelController()->seatId() ) {
    			djDebug()<<"is my hand";
    			DesktopItems &items	= desktopItems( seat, type );
    			DJDesktopPokerItem *firstPokerItem = 0;
    			if ( !items.isEmpty() )
    				firstPokerItem	= (DJDesktopPokerItem*)items.first();
    			if ( item == firstPokerItem->graphicsItem() ) {
    				djDebug()<<"is first poker";
    				firstPokerItem->setBack( false );
    				firstPokerItem->setGraphicsItem( 0 );
    				repaintHandPokers( seat, type, true, true, false );
    			}
    		}
    	}
	}
}
void SuohaDesktopController::handleMouseReleased( QGraphicsSceneMouseEvent *mouseEvent )
{
	djDebug()<<"SuohaDesktopController::handleMouseReleased"<<mouseEvent->button()<<mouseEvent->buttons();
	
	int seat	= panelController()->seatId();
	int type	= POKER_TYPE_HAND;
	DesktopItems &items	= desktopItems( seat, type );
    if ( !items.isEmpty() ) {
    	DJDesktopPokerItem *firstPokerItem = (DJDesktopPokerItem*)items.first();
    	if ( !firstPokerItem->isBack() ) {
    		firstPokerItem->setBack( true );
    		firstPokerItem->setGraphicsItem( 0 );
    		repaintHandPokers( seat, type, true, true, false );
    	}
    }
}
void SuohaDesktopController::repaintHandChips()
{
	int seat	= panelController()->seatId();
	int type	= CHIP_TYPE_HAND;
	clearDesktopItems( seat, type );
		
	for( int i = 0; i < c_HandCounters; i++ ) {
		DJDesktopItem *item	= DJDesktopController::createDesktopItem( seat, CHIP_TYPE_HAND, CHIP_BLUE2 );
		appendDesktopItem( seat, CHIP_TYPE_HAND, item );
	}
	DesktopItems &items	= desktopItems( seat, type );
	int x	= 700;
	int	y	= 760;
	int	z	= 1000;
	Qt::Alignment align	= 0;
	qreal scale	= 1;
	
	foreach( DJDesktopItem *desktopItem, items ) {
		QGraphicsItem *item	= desktopItem->graphicsItem();
		if ( 0 == item ) {
			int chipValue	= desktopItem->value();
			QPixmap pix	= chipPixmap( chipValue );
			DJGraphicsPixmapItem *imageItem	= new DJGraphicsPixmapItem( pix, 0, desktop()->scene() );	
			desktopItem->setGraphicsItem( imageItem );
			imageItem->setVirtualPos( QPointF(x,y) );
			imageItem->setAlignment( align );
			imageItem->setExternalScale( desktop()->graphicsScale() );
			imageItem->setInternalScale( scale );
			imageItem->setZValue(z++);
			imageItem->adjustPos( desktop()->graphicsMatrix() );
			imageItem->show();
		}else {
			DJGraphicsPixmapItem *imageItem	= (DJGraphicsPixmapItem*)item;
			imageItem->setVirtualPos( QPointF(x,y) );
			imageItem->setAlignment( align );
			imageItem->setExternalScale( desktop()->graphicsScale() );
			imageItem->setInternalScale( scale );
			imageItem->setZValue(z++);
			imageItem->adjustPos( desktop()->graphicsMatrix() );
			imageItem->show();
		}
		y	-=	6;
	}
}
void SuohaDesktopController::locatePoolSpace( QRect& rect )
{
	rect.setBottom( availableSpace().bottom() - static_cast<int>(scaleOfHand(1) * (pokerSize().height())) );
	rect.setLeft( availableSpace().left() + static_cast<int>(scaleOfHand(2) * pokerSize().height()) );
	rect.setTop( availableSpace().top() + static_cast<int>(scaleOfHand(3) * pokerSize().height()) );
	rect.setRight( availableSpace().right() - static_cast<int>(scaleOfHand(4) * pokerSize().height()) );
}
void SuohaDesktopController::locateCenterSpace( QRect& rect )
{
	int space	= 40;
	
	rect.setBottom( centerPoint().y() + space );
	rect.setLeft( centerPoint().x() - space * 2 );
	rect.setTop( centerPoint().y() - space );
	rect.setRight( centerPoint().x() + space * 2 );
}
void SuohaDesktopController::repaintCurrentStatus()
{
	djDebug()<<"SuohaDesktopController::repaintCurrentStatus";
	
	repaintAll();
}
void SuohaDesktopController::repaintAll()
{
	djDebug()<<"SuohaDesktopController::repaintAll";
	DJDesktopPokerController::repaintAll();
	
	PSuohaRoom  pshroom = (PSuohaRoom)panelController()->gameRoom()->privateRoom();
	int chipValue	= letoh4(pshroom->uMinYazhu);	
	quint8 seat;
	for( seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
		repaintHandPokers( seat, POKER_TYPE_HAND, true, true, false );
		DJGraphicsTextItem *textItem	= m_wagerTextItems[seat];
		repaintWagerChips( seat, CHIP_TYPE_WAGER, textItem, chipValue, true, true, false );
	}
	repaintHandChips();
}
bool SuohaDesktopController::isUserPlaying( quint8 seatId ) const
{
	for( quint8 seat = 0; seat <= DJGAME_SUOHA_MAX_PLAYERS; ++seat ) {
		if ( seatId == m_current.chMapSites[seat] )
			return true;
	}
	return false;
}
bool SuohaDesktopController::isCloseable() const
{
	bool closable	= DJDesktopController::isCloseable();
	if ( !closable ) {
		QList<quint8> seats	= seatIdsOfPlayingUsers();
		if ( !seats.contains(panelController()->seatId()) )
			closable	= true;
	}
	return closable;
}
QString	SuohaDesktopController::userItemNameSuffix( DJGameUser* user ) const
{
	if ( 0 != m_current.chMaster 
		&& user->seatId() == m_current.chMaster ) {
		return QString("(")+tr("master")+QString(")");
	}
	return DJDesktopController::userItemNameSuffix(user);
}
void SuohaDesktopController::repaintHost()
{
	quint8 masterSeat	= m_current.chMaster;
	if ( 0 != masterSeat ) {
		repaintHostItem( masterSeat );	
	}
}
void SuohaDesktopController::playerBet( quint8 chips )
{
	djDebug()<<"SuohaDesktopController::playerBet"<<chips;
	if(!panelController()->isLookingOn()
		&& isWaitingForMe() 
		&& gameWaitStatus() == DJGAME_SUOHA_TABLE_STATUS_WAIT_BET ) {
		
		SuohaBet bet;
		bet.chMapSite = 0;
		bet.chMultiple = chips;
		QByteArray buf( (char*)&bet, sizeof(SuohaBet) );
		sendGameTrace(SUOHA_GAMETRACE_BET,buf);
	}
	m_btnQi->hide();
	m_btnSuo->hide();
	m_btnJia->hide();
	m_btnGen->hide();
}
void SuohaDesktopController::clickQi()
{
	playerBet( SUOHA_BET_AWAY );
}
void SuohaDesktopController::clickSuo()
{
	if(m_steps >= 4)
		playerBet(SUOHA_BET_MAX);
}
void SuohaDesktopController::clickJia()
{
	PSuohaRoom  pshroom = (PSuohaRoom)panelController()->gameRoom()->privateRoom();
	quint8 wager	= pshroom->chMaxMulti;
	if ( 0 == c_CurrentCounters )
		wager	= wager >> 1;
	playerBet(wager);
}
void SuohaDesktopController::clickGen()
{
	playerBet( 0 );
}
void SuohaDesktopController::locateButtons()
{
	QPoint pos( 680, 600 );
	QMatrix matrix	= desktop()->graphicsMatrix();
	pos	= matrix.map( pos );
	//pos.rx()	-= m_btnStart->width()>>1;
	int x	= pos.x();
	int	y	= pos.y();
	
	if ( m_btnQi->isVisible() ) {
		m_btnQi->move( x - m_btnQi->width(), y );
		y	+= m_btnQi->height();
	}
	if ( m_btnSuo->isVisible() ) {
		m_btnSuo->move( x - m_btnSuo->width(), y );
		y	+= m_btnSuo->height();
	}
	if ( m_btnJia->isVisible() ) {
		m_btnJia->move( x - m_btnJia->width(), y );
		y	+= m_btnJia->height();
	}
	if ( m_btnGen->isVisible() ) {
		m_btnGen->move( x - m_btnGen->width(), y );
		y	+= m_btnGen->height();
	}
}
void SuohaDesktopController::locateAllNonMatrimoveItems()
{
	DJDesktopPokerController::locateAllNonMatrimoveItems();
	
	locateButtons();
}
bool SuohaDesktopController::isLeader( int seat )
{
	int numberOfWagerChips	= c_PlayerCounters[seat];
	for( int i = 0; i <= DJGAME_SUOHA_MAX_PLAYERS; i++ ) {
		if ( i != seat ) {
			int chips	= c_PlayerCounters[i];
			if ( chips && chips > numberOfWagerChips )
				return false;
		}
	}
	return true;
}
/*
PSuohaRoom  pshroom = (PSuohaRoom)panelController()->gameRoom()->privateRoom();
	int minWager	= letoh4(pshroom->uMinYazhu);
	int wager	= minWager * items.size();
	*/