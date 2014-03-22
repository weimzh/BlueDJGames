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
#include "GDMJDesktopController.h"
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
#include "mj2profile.h"

static int TEXT_ITEM_FONT_SIZE		= 	16;

GDMJDesktopController::GDMJDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent )
	: DJDesktopMahjongController(panelController,size,parent)
{
	djDebug()<<"GDMJDesktopController constructor"<<size;
	
	chiWidget()->hide();
	tingWidget()->hide();
	
	QFont font;
	font.setPointSize( TEXT_ITEM_FONT_SIZE );
	font.setBold( true );
	
	m_zhuangText = new DJGraphicsTextItem( tr("Zhuang:"), 0, desktop()->scene(), false );
	m_zhuangText->setFont( font );
}
GDMJDesktopController::~GDMJDesktopController()
{
	djDebug()<<"GDMJDesktopController destructor";
}

void GDMJDesktopController::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	djDebug() << "GBMJDesktopController::gameWait";
	
	DJDesktopMahjongController::gameWait(mask,status,timeout);
	
	chuWidget()->setEnabled( false );
	gangWidget()->setEnabled( false );
	huWidget()->setEnabled( false );
	pengWidget()->setEnabled( false );
	qiWidget()->setEnabled( false );
	
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
			/*
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
			*/
			MJCardsGroups zimoGangGroups	= testZimoGang();
			if ( !zimoGangGroups.empty() ) {
				gangWidget()->setEnabled( true );
				isZimoGang	= true;
			}
			chuWidget()->setEnabled( true );
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
			/*
			MJCardsGroups chiGroups	= testChi();
			if ( !chiGroups.empty() ) {
				chiWidget()->setEnabled( true );
				noaction	= false;
			}
			*/
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
	m_qiangGangCard	= 0;
}
int GDMJDesktopController::bricksPerWall() const
{
	return 17;
}
void GDMJDesktopController::initUnderGameInfo( const QByteArray& buf )
{
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
    
	repaintZhuang();
}
void GDMJDesktopController::repaintHost()
{
	if ( 0 != m_current.chMaster )
		repaintHostItem( m_current.chMaster );
}
void GDMJDesktopController::repaintZhuang()
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
void GDMJDesktopController::locateAllNonMatrimoveItems()
{
	djDebug()<<"GDMJDesktopController::locateAllNonMatrimoveItems";
	
	DJDesktopMahjongController::locateAllNonMatrimoveItems();
	
	int x = 0;
	int y = 0;
	m_zhuangText->setVirtualPos( QPoint(x,y) );
	m_zhuangText->adjustPos( desktop()->graphicsMatrix() );
}
void GDMJDesktopController::gameTraceModel( const GeneralGameTrace2Head *gameTrace )
{
	djDebug() << "GBMJDesktopController::gameTraceModel";
	
	quint8 gameTraceType	= gameTrace->chType;
	quint8 gameTraceSeat	= gameTrace->chSite;
	quint8 bufLen	= gameTrace->chBufLen;
	const unsigned char *buf	= gameTrace->chBuf;
	
	djDebug()<<"gameTraceSeat"<<gameTraceSeat<<"bufLen"<<bufLen;
	DebugBuffer( (const char*)buf, bufLen );
		
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
		case MAHJONG_GAMETRACE_RESULT2 :
		{
			PMJResult2 presult2 = (PMJResult2)gameTrace->chBuf;
			clearDesktopItems( gameTraceSeat, MAHJONG_TYPE_STANDING_HAND );
			clearDesktopItems( gameTraceSeat, MAHJONG_TYPE_LYING_HAND );
			for ( quint8 i = 0; i < 20; i++ ) {
				if ( 0 != presult2->chCards[i] )
				appendDesktopItem( gameTraceSeat, MAHJONG_TYPE_STANDING_HAND, MAHJONG_CARD(presult2->chCards[i]) );
			}
			break;
		}
		default:
			break;
	}
}
void GDMJDesktopController::gameTraceView( const GeneralGameTrace2Head *gameTrace )
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
		case MAHJONG_GAMETRACE_RESULT2 :
		{
			PMJResult2 presult2 = (PMJResult2)gameTrace->chBuf;
			bool b=false;
			repaintSeatHand( gameTraceSeat, false, true, true );
			QString info;
			DJGameUser *puser	= panelController()->player(gameTrace->chSite);
			if ( puser )
				info = puser->userName();
			else
				info	= tr("no name");
			info	+= " ";
			info 	+= tr("hu");
			info	+= "\n";
			for(int i=1;i<=4;i++)
			{
				if((short)letoh2(presult2->shScore[i]) < 0)
				{
					b = true;
					break;
				}
			}
			if(b)
			{
				for(int i=1;i<=4;i++)
				{
					puser = panelController()->player(i);
					if ( puser )
						info += puser->userName();
					else
						info += tr("no name");
					info += QString(" : %1\n").arg((short)letoh2(presult2->shScore[i]));
				}					
			}
			panelController()->insertText2Browser(info);
			playWave("hu.wav");
			break;
		}
		default:
			break;
	}
}
QString GDMJDesktopController::bonusTarget( const DJGameBonus* bonus )
{
	QString text;
	switch( bonus->chTarget ) {
	case GDMJ_GAMEBONUS_TARGET_FAN: {
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
QString	GDMJDesktopController::userItemNameSuffix( DJGameUser* user ) const
{
	if ( 0 != m_current.chMaster 
		&& user->seatId() == m_current.chMaster ) {
		return QString("(")+tr("zhuang")+QString(")");
	}
	return DJDesktopController::userItemNameSuffix(user);
}
void GDMJDesktopController::repaintAll()
{
	djDebug()<<"GDMJDesktopController::repaintAll";
	
	DJDesktopMahjongController::repaintAll();
	
	for( quint8 seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
		repaintSeatHand( seat, true, true, true );
		repaintSeatPool( seat, false, false );
		repaintSeatFlower( seat, false, false );
	}
	
	repaintZhuang();
}
void GDMJDesktopController::repaintCurrentStatus()
{
	djDebug()<<"GDMJDesktopController::repaintCurrentStatus";
	
	repaintAll();
}