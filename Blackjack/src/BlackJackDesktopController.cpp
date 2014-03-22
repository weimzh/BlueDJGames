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
#include "BlackJackDesktopController.h"
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

BlackJackDesktopController::BlackJackDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent )
	: DJDesktopPokerController(panelController,size,parent)
{
	djDebug()<<"BlackJackDesktopController constructor"<<size;
	
	memset(&m_current,0,sizeof(BlackjackCurrent));
	
	setSelectRectEnabled(false);
	setSelectEnabled(false);
	
	QPixmap pix(":/BlackjackRes/image/casinodesk.png");
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
	font.setBold( true );
	
	PBlackjackRoom  pbjroom = (PBlackjackRoom)panelController->gameRoom()->privateRoom();
	int minWager	= letoh4(pbjroom->uChipFaceValue);
	QString text	= tr("Min:")+QString::number(minWager);
	
	DJGraphicsTextItem *textItem	= new DJGraphicsTextItem( 0, desktop()->scene() );
	textItem->setFont( font );
	textItem->setPlainText( text );
	textItem->setZValue( 20 );
	textItem->setVirtualPos( QPoint(desktop()->realWidth()/2-177,desktop()->realHeight()/2-245) );
	
	int maxWager	= letoh4(pbjroom->uGameMaxValues);
	text	= tr("Max:")+QString::number(maxWager);
	textItem	= new DJGraphicsTextItem( 0, desktop()->scene() );
	textItem->setFont( font );
	textItem->setPlainText( text );
	textItem->setZValue( 20 );
	textItem->setVirtualPos( QPoint(desktop()->realWidth()/2-177,desktop()->realHeight()/2-225) );
	
	m_btnTingShou = new QPushButton( desktop() );
	m_btnTingShou->setText( tr("Stand") );
	m_btnTingShou->adjustSize();
	m_btnTingShou->hide();
	connect(m_btnTingShou,SIGNAL(clicked()),this,SLOT(clickTingShou()));
	
	m_btnJiaBei = new QPushButton( desktop() );
	m_btnJiaBei->setText( tr("Double") );
	m_btnJiaBei->adjustSize();
	m_btnJiaBei->hide();
	connect(m_btnJiaBei,SIGNAL(clicked()),this,SLOT(clickJiaBei()));
	
	m_btnYaoPai = new QPushButton( desktop() );
	m_btnYaoPai->setText( tr("Draw") );
	m_btnYaoPai->adjustSize();
	m_btnYaoPai->hide();
	connect(m_btnYaoPai,SIGNAL(clicked()),this,SLOT(clickFangQi()));
}
BlackJackDesktopController::~BlackJackDesktopController()
{
	djDebug()<<"BlackJackDesktopController destructor";
}
void BlackJackDesktopController::initUnderGameInfo( const QByteArray& buf )
{
	djDebug()<<"BlackJackDesktopController::initUnderGameInfo";
	
	DJDesktopPokerController::initUnderGameInfo( buf );
		
	memcpy(&m_current,buf.data(),sizeof(BlackjackCurrent));
	
	PBlackjackRoom  pbjroom = (PBlackjackRoom)panelController()->gameRoom()->privateRoom();
	for(int i=1;i<=4;i++)
	{
		c_PlayerDoubles[i] = pbjroom->chDouble+1;
	}
}
void BlackJackDesktopController::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	DJDesktopPokerController::gameWait(mask,status,timeout);
	
	m_btnTingShou->hide();
	m_btnJiaBei->hide();
	m_btnYaoPai->hide();
	
	switch(status)
	{
		case DJGAME_BLACKJACK_TABLE_STATUS_WAIT_PLAYER :
		{
			if(isWaitingForMe() && !panelController()->isLookingOn() )
			{
				m_btnYaoPai->show();
				m_btnTingShou->show();
				quint8 selfSeat	= panelController()->seatId();
				if( selfSeat != m_current.chDealer
					&& c_PlayerDoubles[selfSeat] > 0)
					m_btnJiaBei->show();
			}
			break;
		}
	}
}
void BlackJackDesktopController::gameTraceModel( const GeneralGameTrace2Head *gameTrace )
{
	djDebug() << "BlackJackDesktopController::gameTraceModel";
	
	quint8 gameTraceSeat	= gameTrace->chSite;
	quint8 bufLen	= gameTrace->chBufLen;
	const unsigned char *buf	= gameTrace->chBuf;
	
	djDebug()<<"gameTraceSeat"<<gameTraceSeat<<"bufLen"<<bufLen;
	DebugBuffer( (const char*)buf, bufLen );
	
	switch(gameTrace->chType) {
	case BLACKJACK_GAMETRACE_PICKUP:
	case BLACKJACK_GAMETRACE_PICKUP2: {
		PBlackjackPickup ppickup = (PBlackjackPickup)gameTrace->chBuf;
		if(ppickup->chCardNumber >0 && ppickup->chCardNumber <=BLACKJACK_PLAYER_MAX_CARDS)
		{
			DJDesktopItem *item	= createDesktopItem( gameTraceSeat, POKER_TYPE_HAND, ppickup->chCard );
			replaceDesktopItem( gameTraceSeat, POKER_TYPE_HAND, ppickup->chCardNumber-1, item );
		}
		break;
	}
	case BLACKJACK_GAMETRACE_SHOWCARDS: {
		clearDesktopItems( gameTraceSeat, POKER_TYPE_HAND );
		PBlackjackShowCards pshows = (PBlackjackShowCards)gameTrace->chBuf;
		if(pshows->chCards >0 && pshows->chCards <=BLACKJACK_PLAYER_MAX_CARDS) {
			for ( quint8 i = 0; i < pshows->chCards; i++ ) {
				if ( 0 != pshows->chCard[i] )
					appendDesktopItem( gameTraceSeat, POKER_TYPE_HAND, pshows->chCard[i] );
			}
		}
		break;
	}
	case BLACKJACK_GAMETRACE_BET: {
		PBlackjackRoom  pbjroom = (PBlackjackRoom)panelController()->gameRoom()->privateRoom();
		c_PlayerDoubles[gameTraceSeat]--;
		for ( quint8 i = 0; i < pbjroom->chNormalChips; i++ ) {
			DJDesktopItem *item	= DJDesktopController::createDesktopItem( gameTraceSeat, CHIP_TYPE_WAGER, CHIP_BLUE1 );
			appendDesktopItem( gameTraceSeat, CHIP_TYPE_WAGER, item );
			item	= DJDesktopController::createDesktopItem( gameTraceSeat, CHIP_TYPE_WAGER, CHIP_BLUE1 );
			appendDesktopItem( m_current.chDealer, CHIP_TYPE_WAGER, item );			
		}
		break;
	}
	case BLACKJACK_GAMETRACE_RESULT: {
		PBlackjackRoom  pbjroom = (PBlackjackRoom)panelController()->gameRoom()->privateRoom();
		int chipValue	= letoh4(pbjroom->uChipFaceValue);
		PBlackjackResult presult = (PBlackjackResult)gameTrace->chBuf;
		int chips	= presult->chChips;
		int value	= chips * chipValue;
		
		DJGameUser	*player	= panelController()->player(gameTraceSeat);
		DJGameUser	*dealer	= panelController()->player(m_current.chDealer);
			
		if( BLACKJACK_GAMERESULT_WIN == presult->chResult ) {
			djDebug()<<"win";
			
			clearDesktopItems( gameTraceSeat, CHIP_TYPE_WAGER );
			for ( int i = 0; i < chips; i++ ) {
				DJDesktopItem *item	= DJDesktopController::createDesktopItem( gameTraceSeat, CHIP_TYPE_WAGER, CHIP_BLUE1 );
				appendDesktopItem( gameTraceSeat, CHIP_TYPE_WAGER, item );
				item	= DJDesktopController::createDesktopItem( gameTraceSeat, CHIP_TYPE_WAGER, CHIP_BLUE1 );
				appendDesktopItem( gameTraceSeat, CHIP_TYPE_WAGER, item );
				item	= takeDesktopItem( m_current.chDealer, CHIP_TYPE_WAGER, CHIP_BLUE1 );
				delete item;			
			}
			QString result;
			if ( dealer ) {
				result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( dealer->userName() + tr("(dealer)") )
								.arg( -value );
			}
			if ( player ) {
				result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( player->userName() )
								.arg( value );
			}
			panelController()->insertGameResult2Browser( result );
		}else if( BLACKJACK_GAMERESULT_LOST == presult->chResult ) {
			djDebug()<<"lose";
			clearDesktopItems( gameTraceSeat, CHIP_TYPE_WAGER );
			for ( quint8 i = 0; i < presult->chChips; i++ ) {
				DJDesktopItem *item	= DJDesktopController::createDesktopItem( m_current.chDealer, CHIP_TYPE_WAGER, CHIP_BLUE1 );
				appendDesktopItem( m_current.chDealer, CHIP_TYPE_WAGER, item );		
			}
			QString result;
			if ( dealer ) {
				result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( dealer->userName() + tr("(dealer)") )
								.arg( value );
			}
			if ( player ) {
				result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( player->userName() )
								.arg( -value );
			}
			panelController()->insertGameResult2Browser( result );
		}else if ( BLACKJACK_GAMERESULT_DRAW == presult->chResult ) {
			QString result;
			if ( dealer ) {
				result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( dealer->userName() + tr("(dealer)") )
								.arg( 0 );
			}
			if ( player ) {
				result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( player->userName() )
								.arg( 0 );
			}
			panelController()->insertGameResult2Browser( result );
		}
		break;
	}
	default:
		break;
	}
}
void BlackJackDesktopController::gameTraceView( const GeneralGameTrace2Head *gameTrace )
{
	djDebug()<<"BlackJackDesktopController::gameTraceView";
	
	quint8 gameTraceSeat	= gameTrace->chSite;
	quint8 bufLen	= gameTrace->chBufLen;
	const unsigned char *buf	= gameTrace->chBuf;
	
	switch( gameTrace->chType ) {
	case BLACKJACK_GAMETRACE_PICKUP:
	case BLACKJACK_GAMETRACE_PICKUP2:
	case BLACKJACK_GAMETRACE_SHOWCARDS:
		repaintHandPokers( gameTraceSeat, POKER_TYPE_HAND, true, true, false );
		break;
	case BLACKJACK_GAMETRACE_BET: {
		PBlackjackRoom  pbjroom = (PBlackjackRoom)panelController()->gameRoom()->privateRoom();
		int chipValue	= letoh4(pbjroom->uChipFaceValue);
		DJGraphicsTextItem *textItem	= m_wagerTextItems[gameTraceSeat];
		repaintWagerChips( gameTraceSeat, CHIP_TYPE_WAGER, textItem, chipValue, true, true, false );
		textItem	= m_wagerTextItems[m_current.chDealer];
		repaintWagerChips( m_current.chDealer, CHIP_TYPE_WAGER, textItem, chipValue, true, true, false );
		break;
	}
	case BLACKJACK_GAMETRACE_RESULT: {
		PBlackjackRoom  pbjroom = (PBlackjackRoom)panelController()->gameRoom()->privateRoom();
		int chipValue	= letoh4(pbjroom->uChipFaceValue);
		DJGraphicsTextItem *textItem	= m_wagerTextItems[gameTraceSeat];
		repaintWagerChips( gameTraceSeat, CHIP_TYPE_WAGER, textItem, chipValue, true, true, false );
		textItem	= m_wagerTextItems[m_current.chDealer];
		repaintWagerChips( m_current.chDealer, CHIP_TYPE_WAGER, textItem, chipValue, true, true, false );
		break;
	}
	default:
		break;
	}
}
void BlackJackDesktopController::locatePoolSpace( QRect& rect )
{
	rect.setBottom( availableSpace().bottom() - static_cast<int>(scaleOfHand(1) * (pokerSize().height())) );
	rect.setLeft( availableSpace().left() + static_cast<int>(scaleOfHand(2) * pokerSize().height()) );
	rect.setTop( availableSpace().top() + static_cast<int>(scaleOfHand(3) * pokerSize().height()) );
	rect.setRight( availableSpace().right() - static_cast<int>(scaleOfHand(4) * pokerSize().height()) );
}
void BlackJackDesktopController::locateCenterSpace( QRect& rect )
{
	int space	= 40;
	
	rect.setBottom( centerPoint().y() + space + 48 );
	rect.setLeft( centerPoint().x() - space * 2 );
	rect.setTop( centerPoint().y() - space );
	rect.setRight( centerPoint().x() + space * 2 );
}
bool BlackJackDesktopController::isUserPlaying( quint8 seatId ) const
{
	for( quint8 seat = 0; seat <= DJGAME_BLACKJACK_MAX_PLAYERS; ++seat ) {
		if ( seatId == m_current.chMapSites[seat] )
			return true;
	}
	return false;
}
void BlackJackDesktopController::repaintHost()
{
	quint8 dealerSeat	= m_current.chDealer;
	if ( 0 != dealerSeat ) {
		repaintHostItem( dealerSeat );	
	}
}
void BlackJackDesktopController::repaintCurrentStatus()
{
	djDebug()<<"BlackJackDesktopController::repaintCurrentStatus";
	
	repaintAll();
}
void BlackJackDesktopController::repaintAll()
{
	djDebug()<<"BlackJackDesktopController::repaintAll";
	DJDesktopPokerController::repaintAll();
	
	PBlackjackRoom  pbjroom = (PBlackjackRoom)panelController()->gameRoom()->privateRoom();
	int chipValue	= letoh4(pbjroom->uChipFaceValue);	
	quint8 seat;
	for( seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
		repaintHandPokers( seat, POKER_TYPE_HAND, true, true, false );
		DJGraphicsTextItem *textItem	= m_wagerTextItems[seat];
		repaintWagerChips( seat, CHIP_TYPE_WAGER, textItem, chipValue, true, true, false );
	}
}
void BlackJackDesktopController::clickTingShou()
{
	sendGameTrace(BLACKJACK_GAMETRACE_STOP);
	m_btnTingShou->hide();
	m_btnJiaBei->hide();
	m_btnYaoPai->hide();
}
void BlackJackDesktopController::clickJiaBei()
{
	sendGameTrace(BLACKJACK_GAMETRACE_BET);
	m_btnTingShou->hide();
	m_btnJiaBei->hide();
	m_btnYaoPai->hide();
}
void BlackJackDesktopController::clickFangQi()
{
	sendGameTrace(BLACKJACK_GAMETRACE_DRAW);
	m_btnTingShou->hide();
	m_btnJiaBei->hide();
	m_btnYaoPai->hide();
}
void BlackJackDesktopController::locateAllNonMatrimoveItems()
{
	DJDesktopPokerController::locateAllNonMatrimoveItems();
	
	QPoint pos( 680, 600 );
	QMatrix matrix	= desktop()->graphicsMatrix();
	pos	= matrix.map( pos );
	int x	= pos.x();
	int	y	= pos.y();
	
	m_btnTingShou->move( x - m_btnTingShou->width(), y );
	y	+= m_btnTingShou->height();
	
	m_btnYaoPai->move( x - m_btnYaoPai->width(), y );
	y	+= m_btnYaoPai->height();
	
	m_btnJiaBei->move( x - m_btnJiaBei->width(), y );
	y	+= m_btnJiaBei->height();
}
bool BlackJackDesktopController::isCloseable() const
{
	bool closable	= DJDesktopController::isCloseable();
	if ( !closable ) {
		QList<quint8> seats	= seatIdsOfPlayingUsers();
		if ( !seats.contains(panelController()->seatId()) )
			closable	= true;
	}
	return closable;
}