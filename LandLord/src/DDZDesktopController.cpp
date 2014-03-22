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
#include "DDZDesktopController.h"
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
#include "ddzrule.h"

DDZCallButton::DDZCallButton( QWidget * parent )
	:QPushButton(parent)
{
	init();
}
DDZCallButton::DDZCallButton( const QString & text, QWidget * parent )
	:QPushButton(text,parent)
{
	init();
}
DDZCallButton::DDZCallButton( const QIcon & icon, const QString & text, QWidget * parent )
	:QPushButton(icon,text,parent)
{
	init();
}
DDZCallButton::~DDZCallButton()
{
}
void DDZCallButton::init()
{
	m_score	= 0;
	connect( this, SIGNAL(clicked()), SLOT(handleClicked()) );
}
void DDZCallButton::setScore( int score )
{
	m_score	= score;
}
void DDZCallButton::handleClicked()
{
	emit scoreCalled(m_score);
}

static int TEXT_ITEM_FONT_SIZE		= 	16;
static int TOOL_BUTTON_ICON_SIZE	=	24;
static	qreal	Z_EMOTION			= 2000;

static uint qHash ( DJPoker poker )
{
	return (poker.suit() << 16) | poker.point();
}

DDZDesktopController::DDZDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent )
	: DJDesktopPokerController(panelController,size,parent)
{
	djDebug()<<"DDZDesktopController constructor"<<size;
	
	PDoudzhuRoom pddzroom = (PDoudzhuRoom)((panelController->gameRoom())->privateRoom());
	int i=0;
	while( 0 != pddzroom->rule.chCall[i] )
	{
		DDZCallButton *callButton = new DDZCallButton( desktop() );
		int score	= pddzroom->rule.chCall[i];
		callButton->setScore( score );
		callButton->setText( tr("Call %1").arg(score) );
		callButton->adjustSize();
		callButton->hide();
		m_callButtons << callButton;
		i++;
	}
	DDZCallButton *callButton = new DDZCallButton( desktop() );
	callButton->setScore( 0 );
	callButton->setText( tr("No Call") );
	callButton->hide();
	m_callButtons << callButton;
	
	foreach( DDZCallButton *callButton, m_callButtons ) {
		connect( callButton, SIGNAL(scoreCalled(int)), SLOT(handleScoreCalled(int)) );
	}
	
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
    
    QFont font;
	font.setPointSize( TEXT_ITEM_FONT_SIZE );
	font.setBold( true );
	
	m_dizhuText = new DJGraphicsTextItem( tr("Dizhu:"), 0, desktop()->scene(), false );
	m_dizhuText->setFont( font );
	m_bombText = new DJGraphicsTextItem( tr("Bombs:"), 0, desktop()->scene(), false );
	m_bombText->setFont( font );
	m_scoreText = new DJGraphicsTextItem( tr("Score:"), 0, desktop()->scene(), false );
	m_scoreText->setFont( font );

	for ( int i = 0; i <= panelController->numberOfSeats(); ++i ) {
		DJGraphicsPixmapItem	*emotionItem	= new DJGraphicsPixmapItem( 0, desktop()->scene() );
		emotionItem->setZValue( Z_EMOTION );
		emotionItem->hide();
		m_emotionItems  << emotionItem;
	}
	memset(&m_current,0,sizeof(DoudzhuCurrent));
	
	m_scoreOfLastCall	= 0;
	m_seatOfLastCall	= 0;
	m_seatOfLastThrow	= 0;
	m_bombs				= 0;
	m_score				= 0;
	m_lastThrowType		= 0;
	m_styleOfArrange	= 0;
	
	setPokerSortMode( ASCountPointSuit );
}
DDZDesktopController::~DDZDesktopController()
{
	djDebug()<<"DDZDesktopController destructor";
}
QPixmap DDZDesktopController::createCallPixmap( int score )
{
	if ( 0 == score ) {
		return QPixmap(":/LandLordRes/image/fear.png");
	}
	if ( score >= 100 ) {
		return QPixmap(":/LandLordRes/image/victory.png");
	}
	QPixmap numberPix;
	int total	= score;
	int shi	= total / 10;
	total	-= shi * 10;
	if ( 0 != shi ) {
		QList<QPixmap> pixmaps;
		pixmaps << QPixmap( QString(":/BaseRes/image/clock/%1.png").arg(shi) )
				<< QPixmap( QString(":/BaseRes/image/clock/%1.png").arg(total) );
		numberPix	= CreateConjointPixmap( pixmaps );
	}else {
		numberPix	= QPixmap( QString(":/BaseRes/image/clock/%1.png").arg(total) );
	}
	/*
	ScalePixmap( numberPix, 0.5 );
	
	QPixmap	boardPix	= QPixmap(":/LandLordRes/image/call1.png");
	QPainter painter( &boardPix );
		
	painter.drawPixmap( 0, 0, numberPix );
	painter.end();
	*/
	return numberPix;
}
bool DDZDesktopController::isFriend( quint8 seat1, quint8 seat2 ) const
{
	if(seat1 == seat2)
		return true;
	if(seat1 == m_seatOfLastCall || seat2 == m_seatOfLastCall)
		return false;
	return true;
}
void DDZDesktopController::initUnderGameInfo( const QByteArray& buf )
{
	djDebug()<<"DDZDesktopController::initUnderGameInfo";
	
	DJDesktopPokerController::initUnderGameInfo( buf );
		
	memcpy(&m_current,buf.data(),sizeof(DoudzhuCurrent));
	
	m_scoreOfLastCall	= 0;
	m_seatOfLastCall	= 0;
	m_seatOfLastThrow	= 0;
	m_bombs				= 0;
	m_score				= 0;
	m_lastThrowType		= 0;
	//m_styleOfArrange	= 0;
	
	repaintDizhu();
	repaintBombScore();
}
void DDZDesktopController::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	DJDesktopPokerController::gameWait(mask,status,timeout);
	
	foreach( DDZCallButton *callButton, m_callButtons ) {
		callButton->hide();
	}
	m_tip->setEnabled(false);
	m_throw->setEnabled( false );
	m_arrange->setEnabled(true);
	m_pass->setEnabled( false );
	
	switch(status) {
	case DJGAME_DOUDZHU_TABLE_STATUS_WAIT_CALL :
		foreach( DDZCallButton* callButton, m_callButtons ) {
			if( isWaitingForMe() ) {
				callButton->show();
				if ( callButton->score() > m_scoreOfLastCall
					|| 0 == callButton->score() ) {
					callButton->setEnabled(true);
				}else {
					callButton->setEnabled(false);
				}
			}
		}
		break;
	case DJGAME_DOUDZHU_TABLE_STATUS_WAIT_THROW : {
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
void DDZDesktopController::gameTraceModel( const GeneralGameTrace2Head *gameTrace )
{
	djDebug() << "DDZDesktopController::gameTraceModel";
	
	quint8 gameTraceSeat	= gameTrace->chSite;
	quint8 bufLen	= gameTrace->chBufLen;
	const unsigned char *buf	= gameTrace->chBuf;
	
	djDebug()<<"gameTraceSeat"<<gameTraceSeat<<"bufLen"<<bufLen;
	DebugBuffer( (const char*)buf, bufLen );
	
	switch(gameTrace->chType) {
	case DOUDZHU_GAMETRACE_DEAL : 
	case DOUDZHU_GAMETRACE_PICKUP : {
		djDebug()<<"DOUDZHU_GAMETRACE_PICKUP";
		
		clearDesktopItems( gameTraceSeat, POKER_TYPE_HAND );
		for ( quint8 i = 0; i < bufLen; i++ ) {
			if ( 0 != buf[i] )
				appendDesktopItem( gameTraceSeat, POKER_TYPE_HAND, buf[i] );
		}
		break;
	}
	case DOUDZHU_GAMETRACE_CALL : 
		if( bufLen > 0 ) {
			m_scoreOfLastCall = buf[0];
			if ( m_scoreOfLastCall )
				m_seatOfLastCall = gameTrace->chSite;
			m_current.chMaster =  m_seatOfLastCall;
			m_score	= m_scoreOfLastCall;
		}			
		break;
	case DOUDZHU_GAMETRACE_HAND : {
		djDebug()<<"DOUDZHU_GAMETRACE_HAND";
		
		bool isVisible	= true;
		DesktopItems &items	= desktopItems( gameTraceSeat, POKER_TYPE_HAND );
		if ( !items.isEmpty() ) {
			DJDesktopItem *item	= items.first();
			if ( DJGAME_POKER_ERRORCARD(item->value()) )
				isVisible	= false;
		}else {
			isVisible	= false;
		}
		for ( quint8 i = 0; i < bufLen; i++ ) {
			appendDesktopItem( gameTraceSeat, POKER_TYPE_ATTACHED, buf[i] );
			if ( isVisible ) {
				appendDesktopItem( gameTraceSeat, POKER_TYPE_HAND, buf[i] );
			}else {
				appendDesktopItem( gameTraceSeat, POKER_TYPE_HAND, DJGAME_POKER_BACKGROUND );
			}
		}
		break;
	}
	case DOUDZHU_GAMETRACE_THROW : {
		djDebug()<<"DOUDZHU_GAMETRACE_THROW";
		/*
		if ( gameTraceSeat == m_seatOfLastThrow ) {
			for( quint8 seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
				hideEmotion( seat );
			}
		}
		*/
		clearDesktopItems( gameTraceSeat, POKER_TYPE_THROWN );
		if( bufLen > 0 ) {
			for ( quint8 i = 0; i < bufLen; i++ ) {
				DJDesktopItem *item	= takeDesktopItem( gameTraceSeat, POKER_TYPE_HAND, buf[i] );
				delete item;
				appendDesktopItem( gameTraceSeat, POKER_TYPE_THROWN, buf[i] );
			}
			m_seatOfLastThrow	= gameTraceSeat;
			
			PDoudzhuRoom pddzroom = (PDoudzhuRoom)((panelController()->gameRoom())->privateRoom());
			unsigned char lastThrowValue,lastThrowSerial;
			m_lastThrowType = CheckCardTypeCode(&(pddzroom->rule),(unsigned char*)buf,bufLen,
													&lastThrowValue,&lastThrowSerial);
				
			if( ISROCKET(m_lastThrowType) ) {
				m_bombs ++;
				m_score *= 2;
			}else if( ISBOMB(&(pddzroom->rule),m_lastThrowType) ) {
				m_bombs ++;
				m_score *= 2;
			}
		}else {
			/*
			if( isFriend( gameTraceSeat, m_seatOfLastThrow ) ) {
				repaintEmotion( gameTraceSeat, QPixmap(":/LandLordRes/image/victory.png") );
			}else {
				repaintEmotion( gameTraceSeat, QPixmap(":/LandLordRes/image/fear.png") );	
			}
			*/
		}
		break;
	}
	default:
		break;
	}
}
void DDZDesktopController::gameTraceView( const GeneralGameTrace2Head *gameTrace )
{
	djDebug()<<"DDZDesktopController::gameTraceView";
	
	quint8 gameTraceSeat	= gameTrace->chSite;
	quint8 bufLen	= gameTrace->chBufLen;
	const unsigned char *buf	= gameTrace->chBuf;
	
	switch( gameTrace->chType ) {
	case DOUDZHU_GAMETRACE_DEAL : 
	case DOUDZHU_GAMETRACE_PICKUP : {
		repaintHandPokers( gameTraceSeat, POKER_TYPE_HAND, true, true, true );
		break;
	}
	case DOUDZHU_GAMETRACE_CALL : {
		if( bufLen > 0 ) {
			repaintDizhu();
			repaintBombScore();
		}
		QPixmap pix	= createCallPixmap( buf[0] );
		repaintEmotion( gameTraceSeat, pix );
		break;
	}
	case DOUDZHU_GAMETRACE_HAND : {
		repaintHandPokers( gameTraceSeat, POKER_TYPE_HAND, false, true, true );
		repaintAttachedPokers( gameTraceSeat, POKER_TYPE_ATTACHED );
		
		for( quint8 seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
			hideEmotion( seat );
		}
	}
	case DOUDZHU_GAMETRACE_THROW : {
		repaintThrownPokers( gameTraceSeat, POKER_TYPE_THROWN );
		repaintHandPokers( gameTraceSeat, POKER_TYPE_HAND, false, true, false );
		if( bufLen > 0) {
			repaintBombScore();
			fillPlayerInfo();
			repaintPlayerInfo();
			
			playWave("out.wav");
		}else {
		}
		break;
	}
	default:
		break;
	}
}
int DDZDesktopController::rankOnPoint( const DJPoker& poker ) const
{
	DJPoker::Point point	= poker.point();
	if ( DJPoker::Num2 == point )
		return DJPoker::Shift | point;
	return DJDesktopPokerController::rankOnPoint( poker );
}
void DDZDesktopController::repaintAll()
{
	djDebug()<<"DDZDesktopController::repaintAll";
	
	DJDesktopPokerController::repaintAll();
	
	repaintDizhu();
	repaintBombScore();
}
void DDZDesktopController::repaintEmotion( quint8 seat, const QPixmap& pix )
{
	DJGraphicsPixmapItem *emotionItem	= m_emotionItems.value( seat, 0 );
	if ( 0 == emotionItem )
		return;
		
	QPoint	pos	= m_emotionPoints.value( seat, QPoint() );
	if ( pos.isNull() ) {
		emotionItem->hide();
	}else {
		emotionItem->setPixmap( pix );
		emotionItem->setVirtualPos( pos );
		Qt::Alignment	align	= m_emotionAligns.value( seat, 0 );
		emotionItem->setAlignment( align );
		emotionItem->adjustPos( desktop()->graphicsMatrix() );
		emotionItem->show();
	}
}
void DDZDesktopController::hideEmotion( quint8 seat )
{
	DJGraphicsPixmapItem *emotionItem	= m_emotionItems.value( seat, 0 );
	if ( 0 == emotionItem )
		return;
	emotionItem->hide();
}
void DDZDesktopController::repaintCurrentStatus()
{
	djDebug()<<"DDZDesktopController::repaintCurrentStatus";
	repaintAll();
		
	for( quint8 seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
		repaintHandPokers( seat, POKER_TYPE_HAND, true, true, true );
		repaintThrownPokers( seat, POKER_TYPE_THROWN );
		repaintAttachedPokers( seat, POKER_TYPE_ATTACHED );
	}
}
void DDZDesktopController::repaintHost()
{
	if ( 0 != m_seatOfLastCall )
		repaintHostItem( m_seatOfLastCall );
}
void DDZDesktopController::repaintDizhu()
{
	repaintHost();
	
	QString dizhu	= tr("Undetermined");
	quint8 seatOfDizhu	= m_seatOfLastCall;
	if( 0 != seatOfDizhu ) {
		DJGameUser *player = panelController()->player( seatOfDizhu );
		if ( player )
			dizhu	= player->userName();
		for ( quint8 seat = 1; seat <= panelController()->numberOfSeats(); seat++ ) {
			DJGraphicsTextItem *nameItem	= playerNameItem(seat);
			if ( nameItem ) {
				if ( seat == seatOfDizhu ) {
					nameItem->setDefaultTextColor( QColor(0x80,0,0) );
				}else {
					nameItem->setDefaultTextColor( Qt::black );
				}
			}
		}
	}
	dizhu.prepend("<font color=#800000>");
	dizhu.append("</font>");
	dizhu.prepend( tr("Dizhu:") );
	m_dizhuText->setHtml( dizhu );
	
	panelController()->repaintAllPlayerItems();
}
void DDZDesktopController::repaintBombScore()
{
	QString text	= QString::number(m_bombs);
	text.prepend( tr("Bombs:") );
	m_bombText->setPlainText( text );
	text	= QString::number(m_score);
	text.prepend( tr("Score:") );
	m_scoreText->setPlainText( text );
}
void DDZDesktopController::locateAll()
{
	DJDesktopPokerController::locateAll();
	
	locateEmotionPositions( m_emotionPoints, m_emotionAligns );
}
void DDZDesktopController::locateAllNonMatrimoveItems()
{
	djDebug()<<"DDZDesktopController::locateAllNonMatrimoveItems";
	
	DJDesktopPokerController::locateAllNonMatrimoveItems();
	
	QMatrix matrix	= desktop()->graphicsMatrix();
	QPoint centerPos	= centerPoint();
	centerPos.ry()	+= 48;
	centerPos	= matrix.map( centerPos );
	
	int len	= 0;
	int space	= 4;
	foreach( DDZCallButton *callButton, m_callButtons ) {
		len	+= callButton->width();
		len	+= space;
	}
	if ( 0 != len )
		len	-= space;
	int	x	= ( desktop()->width() - len ) >> 1;
	int	y	= centerPos.y();
	foreach( DDZCallButton *callButton, m_callButtons ) {
		callButton->move(x,y);
		x	+= callButton->width();
		x	+= space;
	}
	
	QRect pool	= matrix.mapRect( poolSpace() );
	m_actionContainer->move( pool.right() - m_actionContainer->width(), pool.bottom() - m_actionContainer->height() );
	
	x = 0;
	y = 0;
	m_dizhuText->setVirtualPos( QPoint(x,y) );
	m_dizhuText->adjustPos( desktop()->graphicsMatrix() );
	
	y	+= m_dizhuText->boundingRect().toRect().height();
	m_bombText->setVirtualPos( QPoint(x,y) );
	m_bombText->adjustPos( desktop()->graphicsMatrix() );
	
	y	+= m_bombText->boundingRect().toRect().height();
	m_scoreText->setVirtualPos( QPoint(x,y) );
	m_scoreText->adjustPos( desktop()->graphicsMatrix() );
}
void DDZDesktopController::locateEmotionPositions( QList<QPoint>& points, QList<Qt::Alignment>& aligns )
{
	djDebug() << "DDZDesktopController::locateNamePositions";
	
	points.clear();
	aligns.clear();
	
	QList<quint8> seats	= seatIdsOfPlayingUsers();
	int players	= seats.size();
		
	for ( int i = 0; i <= panelController()->numberOfSeats(); ++i ) {
		points 	<< QPoint();
		aligns	<< 0;
	}
	int space	= 100;
	foreach( quint8 seat, seats ) {
    	quint8 view = seat2View(seat);
    	if ( 3 == players  || 4 == players  ) {
    		switch( view ) {
        	case 1: {
        		points[seat]	= centerPoint();
        		points[seat].ry()	+= space;
    			aligns[seat]	= Qt::AlignCenter;
        		break;
        	} 
        	case 2:  {
        		points[seat]	= centerPoint();
        		points[seat].rx()	+= space;
    			aligns[seat]	= Qt::AlignCenter;
        		break;
        	}
        	case 3:  {
        		points[seat]	= centerPoint();
        		points[seat].ry()	-= space;
    			aligns[seat]	= Qt::AlignCenter;
        		break;
        	}
        	case 4:  {
        		points[seat]	= centerPoint();
        		points[seat].rx()	-= space;
    			aligns[seat]	= Qt::AlignCenter;
        		break;
        	}
        	default:
        		break;
          	}
    	}
    }
}
void DDZDesktopController::handleScoreCalled( int score )
{
	djDebug()<<"DDZDesktopController::handleScoreCalled"<<score;
	
	quint8 fen	= score;
	QByteArray buf;
	buf.append( fen );
	sendGameTrace(DOUDZHU_GAMETRACE_CALL,buf);
}
QByteArray DDZDesktopController::createThrowTrace( DesktopItems& items )
{	
	djDebug()<<"DDZDesktopController::createThrowTrace";
	
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

	if( DJGAME_DOUDZHU_TABLE_STATUS_WAIT_THROW == gameWaitStatus() ) {
		ptrace->chType = DOUDZHU_GAMETRACE_THROW;
		return QByteArray( buffer, sizeof(GeneralGameTrace2Head) + ptrace->chBufLen );
	}else {
		return QByteArray();
	}
}
bool DDZDesktopController::isChuEnabled() const
{
	djDebug()<<"DDZDesktopController::isChuEnabled";
	return m_throw->isEnabled() && !isWaitingThrowAcl();
}
bool DDZDesktopController::isQiEnabled() const
{
	return m_pass->isEnabled();
}
DJPoker DDZDesktopController::higherSerialPoker( const DJPoker& poker, bool isStrict ) const
{
	if ( DJPoker::Num2 == poker.point() )
		return DJPoker();
	else
		return DJDesktopPokerController::higherSerialPoker(poker,isStrict);
		
}
DJPoker DDZDesktopController::lowerSerialPoker( const DJPoker& poker, bool isStrict ) const
{
	if ( DJPoker::Num3 == poker.point() )
		return DJPoker();
	else
		return DJDesktopPokerController::lowerSerialPoker(poker,isStrict);
}
DJPoker DDZDesktopController::greaterPoker( const DJPoker& poker ) const
{
	if ( !poker.isValid() )
		return DJPoker( DJPoker::AnySuit, DJPoker::Num3 );
	if ( DJPoker::Num2 == poker.point() )
		return DJPoker( DJPoker::Joker, DJPoker::SmallJoker );
	else if ( DJPoker::SmallJoker == poker.point() ) {
		return DJPoker( DJPoker::Joker, DJPoker::BigJoker );
	}else if ( DJPoker::BigJoker == poker.point() ) {
		return DJPoker();
	}else if ( DJPoker::Ace == poker.point() ) {
		return DJPoker( DJPoker::AnySuit, DJPoker::Num2 );
	}else
		return higherSerialPoker( poker, false );
}
QList<DJPoker> DDZDesktopController::findMajor( const QHash<DJPoker,int>& countPokers,
									const DJPoker& otherKeyPoker, 
									int width, int length, 
									QSet<DJPoker>& excluder ) const
{
	djDebug()<<"getMajor"<<otherKeyPoker.description()<<width<<length;
	QList<DJPoker> pokers;
	DJPoker keyPoker	= otherKeyPoker;
	bool foundTip	= false;
	while( !foundTip ) {
		keyPoker	= greaterPoker( keyPoker );
		djDebug()<<"exact keyPoker"<<keyPoker.description();
		if ( !keyPoker.isValid() )
			break;
		if ( excluder.contains(keyPoker) )
			continue;
		QList<DJPoker> shun	= findShun( countPokers, keyPoker,
													width, length,
													true, false, true );
		if ( !shun.isEmpty() ) {
			djDebug()<<"found shun";
			excluder.insert( keyPoker );
			foundTip	= true;
			pokers << shun;
		}
	}
	if ( foundTip )
		return pokers;
	
	keyPoker	= otherKeyPoker;
	while( !foundTip ) {
		keyPoker	= greaterPoker( keyPoker );
		djDebug()<<"nonexact keyPoker"<<keyPoker.description();
		if ( !keyPoker.isValid() )
			break;
		if ( excluder.contains(keyPoker) )
			continue;
		QList<DJPoker> shun	= findShun( countPokers, keyPoker,
													width, length,
													true, false, false );
		if ( !shun.isEmpty() ) {
			djDebug()<<"found shun";
			excluder.insert( keyPoker );
			foundTip	= true;
			pokers << shun;
		}
	}
	if ( foundTip )
		return pokers;
	else
		return QList<DJPoker>();
}
QList<DJPoker> DDZDesktopController::findMinor( const QHash<DJPoker,int>& countPokers, int width, 
										int length, QSet<DJPoker>& excluder ) const
{
	QList<DJPoker> pokers;
	DJPoker keyPoker	= DJPoker();
	bool foundTip	= true;
	djDebug()<<"find exactly equal width"<<length;
	while( length ) {
		keyPoker	= greaterPoker( keyPoker );
		djDebug()<<"keyPoker"<<keyPoker.description();
		if ( !keyPoker.isValid() ) {
			break;
		}
		if ( excluder.contains(keyPoker) )
			continue;
		QList<DJPoker> group	= findGroup( countPokers, keyPoker,
										width, false, true );
		if ( group.isEmpty() )
			continue;
		excluder.insert( keyPoker );
		pokers << group;
		length--;
	}
	
	djDebug()<<"find greater equal width"<<length;
	keyPoker	= DJPoker();
	while( length ) {
		keyPoker	= greaterPoker( keyPoker );
		djDebug()<<"keyPoker"<<keyPoker.description();
		if ( !keyPoker.isValid() ) {
			foundTip	= false;
			break;
		}
		if ( excluder.contains(keyPoker) )
			continue;
		QList<DJPoker> group	= findGroup( countPokers, keyPoker,
										width, false, false );
		if ( group.isEmpty() )
			continue;
		pokers << group;
		length--;
	}
	if ( foundTip )
		return pokers;
	else
		return QList<DJPoker>();
}
void DDZDesktopController::clickTip()
{
	djDebug()<<"DDZDesktopController::clickTip";

	if ( m_tipHandPokers.isEmpty() ) {
		//first tip
		djDebug()<<"begin tip";
		DesktopItems &items	= desktopItems( panelController()->seatId(), POKER_TYPE_HAND );
		m_tipHandPokers	= pokersFromDesktopItems( items );	
		m_tipCountPokers	= createCountPokers( m_tipHandPokers );
		m_tipMajorExcluder.clear();
		m_tipBombExcluder.clear();
		m_tipRocket	= false;
	}
	
	DesktopItems &lastItems	= desktopItems( m_seatOfLastThrow, POKER_TYPE_THROWN );
	QList<DJPoker> lastPokers	= pokersFromDesktopItems( lastItems );
	QByteArray lastCards	= cardsFromPokers( lastPokers );
	
	djDebug()<<"last thrown pokers patterns";
	QList<DJPokerPattern> patterns	= DJPokerPattern::fromPokers( this, lastPokers, false );
	foreach( DJPokerPattern pattern, patterns ) {
		djDebug()<<pattern.description();
	}

	//there are two patterns in thrown pokers at most
	//major pattern
	DJPokerPattern pattern	= patterns.takeFirst();
	int majorWidth	= pattern.width();
	int majorLength	= pattern.length();
	DJPoker	otherMajorKeyPoker	= pattern.keyPoker();
	QList<DJPoker> otherMajorPokers	= pattern.pokers();
	
	int minorWidth	= 0;
	if ( !patterns.isEmpty() ) {
    	//minor pattern
    	pattern	= patterns.takeFirst();
		minorWidth	= pattern.width();
	}
	DJPoker selfMajorKeyPoker	= otherMajorKeyPoker;
	djDebug()<<"otherMajorKeyPoker"<<otherMajorKeyPoker.description();
	while( true ) {
		
		QList<DJPoker> selfMajorPokers	= findMajor( m_tipCountPokers, otherMajorKeyPoker,
													majorWidth, majorLength,
													m_tipMajorExcluder );
		if ( selfMajorPokers.isEmpty() )
			break;
		
		QList<DJPoker> selfMinorPokers;
		
		if ( 0 != minorWidth ) {
			QSet<DJPoker> tipMinorExcluder;
    		DJPoker selfMinorKeyPoker	= selfMajorKeyPoker;
    		//minor exlucder inherits from major exluder
    		for ( int i = 0; i < majorLength; ++i ) {
    			tipMinorExcluder.insert( selfMinorKeyPoker );
    			selfMinorKeyPoker	= higherSerialPoker( selfMinorKeyPoker, false );
    		}
    		selfMinorPokers	= findMinor( m_tipCountPokers, minorWidth,
    													majorLength, tipMinorExcluder );
    		if ( selfMinorPokers.isEmpty() ) {
    			//no minor match
				//don't break,there may be matching group above this
    			continue;
    		}
		}
		selectSelfHandPokers( selfMajorPokers + selfMinorPokers );
		return;
	}
	
	//iterate bomb
	int bombWidth;
	PDoudzhuRoom pddzroom = (PDoudzhuRoom)((panelController()->gameRoom())->privateRoom());
	if( !ISBOMB(&(pddzroom->rule),m_lastThrowType) )
		bombWidth	= pddzroom->rule.chBombPages;
	else {
		//bombs that width == majorWidth have been iterated by last pattern
		bombWidth	= majorWidth + 1;
	}
	DJPoker bombKeyPoker	= DJPoker();
	while( true ) {
		bombKeyPoker	= greaterPoker( bombKeyPoker );
		if ( !bombKeyPoker.isValid() )
			break;
		if ( m_tipBombExcluder.contains(bombKeyPoker) )
			continue;
		m_tipBombExcluder.insert( bombKeyPoker );
		QList<DJPoker> bombPokers	= findGroup( m_tipCountPokers, bombKeyPoker,
												bombWidth, false, false );		
		if ( bombPokers.isEmpty() )
			continue;
		selectSelfHandPokers( bombPokers );
		return;
	}
	//iterate rocket
	int deck	= pddzroom->rule.chCardPages;
	if ( !m_tipRocket
		&& deck == m_tipCountPokers.value(DJPoker((int)DJPoker::Joker,(DJPoker::Point)DJPoker::SmallJoker))
		&& deck == m_tipCountPokers.value(DJPoker((int)DJPoker::Joker,(DJPoker::Point)DJPoker::BigJoker)) ) {
		QList<DJPoker> rocketPokers;
		while( deck-- ) {
			rocketPokers << DJPoker((int)DJPoker::Joker,(DJPoker::Point)DJPoker::SmallJoker)
						<< DJPoker((int)DJPoker::Joker,(DJPoker::Point)DJPoker::BigJoker);
		}
		m_tipRocket	= true;
		selectSelfHandPokers( rocketPokers );
		return;		
	}
	//iteration end
	//reset to start iteration again
	m_tipHandPokers.clear();
	selectSelfHandPokers();
}
void DDZDesktopController::clickThrow()
{
	djDebug()<<"DDZDesktopController::clickThrow";
	if( isChuEnabled() ) {
		DesktopItems selectedItems	= selectedDesktopItems( panelController()->seatId(), POKER_TYPE_HAND );
		throwPokerItems( selectedItems );
	}
}
void DDZDesktopController::clickArrange()
{
	djDebug()<<"DDZDesktopController::clickArrange";
	m_styleOfArrange++;
	m_styleOfArrange = m_styleOfArrange % 2;
	if ( 0 == m_styleOfArrange )
		setPokerSortMode( ASCountPointSuit );
	else
		setPokerSortMode( PointASCountSuit );
	repaintHandPokers( panelController()->seatId(), POKER_TYPE_HAND, false, true, true );
	for( quint8 seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
		repaintThrownPokers( seat, POKER_TYPE_THROWN );
	}
}
void DDZDesktopController::clickPass()
{
	djDebug()<<"DDZDesktopController::clickPass";
	if ( isQiEnabled() ) {
		sendGameTrace(DOUDZHU_GAMETRACE_THROW);
	}
}
void DDZDesktopController::throwSelectedCards()
{
	djDebug()<<"DDZDesktopController::throwSelectedCards";
	DesktopItems selectedItems	= selectedDesktopItems( panelController()->seatId(), POKER_TYPE_HAND );
	djDebug()<<"selectedItem.size"<<selectedItems.size();
	
	if ( selectedItems.isEmpty() ) {
		clickPass();
	}else {
		throwPokerItems( selectedItems );
	}
}
void DDZDesktopController::fillPlayerInfo()
{
	djDebug()<<"DDZDesktopController::fillPlayerInfo";
	
	for( quint8 seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
		DJGraphicsTextItem *info	= playerInfoItem(seat);
		int remain	= numberOfPokers( seat, POKER_TYPE_HAND );
		info->setPlainText( tr("%1 left").arg(remain) );
	}
}
QString	DDZDesktopController::userItemNameSuffix( DJGameUser* user ) const
{
	djDebug()<<"DDZDesktopController::userItemNameSuffix"<<m_current.chMaster;
	if ( 0 != m_current.chMaster 
		&& user->seatId() == m_current.chMaster ) {
		return QString("(")+tr("landlord")+QString(")");
	}
	return DJDesktopController::userItemNameSuffix(user);
}
QString DDZDesktopController::bonusTarget( const DJGameBonus* bonus )
{
	QString text;
	switch( bonus->chTarget ) {
	case DOUDZHU_GAMEBONUS_TARGET_LASTCARD: {
		quint8 value	= bonus->chParam1;
		text	= tr("The last thrown cards contains %1").arg(value);
		break;
	}
	default:
		break;
	}
	return text;
}
