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
#include "GBMJDesktopController.h"
#include "DJPanelController.h"
#include "DJPanel.h"
#include "DJGameRoom.h"
#include "DJDesktop.h"
#include "DJDesktopItem.h"
#include "DJGraphicsTextItem.h"
#include "DJGraphicsPixmapItem.h"
#include "DJGraphicsSvgItem.h"
#include "DJGameUser.h"
#include "DJMessageBox.h"
#include "Utility.h"
#include "DJDesktopScene.h"
#include "UIGBMJTing.h"
#include "mj2profile.h"

static	qreal	Z_TING			= 2000;
static int TEXT_ITEM_FONT_SIZE		= 	16;

static const char *gWinds[]={
    QT_TRANSLATE_NOOP("GBMJDesktopController","none"),
    QT_TRANSLATE_NOOP("GBMJDesktopController","east"),
    QT_TRANSLATE_NOOP("GBMJDesktopController","south"),
    QT_TRANSLATE_NOOP("GBMJDesktopController","west"),
    QT_TRANSLATE_NOOP("GBMJDesktopController","north")
};

GBMJDesktopController::GBMJDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent )
	: DJDesktopMahjongController(panelController,size,parent)
{
	djDebug()<<"GBMJDesktopController constructor"<<size;
	
	memset(&m_current,0,sizeof(MahjongCurrent));
	
    m_qiangGangCard	= 0;
    m_isTingAutoGang	= false;
    m_isTingOnlyZimoGang	= false;
    m_isTingOnlyZimoHu	= false;
    for( quint8 seat = 0; seat <= panelController->numberOfSeats(); ++seat ) {
		m_isTings << false;
		DJGraphicsPixmapItem	*tingItem	= new DJGraphicsPixmapItem( 0, desktop()->scene() );
		tingItem->setZValue( Z_TING );
		tingItem->hide();
		m_tingItems  << tingItem;
	}
	
	QFont font;
	font.setPointSize( TEXT_ITEM_FONT_SIZE );
	font.setBold( true );
	
	m_zhuangText = new DJGraphicsTextItem( tr("Zhuang:"), 0, desktop()->scene(), false );
	m_zhuangText->setFont( font );
	m_quanFengText = new DJGraphicsTextItem( tr("QuanFeng:"), 0, desktop()->scene(), false );
	m_quanFengText->setFont( font );
	
	m_lastGameTraceType	= 0;
}
GBMJDesktopController::~GBMJDesktopController()
{
	djDebug()<<"GBMJDesktopController destructor";
}
void GBMJDesktopController::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	djDebug() << "GBMJDesktopController::gameWait";
	
	DJDesktopMahjongController::gameWait(mask,status,timeout);
	
	chiWidget()->setEnabled( false );
	chuWidget()->setEnabled( false );
	gangWidget()->setEnabled( false );
	huWidget()->setEnabled( false );
	pengWidget()->setEnabled( false );
	qiWidget()->setEnabled( false );
	tingWidget()->setEnabled( false );
	
	if ( panelController()->isLookingOn() ) {
		m_qiangGangCard	= 0;
		return;
	}
	
	bool isDIY	= false;
	bool isZimoGang	= false;
	
	switch(status) {
	case DJGAME_MAHJONG_TABLE_STATUS_WAIT_OUT: {
		djDebug()<<"DJGAME_MAHJONG_TABLE_STATUS_WAIT_OUT";
		if ( isWaitingForMe() ) {
			djDebug()<<"isWaitingForMe";
			MJCard card	= testHua();
			djDebug()<<"testhua card"<<hex<<card;
			if (  0 != card ) {
				djDebug()<<"send gang flower trace";
				QByteArray buf;
				buf.append( card );
				sendGameTrace( MAHJONG_GAMETRACE_GANGFLOWER, buf );
				m_qiangGangCard	= 0;
				return;
			}
			MJCardsGroups zimoGangGroups	= testZimoGang();
			if ( !zimoGangGroups.empty() ) {
				gangWidget()->setEnabled( true );
				isZimoGang	= true;
			}
			chuWidget()->setEnabled( true );
			if ( !m_isTings[panelController()->seatId()] 
				&& m_lastGameTraceType != MAHJONG_GAMETRACE_EATCOLLISION
				&& m_lastGameTraceType != MAHJONG_GAMETRACE_GANGFLOWER )
				tingWidget()->setEnabled( true );
		}
		int fanOfHu	= testHu( m_qiangGangCard );
		if ( 0 != fanOfHu ) {
			huWidget()->setEnabled( true );
			isDIY	= true;
		}
		break;
	}
	case DJGAME_MAHJONG_TABLE_STATUS_WAIT_EATCOLLISION: {
		djDebug()<<"DJGAME_MAHJONG_TABLE_STATUS_WAIT_EATCOLLISION";
		if ( isWaitingForMe() ) {
			djDebug()<<"isWaitingForMe";
			bool noaction = true;
			int fanOfHu	= testHu( m_qiangGangCard );
			if ( 0 != fanOfHu ) {
				huWidget()->setEnabled( true );
				noaction	= false;
			}
			MJCardsGroups chiGroups	= testChi();
			if ( !chiGroups.empty() ) {
				chiWidget()->setEnabled( true );
				noaction	= false;
			}
			MJCards pengCards	= testPeng();
			if ( !pengCards.empty() ) {
				pengWidget()->setEnabled( true );
				noaction	= false;
			}
			MJCards dianGangCards	= testDianGang();
			if ( !dianGangCards.empty() ) {
				gangWidget()->setEnabled ( true );
				noaction	= false;
			}
			qiWidget()->setEnabled(true);
			if( noaction )
				clickQi();
		}
		break;
	}
	default:
		break;
	}
	if ( m_isTings[panelController()->seatId()] ) {
		djDebug() << "self is ting";
		do {
			if ( huWidget()->isEnabled() ) {
    			djDebug() << "hu is enabled";
    			if ( m_isTingOnlyZimoHu ) {
    				if ( isDIY ) {
    					clickHu();
    					break;
    				}
    			}else {
    				clickHu();
    				break;
    			}
    		}
    		if ( gangWidget()->isEnabled() ) {
    			djDebug() << "gang is enabled";
    			if ( m_isTingAutoGang ) {
    				if ( m_isTingOnlyZimoGang ) {
    					if ( isZimoGang ) {
    						clickGang();
    						break;
    					}
    				}else {
    					clickGang();
    					break;
    				}
    			}
    		}
    		if ( !try2Chu() )
				try2Qi(); 
		}while(false);	
	}
	m_qiangGangCard	= 0;
}
void GBMJDesktopController::gameTraceModel( const GeneralGameTrace2Head *gameTrace )
{
	djDebug() << "GBMJDesktopController::gameTraceModel";
	
	quint8 gameTraceType	= gameTrace->chType;
	quint8 gameTraceSeat	= gameTrace->chSite;
	quint8 bufLen	= gameTrace->chBufLen;
	const unsigned char *buf	= gameTrace->chBuf;
	
	djDebug()<<"gameTraceSeat"<<gameTraceSeat<<"bufLen"<<bufLen;
	DebugBuffer( (const char*)buf, bufLen );
	
	m_lastGameTraceType	= gameTraceType;
	
	switch( gameTraceType )
	{
		case MAHJONG_GAMETRACE_WALLSTART :
		{
			djDebug()<<"MAHJONG_GAMETRACE_WALLSTART";
			int wallStartSeat = buf[0] >> 5;
			int wallStartSkips	= buf[0] & 0x1F;
			repaintWall( wallStartSeat, wallStartSkips );
			break;
		}
		case MAHJONG_GAMETRACE_RESET :
		case MAHJONG_GAMETRACE_INIT :
		{
			djDebug()<<"MAHJONG_GAMETRACE_INIT";
			
			clearDesktopItems( gameTraceSeat, MAHJONG_TYPE_STANDING_HAND );
			
			for ( quint8 i = 0; i < bufLen; i++ ) {
				appendDesktopItem( gameTraceSeat, MAHJONG_TYPE_STANDING_HAND, MAHJONG_CARD(buf[i]) );
				if ( MAHJONG_GAMETRACE_RESET == gameTraceType )
					removeFromWall( true );
			}
			if ( gameTraceSeat == panelController()->seatId() ) {
				syncSelfStandingCards();
			}
			break;
		}
		case MAHJONG_GAMETRACE_OUT :
		{
			djDebug()<<"MAHJONG_GAMETRACE_OUT";

			chu( gameTraceSeat, buf[0] );
			if ( gameTraceSeat == panelController()->seatId() ) {
				syncSelfStandingCards();
			}
			break;
		}
		case MAHJONG_GAMETRACE_PICKUP :
		case MAHJONG_GAMETRACE_DRAW :
		{
			djDebug()<<"MAHJONG_GAMETRACE_PICKUP";
			
			zhua( gameTraceSeat, buf[0] );
			if ( gameTraceSeat == panelController()->seatId() ) {
				syncSelfStandingCards();
			}
			break;
		}
		case MAHJONG_GAMETRACE_EATCOLLISION :
		{
			djDebug()<<"MAHJONG_GAMETRACE_EATCOLLISION";
			
			MJCards cards;
			for( int i = 0; i < 3; i++ ) {
				cards.push_back( buf[i] );
			}
			chipeng( gameTraceSeat, cards );
			if ( gameTraceSeat == panelController()->seatId() ) {
				syncSelfStandingCards();
				syncSelfLyingCards();
			}
			break;
		}
		case MAHJONG_GAMETRACE_GANGFLOWER :
		{
			djDebug()<<"MAHJONG_GAMETRACE_GANGFLOWER";
			bool result	= gangflower( gameTraceSeat, buf[0] );
			if ( result ) {
				djDebug()<<"qianggang is allowed";
				m_qiangGangCard	= buf[0];
			}
			if ( gameTraceSeat == panelController()->seatId() ) {
				syncSelfStandingCards();
				syncSelfLyingCards();
			}
			break;
		}
		case MAHJONG_GAMETRACE_TING :
		{
			djDebug()<<"MAHJONG_GAMETRACE_TING";
			m_isTings[gameTraceSeat]	= true;
			break;
		}
		case MAHJONG_GAMETRACE_HU :
		{
			djDebug()<<"MAHJONG_GAMETRACE_HU";
			quint8 cardLen	= buf[2];
			const unsigned char *cardBuf	= &buf[3];
			for ( quint8 i = 0; i < cardLen; i++ ) {
				appendDesktopItem( gameTraceSeat, MAHJONG_TYPE_HU, MAHJONG_CARD(cardBuf[i]) );
			}
			break;
		}
		default:
			break;
	}
}
void GBMJDesktopController::gameTraceView( const GeneralGameTrace2Head *gameTrace )
{
	djDebug() << "GBMJDesktopController::gameTraceView";
	
	quint8 gameTraceType	= gameTrace->chType;
	quint8 gameTraceSeat	= gameTrace->chSite;
	quint8 bufLen	= gameTrace->chBufLen;
	const unsigned char *buf	= gameTrace->chBuf;
	
	switch( gameTraceType )
	{
		case MAHJONG_GAMETRACE_RESET :
		case MAHJONG_GAMETRACE_INIT :
		{
			repaintSeatHand( gameTraceSeat, true, true, true );
			break;
		}
		case MAHJONG_GAMETRACE_OUT :
		{
			repaintChu( true );
			repaintSeatHand( gameTraceSeat, false, true, true );
			
			QString wavName	= QString("%1.wav").arg(buf[0],2,16,QChar::fromLatin1('0'));
    		playWave(wavName);
			break;
		}
		case MAHJONG_GAMETRACE_PICKUP :
		case MAHJONG_GAMETRACE_DRAW :
		{
			repaintChu( false );
			DesktopItems &items	= desktopItems( gameTraceSeat, MAHJONG_TYPE_STANDING_HAND );
			int numberOfCards	= items.size();
			if ( numberOfCards % 3 == 2 )
				repaintSeatHand( gameTraceSeat, false, true, false, true );
			else
				repaintSeatHand( gameTraceSeat, false, true, false );
			repaintSeatPool( lastChuSeat(), false, false );
			break;
		}
		case MAHJONG_GAMETRACE_EATCOLLISION :
		{
			repaintChu( false );
			repaintSeatHand( gameTraceSeat, false, true, false );
			if(gameTrace->chBuf[0] == gameTrace->chBuf[1]){
				playWave("peng.wav");
			}else {
				playWave("chi.wav");
			}
			break;
		}
		case MAHJONG_GAMETRACE_GANGFLOWER :
		{
			repaintChu( false );
			repaintSeatHand( gameTraceSeat, false, true, true );
			repaintSeatFlower( gameTraceSeat, false, false );
			if ( MAHJONG_ISFLOWERCARD( buf[0] ) ) {
				playWave("bu.wav");
			}else {
				playWave("gang.wav");
			}
			break;
		}
		case MAHJONG_GAMETRACE_TING :
		{
			repaintSeatTing( gameTraceSeat );
			break;
		}
		case MAHJONG_GAMETRACE_HU :
		{
			for ( int i = 0; i <= panelController()->numberOfSeats(); ++i ) {	
				clearDesktopItems(i,MAHJONG_TYPE_STANDING_HAND);
				for ( int j = 0; j < MAHJONG_MAX_LYING_HAND; j++ ) {
					clearDesktopItems(i,MAHJONG_TYPE_LYING_HAND+j);
				}
				clearDesktopItems(i,MAHJONG_TYPE_FLOWER);
				clearDesktopItems(i,MAHJONG_TYPE_POOL);
			}
			for( int view = 1; view <= 4; view++ ) {
				clearDesktopItems( view, MAHJONG_TYPE_TOP_WALL );
				clearDesktopItems( view, MAHJONG_TYPE_BOTTOM_WALL );
			}
			repaintChu( false );
			repaintHu( gameTraceSeat, buf, bufLen );
			playWave("hu.wav");
			break;
		}
		default:
			break;
	}
}
void GBMJDesktopController::initUnderGameInfo( const QByteArray& buf )
{
	djDebug()<<"GBMJDesktopController::initUnderGameInfo";
	DJDesktopMahjongController::initUnderGameInfo(buf);
	
	memcpy(&m_current,buf.data(),sizeof(MahjongCurrent));
	
	for( int view = 1; view <= 4; view++ ) {
		for ( int i = 0; i < bricksPerWall(); i++ ) {
			appendDesktopItem( view, MAHJONG_TYPE_TOP_WALL, MAHJONG_CARD_BACKGROUND );
			appendDesktopItem( view, MAHJONG_TYPE_BOTTOM_WALL, MAHJONG_CARD_BACKGROUND );
		}
		repaintViewWall( view );
	}
	m_qiangGangCard	= 0;
    m_isTingAutoGang	= false;
    m_isTingOnlyZimoHu	= false;
    m_lastGameTraceType	= 0;
    for( quint8 seat = 0; seat <= panelController()->numberOfSeats(); ++seat ) {
		m_isTings[seat]	= false;
		repaintSeatTing( seat );
	}
	repaintZhuang();
	repaintQuanFeng();
	fillPlayerInfo();
}
void GBMJDesktopController::repaintCurrentStatus()
{
	djDebug()<<"GBMJDesktopController::repaintCurrentStatus";
	quint8 seat;
	for( seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
		repaintSeatHand( seat, true, true, true );
		repaintSeatPool( seat, false, false );
		repaintSeatFlower( seat, false, false );
		repaintSeatTing( seat );
	}
}
void GBMJDesktopController::clickTing()
{
	djDebug()<<"GBMJDesktopController::clickTing";
	if ( !m_isTings[panelController()->seatId()] ) {
		UIGBMJTing mjTing;
		if ( QDialog::Accepted == mjTing.exec() ) {
			djDebug()<<"accept ting";
			m_isTingAutoGang	= mjTing.isAutoGang();
			m_isTingOnlyZimoGang	= mjTing.isOnlyZimoGang();
			m_isTingOnlyZimoHu	= mjTing.isOnlyZimoHu();
			tingWidget()->setEnabled( false );
			DesktopItems selectedItems	= selectedDesktopItems( panelController()->seatId(), MAHJONG_TYPE_STANDING_HAND );
			DJDesktopItem *item	= 0;
			if ( selectedItems.isEmpty() ) {
				DesktopItems items	= desktopItems( panelController()->seatId(), MAHJONG_TYPE_STANDING_HAND );
				item	= items.last();
			}else {
				item	= selectedItems.first();
			}
			quint8 tingID	= gameTraceIDOfTing();
			QByteArray data;
			data.append( item->value() );
			sendGameTrace( tingID, data );
		}
	}
}
QString GBMJDesktopController::bonusTarget( const DJGameBonus* bonus )
{
	QString text;
	switch( bonus->chTarget ) {
	case MAHJONG_GAMEBONUS_TARGET_FAN: {
		quint8 fan	= bonus->chParam1;
		djDebug()<<"bonusTarget fan="<<fan;
		QString name	= fanName(fan);
		djDebug()<<name;
		text	= tr("The hu contains %1").arg(name);
		djDebug()<<text;
		break;
	}
	default:
		break;
	}
	return text;
}
void GBMJDesktopController::locateAll()
{
	DJDesktopMahjongController::locateAll();
	
	locateTingPositions( m_tingPoints, m_tingAligns );
}
void GBMJDesktopController::locateAllNonMatrimoveItems()
{
	djDebug()<<"GBMJDesktopController::locateAllNonMatrimoveItems";
	
	DJDesktopMahjongController::locateAllNonMatrimoveItems();
	
	int x = 0;
	int y = 0;
	m_zhuangText->setVirtualPos( QPoint(x,y) );
	m_zhuangText->adjustPos( desktop()->graphicsMatrix() );
	
	y	+= m_zhuangText->boundingRect().toRect().height();
	m_quanFengText->setVirtualPos( QPoint(x,y) );
	m_quanFengText->adjustPos( desktop()->graphicsMatrix() );
}
void GBMJDesktopController::locateTingPositions( QList<QPoint>& points, QList<Qt::Alignment>& aligns )
{
	djDebug() << "GBMJDesktopController::locateTingPositions";
	
	points.clear();
	aligns.clear();
	
	QList<quint8> seats	= seatIdsOfPlayingUsers();
	int players	= seats.size();
		
	for ( int i = 0; i <= panelController()->numberOfSeats(); ++i ) {
		points 	<< QPoint();
		aligns	<< 0;
	}
	foreach( quint8 seat, seats ) {
		QRect showRect	= rectOfSeatShow( seat );
		QPoint center	= showRect.center();
    	quint8 view = seat2View(seat);
    	if ( 4 == players  ) {
    		switch( view ) {
        	case 1: {
        		points[seat].setX( desktop()->realWidth()>>1 );
    			points[seat].setY( desktop()->realHeight() - 32 );
    			aligns[seat]	= Qt::AlignHCenter | Qt::AlignBottom;
        		break;
        	} 
        	case 2:  {
        		QPoint pos	= center;
        		pos.rx()	+= showRect.width() >> 1;
        		points[seat]	= pos;
        		aligns[seat]	= Qt::AlignVCenter;
        		break;
        	}
        	case 3:  {
        		QPoint pos	= center;
        		pos.ry()	+= showRect.height() >> 1;
        		points[seat]	= pos;
        		aligns[seat]	= Qt::AlignHCenter;
        		break;
        	}
        	case 4:  {
        		QPoint pos	= center;
        		pos.rx()	-= showRect.width() >> 1;
        		points[seat]	= pos;
        		aligns[seat]	= Qt::AlignVCenter | Qt::AlignRight;
        		break;
        	}
        	default:
        		break;
          	}
    	}
    }
}
void GBMJDesktopController::repaintAll()
{
	djDebug()<<"GBMJDesktopController::repaintAll";
	
	DJDesktopMahjongController::repaintAll();
	
	for( quint8 seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
		repaintSeatTing( seat );
	}
	
	repaintZhuang();
	repaintQuanFeng();
}
void GBMJDesktopController::repaintHost()
{
	if ( 0 != m_current.chMaster )
		repaintHostItem( m_current.chMaster );
}
void GBMJDesktopController::repaintZhuang()
{
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
	zhuang.prepend( tr("Zhuang:") );
	m_zhuangText->setHtml( zhuang );
	
	panelController()->repaintAllPlayerItems();
}
void GBMJDesktopController::repaintQuanFeng()
{
	quint8 wind	= m_current.chRingOrientation;
	QString text	= tr(gWinds[wind]);
	text.prepend( tr("QuanFeng:") );
	m_quanFengText->setPlainText( text );
}
void GBMJDesktopController::repaintSeatTing( quint8 seat )
{
	DJGraphicsPixmapItem *tingItem	= m_tingItems.value( seat, 0 );
	if ( 0 == tingItem )
		return;
	quint8 view = seat2View(seat);
	QPoint	pos	= m_tingPoints.value( seat, QPoint() );
	if ( pos.isNull() ) {
		tingItem->hide();
	}else {
		if ( m_isTings[seat] ) {
			if ( 1 == view || 3 == view )
				tingItem->setPixmap( QPixmap(":/BaseRes/image/mj/ting13.png") );
			else if ( 2 == view || 4 == view )
				tingItem->setPixmap( QPixmap(":/BaseRes/image/mj/ting24.png") );
			tingItem->setVirtualPos( pos );
			Qt::Alignment	align	= m_tingAligns.value( seat, 0 );
			tingItem->setAlignment( align );
			tingItem->adjustPos( desktop()->graphicsMatrix() );
			tingItem->show();
		}else {
			tingItem->hide();
		}	
	}
}
void GBMJDesktopController::fillPlayerInfo()
{
	djDebug()<<"GBMJDesktopController::fillPlayerInfo";
		
	for( quint8 seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
		DJGraphicsTextItem *info	= playerInfoItem(seat);
		quint8 wind	= m_current.chMenOrientation[seat];
		info->setPlainText( tr("MenFeng:") + tr(gWinds[wind]) );
	}
}
QString	GBMJDesktopController::userItemNameSuffix( DJGameUser* user ) const
{
	djDebug()<<"GBMJDesktopController::userItemNameSuffix"<<m_current.chMaster;
	if ( 0 != m_current.chMaster 
		&& user->seatId() == m_current.chMaster ) {
		return QString("(")+tr("zhuang")+QString(")");
	}
	return DJDesktopController::userItemNameSuffix(user);
}
