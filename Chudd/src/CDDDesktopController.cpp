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
#include "CDDDesktopController.h"
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
#include "chuddrule.h"

static int TOOL_BUTTON_ICON_SIZE	=	24;

CDDDesktopController::CDDDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent )
	: DJDesktopPokerController(panelController,size,parent)
{
	djDebug()<<"CDDDesktopController constructor"<<size;
	
	setBridgeCard( false );
	setPokerSortMode( PointSuit );
	
	m_actionContainer	= new QWidget( desktop() );
    QHBoxLayout* layout	= new QHBoxLayout( m_actionContainer );
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    
    QSize iconSize( TOOL_BUTTON_ICON_SIZE, TOOL_BUTTON_ICON_SIZE );

    m_arrange = new QToolButton;
    m_arrange->setEnabled(false);
	m_arrange->setToolTip(tr("rearrange"));
	m_arrange->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/arrange.png") );
	m_arrange->setIconSize( iconSize );
    connect(m_arrange,SIGNAL(clicked()),SLOT(clickArrange()));
    layout->addWidget( m_arrange );
    
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
    
    m_pass = new QToolButton;
    m_pass->setEnabled(false);
	m_pass->setToolTip(tr("pass"));
	m_pass->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/qi.png") );
	m_pass->setIconSize( iconSize );
    connect(m_pass,SIGNAL(clicked()),SLOT(clickPass()));
    layout->addWidget( m_pass );
    
    m_seatOfLastThrow	= 0;
    m_styleOfArrange	= 0;
}
CDDDesktopController::~CDDDesktopController()
{
	djDebug()<<"CDDDesktopController destructor";
}
void CDDDesktopController::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	DJDesktopPokerController::gameWait(mask,status,timeout);
	
	m_tip->setEnabled(false);
	m_throw->setEnabled( false );
	m_arrange->setEnabled(true);
	m_pass->setEnabled( false );
	
	switch( status ) {
	case DJGAME_CHUDD_TABLE_STATUS_WAIT_THROW :{
		QList<quint8> seats	= mask2Seat(mask);
		if ( !seats.isEmpty() ) {
			quint8 seat	= seats.first();
			if ( seat != m_seatOfLastThrow ) {
				clearDesktopItems( seat, POKER_TYPE_THROWN );
			}
		}
		if ( isWaitingForMe() && !panelController()->isLookingOn() ) {
			m_throw->setEnabled( true );
			setWaitingThrowAcl( false );
			if ( 0 != m_seatOfLastThrow &&
				m_seatOfLastThrow != panelController()->seatId() ) {
				m_pass->setEnabled( true );
				m_tip->setEnabled( true );
			}
		}
		break;
	}
	default:
		break;
	}
}
void CDDDesktopController::gameTraceModel( const GeneralGameTrace2Head *gameTrace )
{
	djDebug() << "CDDDesktopController::gameTraceModel";
	
	quint8 gameTraceSeat	= gameTrace->chSite;
	quint8 bufLen	= gameTrace->chBufLen;
	const unsigned char *buf	= gameTrace->chBuf;
	
	djDebug()<<"gameTraceSeat"<<gameTraceSeat<<"bufLen"<<bufLen;
	DebugBuffer( (const char*)buf, bufLen );
	
	switch(gameTrace->chType) {
	case CHUDD_GAMETRACE_PICKUP:
	case CHUDD_GAMETRACE_PICKUP2:
		clearDesktopItems( gameTraceSeat, POKER_TYPE_HAND );
		for ( quint8 i = 0; i < bufLen; i++ ) {
			if ( 0 != buf[i] )
				appendDesktopItem( gameTraceSeat, POKER_TYPE_HAND, buf[i] );
		}
		break;
	case CHUDD_GAMETRACE_THROW:
		clearDesktopItems( gameTraceSeat, POKER_TYPE_THROWN );
		if( bufLen > 0 ) {
			for ( quint8 i = 0; i < bufLen; i++ ) {
				DJDesktopItem *item	= takeDesktopItem( gameTraceSeat, POKER_TYPE_HAND, buf[i] );
				delete item;
				appendDesktopItem( gameTraceSeat, POKER_TYPE_THROWN, buf[i] );
			}
			m_seatOfLastThrow	= gameTraceSeat;
		}
		break;
	default:
		break;
	}
}
void CDDDesktopController::gameTraceView( const GeneralGameTrace2Head *gameTrace )
{
	djDebug()<<"CDDDesktopController::gameTraceView";
	
	quint8 gameTraceSeat	= gameTrace->chSite;
	quint8 bufLen	= gameTrace->chBufLen;
	const unsigned char *buf	= gameTrace->chBuf;
	
	switch( gameTrace->chType ) {
	case CHUDD_GAMETRACE_PICKUP:
	case CHUDD_GAMETRACE_PICKUP2:
		repaintHandPokers( gameTraceSeat, POKER_TYPE_HAND, true, true, true );
		break;
	case CHUDD_GAMETRACE_THROW:
		repaintThrownPokers( gameTraceSeat, POKER_TYPE_THROWN );
		repaintHandPokers( gameTraceSeat, POKER_TYPE_HAND, false, true, false );
		fillPlayerInfo();
		repaintPlayerInfo();
		playWave("out.wav");
		break;
	default:
		break;
	}
}
void CDDDesktopController::initUnderGameInfo( const QByteArray& buf )
{
	djDebug()<<"CDDDesktopController::initUnderGameInfo";
	
	DJDesktopPokerController::initUnderGameInfo( buf );
		
	memcpy(&m_current,buf.data(),sizeof(ChuddCurrent));
	
	m_seatOfLastThrow	= 0;
}
bool CDDDesktopController::isChuEnabled() const
{
	djDebug()<<"DDZDesktopController::isChuEnabled";
	return m_throw->isEnabled() && !isWaitingThrowAcl();
}
bool CDDDesktopController::isQiEnabled() const
{
	return m_pass->isEnabled();
}
QByteArray CDDDesktopController::createThrowTrace( DesktopItems& items )
{	
	djDebug()<<"CDDDesktopController::createThrowTrace";
	
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

	if( DJGAME_CHUDD_TABLE_STATUS_WAIT_THROW == gameWaitStatus() ) {
		ptrace->chType = CHUDD_GAMETRACE_THROW;
		return QByteArray( buffer, sizeof(GeneralGameTrace2Head) + ptrace->chBufLen );
	}else {
		return QByteArray();
	}
}
void CDDDesktopController::clickTip()
{
	djDebug()<<"CDDDesktopController::clickTip";
	
	if ( m_tipHandCards.isEmpty() ) {
		//first tip
		djDebug()<<"it is the first tip";
		DesktopItems &lastItems	= desktopItems( m_seatOfLastThrow, POKER_TYPE_THROWN );
		QList<DJPoker> lastPokers	= pokersFromDesktopItems( lastItems );
		QByteArray lastCards	= cardsFromPokers( lastPokers );
		
		bool result	= ChuddRule_GetCardType( (unsigned char*)lastCards.data(), lastCards.size(), &m_tipOtherType );
		if ( !result ) {
			return;
		}
		DesktopItems &handItems	= desktopItems( panelController()->seatId(), POKER_TYPE_HAND );
		QList<DJPoker> handPokers	= pokersFromDesktopItems( handItems );
		
		m_tipHandCards	= cardsFromPokers( handPokers );
		m_tipHandIndexes.clear();
	}
	do {
    	iterateCombination( m_tipHandCards.size(), m_tipOtherType.chCardPages, m_tipHandIndexes );
    	if ( !m_tipHandIndexes.isEmpty() ) {
    		int count	= 0;
        	for (int i = 0; i < m_tipHandIndexes.size(); ++i ) {
        		if ( m_tipHandIndexes.at(i) ) {
        			m_tipSelectedCards[count++]	= m_tipHandCards.at(i);
        		}
        	}
        	memcpy(&m_tipSelfType,&m_tipOtherType,sizeof(ChuddRuleCardType));
        	DJGamePokerImage pokerImage;
        	createDJGamePokerImage( m_tipHandCards, pokerImage );
        	bool result	= ChuddRule_CheckThrow( &pokerImage,
        									&m_tipSelfType,
        									m_tipSelectedCards,
        									count );
        	if ( result ) {
        		QList<DJPoker>	pokers;
    			for( int i = 0; i < count; ++i ) {
    				pokers << DJPoker( m_tipSelectedCards[i], isBridgeCard() );
    			}
    			selectSelfHandPokers( pokers );
    			return;
        	}
    	}
	}while( !m_tipHandIndexes.isEmpty() );
	
	m_tipHandCards.clear();
	selectSelfHandPokers();
}
void CDDDesktopController::clickThrow()
{
	djDebug()<<"CDDDesktopController::clickThrow";
	if( isChuEnabled() ) {
		DesktopItems selectedItems	= selectedDesktopItems( panelController()->seatId(), POKER_TYPE_HAND );
		throwPokerItems( selectedItems );
	}
}
void CDDDesktopController::clickArrange()
{
	djDebug()<<"CDDDesktopController::clickArrange";
	m_styleOfArrange++;
	m_styleOfArrange = m_styleOfArrange % 3;
	switch ( m_styleOfArrange ) {
	case 0:
		setPokerSortMode( PointSuit );
		break;
	case 1:
		setPokerSortMode( ASCountPointSuit );
		break;
	case 2:
		setPokerSortMode( SuitPoint );
		break;
	default:
		break;
	}
	
	repaintHandPokers( panelController()->seatId(), POKER_TYPE_HAND, false, true, true );
	for( quint8 seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
		repaintThrownPokers( seat, POKER_TYPE_THROWN );
	}
}
void CDDDesktopController::clickPass()
{
	djDebug()<<"DDZDesktopController::clickPass";
	if ( isQiEnabled() ) {
		sendGameTrace(CHUDD_GAMETRACE_THROW);
	}
}
void CDDDesktopController::locateAllNonMatrimoveItems()
{
	djDebug()<<"CDDDesktopController::locateAllNonMatrimoveItems";
	
	DJDesktopPokerController::locateAllNonMatrimoveItems();
	
	QMatrix matrix	= desktop()->graphicsMatrix();
	QRect pool	= matrix.mapRect( poolSpace() );
	m_actionContainer->move( pool.right() - m_actionContainer->width(), pool.bottom() - m_actionContainer->height() );
	
}
QString CDDDesktopController::bonusTarget( const DJGameBonus* bonus )
{
	QString text;
	switch( bonus->chTarget ) {
	case CHUDD_GAMEBONUS_TARGET_MAX: {
		text	= tr("All enemies were blocked");
		break;
	}
	case CHUDD_GAMEBONUS_TARGET_13: {
		text	= tr("One enemy was blocked");
		break;
	}
	case CHUDD_GAMEBONUS_TARGET_LASTCARD: {
		quint8 value	= bonus->chParam1;
		text	= tr("The last thrown cards contains %1").arg(value);
		break;
	}
	default:
		break;
	}
	return text;
}
void CDDDesktopController::repaintCurrentStatus()
{
	djDebug()<<"CDDDesktopController::repaintCurrentStatus";
	repaintAll();
	
	for( quint8 seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
		repaintHandPokers( seat, POKER_TYPE_HAND, true, true, true );
		repaintThrownPokers( seat, POKER_TYPE_THROWN );
	}
}
void CDDDesktopController::fillPlayerInfo()
{
	djDebug()<<"CDDDesktopController::fillPlayerInfo";
	
	for( quint8 seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
		DJGraphicsTextItem *info	= playerInfoItem(seat);
		int remain	= numberOfPokers( seat, POKER_TYPE_HAND );
		info->setPlainText( tr("%1 left").arg(remain) );
	}
}
