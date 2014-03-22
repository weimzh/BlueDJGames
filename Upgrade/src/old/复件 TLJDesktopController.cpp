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
#include "TLJDesktopController.h"
#include "DJPanelController.h"
#include "DJPanel.h"
#include "DJGameRoom.h"
#include "DJDesktop.h"
#include "DJGraphicsTextItem.h"
#include "DJGraphicsPixmapItem.h"
#include "DJGraphicsSvgItem.h"
#include "DJDesktopItem.h"
#include "DJGameUser.h"
#include "DJPoker.h"
#include "DJPokerPattern.h"
#include "DJMessageBox.h"
#include "Utility.h"
#include "DJDesktopScene.h"

TLJDesktopController::TLJDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent )
	: DJDesktopPokerController(panelController,size,parent)
{
	djDebug()<<"TLJDesktopController constructor"<<size;
	
	m_timeline = new QTimeLine(1000, this);
	m_timeline->setFrameRange(0,0);
 	m_timeline->setCurveShape(QTimeLine::LinearCurve);
 	connect(m_timeline, SIGNAL(frameChanged(int)), SLOT(handleFrameChanged(int)));
 	connect(m_timeline, SIGNAL(stateChanged(QTimeLine::State)), SLOT(handleStateChanged(QTimeLine::State)));
 	
 	for( int seat = 0; seat <= panelController->numberOfSeats(); ++seat ) {
 		QList<unsigned char> cards;
		m_dealingCards << cards;
	}
	
	StaticInitDesktop();
}
TLJDesktopController::~TLJDesktopController()
{
	djDebug()<<"TLJDesktopController destructor";
}
void TLJDesktopController::gameInfo( const QByteArray& buf )
{
	djDebug()<<"TLJDesktopController::gameInfo";
	
	DJDesktopPokerController::gameInfo( buf );
	
	memcpy(&c_current,buf.data(),sizeof(UpgradeCurrent));
	m_levelPoker	= DJPoker( (int)DJPoker::AnySuit, (DJPoker::Point)c_current.chLevel );
}
void TLJDesktopController::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	DJDesktopPokerController::gameWait(mask,status,timeout);
}
void TLJDesktopController::gameTraceModel( const GeneralGameTrace2Head *gameTrace )
{
	djDebug() << "TLJDesktopController::gameTraceModel"<<"seat"<<gameTrace->chSite<<"len"<<gameTrace->chBufLen;
	switch(gameTrace->chType)
	{
		case UPGRADE2_GAMETRACE_RESET :
		case UPGRADE2_GAMETRACE_PICKUP :
		{
			djDebug()<<"UPGRADE2_GAMETRACE_PICKUP";
			/*
			ResetPlayerCards(gameTrace->chSite,NULL,0);
			memcpy(c_chPlayerCardsBuffer[gameTrace->chSite],gameTrace->chBuf,gameTrace->chBufLen);
			c_chPlayerCardPages[gameTrace->chSite] = gameTrace->chBufLen;
			
			quint8 numberOfCards	= c_chPlayerCardPages[gameTrace->chSite];
			m_timeline->setDuration( DealInterval * numberOfCards );
			m_timeline->setFrameRange(0, numberOfCards );
			*/
			DebugBuffer( (const char*)gameTrace->chBuf, gameTrace->chBufLen );
			quint8 len	= gameTrace->chBufLen;
			const unsigned char *buf	= gameTrace->chBuf;
			
			/*
			clearDesktopItems( gameTrace->chSite, DJGAME_POKERDESKTOP_TYPE_HANDCARD );		
			for ( quint8 i = 0; i < len; i++ ) {
				appendDesktopItem( gameTrace->chSite, DJGAME_POKERDESKTOP_TYPE_HANDCARD, buf[i] );
			}
			*/
			QList<unsigned char> &cards	= m_dealingCards[gameTrace->chSite];
			cards.clear();
			for ( quint8 i = 0; i < len; i++ ) {
				cards.append( buf[i] );
			}
			m_timeline->setDuration( 500 * len );
			m_timeline->setFrameRange( 0, len );
			m_timeline->start();
			break;
		}
		case UPGRADE_GAMETRACE_THROW : 
		{
			djDebug()<<"UPGRADE_GAMETRACE_THROW";
			
			m_timeline->stop();
			
			DebugBuffer( (const char*)gameTrace->chBuf, gameTrace->chBufLen );
			quint8 len	= gameTrace->chBufLen;
			const unsigned char *buf	= gameTrace->chBuf;
			
			for ( quint8 i = 0; i < len; i++ ) {
				DJDesktopItem *item	= takeDesktopItem( gameTrace->chSite, DJGAME_POKERDESKTOP_TYPE_HANDCARD, buf[i] );
				delete item;
				appendDesktopItem( gameTrace->chSite, DJGAME_POKERDESKTOP_TYPE_THROWCARD, buf[i] );
			}
			/*
			if ( 0 == m_masterSeat )
				m_masterSeat	= gameTrace->chSite;
			DrawOver(false);
			if(c_bFirstPlayer == true)
			{
				if(c_current.chMaster == 0)
				{
					c_current.chMaster = gameTrace->chSite;
					ModifyMaster();
				}
				if(gameTrace->chSite != panelController()->seatId())
					c_bSelfThrow = false;
				c_bFirstPlayer = false;
				c_chFirstPages = gameTrace->chBufLen;
				c_chFirstClass = DJGAME_POKER_CLASS(gameTrace->chBuf[0]);
			}
			if(gameTrace->chSite == panelController()->seatId())
				c_bSelfThrow = true;
			RemovePlayerCards(gameTrace->chSite,gameTrace->chBuf,gameTrace->chBufLen);
			memcpy(&(c_ThrowCards[gameTrace->chSite][0][1]),gameTrace->chBuf,gameTrace->chBufLen);
			c_ThrowCards[gameTrace->chSite][0][0] = gameTrace->chBufLen;
			*/
			break;
		}
		case UPGRADE2_GAMETRACE_CLEARDESKTOP : 
		{
			djDebug() << "UPGRADE2_GAMETRACE_CLEARDESKTOP";
			
			m_timeline->stop();
			for ( int seat = 1; seat <= panelController()->numberOfSeats(); ++seat) {
				clearDesktopItems(seat,DJGAME_POKERDESKTOP_TYPE_THROWCARD);
			}
			/*
			djDebug() << "m_masterSeat" << m_masterSeat 
						<< "c_bFirstPlayer" << c_bFirstPlayer
						<< "c_bSelfThrow" <<c_bSelfThrow
						<< "c_chFirstPages" << c_chFirstPages;
			DJGameRoom *room= panelController()->gameRoom();
			for(int i= UPGRADE_MAX_SAVE_ROUND;i>= 0;i--)
			{
				for(int j=1;j<=room->numberOfSeats();j++)
				{
					if(i>0)
						memcpy(c_ThrowCards[j][i],c_ThrowCards[j][i-1],60);
					else
						memset(c_ThrowCards[j][0],0,60);
				}
			}
			
			m_previous->setEnabled(true);
			c_bFirstPlayer = true;
			c_chFirstPages = 0;
			c_bSelfThrow = false;
			djDebug() << "m_masterSeat" << m_masterSeat 
						<< "c_bFirstPlayer" << c_bFirstPlayer
						<< "c_bSelfThrow" <<c_bSelfThrow
						<< "c_chFirstPages" << c_chFirstPages;
						*/
			break;
		}
		case UPGRADE_GAMETRACE_MASTER :
		{
			djDebug() << "UPGRADE_GAMETRACE_MASTER";
			/*
			m_masterSeat	= gameTrace->chSite;
			if(gameTrace->chBufLen > 0)
			{
				memcpy(&(c_ScoreCards[c_ScorePages]),gameTrace->chBuf,gameTrace->chBufLen);
				c_ScorePages += gameTrace->chBufLen;
				for(int i=0;i<gameTrace->chBufLen;i++)
				{
					if(DJGAME_POKER_VALUE(gameTrace->chBuf[i]) == 5)
						c_shCurrentScore += 5;
					else
						c_shCurrentScore += 10;
				}
			}
			*/
			break;
		}
		case UPGRADE_GAMETRACE_SHOW :
		{
			djDebug() << "UPGRADE_GAMETRACE_SHOW";
			/*
			if(gameTrace->chBuf[0] > 0)
			{
				quint8 buffer[6];
				quint8 chPages = (gameTrace->chBuf[0]>>6)+1;
				for(int i=0;i<chPages;i++)
					buffer[i] = gameTrace->chBuf[0]&0x3F;
				
				memcpy(&(c_ThrowCards[gameTrace->chSite][0][1]),buffer,chPages);
				c_ThrowCards[gameTrace->chSite][0][0] = chPages;
				c_chLastShow = gameTrace->chBuf[0];
				c_chLastShowSeat = gameTrace->chSite;
			}
			CheckShowStatus();
			ModifyShow(gameTrace->chSite,gameTrace->chBuf[0]);
			*/
			m_showedPoker	= DJPoker( (quint8)DJGAME_POKER_CARD(gameTrace->chBuf[0]), (bool)isBridgeCard());
			break;
		}
		//case UPGRADE_GAMETRACE_HANDS :
		case UPGRADE2_GAMETRACE_HANDS :
		{
			djDebug() << "UPGRADE2_GAMETRACE_HANDS";
			
			m_timeline->stop();
			
			quint8 len	= gameTrace->chBufLen;
			const unsigned char *buf	= gameTrace->chBuf;
			
			for ( quint8 i = 0; i < len; i++ ) {
				appendDesktopItem( gameTrace->chSite, DJGAME_POKERDESKTOP_TYPE_HANDCARD, buf[i] );
			}
			/*
			DrawOver(false);
			AppendPlayerCards(gameTrace->chSite,gameTrace->chBuf,gameTrace->chBufLen);
			*/
			break;
		}
		//case UPGRADE2_GAMETRACE_SUBSTRACT :
		case UPGRADE3_GAMETRACE_SUBSTRACT :
		{
			djDebug() << "UPGRADE3_GAMETRACE_SUBSTRACT";
			
			m_timeline->stop();
			
			quint8 len	= gameTrace->chBufLen;
			const unsigned char *buf	= gameTrace->chBuf;
			
			for ( quint8 i = 0; i < len; i++ ) {
				DJDesktopItem *item	= takeDesktopItem( gameTrace->chSite, DJGAME_POKERDESKTOP_TYPE_HANDCARD, buf[i] );
				delete item;
			}
			/*
			DrawOver(false);
			if(c_current.chMaster == 0)
			{
				c_current.chMaster = gameTrace->chSite;
				ModifyMaster();
			}
			RemovePlayerCards(gameTrace->chSite,gameTrace->chBuf,gameTrace->chBufLen);
			memcpy(c_chSubstract,gameTrace->chBuf,gameTrace->chBufLen);
			c_chSubstractPages = gameTrace->chBufLen;
			m_subtract->setEnabled(true);
			*/
			break;
		}
		case UPGRADE_GAMETRACE_GAMEOVER :
		{
			djDebug()<<"UPGRADE_GAMETRACE_GAMEOVER";
			/*
			c_chLastMulti = gameTrace->chBuf[0];
			c_chLastScore = 0;
			for(int i=1;i<gameTrace->chBufLen;i++)
			{
				if(DJGAME_POKER_VALUE(gameTrace->chBuf[i]) == 5)
					c_chLastScore += 5;
				else if(DJGAME_POKER_VALUE(gameTrace->chBuf[i]) == 10 || DJGAME_POKER_VALUE(gameTrace->chBuf[i])==DJGAME_POKER_K)
					c_chLastScore += 10;
			}
			*/
			break;
		}
	}
}
void TLJDesktopController::gameTraceView( const GeneralGameTrace2Head *gameTrace )
{
	djDebug()<<"TLJDesktopController::gameTraceView";
		
	switch( gameTrace->chType ) {
		case UPGRADE2_GAMETRACE_RESET :
		case UPGRADE2_GAMETRACE_PICKUP :
		{
			djDebug()<<"UPGRADE2_GAMETRACE_PICKUP";
			
			repaintHandPokers( gameTrace->chSite, true, true, true );
			/*
			if(panelController()->isLookingOn())
				DrawOver(true);
			*/
			break;
		}
		case UPGRADE_GAMETRACE_THROW: {
			djDebug()<<"UPGRADE_GAMETRACE_THROW";
			repaintHandPokers( gameTrace->chSite, false, true, false );
			repaintThrownPokers( gameTrace->chSite );
			/*
			RepaintSeatCards(gameTrace->chSite);
			RepaintThrowCards(gameTrace->chSite,&(c_ThrowCards[gameTrace->chSite][0][1]),c_ThrowCards[gameTrace->chSite][0][0]);
			playWave("out.wav");
			*/
			break;
		}
		case UPGRADE2_GAMETRACE_CLEARDESKTOP : 
		{
			djDebug()<<"UPGRADE2_GAMETRACE_CLEARDESKTOP";
			break;
		}
		case UPGRADE_GAMETRACE_MASTER :
		{
			djDebug()<<"UPGRADE_GAMETRACE_MASTER";
			/*
			if(gameTrace->chBufLen > 0)
			{
				quint8 seat;
				if(c_current.chMaster%2 == panelController()->seatId()%2)
				{
					seat = (panelController()->seatId()==1)?4:panelController()->seatId()-1;
				}else
					seat = panelController()->seatId();
				RepaintAttachCards(seat,c_ScoreCards,c_ScorePages);
				ModifyScoreNumber();
			}
			*/
			break;
		}
		case UPGRADE_GAMETRACE_SHOW :
		{
			djDebug()<<"UPGRADE_GAMETRACE_SHOW";
			/*
			for ( int seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
				repaintHandPokers( seat, false, true, true );
			}
			*/
			/*
			RepaintThrowCards(gameTrace->chSite,&(c_ThrowCards[gameTrace->chSite][0][1]),c_ThrowCards[gameTrace->chSite][0][0]);
			RepaintSeatCards(1);
			RepaintSeatCards(2);
			RepaintSeatCards(3);
			RepaintSeatCards(4);
			*/
			break;
		}
		case UPGRADE2_GAMETRACE_HANDS :
		{
			djDebug()<<"UPGRADE2_GAMETRACE_HANDS";
			repaintHandPokers( gameTrace->chSite, false, true, true );
			/*
			RepaintSeatCards(gameTrace->chSite);
			*/
			break;
		}
		case UPGRADE3_GAMETRACE_SUBSTRACT :
		{
			djDebug()<<"UPGRADE3_GAMETRACE_SUBSTRACT";
			repaintHandPokers( gameTrace->chSite, false, true, false );
			/*
			RepaintSeatCards(gameTrace->chSite);
			*/
			break;
		}
		case UPGRADE_GAMETRACE_GAMEOVER :	
		{
			djDebug()<<"UPGRADE_GAMETRACE_GAMEOVER";
			/*
			int x,y;
			GetDesktopCenterPoint(&x,&y,NULL);
			c_Result->setPlainText(QString("%1 + %2 x %3 = %4").arg(c_shCurrentScore).arg(c_chLastScore).arg(c_chLastMulti).arg(c_shCurrentScore+c_chLastScore*c_chLastMulti));
			c_Result->setAlignment( Qt::AlignCenter );
			c_Result->setVirtualPos( QPointF(x,y) );
			c_Result->setMatrix( desktop()->scaleMatrix() );
			c_Result->adjustPos( desktop()->totalMatrix() );
			c_Result->show();
			DrawHCards(x,y,1.0,0,DJGAME_UPGRADE_TYPE_RESULT,(quint8 *)&(gameTrace->chBuf[1]),gameTrace->chBufLen-1,Qt::AlignHCenter,1900);
			*/
			break;
		}
		default:
			break;
	}
}
void TLJDesktopController::StaticInitDesktop()
{
	m_showedPoker	= DJPoker();
	m_levelPoker	= DJPoker();
	memset(&c_current,0,sizeof(UpgradeCurrent));
	
	djDebug()<<"ooooo";
	for( int seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
		QList<unsigned char> &cards	= m_dealingCards[seat];
		cards.clear();	
	}
	djDebug()<<"yyyyy";
}
void TLJDesktopController::repaintCurrentStatus()
{
	djDebug()<<"TLJDesktopController::repaintCurrentStatus";
	
	for( int seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
		repaintHandPokers( seat, false, true, true );
	}
	repaintInfo();
	/*
	for(int i=1;i<=panelController()->numberOfSeats();i++)
	{
		RepaintSeatCards(i);
		RepaintThrowCards(i,&(c_ThrowCards[i][0][1]),c_ThrowCards[i][0][0]);
	}
	quint8 seat;
	if(c_current.chMaster%2 == panelController()->seatId()%2)
	{
		seat = (panelController()->seatId()==1)?4:panelController()->seatId()-1;
	}else
		seat = panelController()->seatId();
	RepaintAttachCards(seat,c_ScoreCards,c_ScorePages);
	ModifyScoreNumber();
	ModifyMaster();
	*/
}
/*
void TLJDesktopController::locateCenterPoint( QPoint& point )
{
	djDebug()<<"TLJDesktopController::locateCenterPoint";
	
	point.setX( desktop()->realWidth() >> 1 );
	point.setY( (desktop()->realHeight() >> 1) - 100 );
}
*/
int TLJDesktopController::rankOnSuit( const DJPoker& poker ) const
{
	//djDebug()<<"TLJDesktopController::rankOnSuit";
	//djDebug()<<"poker"<<poker.description();
	if ( !poker.isValid() )
		return 0;
	//djDebug()<<"m_showedPoker"<<m_showedPoker.description();
	int rank	= DJDesktopPokerController::rankOnSuit(poker);
	
	//djDebug()<<"rank"<<hex<<rank;
	//DJPoker levelPoker( (int)DJPoker::AnySuit, (DJPoker::Point)c_current.chLevel );
	//djDebug()<<"levelPoker"<<levelPoker.description();
	
	if ( m_levelPoker.isValid() && m_levelPoker.point() == poker.point() ) {
		//djDebug()<<"const trump";
		rank	|= DJPoker::ConstTrump;
	}
	if ( m_showedPoker.isValid() && poker.suit() == m_showedPoker.suit() ) {
		//djDebug()<<"suit trump";
		rank	|= DJPoker::SuitTrump;
	}
	//djDebug()<<"final rank ="<<hex<<rank;

	return rank;
}
int TLJDesktopController::distanceBetweenPokers( const DJPoker& poker1, const DJPoker& poker2 ) const
{
	int distance	= DJDesktopPokerController::distanceBetweenPokers(poker1,poker2);
	if ( !isSameClass(poker1,poker2) )
		distance = (distance << 1);
	return distance;
}
bool TLJDesktopController::isSameClass( const DJPoker& poker1, const DJPoker& poker2 ) const
{
	return classOfPoker(poker1) == classOfPoker(poker2); 
}
bool TLJDesktopController::isTrump( const DJPoker& poker ) const
{
	int rank	= rankOnSuit( poker );
	if ( (rank & DJPoker::SuitTrump) || (rank & DJPoker::ConstTrump) )
		return true;
	else
		return false;
}
TLJDesktopController::PokerClass TLJDesktopController::classOfPoker( const DJPoker& poker ) const
{
	if ( isTrump( poker ) )
		return Trump;
	int suit	= poker.suit();
	switch( suit ) {
	case DJPoker::Diamond:
		return Diamond;
	case DJPoker::Club:
		return Club;
	case DJPoker::Heart:
		return Heart;
	case DJPoker::Spade:
		return Spade;
	default:
		return Unknown;
	}
}

void TLJDesktopController::handleFrameChanged( int frame )
{
	djDebug()<<"TLJDesktopController::handleFrameChanged"<<frame;
	
	int totalCards	= m_timeline->endFrame();
	
	for( int seat = 1; seat <= 4; seat++ ) {
		int undealedCards	= m_dealingCards[seat].size();
		int dealedCards	= totalCards - undealedCards;
		int shouldDealCards	= frame - dealedCards;
		if ( undealedCards > 0 && shouldDealCards > 0 ) {
			int delta	= qMin(undealedCards,shouldDealCards);
			while( delta-- ) {
				djDebug()<<"delta"<<delta;
				appendDesktopItem( seat, DJGAME_POKERDESKTOP_TYPE_HANDCARD, m_dealingCards[seat].takeFirst() );
			}
			repaintHandPokers( seat, true, true, true );
		}
	}
	
	/*
	quint8 totalFrame	= m_timeline->endFrame();
	djDebug()<<"totalFrame="<<totalFrame<<"currentCardFrame"<<currentCardFrame;
	if ( 0 == totalFrame ) {
		return;
	}
	
	int i;
	for(i=1;i<=4;i++)
	{
		quint8 numberOfUndealedCard	=  c_chPlayerCardPages[i];
		quint8 numberOfDealedCards	= totalFrame - numberOfUndealedCard;
		quint8 delta	= currentCardFrame - numberOfDealedCards;
	
		djDebug()<<"i="<<i
				<<"numberOfUndealedCard"<<numberOfUndealedCard
				<<"numberOfDealedCards"<<numberOfDealedCards
				<<"delta"<<delta;
			
		if( c_chPlayerCardPages[i] > 0 && delta )
		{
			//DebugBuffer( (const char*)c_chPlayerCardsBuffer[i], c_chPlayerCardPages[i] );
			while( delta-- ) {
				AppendPlayerCards(i,&(c_chPlayerCardsBuffer[i][c_chPlayerCardPages[i]-1]),1);
				c_chPlayerCardPages[i] --;
			}
			//RepaintSeatCards(i,false);
			if( i == panelController()->seatId() ) {
				RepaintSeatCards(i,false);
				playWave("out.wav");
				CheckShowStatus();
			}
		}
	}
	*/
}
void TLJDesktopController::handleStateChanged( QTimeLine::State newState )
{
	djDebug()<<"TLJDesktopController::handleStateChanged"<<newState;
	if ( QTimeLine::NotRunning == newState ) {
		djDebug()<<"NotRunning";
		handleFrameChanged( m_timeline->endFrame() );
	}
}
