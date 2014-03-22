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
#include "NiuNiuDesktopController.h"
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
static	const	int 	MINMAX_TEXT_ITEM_FONT_SIZE		= 	12;

NiuNiuDesktopController::NiuNiuDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent )
	: DJDesktopPokerController(panelController,size,parent)
{
	djDebug()<<"NiuNiuDesktopController constructor"<<size;
	
	memset(&m_current,0,sizeof(NiuniuCurrent));
	
	setSelectRectEnabled(false);
	setSelectEnabled(false);
	
	QPixmap pix(":/NiuNiuRes/image/casinodesk.png");
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
	
	PNiuniuRoom  pnnroom = (PNiuniuRoom)panelController->gameRoom()->privateRoom();
	int minWager	= letoh4(pnnroom->uMinYazhu);
	QString text	= tr("Min:")+QString::number(minWager);
	
	DJGraphicsTextItem *textItem	= new DJGraphicsTextItem( 0, desktop()->scene() );
	textItem->setFont( font );
	textItem->setPlainText( text );
	textItem->setZValue( 20 );
	textItem->setVirtualPos( QPoint(desktop()->realWidth()/2-185,desktop()->realHeight()/2-250) );
	
	int maxWager	= letoh4(pnnroom->uGameMaxCounter);
	text	= tr("Max:")+QString::number(maxWager);
	textItem	= new DJGraphicsTextItem( 0, desktop()->scene() );
	textItem->setFont( font );
	textItem->setPlainText( text );
	textItem->setZValue( 20 );
	textItem->setVirtualPos( QPoint(desktop()->realWidth()/2-185,desktop()->realHeight()/2-230) );
	
	m_btnQi = new QPushButton( desktop() );
	m_btnQi->setText( tr("Away") );
	m_btnQi->adjustSize();
	m_btnQi->hide();
	connect(m_btnQi,SIGNAL(clicked()),this,SLOT(clickQi()));
	
	m_btnGen = new QPushButton( desktop() );
	m_btnGen->setText( tr("Keep") );
	m_btnGen->adjustSize();
	m_btnGen->hide();
	connect(m_btnGen,SIGNAL(clicked()),this,SLOT(clickGen()));
	
	m_btnYaZhu = new QPushButton( desktop() );
	m_btnYaZhu->setText( tr("Wager") );
	m_btnYaZhu->adjustSize();
	m_btnYaZhu->hide();
	connect(m_btnYaZhu,SIGNAL(clicked()),this,SLOT(clickYaZhu()));
	
	m_btnJiaBei = new QPushButton( desktop() );
	m_btnJiaBei->setText( tr("Double") );
	m_btnJiaBei->adjustSize();
	m_btnJiaBei->hide();
	connect(m_btnJiaBei,SIGNAL(clicked()),this,SLOT(clickJiaBei()));
	
	memset(c_PlayerCounters,0,(DJGAME_NIUNIU_MAX_PLAYERS+1)*sizeof(qint16));
	memset(c_PlayerSelfCounters,0,(DJGAME_NIUNIU_MAX_PLAYERS+1)*sizeof(qint16));
	
	c_HandCounters = pnnroom->chMaxMulti *(NIUNIU_PLAYER_MAX_CARDS+1);
	
	repaintHandChips();
}
NiuNiuDesktopController::~NiuNiuDesktopController()
{
	djDebug()<<"NiuNiuDesktopController destructor";
}
void NiuNiuDesktopController::initUnderGameInfo( const QByteArray& buf )
{
	djDebug()<<"NiuNiuDesktopController::initUnderGameInfo";
	
	DJDesktopPokerController::initUnderGameInfo( buf );
		
	memcpy(&m_current,buf.data(),sizeof(NiuniuCurrent));
	
	memset(c_PlayerCounters,0,(DJGAME_NIUNIU_MAX_PLAYERS+1)*sizeof(qint16));
	memset(c_PlayerSelfCounters,0,(DJGAME_NIUNIU_MAX_PLAYERS+1)*sizeof(qint16));
	
	PNiuniuRoom  pnnroom = (PNiuniuRoom)panelController()->gameRoom()->privateRoom();
	c_HandCounters = pnnroom->chMaxMulti *(NIUNIU_PLAYER_MAX_CARDS+1);
	
	repaintHandChips();
}
void NiuNiuDesktopController::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	DJDesktopPokerController::gameWait(mask,status,timeout);
	
	m_btnQi->hide();
	m_btnYaZhu->hide();
	m_btnJiaBei->hide();
	m_btnGen->hide();
	
	switch(status) {
	case DJGAME_NIUNIU_TABLE_STATUS_WAIT_YAZHU:
		if( isWaitingForMe() && !panelController()->isLookingOn() )
		{
			m_btnYaZhu->show();
			m_btnJiaBei->show();
			m_btnQi->show();
		}
		break;
	case DJGAME_NIUNIU_TABLE_STATUS_WAIT_MASTER:
		if( isWaitingForMe() && !panelController()->isLookingOn() )
		{
			m_btnGen->show();
			m_btnQi->show();
		}
		break;
	default:
		break;
	}
	locateButtons();
}
void NiuNiuDesktopController::gameTraceModel( const GeneralGameTrace2Head *gameTrace )
{
	djDebug() << "NiuNiuDesktopController::gameTraceModel";
	
	quint8 gameTraceSeat	= gameTrace->chSite;
	quint8 bufLen	= gameTrace->chBufLen;
	const unsigned char *buf	= gameTrace->chBuf;
	
	djDebug()<<"gameTraceSeat"<<gameTraceSeat<<"bufLen"<<bufLen;
	DebugBuffer( (const char*)buf, bufLen );
	
	switch(gameTrace->chType) {
	case NIUNIU_GAMETRACE_PICKUP:
	case NIUNIU_GAMETRACE_PICKUP2: {
		PNiuniuPickup ppickup = (PNiuniuPickup)gameTrace->chBuf;
		if(ppickup->chCardNumber >0 && ppickup->chCardNumber <=NIUNIU_PLAYER_MAX_CARDS)
		{
			DJDesktopItem *item	= createDesktopItem( gameTraceSeat, POKER_TYPE_HAND, ppickup->chCard );
			replaceDesktopItem( gameTraceSeat, POKER_TYPE_HAND, ppickup->chCardNumber-1, item );
		}
		break;
	}
	case NIUNIU_GAMETRACE_SHOWCARDS: {
		clearDesktopItems( gameTraceSeat, POKER_TYPE_HAND );
		PNiuniuShowCards pshows = (PNiuniuShowCards)gameTrace->chBuf;
		if(pshows->chCards >0 && pshows->chCards <=NIUNIU_PLAYER_MAX_CARDS) {
			for ( quint8 i = 0; i < pshows->chCards; i++ ) {
				if ( 0 != pshows->chCard[i] )
					appendDesktopItem( gameTraceSeat, POKER_TYPE_HAND, pshows->chCard[i] );
			}
		}
		break;
	}
	case NIUNIU_GAMETRACE_YAZHU: {
		djDebug()<<"NIUNIU_GAMETRACE_YAZHU"<<"seat"<<gameTraceSeat;
		PNiuniuYazhu pyazhu = (PNiuniuYazhu)gameTrace->chBuf;
		djDebug()<<"pyazhu->chMultiple"<<pyazhu->chMultiple;
		if(pyazhu->chMultiple > 0) {
			c_PlayerCounters[gameTraceSeat] += pyazhu->chMultiple;
			c_PlayerSelfCounters[gameTraceSeat] = c_PlayerCounters[gameTraceSeat];
			if(gameTraceSeat == panelController()->seatId()) {
				if(c_HandCounters > pyazhu->chMultiple)
					c_HandCounters -= pyazhu->chMultiple;
				else
					c_HandCounters = 0;
			}
			clearDesktopItems( gameTraceSeat, CHIP_TYPE_WAGER );
			for( quint16 i = 0; i <c_PlayerCounters[gameTraceSeat]; i++ ) {
				DJDesktopItem *item	= DJDesktopController::createDesktopItem( gameTraceSeat, CHIP_TYPE_WAGER, CHIP_BLUE1 );
				appendDesktopItem( gameTraceSeat, CHIP_TYPE_WAGER, item );				
			}
		}
		break;
	}
	case NIUNIU_GAMETRACE_MASTER: {
		djDebug()<<"NIUNIU_GAMETRACE_MASTER"<<"seat"<<gameTraceSeat;
		if( 0 != buf[0] ) {
			c_PlayerCounters[gameTraceSeat] = 0;
			c_PlayerSelfCounters[gameTraceSeat] = 0;
			for(int i=1;i<=4;i++) {
				if( i != gameTraceSeat ) {
					c_PlayerCounters[gameTraceSeat] += c_PlayerCounters[i];
				}
			}
			c_PlayerSelfCounters[gameTraceSeat] = c_PlayerCounters[gameTraceSeat];
			
			clearDesktopItems( gameTraceSeat, CHIP_TYPE_WAGER );
			for( quint16 i = 0; i <c_PlayerCounters[gameTraceSeat]; i++ ) {
				DJDesktopItem *item	= DJDesktopController::createDesktopItem( gameTraceSeat, CHIP_TYPE_WAGER, CHIP_BLUE1 );
				appendDesktopItem( gameTraceSeat, CHIP_TYPE_WAGER, item );				
			}
		}
		break;
	}
	case NIUNIU_GAMETRACE_RESULT: {
		djDebug()<<"NIUNIU_GAMETRACE_RESULT";
		PNiuniuRoom  pnnroom = (PNiuniuRoom)panelController()->gameRoom()->privateRoom();
		int chipValue	= letoh4(pnnroom->uMinYazhu);
		PNiuniuResult presult = (PNiuniuResult)gameTrace->chBuf;
		int chips	= presult->chMultiple;
		int value	= chips * chipValue;
		
		DJGameUser	*player	= panelController()->player(gameTraceSeat);
		DJGameUser	*master	= panelController()->player(m_current.chMaster);
			
		if( NIUNIU_GAMERESULT_WIN == presult->chResult ) {
			djDebug()<<"win";
			
			c_PlayerCounters[gameTraceSeat] = chips*2;
			c_PlayerCounters[m_current.chMaster] -= chips;
			c_PlayerSelfCounters[gameTraceSeat] = chips;
			
			clearDesktopItems( gameTraceSeat, CHIP_TYPE_WAGER );
			clearDesktopItems( m_current.chMaster, CHIP_TYPE_WAGER );
			
			for ( int i = 0; i < c_PlayerCounters[gameTraceSeat]; i++ ) {
				DJDesktopItem *item	= DJDesktopController::createDesktopItem( gameTraceSeat, CHIP_TYPE_WAGER, CHIP_BLUE1 );
				appendDesktopItem( gameTraceSeat, CHIP_TYPE_WAGER, item );		
			}
			for ( int i = 0; i < c_PlayerCounters[m_current.chMaster]; i++ ) {
				DJDesktopItem *item	= DJDesktopController::createDesktopItem( gameTraceSeat, CHIP_TYPE_WAGER, CHIP_BLUE1 );
				appendDesktopItem( m_current.chMaster, CHIP_TYPE_WAGER, item );		
			}
			QString result;
			if ( master ) {
				result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( master->userName() + tr("(master)") )
								.arg( -value );
			}
			if ( player ) {
				result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( player->userName() )
								.arg( value );
			}
			panelController()->insertGameResult2Browser( result );
		}else {
			djDebug()<<"lose";
			c_PlayerCounters[m_current.chMaster] += chips;
			c_PlayerCounters[gameTraceSeat] = 0;
			c_PlayerSelfCounters[gameTraceSeat] = chips;
			
			clearDesktopItems( gameTraceSeat, CHIP_TYPE_WAGER );
			clearDesktopItems( m_current.chMaster, CHIP_TYPE_WAGER );
			
			for ( int i = 0; i < c_PlayerCounters[gameTraceSeat]; i++ ) {
				DJDesktopItem *item	= DJDesktopController::createDesktopItem( gameTraceSeat, CHIP_TYPE_WAGER, CHIP_BLUE1 );
				appendDesktopItem( gameTraceSeat, CHIP_TYPE_WAGER, item );		
			}
			for ( int i = 0; i < c_PlayerCounters[m_current.chMaster]; i++ ) {
				DJDesktopItem *item	= DJDesktopController::createDesktopItem( gameTraceSeat, CHIP_TYPE_WAGER, CHIP_BLUE1 );
				appendDesktopItem( m_current.chMaster, CHIP_TYPE_WAGER, item );		
			}
			QString result;
			if ( master ) {
				result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( master->userName() + tr("(master)") )
								.arg( value );
			}
			if ( player ) {
				result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( player->userName() )
								.arg( -value );
			}
			panelController()->insertGameResult2Browser( result );
		}
		if(gameTraceSeat == panelController()->seatId()) {
			c_HandCounters = (pnnroom->chMaxMulti *(NIUNIU_PLAYER_MAX_CARDS+1))-c_PlayerSelfCounters[gameTraceSeat];
		}
		break;
	}
	default:
		break;
	}
}
void NiuNiuDesktopController::gameTraceView( const GeneralGameTrace2Head *gameTrace )
{
	djDebug()<<"NiuNiuDesktopController::gameTraceView";
	
	quint8 gameTraceSeat	= gameTrace->chSite;
	quint8 bufLen	= gameTrace->chBufLen;
	const unsigned char *buf	= gameTrace->chBuf;
	
	switch( gameTrace->chType ) {
	case NIUNIU_GAMETRACE_PICKUP:
	case NIUNIU_GAMETRACE_PICKUP2:
	case NIUNIU_GAMETRACE_SHOWCARDS:
		repaintHandPokers( gameTraceSeat, POKER_TYPE_HAND, true, true, false );
		break;
	case NIUNIU_GAMETRACE_YAZHU: {
		PNiuniuYazhu pyazhu = (PNiuniuYazhu)gameTrace->chBuf;
		if(pyazhu->chMultiple > 0) {
			PNiuniuRoom  pnnroom = (PNiuniuRoom)panelController()->gameRoom()->privateRoom();
			int chipValue	= letoh4(pnnroom->uMinYazhu);
			DJGraphicsTextItem *textItem	= m_wagerTextItems[gameTraceSeat];
			repaintWagerChips( gameTraceSeat, CHIP_TYPE_WAGER, textItem, chipValue, true, true, false );
			
			if( gameTraceSeat == panelController()->seatId()) {
				repaintHandChips();
			}
		}
		break;
	}
	case NIUNIU_GAMETRACE_MASTER: {
		if( 0 != buf[0] ) {
			PNiuniuRoom  pnnroom = (PNiuniuRoom)panelController()->gameRoom()->privateRoom();
			int chipValue	= letoh4(pnnroom->uMinYazhu);
			DJGraphicsTextItem *textItem	= m_wagerTextItems[gameTraceSeat];
			repaintWagerChips( gameTraceSeat, CHIP_TYPE_WAGER, textItem, chipValue, true, true, false );
		}
		break;
	}
	case NIUNIU_GAMETRACE_RESULT: {
		PNiuniuRoom  pnnroom = (PNiuniuRoom)panelController()->gameRoom()->privateRoom();
		int chipValue	= letoh4(pnnroom->uMinYazhu);
		DJGraphicsTextItem *textItem	= m_wagerTextItems[gameTraceSeat];
		repaintWagerChips( gameTraceSeat, CHIP_TYPE_WAGER, textItem, chipValue, true, true, false );
		textItem	= m_wagerTextItems[m_current.chMaster];
		repaintWagerChips( m_current.chMaster, CHIP_TYPE_WAGER, textItem, chipValue, true, true, false );
		repaintHandChips();
		break;
	}
	default:
		break;
	}
}
void NiuNiuDesktopController::locatePoolSpace( QRect& rect )
{
	rect.setBottom( availableSpace().bottom() - static_cast<int>(scaleOfHand(1) * (pokerSize().height())) );
	rect.setLeft( availableSpace().left() + static_cast<int>(scaleOfHand(2) * pokerSize().height()) );
	rect.setTop( availableSpace().top() + static_cast<int>(scaleOfHand(3) * pokerSize().height()) );
	rect.setRight( availableSpace().right() - static_cast<int>(scaleOfHand(4) * pokerSize().height()) );
}
void NiuNiuDesktopController::locateCenterSpace( QRect& rect )
{
	int space	= 40;
	
	rect.setBottom( centerPoint().y() + space + 48 );
	rect.setLeft( centerPoint().x() - space * 2 );
	rect.setTop( centerPoint().y() - space );
	rect.setRight( centerPoint().x() + space * 2 );
}
bool NiuNiuDesktopController::isUserPlaying( quint8 seatId ) const
{
	for( quint8 seat = 0; seat <= DJGAME_NIUNIU_MAX_PLAYERS; ++seat ) {
		if ( seatId == m_current.chMapSites[seat] )
			return true;
	}
	return false;
}
void NiuNiuDesktopController::repaintHost()
{
	quint8 masterSeat	= m_current.chMaster;
	if ( 0 != masterSeat ) {
		repaintHostItem( masterSeat );	
	}
}
void NiuNiuDesktopController::repaintCurrentStatus()
{
	djDebug()<<"NiuNiuDesktopController::repaintCurrentStatus";
	
	repaintAll();
}
void NiuNiuDesktopController::repaintAll()
{
	djDebug()<<"NiuNiuDesktopController::repaintAll";
	DJDesktopPokerController::repaintAll();
	
	PNiuniuRoom  pnnroom = (PNiuniuRoom)panelController()->gameRoom()->privateRoom();
	int chipValue	= letoh4(pnnroom->uMinYazhu);
	quint8 seat;
	for( seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
		repaintHandPokers( seat, POKER_TYPE_HAND, true, true, false );
		DJGraphicsTextItem *textItem	= m_wagerTextItems[seat];
		repaintWagerChips( seat, CHIP_TYPE_WAGER, textItem, chipValue, true, true, false );
	}
	repaintHandChips();
}
void NiuNiuDesktopController::locateAllNonMatrimoveItems()
{
	DJDesktopPokerController::locateAllNonMatrimoveItems();
	
	locateButtons();
}
void NiuNiuDesktopController::locateButtons()
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
	if ( m_btnYaZhu->isVisible() ) {
		m_btnYaZhu->move( x - m_btnYaZhu->width(), y );
		y	+= m_btnYaZhu->height();
	}
	if ( m_btnJiaBei->isVisible() ) {
		m_btnJiaBei->move( x - m_btnJiaBei->width(), y );
		y	+= m_btnJiaBei->height();
	}
	if ( m_btnGen->isVisible() ) {
		m_btnGen->move( x - m_btnGen->width(), y );
		y	+= m_btnGen->height();
	}
}
bool NiuNiuDesktopController::isCloseable() const
{
	bool closable	= DJDesktopController::isCloseable();
	if ( !closable ) {
		QList<quint8> seats	= seatIdsOfPlayingUsers();
		if ( !seats.contains(panelController()->seatId()) )
			closable	= true;
	}
	return closable;
}
void NiuNiuDesktopController::repaintHandChips()
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
void NiuNiuDesktopController::clickGen()
{
	if(!(panelController()->isLookingOn()) 
		&& isWaitingForMe() 
		&& gameWaitStatus() == DJGAME_NIUNIU_TABLE_STATUS_WAIT_MASTER ) {
		char ch=1;
		sendGameTrace(NIUNIU_GAMETRACE_MASTER,QByteArray(&ch,1));
	}
	m_btnQi->hide();
	m_btnYaZhu->hide();
	m_btnJiaBei->hide();
	m_btnGen->hide();
}
void NiuNiuDesktopController::clickYaZhu()
{
	if(!panelController()->isLookingOn()
		&& isWaitingForMe() 
		&& gameWaitStatus() == DJGAME_NIUNIU_TABLE_STATUS_WAIT_YAZHU ) {
		
		PNiuniuRoom  pnnroom = (PNiuniuRoom)panelController()->gameRoom()->privateRoom();
		NiuniuYazhu ya;
		ya.chMultiple = pnnroom->chMaxMulti/2;
		sendGameTrace(NIUNIU_GAMETRACE_YAZHU,QByteArray((char *)&ya,sizeof(NiuniuYazhu)));
	}
	m_btnQi->hide();
	m_btnYaZhu->hide();
	m_btnJiaBei->hide();
	m_btnGen->hide();
}
void NiuNiuDesktopController::clickJiaBei()
{
	if(!panelController()->isLookingOn()
		&& isWaitingForMe() 
		&& gameWaitStatus() == DJGAME_NIUNIU_TABLE_STATUS_WAIT_YAZHU ) {
		
		PNiuniuRoom  pnnroom = (PNiuniuRoom)panelController()->gameRoom()->privateRoom();
		NiuniuYazhu ya;
		ya.chMultiple = pnnroom->chMaxMulti;
		sendGameTrace(NIUNIU_GAMETRACE_YAZHU,QByteArray((char *)&ya,sizeof(NiuniuYazhu)));
	}
	
	m_btnQi->hide();
	m_btnYaZhu->hide();
	m_btnJiaBei->hide();
	m_btnGen->hide();
}
void NiuNiuDesktopController::clickQi()
{
	if(!panelController()->isLookingOn() 
		&& isWaitingForMe() ) {
		if ( DJGAME_NIUNIU_TABLE_STATUS_WAIT_YAZHU == gameWaitStatus() ) {
			NiuniuYazhu ya;
			ya.chMultiple = 0;
			sendGameTrace(NIUNIU_GAMETRACE_YAZHU,QByteArray((char *)&ya,sizeof(NiuniuYazhu)));
		}else {
			char ch=0;
			sendGameTrace(NIUNIU_GAMETRACE_MASTER,QByteArray(&ch,1));
		}
	}
	m_btnQi->hide();
	m_btnYaZhu->hide();
	m_btnJiaBei->hide();
	m_btnGen->hide();
}
QString	NiuNiuDesktopController::userItemNameSuffix( DJGameUser* user ) const
{
	if ( 0 != m_current.chMaster 
		&& user->seatId() == m_current.chMaster ) {
		return QString("(")+tr("master")+QString(")");
	}
	return DJDesktopController::userItemNameSuffix(user);
}