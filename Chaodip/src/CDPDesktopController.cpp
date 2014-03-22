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
#include "CDPDesktopController.h"
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

static qreal Z_RESULT		= 5000;

static int RESULT_TEXT_ITEM_FONT_SIZE	= 	40;
static int TEXT_ITEM_FONT_SIZE		= 	16;

static int TOOL_BUTTON_ICON_SIZE	=	24;

static QList<DJPoker::Point> gSeries	= QList<DJPoker::Point>()	<< DJPoker::Num2
																<< DJPoker::Num3
																<< DJPoker::Num4
																<< DJPoker::Num5
																<< DJPoker::Num6
																<< DJPoker::Num7
																<< DJPoker::Num8
																<< DJPoker::Num9
																<< DJPoker::Num10
																<< DJPoker::Jack
																<< DJPoker::Queen
																<< DJPoker::King
																<< DJPoker::Ace;

static uint qHash ( DJPoker poker )
{
	return (poker.suit() << 16) | poker.point();
}

CDPDesktopController::CDPDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent )
	: DJDesktopPokerController(panelController,size,parent)
{
	djDebug()<<"CDPDesktopController constructor"<<size;
	
	setBridgeCard( false );
	
	m_timeline = new QTimeLine(1000, this);
	m_timeline->setFrameRange(0,0);
 	m_timeline->setCurveShape(QTimeLine::LinearCurve);
 	connect(m_timeline, SIGNAL(frameChanged(int)), SLOT(handleFrameChanged(int)));
 	
 	for( int seat = 0; seat <= panelController->numberOfSeats(); ++seat ) {
 		QList<unsigned char> cards;
		m_undealedCards << cards;
		
		DJGraphicsPixmapItem	*suitItem	= new DJGraphicsPixmapItem( 0, desktop()->scene() );
		suitItem->setZValue( 2000 );
		suitItem->hide();
		m_suitItems  << suitItem;
	}
	
	m_suit2ShowButton.insert(DJPoker::Joker,0);
	m_suit2ShowButton.insert(DJPoker::Spade,1);
	m_suit2ShowButton.insert(DJPoker::Heart,2);
	m_suit2ShowButton.insert(DJPoker::Club,3);
	m_suit2ShowButton.insert(DJPoker::Diamond,4);
	m_suit2ShowButton.insert(DJPoker::NoSuit,5);
					
	
	QFont font;
	font.setPointSize( RESULT_TEXT_ITEM_FONT_SIZE );
	font.setBold( true );
	
	m_resultText	= new DJGraphicsTextItem( 0, desktop()->scene() );
	m_resultText->setFont( font );
	m_resultText->setZValue( Z_RESULT );
	
	font.setPointSize( TEXT_ITEM_FONT_SIZE );
	
	m_zhuangText = new DJGraphicsTextItem( tr("Master:"), 0, desktop()->scene(), false );
	m_zhuangText->setFont( font );
	m_scoreText = new DJGraphicsTextItem( tr("Score:"), 0, desktop()->scene(), false );
	m_scoreText->setFont( font );
	
	m_showContainer	= new QWidget( desktop() );
    QHBoxLayout* layout	= new QHBoxLayout( m_showContainer );
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    
    QSize iconSize( TOOL_BUTTON_ICON_SIZE, TOOL_BUTTON_ICON_SIZE );
    
    int index	= m_suit2ShowButton.value(DJPoker::Joker);
    m_showButtons[index] = new QToolButton;
	m_showButtons[index]->setToolTip(tr("show nt"));
	m_showButtons[index]->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/ntred.png") );
	m_showButtons[index]->setIconSize( iconSize );
    connect(m_showButtons[index],SIGNAL(pressed()),SLOT(showNT()));
    layout->addWidget( m_showButtons[index] );
    
    index	= m_suit2ShowButton.value(DJPoker::Spade);
	m_showButtons[index] = new QToolButton;
	m_showButtons[index]->setToolTip(tr("show spade"));
	m_showButtons[index]->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/s.png") );
	m_showButtons[index]->setIconSize( iconSize );
    connect(m_showButtons[index],SIGNAL(pressed()),SLOT(showSpade()));
    layout->addWidget( m_showButtons[index] );
    
    index	= m_suit2ShowButton.value(DJPoker::Heart);
    m_showButtons[index] = new QToolButton;
	m_showButtons[index]->setToolTip(tr("show heart"));
	m_showButtons[index]->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/h.png") );
	m_showButtons[index]->setIconSize( iconSize );
    connect(m_showButtons[index],SIGNAL(pressed()),this,SLOT(showHeart()));
    layout->addWidget( m_showButtons[index] );
    
    index	= m_suit2ShowButton.value(DJPoker::Club);
    m_showButtons[index] = new QToolButton;
	m_showButtons[index]->setToolTip(tr("show club"));
	m_showButtons[index]->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/c.png") );
	m_showButtons[index]->setIconSize( iconSize );
    connect(m_showButtons[index],SIGNAL(pressed()),SLOT(showClub()));
    layout->addWidget( m_showButtons[index] );
    
    index	= m_suit2ShowButton.value(DJPoker::Diamond);
    m_showButtons[index] = new QToolButton;
	m_showButtons[index]->setToolTip(tr("show diamond"));
	m_showButtons[index]->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/d.png") );
	m_showButtons[index]->setIconSize( iconSize );
    connect(m_showButtons[index],SIGNAL(pressed()),SLOT(showDiamond()));
    layout->addWidget( m_showButtons[index] );
    
    m_actionContainer	= new QWidget( desktop() );
    layout	= new QHBoxLayout( m_actionContainer );
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    
    m_tip = new QToolButton;
    m_tip->setEnabled(false);
	m_tip->setToolTip(tr("tip"));
	m_tip->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/tip.png") );
	m_tip->setIconSize( iconSize );
    connect(m_tip,SIGNAL(clicked()),SLOT(clickTip()));
    layout->addWidget( m_tip );
    
    m_throw = new QToolButton;
    m_throw->setEnabled(false);
	m_throw->setToolTip(tr("throw cards"));
	m_throw->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/chu.png") );
	m_throw->setIconSize( iconSize );
    connect(m_throw,SIGNAL(clicked()),SLOT(clickThrow()));
    layout->addWidget( m_throw );
    
	m_previous = new QToolButton;
    m_previous->setEnabled(false);
	m_previous->setToolTip(tr("look last round"));
	m_previous->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/left.png") );
	m_previous->setIconSize( iconSize );
    connect(m_previous,SIGNAL(pressed()),SLOT(previousPressed()));
    connect(m_previous,SIGNAL(released()),SLOT(previousRelesed()));
    layout->addWidget( m_previous );
    
    m_subtract = new QToolButton;
	m_subtract->setEnabled(false);
	m_subtract->setToolTip(tr("look discarded cards"));
	m_subtract->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/down.png") );
	m_subtract->setIconSize( iconSize );
	connect(m_subtract,SIGNAL(pressed()),SLOT(subtractPressed()));
    connect(m_subtract,SIGNAL(released()),SLOT(subtractReleased()));
    layout->addWidget( m_subtract );
    
	memset(&m_current,0,sizeof(UpgradeCurrent));
	
	m_levelPoker	= DJPoker();
	m_showedPoker	= DJPoker();
	
	m_numberOfShowedPokers	= 0;
	m_seatOfShow	= 0;
	m_currentScore	= 0;
	
	m_seatOfFirstThrow	= 0;
	m_numberOfFirstThrownCards	= 0;
	m_masterSeat	= 0;
	m_showIsOver	= false;
}
CDPDesktopController::~CDPDesktopController()
{
	djDebug()<<"CDPDesktopController destructor";
}
void CDPDesktopController::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	DJDesktopPokerController::gameWait(mask,status,timeout);
	
	m_showContainer->hide();
	m_actionContainer->show();
	m_tip->setEnabled(false);
	m_throw->setEnabled( false );
	
	switch( status ) {
	case DJGAME_TABLE_STATUS_WAITSTART:
		djDebug()<<"DJGAME_TABLE_STATUS_WAITSTART";
		
		break;
	case DJGAME_UPGRADE_TABLE_STATUS_WAIT_HANDS:
		djDebug()<<"DJGAME_UPGRADE_TABLE_STATUS_WAIT_HANDS";
		
		if ( isWaitingForMe() && !panelController()->isLookingOn() ) {
			m_throw->setEnabled( true );
			setWaitingThrowAcl( false );
		}
		break;
	case DJGAME_UPGRADE_TABLE_STATUS_WAIT_SHOW:
		djDebug()<<"DJGAME_UPGRADE_TABLE_STATUS_WAIT_SHOW";
		
		if ( isWaitingForMe() && !panelController()->isLookingOn() ) {
			m_showContainer->show();
			m_actionContainer->hide();
		}
		break;
	case DJGAME_UPGRADE_TABLE_STATUS_WAIT_OUT:
		djDebug()<<"DJGAME_UPGRADE_TABLE_STATUS_WAIT_OUT";
		
		if ( isWaitingForMe() && !panelController()->isLookingOn() ) {
			m_throw->setEnabled( true );
			setWaitingThrowAcl( false );
			
			if ( isFollowing() ) {
				m_tip->setEnabled(true);
				if ( numberOfPokers(panelController()->seatId(),POKER_TYPE_HAND) == m_numberOfFirstThrownCards ) {
					djDebug() << "the final out";
					throwAllCards();
				}
			}
		}
		break;
	default:
		break;
	}
}
void CDPDesktopController::gameTraceModel( const GeneralGameTrace2Head *gameTrace )
{
	djDebug() << "CDPDesktopController::gameTraceModel";
	
	quint8 gameTraceSeat	= gameTrace->chSite;
	quint8 bufLen	= gameTrace->chBufLen;
	const unsigned char *buf	= gameTrace->chBuf;
	
	djDebug()<<"gameTraceSeat"<<gameTraceSeat<<"bufLen"<<bufLen;
	DebugBuffer( (const char*)buf, bufLen );
	
	switch(gameTrace->chType) {
	case UPGRADE2_GAMETRACE_RESET :
	case UPGRADE2_GAMETRACE_PICKUP : {
		djDebug()<<"UPGRADE2_GAMETRACE_PICKUP";

		QList<unsigned char> &cards	= m_undealedCards[gameTraceSeat];
		cards.clear();
		for ( quint8 i = 0; i < bufLen; i++ ) {
			cards.append( buf[i] );
		}
		djDebug()<<"timeline state"<<m_timeline->state();
		if ( QTimeLine::Running != m_timeline->state() ) {
			djDebug()<<"start timeline";
			m_timeline->setDuration( 500 * bufLen );
			m_timeline->setFrameRange( 0, bufLen );
			m_timeline->setCurrentTime(0);
			m_timeline->start();
		}
		break;
	}
	case UPGRADE_GAMETRACE_THROW : {
		djDebug()<<"UPGRADE_GAMETRACE_THROW";
		
		if ( QTimeLine::Running == m_timeline->state() ) {
			dealCards( m_timeline->endFrame() );
		}
		
		for ( quint8 i = 0; i < bufLen; i++ ) {
			DJDesktopItem *item	= takeDesktopItem( gameTraceSeat, POKER_TYPE_HAND, buf[i] );
			delete item;
			appendDesktopItem( gameTraceSeat, POKER_TYPE_THROWN, buf[i] );
		}
		if( 0 == m_seatOfFirstThrow ) {
			m_seatOfFirstThrow	= gameTraceSeat;
			m_numberOfFirstThrownCards	= bufLen;
			if( 0 == m_current.chMaster ) {
				m_current.chMaster = gameTraceSeat;
				repaintZhuang();
			}
		}
		break;
	}
	case UPGRADE2_GAMETRACE_CLEARDESKTOP : {
		djDebug() << "UPGRADE2_GAMETRACE_CLEARDESKTOP";
					
		for ( int seat = 0; seat <= panelController()->numberOfSeats(); ++seat) {
			clearDesktopItems(seat,POKER_TYPE_LASTROUND_THROWN);
			moveDesktopItems( seat, POKER_TYPE_THROWN, POKER_TYPE_LASTROUND_THROWN );
			hideDesktopItems(seat,POKER_TYPE_LASTROUND_THROWN);
		}
		
		m_seatOfFirstThrow	= 0;
		m_numberOfFirstThrownCards	= 0;
		
		m_previous->setEnabled(true);
		break;
	}
	case UPGRADE_GAMETRACE_MASTER : {
		djDebug() << "UPGRADE_GAMETRACE_MASTER";
		
		m_masterSeat	= gameTraceSeat;
		
		if( bufLen > 0 ) {
			quint8 seat;
			if( m_current.chMaster%2 == panelController()->seatId()%2 )
				seat = (panelController()->seatId()==1)?4:panelController()->seatId()-1;
			else
				seat = panelController()->seatId();
			for ( quint8 i = 0; i < bufLen; i++ ) {
				appendDesktopItem( seat, POKER_TYPE_ATTACHED, buf[i] );
			}
			for(int i = 0; i < bufLen; i++ ) {
				if(DJGAME_POKER_VALUE(buf[i]) == 5)
					m_currentScore += 5;
				else
					m_currentScore += 10;
			}
		}
		break;
	}
	case UPGRADE_GAMETRACE_SHOW : {
		djDebug() << "UPGRADE_GAMETRACE_SHOW";
		
		//buf[0] == 0 means show over
		if ( buf[0] > 0 ) {
			quint8 card	= (quint8)DJGAME_POKER_CARD(buf[0]);
			djDebug()<<"card="<<hex<<card;
			m_showedPoker	= DJPoker( card, isBridgeCard());
			m_seatOfShow	= gameTraceSeat;
			m_numberOfShowedPokers	= (buf[0]>>6)+1;
			djDebug()<<"m_showedPoker"<<m_showedPoker.description();
			djDebug()<<"m_numberOfShowedPokers"<<m_numberOfShowedPokers<<"m_seatOfShow"<<m_seatOfShow;
			
			clearDesktopItems( m_seatOfShow, POKER_TYPE_THROWN );
			for ( quint8 i = 0; i < m_numberOfShowedPokers; i++ ) {
				appendDesktopItem( m_seatOfShow, POKER_TYPE_THROWN, card );
			}
			checkShowStatus();
		}else if ( gameTraceSeat == panelController()->seatId() ) {
			m_showIsOver	= true;
			for( int i = 0; i < NUMBER_OF_SHOW_BUTTONS; i++ ) {
				m_showButtons[i]->setEnabled(false);
				m_checkShowResult[i]	= 0;
			}
		}
		break;
	}
	case UPGRADE2_GAMETRACE_HANDS : {
		djDebug() << "UPGRADE2_GAMETRACE_HANDS";
			
		if ( QTimeLine::Running == m_timeline->state() ) {
			dealCards( m_timeline->endFrame() );
		}
		
		for ( quint8 i = 0; i < bufLen; i++ ) {
			appendDesktopItem( gameTraceSeat, POKER_TYPE_HAND, buf[i] );
		}
		break;
	}
	case UPGRADE3_GAMETRACE_SUBSTRACT : {
		djDebug() << "UPGRADE3_GAMETRACE_SUBSTRACT";
			
		if ( QTimeLine::Running == m_timeline->state() ) {
			dealCards( m_timeline->endFrame() );
		}
		
		clearDesktopItems( gameTraceSeat, POKER_TYPE_SUBTRACT );
			
		for ( quint8 i = 0; i < bufLen; i++ ) {
			DJDesktopItem *item	= takeDesktopItem( gameTraceSeat, POKER_TYPE_HAND, buf[i] );
			delete item;
			appendDesktopItem( gameTraceSeat, POKER_TYPE_SUBTRACT, buf[i] );
		}
		m_subtract->setEnabled(true);
		
		if( 0 == m_current.chMaster ) {
			m_current.chMaster = gameTraceSeat;
			repaintZhuang();
		}
		break;
	}
	case UPGRADE_GAMETRACE_GAMEOVER : {
		djDebug()<<"UPGRADE_GAMETRACE_GAMEOVER";
		
		clearDesktopItems( m_current.chMaster, POKER_TYPE_HAND );
		for ( quint8 i = 0; i < bufLen - 1; i++ ) {
			appendDesktopItem( m_current.chMaster, POKER_TYPE_HAND, buf[i+1] );
		}
		break;
	}
	default:
		break;
	}
}
void CDPDesktopController::gameTraceView( const GeneralGameTrace2Head *gameTrace )
{
	djDebug()<<"CDPDesktopController::gameTraceView";
	
	quint8 gameTraceSeat	= gameTrace->chSite;
	quint8 bufLen	= gameTrace->chBufLen;
	const unsigned char *buf	= gameTrace->chBuf;
	
	switch( gameTrace->chType ) {
	case UPGRADE2_GAMETRACE_RESET :
	case UPGRADE2_GAMETRACE_PICKUP : {
		djDebug()<<"UPGRADE2_GAMETRACE_PICKUP";
			
		repaintHandPokers( gameTraceSeat, POKER_TYPE_HAND, true, true, true );
		break;
	}
	case UPGRADE_GAMETRACE_THROW: {
		djDebug()<<"UPGRADE_GAMETRACE_THROW";
			
		repaintThrownPokers( gameTraceSeat, POKER_TYPE_THROWN );
		repaintHandPokers( gameTraceSeat, POKER_TYPE_HAND, false, true, false );
			
		playWave("out.wav");
		break;
	}
	case UPGRADE2_GAMETRACE_CLEARDESKTOP : {
		djDebug()<<"UPGRADE2_GAMETRACE_CLEARDESKTOP";
		break;
	}
	case UPGRADE_GAMETRACE_MASTER : {
		djDebug()<<"UPGRADE_GAMETRACE_MASTER";
		if( bufLen > 0) {
			quint8 seat;
			if(m_current.chMaster%2 == panelController()->seatId()%2) {
				seat = (panelController()->seatId()==1)?4:panelController()->seatId()-1;
			}else {
				seat = panelController()->seatId();
			}
			repaintAttachedPokers( seat, POKER_TYPE_ATTACHED );
			repaintScore();
		}
		break;
	}
	case UPGRADE_GAMETRACE_SHOW : {
		djDebug()<<"UPGRADE_GAMETRACE_SHOW";
		if ( buf[0] > 0 ) {
			repaintShowedSuit();
			repaintThrownPokers( gameTraceSeat, POKER_TYPE_THROWN );
			repaintHandPokers( gameTraceSeat, POKER_TYPE_HAND, false, true, true );
			repaintHandPokers( panelController()->seatId(), POKER_TYPE_HAND, false, true, true );
		}
		break;
	}
	case UPGRADE2_GAMETRACE_HANDS : {
		djDebug()<<"UPGRADE2_GAMETRACE_HANDS";
		repaintHandPokers( gameTraceSeat, POKER_TYPE_HAND, false, true, true );
		break;
	}
	case UPGRADE3_GAMETRACE_SUBSTRACT : {
		djDebug()<<"UPGRADE3_GAMETRACE_SUBSTRACT";
		repaintHandPokers( gameTraceSeat, POKER_TYPE_HAND, false, true, false );
		break;
	}
	case UPGRADE_GAMETRACE_GAMEOVER : {
		djDebug()<<"UPGRADE_GAMETRACE_GAMEOVER";
			
		m_actionContainer->hide();
		m_showContainer->hide();
		
		quint8 punishMultiple = buf[0];
		quint8 coveredScore = 0;
		for ( quint8 i = 0; i < bufLen - 1; i++ ) {
			if( 5 == DJGAME_POKER_VALUE(buf[i+1]) )
				coveredScore += 5;
			else if( 10 == DJGAME_POKER_VALUE(buf[i+1]) || DJGAME_POKER_K == DJGAME_POKER_VALUE(buf[i+1]) )
				coveredScore += 10;
		}
		repaintHandPokers( m_current.chMaster, POKER_TYPE_HAND, true, true, true );
		//QString text	= QString("%1 + %2 x %3 = %4").arg(m_currentScore).arg(coveredScore).arg(punishMultiple).arg(m_currentScore+coveredScore*punishMultiple);
		QString baseScoreText	= QString::number(m_currentScore);
		QString punishScoreText	= QString("%1 x %2").arg(coveredScore).arg(punishMultiple);
		punishScoreText.prepend("<font color=#008000 size=5>");
		punishScoreText.append("</font>");
		int finalScore	= m_currentScore+coveredScore*punishMultiple;
		QString finalScoreText	= QString::number( finalScore );
		PUpgradeRoom pugroom = (PUpgradeRoom)(panelController()->gameRoom()->privateRoom());
		int scoreOfVictory	= letoh2(pugroom->shTeamScore);
		if ( finalScore < scoreOfVictory ) 
			finalScoreText.prepend("<font size=6>");
		else
			finalScoreText.prepend("<font color=#ff0000 size=5>");
		finalScoreText.append("</font>");
		QString text	= baseScoreText + " + " + punishScoreText + " = " + finalScoreText;
		m_resultText->setHtml( text );
		m_resultText->adjustPos( desktop()->graphicsMatrix() );
		m_resultText->show();
		break;
	}
	default:
		break;
	}
}
void CDPDesktopController::initUnderGameInfo( const QByteArray& buf )
{
	djDebug()<<"CDPDesktopController::initUnderGameInfo";
	
	DJDesktopPokerController::initUnderGameInfo( buf );
		
	memcpy(&m_current,buf.data(),sizeof(UpgradeCurrent));
	
	m_levelPoker	= DJPoker( (int)DJPoker::AnySuit, (DJPoker::Point)m_current.chLevel );
	m_showedPoker	= DJPoker();
	
	m_numberOfShowedPokers	= 0;
	m_seatOfShow	= 0;
	m_currentScore	= 0;
	
	m_seatOfFirstThrow	= 0;
	m_numberOfFirstThrownCards	= 0;
	m_masterSeat	= 0;
	m_showIsOver	= false;
	
	for( int seat = 0; seat <= panelController()->numberOfSeats(); ++seat ) {
		QList<unsigned char> &cards	= m_undealedCards[seat];
		cards.clear();	
	}
	fillPlayerInfo();
	repaintPlayerInfo();
	repaintShowedSuit();
	repaintZhuang();
	repaintScore();
	m_resultText->hide();
	
	m_previous->setEnabled(false);
	m_subtract->setEnabled(false);
	
	m_series	= gSeries;
	m_series.removeAll( (DJPoker::Point)m_current.chLevel );
}
void CDPDesktopController::locateAll()
{
	DJDesktopPokerController::locateAll();
	locateSuitPositions( m_suitPoints, m_suitAligns );
	
	QPoint center	= centerPoint();
	m_resultText->setVirtualPos( center );
	m_resultText->setAlignment( Qt::AlignCenter );
}
void CDPDesktopController::locateSuitPositions( QList<QPoint>& points, QList<Qt::Alignment>& aligns )
{
	djDebug() << "DJDesktopController::locateSuitPositions";
	
	points.clear();
	aligns.clear();
	
	QList<quint8> seats	= seatIdsOfPlayingUsers();
	int players	= seats.size();
		
	for ( int i = 0; i <= panelController()->numberOfSeats(); ++i ) {
		points 	<< QPoint();
		aligns	<< 0;
	}
	
	foreach( quint8 seat, seats ) {
     	QRect infoRect	= rectOfSeatInfo( seat );
     	quint8 view = seat2View(seat);
    	if ( 3 == players  || 4 == players  ) {
    		switch( view ) {
        	case 1: {
        		points[seat].rx()	= infoRect.right() + 10;
        		points[seat].ry()	= infoRect.center().y();
        		aligns[seat]	= Qt::AlignVCenter;
    			break;
        	} 
        	case 2:  {
        		points[seat]	= infoRect.bottomRight();
    			aligns[seat]	= Qt::AlignRight;
        		break;
        	}
        	case 3:  {
        		points[seat]	= infoRect.bottomRight();
    			aligns[seat]	= Qt::AlignRight;
        		break;
        	}
        	case 4:  {
        		points[seat]	= infoRect.bottomLeft();
        		break;
        	}
        	default:
        		break;
          	}
    	}else if ( 2 == players ) {
    		/*
    		if ( 1 == view ) {
        		points[seat].setX( showRect.left() - 1 );
        		points[seat].setY( showRect.bottom() - 1 );
        		aligns[seat]	= Qt::AlignRight | Qt::AlignBottom;
        	}else {
        		points[seat].setX( showRect.left() - 1 );
        		points[seat].setY( showRect.top() + 1 );
        		aligns[seat]	= Qt::AlignRight;
        	}
        	*/
    	}else if ( 1 == players ) { 
    		//points[seat].setX( showRect.left() );
            //points[seat].setY( showRect.bottom() );
		}
    }
}
void CDPDesktopController::locateAllNonMatrimoveItems()
{
	DJDesktopPokerController::locateAllNonMatrimoveItems();
		
	QMatrix m	= desktop()->graphicsMatrix();
	QRect pool	= m.mapRect( poolSpace() );
	
	m_showContainer->move( pool.right() - m_showContainer->width(), pool.bottom() - m_showContainer->height() );
	m_actionContainer->move( pool.right() - m_actionContainer->width(), pool.bottom() - m_actionContainer->height() );
	
	int x = 0;
	int y = 0;
	
	m_zhuangText->setVirtualPos( QPoint(x,y) );
	m_zhuangText->adjustPos( desktop()->graphicsMatrix() );
	
	y	+= m_zhuangText->boundingRect().toRect().height();
	m_scoreText->setVirtualPos( QPoint(x,y) );
	m_scoreText->adjustPos( desktop()->graphicsMatrix() );
}
void CDPDesktopController::repaintAll()
{
	djDebug()<<"CDPDesktopController::repaintAll";
	
	DJDesktopPokerController::repaintAll();
		
	repaintShowedSuit();
	repaintZhuang();
	repaintScore();
}
void CDPDesktopController::repaintHost()
{
	quint8 masterSeat	= m_current.chMaster;
	if ( 0 != masterSeat ) {
		repaintHostItem( masterSeat );	
	}
}
void CDPDesktopController::fillPlayerInfo()
{
	djDebug()<<"repaintInfo"<<m_current.chLevels[0]<<m_current.chLevels[1];
	QStringList levels;
	levels <<"0"<<"A"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"10"<<"J"<<"Q"<<"K";

	quint8 seat	= panelController()->seatId();
	bool	isConfederate	= true;
	for( int i = 1; i <= panelController()->numberOfSeats(); ++i ) {
		djDebug()<<"seat"<<seat<<"isConfederate"<<isConfederate;
		DJGraphicsTextItem *info	= playerInfoItem(seat);
		int index;
		if ( 0 == seat % 2 )
			index	= 0;
		else
			index	= 1;
		QString level	= levels.value(m_current.chLevels[index],0);
		info->setPlainText( tr("level:%1").arg(level) );
		seat	= nextPlayerSeatId( seat );
		isConfederate	= !isConfederate;
	}
}
/*
void CDPDesktopController::repaintPlayerInfo()
{
	for( quint8 seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
		repaintPlayerInfoItem( seat );
	}
}
*/
void CDPDesktopController::repaintShowedSuit()
{
	djDebug()<<"CDPDesktopController::repaintShowedSuit";
	djDebug()<<"m_showedPoker"<<m_showedPoker.description();
	djDebug()<<"m_numberOfShowedPokers"<<m_numberOfShowedPokers<<"m_seatOfShow"<<m_seatOfShow;
	
	QPixmap	pix;	
	if ( m_showedPoker.isBigJoker() ) {
		pix	= QPixmap(":/BaseRes/image/desktop/toolbutton/ntred.png");
	}else if ( m_showedPoker.isSmallJoker() ) {
		pix	= QPixmap(":/BaseRes/image/desktop/toolbutton/ntgray.png");
	}else {
		switch(m_showedPoker.suit()) {
		case DJPoker::Diamond:
			pix	= QPixmap(":/BaseRes/image/desktop/toolbutton/d.png");
			break;
		case DJPoker::Club:
			pix	= QPixmap(":/BaseRes/image/desktop/toolbutton/c.png");
			break;
		case DJPoker::Heart:
			pix	= QPixmap(":/BaseRes/image/desktop/toolbutton/h.png");
			break;
		case DJPoker::Spade:
			pix	= QPixmap(":/BaseRes/image/desktop/toolbutton/s.png");
			break;
		default:
			break;
		}
	}
	QList<QPixmap> pixmaps;
	for ( int i = 0; i < m_numberOfShowedPokers; i++ ) {
		pixmaps << pix;
	}
	QPixmap conjoinedPix	= CreateConjointPixmap( pixmaps );
	
	for ( quint8 seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
		DJGraphicsPixmapItem *suitItem	= m_suitItems[seat];
		if ( seat == m_seatOfShow ) {
			suitItem->setPixmap( conjoinedPix );
			repaintShowedSuitItem( seat );
		}else {
			suitItem->hide();
		}
	}
}
void CDPDesktopController::repaintShowedSuitItem( quint8 seat )
{
	djDebug()<<"CDPDesktopController::repaintShowedSuitItem"<<seat;
	
	DJGraphicsPixmapItem	*suitItem	= m_suitItems.value( seat, 0 );
	if ( 0 == suitItem )
		return;		
	QPoint	pos	= m_suitPoints.value( seat, QPoint() );
	if ( pos.isNull() ) {
		suitItem->hide();
	}else {
		suitItem->setVirtualPos( pos );
		Qt::Alignment	align	= m_suitAligns.value( seat, 0 );
		suitItem->setAlignment( align );
		suitItem->adjustPos( desktop()->graphicsMatrix() );
		suitItem->show();
	}
}
void CDPDesktopController::repaintCurrentStatus()
{
	djDebug()<<"CDPDesktopController::repaintCurrentStatus";
	repaintAll();
		
	quint8 seat;
	for( seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
		repaintHandPokers( seat, POKER_TYPE_HAND, true, true, true );
		repaintThrownPokers( seat, POKER_TYPE_THROWN );
	}
	if(m_current.chMaster%2 == panelController()->seatId()%2)
	{
		seat = (panelController()->seatId()==1)?4:panelController()->seatId()-1;
	}else
		seat = panelController()->seatId();
	repaintAttachedPokers( seat, POKER_TYPE_ATTACHED );
	checkShowStatus();	
}
int CDPDesktopController::rankOnSuit( const DJPoker& poker ) const
{
	//djDebug()<<"CDPDesktopController::rankOnSuit";
	//djDebug()<<"poker"<<poker.description();
	if ( !poker.isValid() )
		return 0;
	//djDebug()<<"m_showedPoker"<<m_showedPoker.description();
	int rank	= DJDesktopPokerController::rankOnSuit(poker);
	
	//djDebug()<<"rank"<<hex<<rank;
	//DJPoker levelPoker( (int)DJPoker::AnySuit, (DJPoker::Point)m_current.chLevel );
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
int CDPDesktopController::distanceBetweenPokers( const DJPoker& poker1, const DJPoker& poker2 ) const
{
	if ( isSameClass(poker1,poker2) )
		return 1;
	else
		return 2;
}
bool CDPDesktopController::isSameClass( const DJPoker& poker1, const DJPoker& poker2 ) const
{
	return classOfPoker(poker1) == classOfPoker(poker2); 
}
bool CDPDesktopController::isTrump( const DJPoker& poker ) const
{
	int rank	= rankOnSuit( poker );
	if ( (rank & DJPoker::SuitTrump) || (rank & DJPoker::ConstTrump) )
		return true;
	else
		return false;
}
CDPDesktopController::PokerClass CDPDesktopController::classOfPoker( const DJPoker& poker ) const
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

void CDPDesktopController::handleFrameChanged( int frame )
{
	djDebug()<<"CDPDesktopController::handleFrameChanged"<<frame;
	
	dealCards( frame );
}
void CDPDesktopController::dealCards( int numberOfShouldHaveDealedCards )
{
	djDebug()<<"CDPDesktopController::dealCards"<<numberOfShouldHaveDealedCards;
	
	int numberOfTotalCards	= m_timeline->endFrame();
	for( int seat = 1; seat <= panelController()->numberOfSeats(); seat++ ) {
		int numberOfUndealedCards	= m_undealedCards[seat].size();
		int numberOfDealedCards	= numberOfTotalCards - numberOfUndealedCards;
		int numberOfShouldDealCards	= numberOfShouldHaveDealedCards - numberOfDealedCards;
		if ( numberOfUndealedCards > 0 && numberOfShouldDealCards > 0 ) {
			int delta	= qMin(numberOfUndealedCards,numberOfShouldDealCards);
			djDebug()<<"delta"<<delta;
			while( delta-- ) {
				appendDesktopItem( seat, POKER_TYPE_HAND, m_undealedCards[seat].takeFirst() );
			}
			if ( seat == panelController()->seatId() ) {
				checkShowStatus();
			}
			repaintHandPokers( seat, POKER_TYPE_HAND, false, true, true );
		}
	}
	if ( numberOfShouldHaveDealedCards == numberOfTotalCards ) {
		//m_timeline->setPaused( true );
		m_timeline->stop();
	}
}
void CDPDesktopController::repaintZhuang()
{
	djDebug()<<"CDPDesktopController::repaintZhuang";
	
	repaintHost();
	
	QString zhuang	= tr("Undetermined");
	quint8 seatOfZhuang	= m_current.chMaster;
	if( 0 != seatOfZhuang ) {
		DJGameUser *player = panelController()->player( seatOfZhuang );
		if ( player )
			zhuang	= player->userName();
		for ( quint8 seat = 1; seat <= panelController()->numberOfSeats(); seat++ ) {
			DJGraphicsTextItem *nameItem	= playerNameItem(seat);
			if ( nameItem ) {
				if ( seat == seatOfZhuang ) {
					nameItem->setDefaultTextColor( QColor(0x80,0,0) );
				}else {
					nameItem->setDefaultTextColor( Qt::black );
				}
			}
		}
	}
	zhuang.prepend("<font color=#800000>");
	zhuang.append("</font>");
	zhuang.prepend( tr("Master:") );
	m_zhuangText->setHtml( zhuang );
	
	panelController()->repaintAllPlayerItems();
}
void CDPDesktopController::repaintScore()
{
	djDebug()<<"CDPDesktopController::repaintScore";
	
	QString score	= QString::number(m_currentScore);
	
	PUpgradeRoom pugroom = (PUpgradeRoom)(panelController()->gameRoom()->privateRoom());
	int scoreOfVictory	= letoh2(pugroom->shTeamScore);
	if ( m_currentScore >= scoreOfVictory ) {
		score.prepend("<font color=#ff0000>");
		score.append("</font>");
	}
	score.prepend( tr("Score:") );
	m_scoreText->setHtml( score );
}
QByteArray CDPDesktopController::createThrowTrace( DesktopItems& items )
{	
	djDebug()<<"CDPDesktopController::createThrowTrace";
	
	sortPokers( items );
	
	QList<DJPoker> pokers	=  pokersFromDesktopItems( items );
	QByteArray cards	= cardsFromPokers( pokers );
	
	djDebug()<<"cards size"<<cards.size();
	DebugBuffer( cards );
	
	char buffer[255];
	PGeneralGameTrace2Head ptrace = (PGeneralGameTrace2Head)buffer;
	
	ptrace->chTable = panelController()->tableId();
	ptrace->chSite = panelController()->seatId();
	ptrace->chType = 0;
	ptrace->chBufLen = cards.size();
	
	memcpy( ptrace->chBuf, cards.data(), cards.size() );

	if( DJGAME_UPGRADE_TABLE_STATUS_WAIT_OUT == gameWaitStatus() ) {
		if ( isFollowing() ) {
			//is following
			if( m_numberOfFirstThrownCards != ptrace->chBufLen ) {
				djDebug() << "pages error";
				DJMessageBox::information(10,panelController()->panel(),tr("throw card failed"),tr("You must throw %1 cards").arg(m_numberOfFirstThrownCards),QMessageBox::Ok);
				return QByteArray();
			}
			
		}else {
			//is first throw
			for( int i = 1; i < pokers.size() ; i++ ) {
				if( !isSameClass(pokers.at(i-1), pokers.at(i)) ) {
					djDebug() << "multi class error";
					DJMessageBox::information(10,panelController()->panel(),tr("throw card failed"),tr("You must throw cards with the same suit"),QMessageBox::Ok);
					return QByteArray();
				}
			}
		}
		ptrace->chType = UPGRADE_GAMETRACE_THROW;
	}else if( DJGAME_UPGRADE_TABLE_STATUS_WAIT_HANDS == gameWaitStatus() ) {
		PUpgradeRoom pugroom = (PUpgradeRoom)(panelController()->gameRoom()->privateRoom());
		if( ptrace->chBufLen != pugroom->chHands ) {
			djDebug() << "discard card error";
			DJMessageBox::information(10,panelController()->panel(),tr("discard cards failed"),tr("You must discard %1 cards").arg(pugroom->chHands),QMessageBox::Ok);
			return QByteArray();
		}
		ptrace->chType = UPGRADE2_GAMETRACE_HANDS;
	}
	return QByteArray( buffer, sizeof(GeneralGameTrace2Head) + ptrace->chBufLen );
}
bool CDPDesktopController::isChuEnabled() const
{
	djDebug()<<"CDPDesktopController::isChuEnabled"<<m_throw->isEnabled()<<isWaitingThrowAcl();
	return m_throw->isEnabled() && !isWaitingThrowAcl();
}
void CDPDesktopController::checkShowStatus()
{
	djDebug()<<"CDPDesktopController::checkShowStatus";
	
	for( int i = 0; i < NUMBER_OF_SHOW_BUTTONS; i++ ) {
		m_showButtons[i]->setEnabled(false);
		m_checkShowResult[i]	= 0;
	}
	if( DJGAME_UPGRADE_TABLE_STATUS_WAIT_SHOW != gameWaitStatus()
		|| !isWaitingForMe()
		|| m_showIsOver
		|| panelController()->isLookingOn() )
		return;
		
	DesktopItems &items	= desktopItems( panelController()->seatId(), POKER_TYPE_HAND );
	QList<DJPoker> handPokers	= pokersFromDesktopItems( items );
	QHash<DJPoker,int> countPokers	= createCountPokers( handPokers );
	
	//test SHCD
	for ( int suit = DJPoker::Diamond; suit <= DJPoker::Spade; suit = suit << 1 ) {
		DJPoker poker( suit, m_levelPoker.point() );
		int numberOfLevelSuitPokers	= countPokers.value( poker );
		//djDebug()<<"m_numberOfShowedPokers"<<m_numberOfShowedPokers<<"numberOfLevelSuitPokers"<<numberOfLevelSuitPokers;
		//djDebug()<<"suit"<<suit<<"m_showedPoker.suit()"<<m_showedPoker.suit();
		//djDebug()<<"m_seatOfShow"<<m_seatOfShow<<"panelController()->seatId()"<<panelController()->seatId();
		//djDebug()<<"poker"<<poker.description()<<"m_showedPoker"<<m_showedPoker.description();
   		if ( numberOfLevelSuitPokers > m_numberOfShowedPokers ) {
   			int index	= m_suit2ShowButton.value(suit);
			if ( m_seatOfShow == panelController()->seatId() ) {
				//reinforce self
				if ( m_showedPoker.suit() == suit ) {
					m_showButtons[index]->setEnabled(true);
					m_checkShowResult[index]	= makeShowCard(numberOfLevelSuitPokers,poker);
				}
			}else {
				//against other
				m_showButtons[m_suit2ShowButton.value(suit)]->setEnabled(true);
				m_checkShowResult[index]	= makeShowCard(numberOfLevelSuitPokers,poker);
			}
		}else if ( m_numberOfShowedPokers >= 2
				&& numberOfLevelSuitPokers == m_numberOfShowedPokers
				&& suit > m_showedPoker.suit()
				&& m_seatOfShow != panelController()->seatId() ) {
			int index	= m_suit2ShowButton.value(suit);
			m_showButtons[m_suit2ShowButton.value(suit)]->setEnabled(true);
			m_checkShowResult[index]	= makeShowCard(numberOfLevelSuitPokers,poker);
		}
	}
	//test Joker
	int index	= m_suit2ShowButton.value(DJPoker::Joker);
	DJPoker smallJoker( (int)DJPoker::Joker, DJPoker::SmallJoker );
	int numberOfSmallJoker	= countPokers.value( smallJoker );
	if ( numberOfSmallJoker >= 2
		&& numberOfSmallJoker >= m_numberOfShowedPokers ) {
		if ( m_seatOfShow == panelController()->seatId() ) {
			//reinforce self
			if ( m_showedPoker.isSmallJoker()
				&& numberOfSmallJoker > m_numberOfShowedPokers ) {
				m_showButtons[index]->setEnabled(true);
				m_checkShowResult[index]	= makeShowCard(numberOfSmallJoker, smallJoker );
				return;
			}
		}else {
			//againgst other
			if ( !m_showedPoker.isSmallJoker() 
				&& !m_showedPoker.isBigJoker() ) {
				m_showButtons[index]->setEnabled(true);
				m_checkShowResult[index]	= makeShowCard(numberOfSmallJoker, smallJoker );
				return;
			}
			if ( numberOfSmallJoker > m_numberOfShowedPokers ) {
				m_showButtons[index]->setEnabled(true);
				m_checkShowResult[index]	= makeShowCard(numberOfSmallJoker, smallJoker );
				return;
			} 
		}
	}
	DJPoker bigJoker( (int)DJPoker::Joker, DJPoker::BigJoker );
	int numberOfBigJoker	= countPokers.value( bigJoker );
	if ( numberOfBigJoker >= 2
		&& numberOfBigJoker >= m_numberOfShowedPokers ) {
		if ( m_seatOfShow == panelController()->seatId() ) {
			//reinforce self
			if ( m_showedPoker.isBigJoker()
				&& numberOfBigJoker > m_numberOfShowedPokers ) {
				m_showButtons[index]->setEnabled(true);
				m_checkShowResult[index]	= makeShowCard(numberOfBigJoker, bigJoker );
				return;
			}
		}else {
			//againgst other
			if ( !m_showedPoker.isBigJoker() ) {
				m_showButtons[index]->setEnabled(true);
				m_checkShowResult[index]	= makeShowCard(numberOfBigJoker, bigJoker );
				return;
			}
			if ( numberOfBigJoker > m_numberOfShowedPokers ) {
				m_showButtons[index]->setEnabled(true);
				m_checkShowResult[index]	= makeShowCard(numberOfBigJoker, bigJoker );
				return;
			}
		}
	}
}
quint8 CDPDesktopController::makeShowCard( int numberOfShowPokers, const DJPoker& poker )
{
	if ( 0 == numberOfShowPokers )
		return 0;
	return ((numberOfShowPokers - 1) << 6) | poker.toByte(isBridgeCard());
}
bool CDPDesktopController::isFollowing() const
{
	if ( 0 == m_masterSeat ) {
		return 0 != m_seatOfFirstThrow;
	}else {
		return m_masterSeat != panelController()->seatId();
	} 
}
void CDPDesktopController::showNT()
{
	int index	= m_suit2ShowButton.value(DJPoker::Joker);
	quint8 card	= m_checkShowResult[index];
	djDebug()<<"showNT"<<hex<<card;
	if ( 0 != card ) {
		QByteArray buf;
		buf.append( card );
		sendGameTrace(UPGRADE_GAMETRACE_SHOW,buf);
	}
}
void CDPDesktopController::showSpade()
{
   	int index	= m_suit2ShowButton.value(DJPoker::Spade);
	quint8 card	= m_checkShowResult[index];
	djDebug()<<"showSpade"<<hex<<card;
	if ( 0 != card ) {
		QByteArray buf;
		buf.append( card );
		sendGameTrace(UPGRADE_GAMETRACE_SHOW,buf);
	}
}
void CDPDesktopController::showHeart()
{
	int index	= m_suit2ShowButton.value(DJPoker::Heart);
	quint8 card	= m_checkShowResult[index];
	djDebug()<<"showHeart"<<hex<<card;
	if ( 0 != card ) {
		QByteArray buf;
		buf.append( card );
		sendGameTrace(UPGRADE_GAMETRACE_SHOW,buf);
	}
}
void CDPDesktopController::showClub()
{
	int index	= m_suit2ShowButton.value(DJPoker::Club);
	quint8 card	= m_checkShowResult[index];
	djDebug()<<"showClub"<<hex<<card;
	if ( 0 != card ) {
		QByteArray buf;
		buf.append( card );
		sendGameTrace(UPGRADE_GAMETRACE_SHOW,buf);
	}
}
void CDPDesktopController::showDiamond()
{
	int index	= m_suit2ShowButton.value(DJPoker::Diamond);
	quint8 card	= m_checkShowResult[index];
	djDebug()<<"showDiamond"<<hex<<card;
	if ( 0 != card ) {
		QByteArray buf;
		buf.append( card );
		sendGameTrace(UPGRADE_GAMETRACE_SHOW,buf);
	}
}
QList<DJPoker> CDPDesktopController::handPokersOnClass( int pokerClass, QList<DJPoker>& nonSameClassPokers )
{
	QList<DJPoker> sameClassPokers;
	nonSameClassPokers.clear();
	
	DesktopItems &items	= desktopItems( panelController()->seatId(), POKER_TYPE_HAND );
	QList<DJPoker> handPokers	= pokersFromDesktopItems(items);
	foreach( DJPoker poker, handPokers ) {
		if ( classOfPoker(poker) == pokerClass )
			sameClassPokers << poker;
		else
			nonSameClassPokers << poker;			
	}
	return sameClassPokers;
}
int CDPDesktopController::findMatchingWidth( QList<DJPokerPattern>& patterns, int width )
{
	qSort( patterns.begin(), patterns.end(), qGreater<DJPokerPattern>() );
	
	for ( QList<DJPokerPattern>::iterator it = patterns.begin(); it != patterns.end(); ++it ) {
		DJPokerPattern& pattern	= *(it);
		if ( 0 == pattern.width() )
			return 0;
		if ( pattern.width() < width ) {
			return pattern.width();
		}
	}
	return 0;
}
QList<DJPokerPattern>::iterator CDPDesktopController::findMatchingPattern( QList<DJPokerPattern>& patterns,
														 int width, int length, int mode )
{
	int minCha	= 100;
	
	QList<DJPokerPattern>::iterator matchingIt	= patterns.end();
	for ( QList<DJPokerPattern>::iterator it = patterns.begin(); it != patterns.end(); ++it ) {
		DJPokerPattern& pattern	= *(it);
		if ( pattern.width() == width ) {
			if ( pattern.length() == length && 0 == mode ) {
				return it;
			}else if ( pattern.length() > length && mode > 0 ) {
				int cha	= pattern.length() - length;
				if ( cha < minCha ) {
					minCha	= cha;
					matchingIt	= it;
				}
			}else if ( pattern.length() < length && mode < 0 ) {
				int cha	= length - pattern.length();
				if ( cha < minCha ) {
					minCha	= cha;
					matchingIt	= it;
				}
			}
		}
	}
	return matchingIt;
}
DJPoker CDPDesktopController::higherSerialPoker( const DJPoker& poker, bool isStrict ) const
{
	DJPoker::Point point	= poker.point();
	int	suit	= poker.suit();
	int index	= m_series.indexOf( point );
	if ( -1 != index ) {
		//normal poker
		if ( index + 1 == m_series.size() ) {
			//the top poker in series
			if ( !m_showedPoker.isValid() )
				return DJPoker( DJPoker::AnySuit, m_levelPoker.point() );
			if ( suit == m_showedPoker.suit() ) {
				//is trump, next is non-trump level pokers
				//suit	= DJPoker::AnySuit;
				//suit	^=	trumpLevelPoker.suit();
				return DJPoker( DJPoker::AnySuit ^ m_showedPoker.suit(), m_levelPoker.point() );
			}
		}else {
			//not top,just return the higher poker in series
			index++;
			point	= m_series.at(index);
			return DJPoker ( suit, point );
		}
	}else {
		//special poker
		if ( point == DJPoker::BigJoker )
			return DJPoker();
		if ( point == DJPoker::SmallJoker )
			return DJPoker( DJPoker::Joker, DJPoker::BigJoker );
		if ( point == m_levelPoker.point() ) {
			//level poker
			if ( !m_showedPoker.isValid() )
				return DJPoker();
			if ( suit == m_showedPoker.suit() ) {
				//trump level poker
				return DJPoker( DJPoker::Joker, DJPoker::SmallJoker );
			}else {
				//non-trump level poker
				return m_showedPoker;
			}
		}
	}
	return DJPoker();
}
DJPoker CDPDesktopController::lowerSerialPoker( const DJPoker& poker, bool isStrict ) const
{
	DJPoker::Point point	= poker.point();
	int	suit	= poker.suit();
	int index	= m_series.indexOf( point );
	if ( -1 != index ) {
		//normal poker
		if ( 0 != index ) {
			//not bottom,just return the lower poker in series
			index--;
			point	= m_series.at(index);
			return DJPoker ( suit, point );
		}
	}else {
		//special poker
		if ( point == DJPoker::BigJoker )
			return DJPoker( DJPoker::Joker, DJPoker::SmallJoker );
		if ( point == DJPoker::SmallJoker ) {
			if ( m_showedPoker.isValid() )
				return m_showedPoker;
			else
				return DJPoker();
		}
		if ( point == m_levelPoker.point() ) {
			//level poker
			if ( !m_showedPoker.isValid() )
				return DJPoker();
			if ( suit == m_showedPoker.suit() ) {
				//trump level poker
				//suit	= DJPoker::AnySuit;
				//suit	^=	trumpLevelPoker.suit();
				return DJPoker( DJPoker::AnySuit ^ m_showedPoker.suit(), m_levelPoker.point() );
			}else {
				//non-trump level poker
				return DJPoker( m_showedPoker.suit(), m_series.last() );
			}
		}
	}
	return DJPoker();
}
void CDPDesktopController::clickTip()
{	
	if ( !isFollowing() )
		return;
	
	quint8 otherSeat = m_masterSeat;
	if ( 0 == m_masterSeat )
		otherSeat	= m_seatOfFirstThrow;
	DesktopItems &otherItems	= desktopItems( otherSeat, POKER_TYPE_THROWN );
	
	djDebug()<<"other thrown pokers";
	QList<DJPoker> otherPokers	= pokersFromDesktopItems(otherItems);
	for( int i = 0; i < otherPokers.size(); ++i ) {
		djDebug()<<otherPokers.at(i).description();
	}
	djDebug()<<"other thrown pokers patterns";
	QList<DJPokerPattern> otherPatterns	= DJPokerPattern::fromPokers( this, otherPokers, true );
	foreach( DJPokerPattern otherPattern, otherPatterns ) {
		djDebug()<<otherPattern.description();
	}
	
	int pokerClass	= classOfPoker( otherPokers.first() );
	djDebug()<<"other thrown pokers class"<<pokerClass;
	
	djDebug()<<"self suit pokers";
	QList<DJPoker> nonSameClassPokers;
	QList<DJPoker> sameClassPokers	= handPokersOnClass( pokerClass, nonSameClassPokers );
	foreach( DJPoker poker, sameClassPokers ) {
		djDebug()<<poker.description();
	}
	djDebug()<<"other suit pokers"<<nonSameClassPokers.size();
	
	djDebug()<<"self hand pokers patterns";
	QList<DJPokerPattern> selfPatterns	= DJPokerPattern::fromPokers( this, sameClassPokers, true );
	foreach( DJPokerPattern selfPattern, selfPatterns ) {
		djDebug()<<selfPattern.description();
	}
	
	QList<DJPoker> tipPokers;
	while( !otherPatterns.isEmpty() ) {
		djDebug()<<"otherPatterns is not empty"<<otherPatterns.size();
		qSort( otherPatterns.begin(), otherPatterns.end(), qGreater<DJPokerPattern>() );
		
		QList<DJPokerPattern>::iterator otherIt	= otherPatterns.begin();
		while ( otherIt != otherPatterns.end() ) {
			DJPokerPattern& otherPattern	= *otherIt;
			djDebug()<<"take an other pattern"<<otherPattern.width()<<otherPattern.length();
			QList<DJPokerPattern>::iterator selfIt	= findMatchingPattern( selfPatterns, 
										otherPattern.width(), otherPattern.length(), 0 );
			if ( selfIt != selfPatterns.end() ) {
				//found match mode equal( self length equal to other length )
				DJPokerPattern& selfPattern	= *selfIt;
				djDebug()<<"found match mode equal";
				
				tipPokers << selfPattern.pokers();
				otherPatterns.erase( otherIt );
				selfPatterns.erase( selfIt );
				//search from other patterns begin again
				break;
			}
			
			selfIt	= findMatchingPattern( selfPatterns, otherPattern.width(),
									 otherPattern.length(), 1 );
			if ( selfIt != selfPatterns.end() ) {
				//found match mode greater( self length greater than other length )
				DJPokerPattern& selfPattern	= *selfIt;
				djDebug()<<"found match mode greater"<<selfPattern.length();
								
				tipPokers << selfPattern.takeLength( otherPattern.length() );
				otherPatterns.erase( otherIt );
				//search from other patterns begin again
				break;
			}
			
			selfIt	= findMatchingPattern( selfPatterns, otherPattern.width(),
									 otherPattern.length(), -1 );
			if ( selfIt != selfPatterns.end() ) {
				//found match mode less( self length less than other length )
				DJPokerPattern& selfPattern	= *selfIt;
				djDebug()<<"found match mode less"<<selfPattern.length();
				
				tipPokers << selfPattern.pokers();
				otherPattern.takeLength( selfPattern.length() );
				selfPatterns.erase( selfIt );
				//search from other patterns begin again
				break;
			}
			
			//here: there is no equal width pattern, split other pattern
			//find the biggest width in self patterns ( less than otherPattern.width() )
			djDebug()<<"no equal width";
			int width	= findMatchingWidth( selfPatterns, otherPattern.width() );
			if ( 0 != width ) {
				//split other pattern
				djDebug()<<"split other pattern"<<width<<otherPattern.width();
				DJPokerPattern split1( width, otherPattern.length() );
				DJPokerPattern split2( otherPattern.width() - width, otherPattern.length() );
				otherPatterns.erase( otherIt );
				otherPatterns.append( split1 );
				otherPatterns.append( split2 );
				//search from other patterns begin again
				break;
			}
			//here: no less width than other width
			//split greater width in self patterns
			djDebug()<<"split self greater width";
			QList<DJPoker> skipedPokers;
			foreach( DJPokerPattern pattern, selfPatterns ) {
				skipedPokers << pattern.pokers();
			}
			if ( tipPokers.size() < otherPokers.size() ) {
				djDebug()<<"not enough less width poker"<<tipPokers.size();
				int cha	= otherPokers.size() - tipPokers.size();
				cha	= qMin( skipedPokers.size(), cha );
				djDebug()<<"cha"<<cha;
				for ( int i = 0; i < cha; ++i ) {
					tipPokers << skipedPokers.value(i,DJPoker());
				}
			}
			otherPatterns.clear();
			break;
		}
	}
	if ( tipPokers.size() < otherPokers.size() ) {
		int cha	= otherPokers.size() - tipPokers.size();
		djDebug()<<"not enough same suit poker"<<cha<<otherPokers.size()<<tipPokers.size();
		for ( int i = 0; i < cha; ++i ) {
			tipPokers << nonSameClassPokers.value(i,DJPoker());
		}
	}
	djDebug()<<"final tip"<<tipPokers.size();
	foreach( DJPoker poker, tipPokers ) {
		djDebug()<<poker.description();
	}
    selectSelfHandPokers( tipPokers );  
}
void CDPDesktopController::clickThrow()
{
	throwSelectedCards();
}
void CDPDesktopController::previousPressed()
{
	for ( int seat = 1; seat <= panelController()->numberOfSeats(); seat++ ) {
		hideDesktopItems( seat, POKER_TYPE_THROWN );
		showDesktopItems( seat, POKER_TYPE_LASTROUND_THROWN );
	}
}
void CDPDesktopController::previousRelesed()
{
	for ( int seat = 1; seat <= panelController()->numberOfSeats(); seat++ ) {
		hideDesktopItems( seat, POKER_TYPE_LASTROUND_THROWN );
		showDesktopItems( seat, POKER_TYPE_THROWN );
	}
}
void CDPDesktopController::subtractPressed()
{
	repaintHandPokers( m_current.chMaster, POKER_TYPE_SUBTRACT, true, true, true );
	hideDesktopItems( m_current.chMaster, POKER_TYPE_HAND );
	showDesktopItems( m_current.chMaster, POKER_TYPE_SUBTRACT );
}
void CDPDesktopController::subtractReleased()
{
	hideDesktopItems( m_current.chMaster, POKER_TYPE_SUBTRACT );
	showDesktopItems( m_current.chMaster, POKER_TYPE_HAND );
}
QString	CDPDesktopController::userItemNameSuffix( DJGameUser* user ) const
{
	djDebug()<<"CDPDesktopController::userItemNameSuffix"<<user->userName()<<user->seatId()<<m_current.chMaster;
	if ( 0 != m_current.chMaster 
		&& user->seatId() == m_current.chMaster ) {
		return QString("(")+tr("zhuang")+QString(")");
	}
	return DJDesktopController::userItemNameSuffix(user);
}
