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
#include "GZDesktopController.h"
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
#include "gozhrule.h"

static int TOOL_BUTTON_ICON_SIZE	=	24;

GZDesktopController::GZDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent )
	: DJDesktopPokerController(panelController,size,parent)
{
	djDebug()<<"GZDesktopController constructor"<<size;
	
	m_showContainer	= new QWidget( desktop() );
    QHBoxLayout* layout	= new QHBoxLayout( m_showContainer );
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    
    QSize iconSize( TOOL_BUTTON_ICON_SIZE, TOOL_BUTTON_ICON_SIZE );
    
    m_showButton = new QToolButton;
	m_showButton->setToolTip(tr("show card"));
	QIcon icon	= QIcon(":/BaseRes/image/desktop/toolbutton/liang.png");
	icon.addPixmap( QPixmap(":/BaseRes/image/desktop/toolbutton/liang_disabled.png"), QIcon::Disabled );
	m_showButton->setIcon( icon );
	m_showButton->setIconSize( iconSize );
	connect(m_showButton,SIGNAL(pressed()),SLOT(clickThrow()));
    layout->addWidget( m_showButton );
    
    m_actionContainer	= new QWidget( desktop() );
    layout	= new QHBoxLayout( m_actionContainer );
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    
    m_throw = new QToolButton;
    m_throw->setEnabled(false);
	m_throw->setToolTip(tr("throw cards"));
	icon	= QIcon(":/BaseRes/image/desktop/toolbutton/chu.png");
	icon.addPixmap( QPixmap(":/BaseRes/image/desktop/toolbutton/chu_disabled.png"), QIcon::Disabled );
	m_throw->setIcon( icon );
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
    
    memset(&m_current,0,sizeof(GongzhuCurrent));
}
GZDesktopController::~GZDesktopController()
{
	djDebug()<<"GZDesktopController destructor";
}
void GZDesktopController::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	DJDesktopPokerController::gameWait(mask,status,timeout);
	
	m_showContainer->hide();
	m_actionContainer->show();
	m_throw->setEnabled( false );
	m_showButton->setEnabled( false );
	
	switch( status ) {
	case DJGAME_GONGZHU_TABLE_STATUS_WAIT_THROW:
		if ( isWaitingForMe() && !panelController()->isLookingOn() ) {
			m_actionContainer->show();
			m_throw->setEnabled( true );
			setWaitingThrowAcl( false );
		}
		break;
	case DJGAME_GONGZHU_TABLE_STATUS_WAIT_SHOW:
		if ( isWaitingForMe() && !panelController()->isLookingOn() ) {
			m_showContainer->show();
			m_actionContainer->hide();
			m_showButton->setEnabled( true );
		}
		break;
	default:
		break;
	}
}
void GZDesktopController::gameTraceModel( const GeneralGameTrace2Head *gameTrace )
{
	djDebug() << "CDDDesktopController::gameTraceModel";
	
	quint8 gameTraceSeat	= gameTrace->chSite;
	quint8 bufLen	= gameTrace->chBufLen;
	const unsigned char *buf	= gameTrace->chBuf;
	
	djDebug()<<"gameTraceSeat"<<gameTraceSeat<<"bufLen"<<bufLen;
	DebugBuffer( (const char*)buf, bufLen );
	
	switch(gameTrace->chType) {
	case GONGZHU_GAMETRACE_RESET:
	case GONGZHU_GAMETRACE_PICKUP:
		clearDesktopItems( gameTraceSeat, POKER_TYPE_HAND );
		for ( quint8 i = 0; i < bufLen; i++ ) {
			if ( 0 != buf[i] )
				appendDesktopItem( gameTraceSeat, POKER_TYPE_HAND, buf[i] );
		}
		break;
	case GONGZHU_GAMETRACE_THROW:
		for ( quint8 i = 0; i < bufLen; i++ ) {
			DJDesktopItem *item	= takeDesktopItem( gameTraceSeat, POKER_TYPE_HAND, buf[i] );
			delete item;
			appendDesktopItem( gameTraceSeat, POKER_TYPE_THROWN, buf[i] );
		}
		break;
	case GONGZHU_GAMETRACE_CLEARDESKTOP:
		for ( int seat = 0; seat <= panelController()->numberOfSeats(); ++seat) {
			clearDesktopItems(seat,POKER_TYPE_LASTROUND_THROWN);
			moveDesktopItems( seat, POKER_TYPE_THROWN, POKER_TYPE_LASTROUND_THROWN );
			hideDesktopItems(seat,POKER_TYPE_LASTROUND_THROWN);
		}
		m_previous->setEnabled(true);
		break;
	case GONGZHU_GAMETRACE_ROUNDRESULT:
		for ( quint8 i = 0; i < bufLen; i++ ) {
			appendDesktopItem( gameTraceSeat, POKER_TYPE_ATTACHED, buf[i] );
		}
		break;
	case GONGZHU_GAMETRACE_SHOW: {
		PGongzhuShow pshow =(PGongzhuShow)gameTrace->chBuf;
		if(!DJGAME_POKER_ERRORCARD(pshow->chCard) && pshow->chPages > 0) {
			clearDesktopItems( gameTraceSeat, POKER_TYPE_THROWN );
			for ( quint8 i = 0; i < pshow->chPages; i++ ) {
				appendDesktopItem( gameTraceSeat, POKER_TYPE_THROWN, pshow->chCard );
			}
		}
		break;
	}
	default:
		break;
	}
}
void GZDesktopController::gameTraceView( const GeneralGameTrace2Head *gameTrace )
{
	djDebug()<<"GZDesktopController::gameTraceView";
	
	quint8 gameTraceSeat	= gameTrace->chSite;
	quint8 bufLen	= gameTrace->chBufLen;
	const unsigned char *buf	= gameTrace->chBuf;
	
	switch( gameTrace->chType ) {
	case GONGZHU_GAMETRACE_RESET:
	case GONGZHU_GAMETRACE_PICKUP:
		repaintHandPokers( gameTraceSeat, POKER_TYPE_HAND, true, true, true );
		break;
	case GONGZHU_GAMETRACE_THROW:
		repaintThrownPokers( gameTraceSeat, POKER_TYPE_THROWN );
		repaintHandPokers( gameTraceSeat, POKER_TYPE_HAND, false, true, false );
		playWave("out.wav");
		break;
	case GONGZHU_GAMETRACE_SHOW:
		repaintThrownPokers( gameTraceSeat, POKER_TYPE_THROWN );
		break;
	case GONGZHU_GAMETRACE_CLEARDESKTOP:
		break;
	case GONGZHU_GAMETRACE_ROUNDRESULT:
		repaintAttachedPokers( gameTraceSeat, POKER_TYPE_ATTACHED );
		break;
	default:
		break;
	}
}
void GZDesktopController::initUnderGameInfo( const QByteArray& buf )
{
	djDebug()<<"GZDesktopController::initUnderGameInfo";
	
	DJDesktopPokerController::initUnderGameInfo( buf );
		
	memcpy(&m_current,buf.data(),sizeof(GongzhuCurrent));
	
	m_previous->setEnabled(false);
}
bool GZDesktopController::isChuEnabled() const
{
	djDebug()<<"DDZDesktopController::isChuEnabled";
	return m_throw->isEnabled() && !isWaitingThrowAcl();
}
bool GZDesktopController::isLiangEnabled() const
{
	return m_showButton->isEnabled();
}
void GZDesktopController::clickLiang()
{
	if( isLiangEnabled() ) {
		DesktopItems selectedItems	= selectedDesktopItems( panelController()->seatId(), POKER_TYPE_HAND );
		throwPokerItems( selectedItems );
	}
}
void GZDesktopController::clickThrow()
{
	if( isChuEnabled() ) {
		DesktopItems selectedItems	= selectedDesktopItems( panelController()->seatId(), POKER_TYPE_HAND );
		throwPokerItems( selectedItems );
	}
}
void GZDesktopController::previousPressed()
{
	for ( int seat = 1; seat <= panelController()->numberOfSeats(); seat++ ) {
		hideDesktopItems( seat, POKER_TYPE_THROWN );
		showDesktopItems( seat, POKER_TYPE_LASTROUND_THROWN );
	}
}
void GZDesktopController::previousRelesed()
{
	for ( int seat = 1; seat <= panelController()->numberOfSeats(); seat++ ) {
		hideDesktopItems( seat, POKER_TYPE_LASTROUND_THROWN );
		showDesktopItems( seat, POKER_TYPE_THROWN );
	}
}
void GZDesktopController::locateAllNonMatrimoveItems()
{
	DJDesktopPokerController::locateAllNonMatrimoveItems();
		
	QMatrix m	= desktop()->graphicsMatrix();
	QRect pool	= m.mapRect( poolSpace() );
	
	m_showContainer->move( pool.right() - m_showContainer->width(), pool.bottom() - m_showContainer->height() );
	m_actionContainer->move( pool.right() - m_actionContainer->width(), pool.bottom() - m_actionContainer->height() );
}
void GZDesktopController::repaintCurrentStatus()
{
	djDebug()<<"GZDesktopController::repaintCurrentStatus";
	repaintAll();
		
	quint8 seat;
	for( seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
		repaintHandPokers( seat, POKER_TYPE_HAND, true, true, true );
		repaintThrownPokers( seat, POKER_TYPE_THROWN );
		repaintAttachedPokers( seat, POKER_TYPE_ATTACHED );
	}
}
QByteArray GZDesktopController::createThrowTrace( DesktopItems& items )
{	
	djDebug()<<"GZDesktopController::createThrowTrace";
	
	if ( items.isEmpty() )
		return QByteArray();
		
	sortPokers( items );
	
	QList<DJPoker> pokers	=  pokersFromDesktopItems( items );
	QByteArray cards	= cardsFromPokers( pokers );
	DJGamePokerImage pokerImage;
    createDJGamePokerImage( cards, pokerImage );
        	
	djDebug()<<"cards size"<<cards.size();
	DebugBuffer( cards );
	
	char buffer[255];
	PGeneralGameTrace2Head ptrace = (PGeneralGameTrace2Head)buffer;
	
	ptrace->chTable = panelController()->tableId();
	ptrace->chSite = panelController()->seatId();
	ptrace->chType = 0;
	
	if( DJGAME_GONGZHU_TABLE_STATUS_WAIT_THROW == gameWaitStatus() ) {
		ptrace->chType = GONGZHU_GAMETRACE_THROW;
		ptrace->chBufLen = cards.size();
		memcpy( ptrace->chBuf, cards.data(), cards.size() );
		return QByteArray( buffer, sizeof(GeneralGameTrace2Head) + ptrace->chBufLen );
	}else if( DJGAME_GONGZHU_TABLE_STATUS_WAIT_SHOW == gameWaitStatus() ) {
		GongzhuShow show;
		quint8 card	= cards[0];
		show.chCard = card;
		show.chPages = pokerImage.chPage[card];
		ptrace->chType = GONGZHU_GAMETRACE_SHOW;
		ptrace->chBufLen = sizeof(GongzhuShow);
		memcpy( ptrace->chBuf, &show, sizeof(GongzhuShow) );
		return QByteArray( buffer, sizeof(GeneralGameTrace2Head) + ptrace->chBufLen );
	}else {
		return QByteArray();
	}
}
