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

#include "DJDesktopController.h"
#include "DJPanelController.h"
#include "DJPanel.h"
#include "DJGameController.h"
#include "DJGameRoom.h"
#include "DJDesktop.h"
#include "DJDesktopScene.h"
#include "DJGameUser.h"
#include "DJShowView.h"
#include "DJGraphicsTextItem.h"
#include "DJGraphicsTimerItem.h"
#include "DJGraphicsPixmapItem.h"
#include "DJGraphicsChessTimerItem.h"
#include "DJGraphicsSvgItem.h"
#include "DJGraphicsLineItem.h"
#include "DJGraphicsRectItem.h"
#include "DJDesktopItem.h"
#include "Utility.h"
#include "DJClientRequest.h"
#include "DJWidget.h"
#include "DJCard.h"

static	qreal	Z_NAME			= 2000;
static	qreal	Z_INFO			= 2001;
static	qreal 	Z_CLOCK			= 3000;
static 	qreal 	Z_HOST			= 1000;

static 	int		TEXT_ITEM_FONT_SIZE	= 16;
static 	qreal	CLOCK_ITEM_SCALE	= 0.4;

DJDesktopController::DJDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent )
	:m_panelController(panelController)
{	
	djDebug() << "DJDesktopController constructor"<<size<<size;
	
	m_gameWaitStatus	= 0;
	m_gameWaitMask		= 0;
	m_gameWaitTimeout	= 0;
	
	m_desktop	= new DJDesktop( this, size, parent );
	connect(m_desktop,SIGNAL(matrixChanged()), SLOT(handleMatrixChanged()));
	
	m_btnStart = new QPushButton( m_desktop );
	m_btnStart->setText( tr("start") );
	m_btnStart->adjustSize();
	m_btnStart->hide();
	connect(m_btnStart,SIGNAL(clicked()),SLOT(handleStartClicked()));
	
	QFont font;
	font.setBold( true );
	font.setPointSize( TEXT_ITEM_FONT_SIZE );
	
	for ( int i = 0; i <= m_panelController->numberOfSeats(); ++i ) {
		
		DesktopTypeItems  typeItems;
		m_seatTypeItems  << typeItems;
		
		DJGameUser *player	= m_panelController->player(i);
		DJShowView *showView	= new DJShowView( player, m_desktop );
		connect( showView, SIGNAL(rightClicked(DJGameUser*)), m_panelController, SLOT(showUserContextMenu(DJGameUser*)) );
		
		showView->hide();
		m_showViews  << showView;
		
		QString name;
		if ( player ) {
			name	= player->userName();
			connect( player, SIGNAL(reloaded(DJGameUser*)), m_panelController, SLOT(showUserContextMenu(DJGameUser*)) );
		}
		DJGraphicsTextItem	*nameItem	= new DJGraphicsTextItem( name, 0, m_desktop->scene() );
		nameItem->setFont( font );
		nameItem->setZValue( Z_NAME );
		nameItem->hide();
		m_nameItems  << nameItem;
		
		DJGraphicsTextItem	*infoItem	= new DJGraphicsTextItem( 0, m_desktop->scene() );
		infoItem->setFont( font );
		infoItem->setZValue( Z_INFO );
		infoItem->hide();
		m_infoItems  << infoItem;
	}
	
	m_clockItem 	= new DJGraphicsTimerItem( CLOCK_ITEM_SCALE, 0, m_desktop->scene() );
	m_clockItem->setZValue( Z_CLOCK );
	m_clockItem->hide();
	
	m_hostItem	= new DJGraphicsPixmapItem( QPixmap( gameController()->gameResDir() + "/image/base/game.png"), 0, m_desktop->scene() );
	m_hostItem->setZValue( Z_HOST );
	m_hostItem->hide();
	//m_hostEnabled	= false;
	
	/*	
	m_bonusWidget	= new DJWidget( m_desktop, DJWidget::Pixmap );
	m_bonusWidget->setPixmap( QPixmap( ":/BaseRes/image/bonus/background.png") );
	QVBoxLayout* layout	= new QVBoxLayout( m_bonusWidget );
	layout->setMargin( 5 );
	layout->setSpacing( 0 );
	m_bonusTargetWidget	= new QLabel;
	layout->addWidget( m_bonusTargetWidget );
	m_bonusResultWidget	= new QLabel;
	layout->addWidget( m_bonusResultWidget );
	m_bonusWidget->hide();	
	*/
	m_bonusTargetText	= new DJGraphicsTextItem( tr("Bonus Target:"), 0, desktop()->scene(), false );
	m_bonusTargetText->setFont( font );
	m_bonusTargetText->setTextWidth( 120 );
	//m_bonusTargetText->setAlignment( Qt::AlignRight );
	m_bonusTargetText->hide();
	m_bonusAwardText	= new DJGraphicsTextItem( tr("Bonus Award:"), 0, desktop()->scene(), false );
	m_bonusAwardText->setFont( font );
	//m_bonusAwardText->setAlignment( Qt::AlignRight );
	m_bonusAwardText->hide();
	
	QPixmap pixBackground	= QPixmap(gameController()->gameResDir() + "/image/base/background.png");
	if ( !pixBackground.isNull() ) {
		pixBackground	= pixBackground.scaled( desktop()->realWidth(),
							desktop()->realHeight(),
							Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
		desktop()->setBackgroundBrush( QBrush(pixBackground) );
	}
	
	connect( m_desktop->desktopScene(),SIGNAL(mousePressed(QGraphicsSceneMouseEvent*)),SLOT(handleMousePressed(QGraphicsSceneMouseEvent*)));
	connect( m_desktop->desktopScene(),SIGNAL(mouseReleased(QGraphicsSceneMouseEvent*)),SLOT(handleMouseReleased(QGraphicsSceneMouseEvent*)));
	connect( m_desktop->desktopScene(),SIGNAL(mouseDoubleClicked(QGraphicsSceneMouseEvent*)),SLOT(handleMouseDoubleClicked(QGraphicsSceneMouseEvent*)));
	connect( m_desktop->desktopScene(),SIGNAL(mouseMoving(QGraphicsSceneMouseEvent*)),SLOT(handleMouseMoving(QGraphicsSceneMouseEvent*)));
}
DJDesktopController::~DJDesktopController()
{
	djDebug() << "DJDesktopController destructor";
	delete m_desktop;
	m_desktop	= 0;
}
DJGameController* DJDesktopController::gameController() const
{
	return m_panelController->gameController();
}
void DJDesktopController::gameInfo( const QByteArray& buf )
{
	djDebug()<<"DJDesktopController::gameInfo";
	
	for ( int i = 0; i <= m_panelController->numberOfSeats(); ++i ) {	
		clearDesktopItems(i,0);
	}
	m_bonusTargetText->hide();
	m_bonusAwardText->hide();
	
	initUnderGameInfo( buf );
}
void DJDesktopController::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	djDebug()<<"DJDesktopController::gameWait";
	
	if ( 0 == m_gameWaitStatus ) {
		repaintCurrentStatus();
	}
	m_gameWaitStatus = status;
	m_gameWaitMask = mask;
	m_gameWaitTimeout 	= timeout;
	
	setClockTimeout( mask, timeout );
	
	switch(status)
	{
		case DJGAME_TABLE_STATUS_WAITSTART :
			if(isWaitingForMe() && !panelController()->isLookingOn() )
				m_btnStart->show();
			break;
		default :
			m_btnStart->hide();
			break;
	}
}
void DJDesktopController::gameTrace( const GeneralGameTrace2Head *gameTraceHead )
{
	djDebug()<<"DJDesktopController::gameTrace";
	gameTraceModel( gameTraceHead );
	if ( 0 != m_gameWaitStatus )
		gameTraceView( gameTraceHead );
}
void DJDesktopController::gameTraceModel( const GeneralGameTrace2Head *gameTraceHead )
{
	djDebug()<<"DJDesktopController::gameTraceModel";
}
void DJDesktopController::gameTraceView( const GeneralGameTrace2Head *gameTrace )
{
	djDebug()<<"DJDesktopController::gameTraceView";
}
void DJDesktopController::initUnderGameInfo( const QByteArray& buf )
{
	djDebug()<<"DJDesktopController::initUnderGameInfo";
}
void DJDesktopController::repaintCurrentStatus()
{
	djDebug()<<"DJDesktopController::repaintCurrentStatus";
}
void DJDesktopController::magicResult( const MagicResult* magicResult )
{
}
void DJDesktopController::gameOver( const DJGameOver* gameOver )
{
	djDebug()<<"DJDesktopController::gameOver";
	
	QString resultOfScore,resultOfWL;
	for( int i = 0; i < gameOver->chSites; i++ ) {
		if( gameOver->site[i].chSite == DJGAME_GAMEOVER_STATUS_SITES ) {
			PDJGameOverStatus poverstatus = (PDJGameOverStatus)&(gameOver->site[i]);
			QList<quint8> seats	= mask2Seat( letoh2(poverstatus->shWinSites) );
			foreach( quint8 seat, seats ) {
				DJGameUser *puser	= m_panelController->player( seat );
				if ( puser ) {
					resultOfWL	+= puser->userName()
							+ userItemNameSuffix( puser )
							+ " "
							+ m_panelController->gameController()->baseSectionName(DJGameController::Wins)
						 	+ "<br>";
				}
			}
			seats	= mask2Seat( letoh2(poverstatus->shLoseSites) );
			foreach( quint8 seat, seats ) {
				DJGameUser *puser	= m_panelController->player( seat );
				if ( puser ) {
					resultOfWL	+= puser->userName()
							+ userItemNameSuffix( puser )
							+ " "
							+ m_panelController->gameController()->baseSectionName(DJGameController::Loses)
						 	+ "<br>";
				}
			}
		}else {
			DJGameUser *puser	= m_panelController->player(gameOver->site[i].chSite);
			int score = letoh4(gameOver->site[i].score);
			m_panelController->deltaUserData( puser, score );
			
			if( puser ) {
				resultOfScore	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( puser->userName() + userItemNameSuffix( puser ) )
								.arg( score );
			}
		}
	}
	m_panelController->insertText2Browser( resultOfWL );
	m_panelController->insertGameResult2Browser( resultOfScore );
}
QString DJDesktopController::bonusTarget( const DJGameBonus* bonus )
{
	return QString();
}
void DJDesktopController::bonusCondition( const DJGameBonus* condition )
{
	djDebug()<<"DJDesktopController::bonusCondition";
	/*
	QString target	= bonusTarget( condition );
	m_bonusTargetWidget->setText( tr("bonus target :") + "\n" + target );
    quint16	bonus	= letoh2( condition->shBonus );
    QString text	= tr("bonus :") + "\n";
    DJGameController *gc = m_panelController->gameController();
    switch( condition->chBonusType ) {
    case DJGAME_PROTOCOL_GAMEBONUS_TYPE_CARD:
    	text	+= DJCard::name(bonus);
    	break;
    case DJGAME_PROTOCOL_GAMEBONUS_TYPE_COUNTER:
    	text	+= QString::number( bonus );
		text	+= " ";
		text	+= gc->baseSectionName(DJGameController::Chips);
    	break;
    case DJGAME_PROTOCOL_GAMEBONUS_TYPE_MONEY:
    	text	+= QString::number( bonus );
		text	+= " ";
		text	+= gc->baseSectionName(DJGameController::Money);
		break;
    default:
    	text	+= tr("unknown bonus");
    	break;
    }
    m_bonusResultWidget->setText( text );
    m_bonusWidget->adjustSize();
    m_bonusWidget->move( desktop()->viewport()->width() - m_bonusWidget->width(),2);
    m_bonusWidget->show();
    */
    QString target	= bonusTarget( condition );
	m_bonusTargetText->setPlainText( tr("Bonus Target:") + "\n" + target );
	m_bonusTargetText->setVirtualPos( QPoint(desktop()->viewport()->width() - m_bonusTargetText->boundingRect().toRect().width(), 0) );
	m_bonusTargetText->adjustPos( desktop()->graphicsMatrix() );
	m_bonusTargetText->show();
	
    quint16	bonus	= letoh2( condition->shBonus );
    QString text	= tr("Bonus Award:") + "\n";
    DJGameController *gc = m_panelController->gameController();
    switch( condition->chBonusType ) {
    case DJGAME_PROTOCOL_GAMEBONUS_TYPE_CARD:
    	text	+= DJCard::name(bonus);
    	break;
    case DJGAME_PROTOCOL_GAMEBONUS_TYPE_COUNTER:
    	text	+= QString::number( bonus );
		text	+= " ";
		text	+= gc->baseSectionName(DJGameController::Chips);
    	break;
    case DJGAME_PROTOCOL_GAMEBONUS_TYPE_MONEY:
    	text	+= QString::number( bonus );
		text	+= " ";
		text	+= gc->baseSectionName(DJGameController::Money);
		break;
    default:
    	text	+= tr("unknown bonus");
    	break;
    }
    m_bonusAwardText->setPlainText( text );
    m_bonusAwardText->setVirtualPos( QPoint(desktop()->viewport()->width() - m_bonusTargetText->boundingRect().width(), m_bonusTargetText->boundingRect().height()) );
	m_bonusAwardText->adjustPos( desktop()->graphicsMatrix() );
	m_bonusAwardText->show();
}
void DJDesktopController::bonusFulfilled( const DJGameBonusFulfill* fulfill )
{
	djDebug()<<"DJDesktopController::bonusFulfilled";
}
bool DJDesktopController::isCloseable() const
{
	if( DJGAME_TABLE_STATUS_WAITSTART == m_gameWaitStatus && isWaitingForMe())
		return true;
	return false;
}
void DJDesktopController::locateAll()
{
	djDebug()<<"DJDesktopController::locateAll";
	locateCenterPoint( m_centerPoint );
	
	locateShowPositions( m_showPoints, m_showAligns );
	locateNamePositions( m_namePoints, m_nameAligns );
	fillPlayerInfo();
	locateInfoPositions( m_infoPoints, m_infoAligns );
	locateClockPosition( m_clockPoint, m_clockAlign );
	locateHostPositions( m_hostPoints, m_hostAligns );
	
	locateAvailableSpace( m_availableSpace );
	
	locateAllNonMatrimoveItems();
	
	djDebug()<<"m_showPoints"<<m_showPoints;
	djDebug()<<"m_namePoints"<<m_namePoints;
	djDebug()<<"m_clockPoint"<<m_clockPoint;
	djDebug()<<"m_hostPoints"<<m_hostPoints;
}
void DJDesktopController::locateAllNonMatrimoveItems()
{
	locateStartButton();
	locateBonusPosition();
}
void DJDesktopController::locateCenterPoint( QPoint& point )
{
	djDebug()<<"DJDesktopController::locateCenterPoint";
	
	point.setX( desktop()->realWidth() >> 1 );
	point.setY( desktop()->realHeight() >> 1 );
}
void DJDesktopController::locateShowPositions( QList<QPoint>& points, QList<Qt::Alignment>& aligns )
{
	djDebug() << "DJDesktopController::locateShowPositions";
	
	points.clear();
	aligns.clear();
		
	int leftX	= 1;
	int rightX	= m_desktop->realWidth();
	int topY	= 1;
	int bottomY	= m_desktop->realHeight(); 
	
	for ( int i = 0; i <= panelController()->numberOfSeats(); ++i ) {
		points 	<< QPoint();
		aligns	<< 0;
	}
	QList<quint8> seats	= seatIdsOfPlayingUsers();
	int players	= seats.size();
	djDebug()<<"players"<<players<<"seatIdsOfPlayingUsers="<<seats;
	
    foreach( quint8 seat, seats ) {
 		quint8 view = seat2View(seat);
 		djDebug()<<"seat"<<seat<<"map to view"<<view;
 		if ( 3 == players  || 4 == players  ) {
    		switch( view ) {
        	case 1: {
        		//points[seat].setX( leftX );
        		//points[seat].setY( bottomY - 1 );
        		//aligns[seat]	= Qt::AlignBottom;
        		break;
        	} 
        	case 2: {
        		points[seat].setX( leftX );
        		points[seat].setY( centerPoint().y() );
        		aligns[seat]	= Qt::AlignVCenter;
        		break;
        	}
        	case 3:  {
        		points[seat].setX( centerPoint().x() );
        		points[seat].setY( topY );
        		aligns[seat]	= Qt::AlignHCenter;
        		break;
        	}
        	case 4:  {
        		points[seat].setX( rightX );
        		points[seat].setY( centerPoint().y() );
        		aligns[seat]	= Qt::AlignRight | Qt::AlignVCenter;
        		break;
        	}
        	default:
        		break;
          	}
        }else if ( 2 == players ) {
    		if ( 1 == view ) {
    			points[seat].setX( rightX );
        		points[seat].setY( bottomY );
        		aligns[seat]	= Qt::AlignRight | Qt::AlignBottom;
        	}else {
        		points[seat].setX( rightX );
        		points[seat].setY( topY );
        		aligns[seat]	= Qt::AlignRight;
        	}
    	}else if ( 1 == players ){
 			//points[seat].setX( leftX );
        	//points[seat].setY( topY );
 		}
 	}
}
void DJDesktopController::locateNamePositions( QList<QPoint>& points, QList<Qt::Alignment>& aligns )
{
	djDebug() << "DJDesktopController::locateNamePositions";
	
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
     	djDebug()<<"showRect"<<"seat"<<seat<<showRect;
    	quint8 view = seat2View(seat);
    	if ( 3 == players  || 4 == players  ) {
    		switch( view ) {
        	case 1: {
        		points[seat].setX( desktop()->realWidth()>>1 );
    			points[seat].setY( desktop()->realHeight() - 1 );
    			aligns[seat]	= Qt::AlignHCenter | Qt::AlignBottom;
        		break;
        	} 
        	case 2:  {
        		points[seat].setX( showRect.right() - 1 );
        		points[seat].setY( showRect.bottom() + 1 );
        		aligns[seat]	= Qt::AlignRight;
        		break;
        	}
        	case 3:  {
        		points[seat].setX( showRect.left() - 1 );
        		points[seat].setY( showRect.top() + 1 );
        		aligns[seat]	= Qt::AlignRight;
        		break;
        	}
        	case 4:  {
        		points[seat].setX( showRect.left() + 1 );
        		points[seat].setY( showRect.bottom() + 1 );
        		break;
        	}
        	default:
        		break;
          	}
    	}else if ( 2 == players ) {
    		if ( 1 == view ) {
        		points[seat].setX( showRect.left() - 1 );
        		points[seat].setY( showRect.bottom() - 1 );
        		aligns[seat]	= Qt::AlignRight | Qt::AlignBottom;
        	}else {
        		points[seat].setX( showRect.left() - 1 );
        		points[seat].setY( showRect.top() + 1 );
        		aligns[seat]	= Qt::AlignRight;
        	}
        	
    	}else if ( 1 == players ) { 
    		//points[seat].setX( showRect.left() );
            //points[seat].setY( showRect.bottom() );
		}
    }
}
void DJDesktopController::locateInfoPositions( QList<QPoint>& points, QList<Qt::Alignment>& aligns )
{
	djDebug() << "DJDesktopController::locateInfoPositions";
	
	points.clear();
	aligns.clear();
	
	QList<quint8> seats	= seatIdsOfPlayingUsers();
	int players	= seats.size();
		
	for ( int i = 0; i <= panelController()->numberOfSeats(); ++i ) {
		points 	<< QPoint();
		aligns	<< 0;
	}
	
	foreach( quint8 seat, seats ) {
     	QRect nameRect	= rectOfSeatName( seat );
     	djDebug()<<"nameRect"<<"seat"<<seat<<nameRect;
    	quint8 view = seat2View(seat);
    	if ( 3 == players  || 4 == players  ) {
    		switch( view ) {
        	case 1: {
        		points[seat]	= nameRect.topRight();
        		points[seat].rx()	+= 10;
    			break;
        	} 
        	case 2:  {
        		points[seat]	= nameRect.bottomRight();
    			aligns[seat]	= Qt::AlignRight;
        		break;
        	}
        	case 3:  {
        		points[seat]	= nameRect.bottomRight();
    			aligns[seat]	= Qt::AlignRight;
        		break;
        	}
        	case 4:  {
        		points[seat]	= nameRect.bottomLeft();
        		break;
        	}
        	default:
        		break;
          	}
    	}else if ( 2 == players ) {
    		if ( 1 == view ) {
        		points[seat].setX( nameRect.right() - 1 );
        		points[seat].setY( nameRect.top() - 1 );
        		aligns[seat]	= Qt::AlignRight | Qt::AlignBottom;
        	}else {
        		points[seat].setX( nameRect.right() - 1 );
        		points[seat].setY( nameRect.bottom() + 1 );
        		aligns[seat]	= Qt::AlignRight;
        	}
    	}else if ( 1 == players ) { 
    		//points[seat].setX( showRect.left() );
            //points[seat].setY( showRect.bottom() );
		}
    }
}
void DJDesktopController::locateClockPosition( QPoint& point, Qt::Alignment& align )
{
	djDebug() << "DJDesktopController::locateClockPosition";
	
	QList<quint8> seats	= seatIdsOfPlayingUsers();
	int players	= seats.size();
	
	if ( players > 2 ) {		
    	point	= centerPoint();
    	align	= Qt::AlignCenter;
	}else {
		point.setX( desktop()->realWidth() );
    	point.setY( centerPoint().y() );
    	align	= Qt::AlignRight | Qt::AlignVCenter;
	}
}
void DJDesktopController::locateHostPositions( QList<QPoint>& points, QList<Qt::Alignment>& aligns )
{
	djDebug()<<"DJDesktopController::locateHostPositions";
	points.clear();
	aligns.clear();
	
	QList<quint8> seats	= seatIdsOfPlayingUsers();
	int players	= seats.size();
		
	for ( int i = 0; i <= panelController()->numberOfSeats(); ++i ) {
		points 	<< QPoint();
		aligns 	<< 0;
	}
	
	int space	= 24;
	foreach( quint8 seat, seats ) {
		QRect	showRect	= rectOfSeatShow( seat );
    	quint8 view = seat2View(seat);
    	if ( 3 == players  || 4 == players  ) {
        	switch( view ) {
        	case 1: {
        		QRect nameRect	= rectOfSeatName( seat );
        		points[seat].setX( nameRect.left() - space );
        		points[seat].setY( desktop()->realHeight() - 1 );
        		aligns[seat]	= Qt::AlignRight | Qt::AlignBottom;
        		break;
        	} 
        	case 2:  {
        		points[seat].setX( showRect.right() + space );
        		points[seat].setY( showRect.center().y() );
        		aligns[seat]	= Qt::AlignVCenter;
        		break;
        	}
        	case 3:  {
        		points[seat].setX( showRect.center().x() );
        		points[seat].setY( showRect.bottom() + space );
        		aligns[seat]	= Qt::AlignHCenter;
        		break;
        	}
        	case 4:  {
        		points[seat].setX( showRect.left() - space );
        		points[seat].setY( showRect.center().y() );
        		aligns[seat]	= Qt::AlignRight | Qt::AlignVCenter;
        		break;
        	}
        	default:
        		break;
          	}
    	}else if ( 2 == players ){
    		if ( 1 == view ) {
        		points[seat].setX( desktop()->realWidth() >> 1 );
        		points[seat].setY( desktop()->realHeight() - 1 );
        		aligns[seat]	= Qt::AlignHCenter | Qt::AlignBottom;
        	}else {
        		points[seat].setX( desktop()->realWidth() >> 1 );
        		points[seat].setY( 1 );
        		aligns[seat]	= Qt::AlignHCenter;
        	}
    	}
	}
}
void DJDesktopController::locateAvailableSpace( QRect& rect )
{
	DJShowView *showView	= m_showViews.at(0);
	QSize s	= showView->realSize();
	
	rect.setBottom( desktop()->realHeight() - 40 );
	rect.setLeft( s.width() );
	rect.setTop( s.height() );
	rect.setRight( desktop()->realWidth() - s.width() );
}
void DJDesktopController::locateStartButton()
{
	QMatrix matrix	= desktop()->graphicsMatrix();
	QPoint pos	= centerPoint();
	pos.ry()	+= 48;
	pos	= matrix.map( pos );
	pos.rx()	-= m_btnStart->width()>>1;
	m_btnStart->move( pos );
}
void DJDesktopController::locateBonusPosition()
{
	m_bonusTargetText->setVirtualPos( QPoint(desktop()->viewport()->width() - m_bonusTargetText->boundingRect().toRect().width(), 0) );
	m_bonusTargetText->adjustPos( desktop()->graphicsMatrix() );
	m_bonusAwardText->setVirtualPos( QPoint(desktop()->viewport()->width() - m_bonusTargetText->boundingRect().width(), m_bonusTargetText->boundingRect().height()) );
	m_bonusAwardText->adjustPos( desktop()->graphicsMatrix() );
}
void DJDesktopController::repaintAll()
{
	repaintAllPlayers();
	repaintPlayerInfo();
	repaintClock();
	repaintHost();
}
void DJDesktopController::reloadPlayer( DJGameUser *player )
{
  	if ( 0 == player )
  		return;
  	quint8 table	= player->tableId();
  	if ( table != panelController()->tableId() )
  		return;
  	quint8 seat	= player->seatId();	
  	DJShowView	*showView	= m_showViews.value( seat, 0 );
  	if ( 0 == showView )
  		return;
  	showView->setUser( player );
  	
  	DJGraphicsTextItem	*nameItem	= m_nameItems.value( seat, 0 );
  	if ( 0 == nameItem )
  		return;
  	nameItem->setPlainText( player->userName() );
}
void DJDesktopController::repaintAllPlayers()
{
	/*
	QList<DJGameUser*> players	= m_panelController->players();
	foreach( DJGameUser* player, players ) {
		if ( player )
			repaintPlayer( player->seatId() ); 
	}
	*/
	for ( quint8 seat = 1; seat <= m_panelController->numberOfSeats(); ++seat ) {
		repaintPlayer( seat );
	}
}
void DJDesktopController::repaintPlayer( quint8 seat )
{
	repaintShowView( seat );
	repaintNameItem( seat );
}
void DJDesktopController::repaintShowView( quint8 seat )
{
	djDebug()<<"DJDesktopController::repaintShowView"<<seat;
  	
  	DJShowView	*showView	= m_showViews.value( seat, 0 );
  	if ( 0 == showView ) {
  		djDebug()<<"showView is null";
  		return;
  	}
  	QPoint	pos	= m_showPoints.value( seat, QPoint() );
	if ( pos.isNull() ) {
		djDebug()<<"showView's pos is null";
		showView->hide();
	}else {
		showView->setVirtualPos( pos );
		Qt::Alignment	align	= m_showAligns.value( seat, 0 );
		showView->setViewAlignment( align );
		showView->adjustPos( desktop()->graphicsMatrix() );
		showView->show();
	}
}
void DJDesktopController::repaintNameItem( quint8 seat )
{
	djDebug()<<"DJDesktopController::repaintNameItem"<<seat;
	
	DJGraphicsTextItem	*nameItem	= m_nameItems.value( seat, 0 );
	if ( 0 == nameItem ) {
		djDebug()<<"nameItem is null";
		return;	
	}
	QPoint	pos	= m_namePoints.value( seat, QPoint() );
	if ( pos.isNull() ) {
		djDebug()<<"nameItem's pos is null";
		nameItem->hide();
	}else {
		djDebug()<<"nameItem's text"<<nameItem->toPlainText();
		nameItem->setVirtualPos( pos );
		Qt::Alignment	align	= m_nameAligns.value( seat, 0 );
		nameItem->setAlignment( align );
		nameItem->adjustPos( desktop()->graphicsMatrix() );
		nameItem->show();
	}
}
void DJDesktopController::repaintPlayerInfoItem( quint8 seat )
{
	djDebug()<<"DJDesktopController::repaintPlayerInfoItem"<<seat;
	
	DJGraphicsTextItem	*infoItem	= m_infoItems.value( seat, 0 );
	if ( 0 == infoItem )
		return;		
	QPoint	pos	= m_infoPoints.value( seat, QPoint() );
	if ( pos.isNull() ) {
		infoItem->hide();
	}else {
		infoItem->setVirtualPos( pos );
		Qt::Alignment	align	= m_infoAligns.value( seat, 0 );
		infoItem->setAlignment( align );
		infoItem->adjustPos( desktop()->graphicsMatrix() );
		infoItem->show();
	}
}
void DJDesktopController::repaintClock()
{
	djDebug()<<"DJDesktopController::repaintClock";
	
	if ( m_clockPoint.isNull() ) {
		m_clockItem->hide();
	}else {
		m_clockItem->setVirtualPos( m_clockPoint );
 		m_clockItem->setAlignment( m_clockAlign ); 
 		m_clockItem->adjustPos( desktop()->graphicsMatrix() );
		m_clockItem->show();
	}
}
void DJDesktopController::repaintHost()
{
}
void DJDesktopController::fillPlayerInfo()
{
}
void DJDesktopController::repaintPlayerInfo()
{
	for( quint8 seat = 1; seat <= panelController()->numberOfSeats(); ++seat ) {
		repaintPlayerInfoItem( seat );
	}
}
void DJDesktopController::repaintHostItem( quint8 seat )
{
	djDebug()<<"DJDesktopController::repaintHostItem"<<seat;
	
	if ( seat < 1 || seat > m_panelController->numberOfSeats() )
		return;
	//if ( !m_hostEnabled )
		//return;
	QPoint	pos	= m_hostPoints.value( seat, QPoint() );
	if ( pos.isNull() ) {
		m_hostItem->hide();
	}else {
		Qt::Alignment	align	= m_hostAligns.value( seat, 0 );
		m_hostItem->setAlignment( align );
		m_hostItem->setVirtualPos( pos );
		m_hostItem->adjustPos( desktop()->graphicsMatrix() );
		m_hostItem->show();
	}
}
void DJDesktopController::setCenterPoint( const QPoint& point )
{
	m_centerPoint	= point;
}
void DJDesktopController::setAvailableSpace( const QRect& availableSpace )
{
	m_availableSpace	= availableSpace;
}
void DJDesktopController::setShowPositions( const QList<QPoint>& points, const QList<Qt::Alignment>& aligns )
{
	m_showPoints	= points;
	m_showAligns	= aligns;
}
void DJDesktopController::setNamePositions( const QList<QPoint>& points, const QList<Qt::Alignment>& aligns )
{
	m_namePoints	= points;
	m_nameAligns	= aligns;
}
void DJDesktopController::setClockPosition( const QPoint& point, Qt::Alignment align )
{
	m_clockPoint	= point;
	m_clockAlign	= align;
}
void DJDesktopController::setHostPositions( const QList<QPoint>& points, const QList<Qt::Alignment>& aligns )
{
	m_hostPoints	= points;
	m_hostAligns	= aligns;
}
DJShowView* DJDesktopController::playerShowView( quint8 seat ) const
{ 
	return m_showViews.value( seat, 0 );
}
DJGraphicsTextItem* DJDesktopController::playerNameItem( quint8 seat ) const
{
	return m_nameItems.value( seat, 0 );
}
DJGraphicsTextItem* DJDesktopController::playerInfoItem( quint8 seat ) const
{
	return m_infoItems.value( seat, 0 );
}
QRect DJDesktopController::rectOfSeatShow( quint8 seat ) const
{
	DJShowView	*showView	= playerShowView( seat );
	if ( !showView )
		return QRect();
	QPoint	pos	= m_showPoints.value( seat, QPoint() );
	Qt::Alignment	align	= m_showAligns.value( seat, 0 );	
	QSize size	= showView->realSize();
	PosByAlign( pos, align, size );
	return QRect( pos, size );
}
QRect DJDesktopController::rectOfViewShow( quint8 view ) const
{
	quint8 seat	= view2Seat( view );
	return rectOfSeatShow( seat );
}
QRect DJDesktopController::rectOfSeatName( quint8 seat ) const
{
	DJGraphicsTextItem	*nameItem	= playerNameItem( seat );
	if ( !nameItem )
		return QRect();
	QPoint	pos	= m_namePoints.value( seat, QPoint() );
	Qt::Alignment	align	= m_nameAligns.value( seat, 0 );	
	QSize size	= nameItem->boundingRect().size().toSize();
	size *= nameItem->internalScale();
	PosByAlign( pos, align, size );
	return QRect( pos, size );
}
QRect DJDesktopController::rectOfViewName( quint8 view ) const
{
	quint8 seat	= view2Seat( view );
	return rectOfSeatName( seat );
}
QRect DJDesktopController::rectOfSeatInfo( quint8 seat ) const
{
	DJGraphicsTextItem	*infoItem	= playerInfoItem( seat );
	if ( !infoItem )
		return QRect();
	QPoint	pos	= m_infoPoints.value( seat, QPoint() );
	Qt::Alignment	align	= m_infoAligns.value( seat, 0 );	
	QSize size	= infoItem->boundingRect().size().toSize();
	size *= infoItem->internalScale();
	PosByAlign( pos, align, size );
	return QRect( pos, size );
}
QRect DJDesktopController::rectOfViewInfo( quint8 view ) const
{
	quint8 seat	= view2Seat( view );
	return rectOfSeatInfo( seat );
}
void DJDesktopController::handleStartClicked()
{
	djDebug()<<"DJDesktopController::handleStartClicked";
	m_btnStart->hide();
	
	QByteArray buf	= panelController()->makeGameData( DJGAME_PROTOCOL_SUBTYPE_START );
	DJClientRequest request( buf );
	panelController()->sendRequest( request );	
}
void DJDesktopController::handleMatrixChanged()
{
	djDebug()<<"DJDesktopController::handleMatrixChanged";
	
	locateAllNonMatrimoveItems();
	
	QMatrix matrix	= desktop()->graphicsMatrix();
	qreal scale	= desktop()->graphicsScale();
	
	foreach( DJShowView *showView, m_showViews ) {
		if ( showView  ) {
			QSize size	= showView->realSize();
			size	*= scale;
			showView->resize( size );
			showView->setMatrix( QMatrix(scale, 0, 0, scale, 0, 0) );
			showView->adjustPos( matrix );
		}
	}
	
	QList<QGraphicsItem*> items	= m_desktop->items();
	foreach( QGraphicsItem *item, items ) {
		//if ( item->parentItem() )
			//continue;
		switch ( item->type() ) {
		case DJGraphicsTextItem::DJGraphicsTextItemType: {
			DJGraphicsTextItem *textItem	= (DJGraphicsTextItem*)item;
			textItem->setExternalScale( scale );
			textItem->adjustPos( matrix );
			break;
		}
		case DJGraphicsPixmapItem::DJGraphicsPixmapItemType: {
			DJGraphicsPixmapItem *pixItem	= (DJGraphicsPixmapItem*)item;
			pixItem->setExternalScale( scale );
			pixItem->adjustPos( matrix );
			break;
		}
		case DJGraphicsTimerItem::DJGraphicsTimerItemType: {
			DJGraphicsTimerItem *timerItem	= (DJGraphicsTimerItem*)item;
			timerItem->setExternalScale( scale );
			timerItem->adjustPos( matrix );
			break;
		}/*
		case DJGraphicsChessTimerItem::DJGraphicsChessTimerItemType: {
			DJGraphicsChessTimerItem *chessTimerItem	= (DJGraphicsChessTimerItem*)item;
			chessTimerItem->setExternalScale( scale );
			chessTimerItem->adjustPos( matrix );
			break;
		}*/
		case DJGraphicsSvgItem::DJGraphicsSvgItemType: {
			DJGraphicsSvgItem *svgItem	= (DJGraphicsSvgItem*)item;
			svgItem->setExternalScale( scale );
			svgItem->adjustPos( matrix );
			break;
		}
		case DJGraphicsLineItem::DJGraphicsLineItemType: {
			DJGraphicsLineItem *lineItem	= (DJGraphicsLineItem*)item;
			lineItem->adjustPos( matrix );
			break;
		}
		case DJGraphicsRectItem::DJGraphicsRectItemType: {
			DJGraphicsRectItem *rectItem	= (DJGraphicsRectItem*)item;
			rectItem->adjustPos( matrix );
			break;
		}
		default:
			break;
		}
	}
	
	QPixmap pixBackground	= QPixmap(gameController()->gameResDir() + "/image/base/background.png");
	if ( !pixBackground.isNull() ) {
		pixBackground	= pixBackground.scaled( desktop()->viewport()->width(),
							desktop()->viewport()->height(),
							Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
		desktop()->setBackgroundBrush( QBrush(pixBackground) );
	}
}
void DJDesktopController::handleMousePressed( QGraphicsSceneMouseEvent * mouseEvent )
{
	djDebug()<<"DJDesktopController::handleMousePressed"<<mouseEvent->button()<<mouseEvent->buttons();;
	//QPointF scenePos	= mouseEvent->scenePos();
	//QPointF virtualPos	= desktop()->totalMatrix().inverted().map(scenePos);
	//QGraphicsItem *item	= itemAt( scenePos );
}
void DJDesktopController::handleMouseReleased( QGraphicsSceneMouseEvent * mouseEvent )
{
	djDebug()<<"DJDesktopController::handleMouseReleased"<<mouseEvent->button()<<mouseEvent->buttons();;
}
void DJDesktopController::handleMouseDoubleClicked( QGraphicsSceneMouseEvent * mouseEvent )
{
	djDebug()<<"DJDesktopController::handleMouseDoubleClicked"<<mouseEvent->button()<<mouseEvent->buttons();;
}
void DJDesktopController::handleMouseMoving( QGraphicsSceneMouseEvent *mouseEvent )
{
}
/*
void DJDesktopController::setHostEnabled( bool enabled )
{
	m_hostEnabled	= enabled;
}
*/
void DJDesktopController::hideHost()
{
	m_hostItem->hide();
}
QList<quint8> DJDesktopController::mask2Seat(quint16 mask)
{
	QList<quint8> seatIds;
	quint8 seat	= 1;
	while( 0 != mask ) {
		if( mask & 1 )
			seatIds << seat;
		seat++;
		mask >>= 1;
	}
	return seatIds ;
}
void DJDesktopController::setClockTimeout( quint16 mask, int seconds )
{  
	djDebug()<<"DJDesktopController::setClockTimeout"<<mask<<seconds;
	
 	QList<quint8> seats	= mask2Seat( mask );
 	QList<quint8> views;
 	foreach( quint8 seat, seats ) {
 		views << seat2View( seat );
 	}
	
	m_clockItem->setTimeout( views, seconds );
}
QList<quint8> DJDesktopController::seatIdsOfPlayingUsers() const
{
	QList<quint8>	seatIds;
	bool isAnyPlayerRoom	= (m_panelController->gameRoom()->options() & DJGAME_ROOMOPTION_ANYPLAYER);
	
	for ( int seatId = 1; seatId <= m_panelController->numberOfSeats(); ++seatId ) {
		if ( isAnyPlayerRoom ) {
			if ( isUserPlaying( seatId ) )
				seatIds << seatId;				
		}else {
			seatIds << seatId;
		}
	}
	return seatIds;
}
bool DJDesktopController::isUserPlaying( quint8 seatId ) const
{
	DJGameUser *player	= m_panelController->player( seatId );
	if ( player )
		return true;
	else
		return false;
}
quint8 DJDesktopController::seat2View(quint8 seat) const
{
	/*
	djDebug()<<"DJDesktopController::seat2View"<<seat;
	if ( 0 == seat )
		return 0;
	
	QList<quint8> seats	= seatIdsOfPlayingUsers();
	int players	= seats.size();
	djDebug()<<"playing users' seats"<<seats;
	int indexOfUserSeat	= seats.indexOf( seat );
	int indexOfSelfSeat	= seats.indexOf( m_panelController->seatId() );
	if ( -1 == indexOfUserSeat || -1 == indexOfSelfSeat )
		return 0;
	djDebug()<<"indexOfUserSeat"<<indexOfUserSeat;
	djDebug()<<"indexOfSelfSeat"<<indexOfSelfSeat;
	
	QList<quint8>	views;
	if ( 1 == players )
		views << 1;
	else if ( 2 == players )
		views << 1 << 3;	
	else if ( 3 == players )
		views << 1 << 2 << 4;
	else if ( 4 == players )
		views << 1 << 2 << 3 << 4;
	
	for ( int i = 0; i < seats.size(); ++i ) {
		int indexOfSeat	= (indexOfSelfSeat + i) % seats.size();
		if ( seat == seats.at(indexOfSeat) ) {
			return views.value(i,0);
		}
	}
	return 0;
	*/
	djDebug()<<"JQDesktopController::seat2View"<<seat;
	if ( 0 == seat )
		return 0;
	
	QList<quint8> seats	= seatIdsOfPlayingUsers();
	if ( seats.isEmpty() )
		return 0;
	int players	= seats.size();
	
	djDebug()<<"playing users' seats"<<seats;
	int indexOfUserSeat	= seats.indexOf( seat );
	int indexOfSelfSeat	= seats.indexOf( panelController()->seatId() );
	if ( -1 == indexOfUserSeat )
		return 0;
	
	QList<quint8>	views;
	if ( 1 == players )
		views << 1;
	else if ( 2 == players )
		views << 1 << 3;	
	else if ( 3 == players )
		views << 1 << 2 << 4;
	else if ( 4 == players )
		views << 1 << 2 << 3 << 4;
	if ( -1 == indexOfSelfSeat ) {
		return views.value(indexOfUserSeat,0);
	}else {
		for ( int i = 0; i < seats.size(); ++i ) {
			int indexOfSeat	= (indexOfSelfSeat + i) % seats.size();
			if ( seat == seats.at(indexOfSeat) ) {
				return views.value(i,0);
			}
		}
	}
	return 0;
}
quint8 DJDesktopController::view2Seat(quint8 view) const
{
	for ( quint8 seatId = 1; seatId <= panelController()->numberOfSeats(); seatId++ ) {
		if ( view == seat2View( seatId ) )
			return seatId;
	}
	return 0;
}
QString	DJDesktopController::userItemNameSuffix( DJGameUser* user ) const
{
	return QString();
}
bool DJDesktopController::isWaitingForMe() const
{
	quint16 mymask = (1<<(panelController()->seatId()-1));
	return ( m_gameWaitMask & mymask );
}
void DJDesktopController::playWave( const QString& waveName, const QString& gameName ) const
{
	QString language	= QLocale::languageToString(QLocale::system().language());
	djDebug() << "DJDesktopController::playWave" << language;
	QString	waveDir	= QString("wav") + "/";
	QString waveLanguageDir	= waveDir + "language/" + language + "/";
	QString waveGameDir;
	if ( gameName.isEmpty() ) {
		waveGameDir	= waveDir + gameController()->gameName() + "/";
	}else {
		waveGameDir	= waveDir + gameName + "/";
	}
	QString waveGameLanguageDir	= waveGameDir + language + "/";
	QString	wavePath;
	
	do {
		wavePath	= waveGameLanguageDir + waveName;
		if ( QFile::exists(wavePath) )
			break;
		wavePath	= waveGameDir + waveName;
		if ( QFile::exists(wavePath) )
			break;
		wavePath	= waveLanguageDir + waveName;
		if ( QFile::exists(wavePath) )
			break;
		wavePath	= waveDir + waveName;
	}while(false);
	
	gameController()->playWave( wavePath );
}
quint8 DJDesktopController::prevPlayerSeatId( quint8 seatId, bool isClockwise ) const
{ 
	quint8 prevSeat	= seatId;
	quint8 numberOfSeats	= panelController()->numberOfSeats();
	do {
		if ( isClockwise  ) {
			prevSeat--;
		}else {
			prevSeat++;
		}
		if ( prevSeat > numberOfSeats ) {
			prevSeat 	-= numberOfSeats;
		}else if ( 0 == prevSeat ) {
			prevSeat 	= numberOfSeats;
		}
	}while( 0 == panelController()->player(prevSeat) ); 
	return prevSeat ;
}
quint8 DJDesktopController::nextPlayerSeatId( quint8 seatId, bool isClockwise ) const
{ 
	quint8 nextSeat	= seatId;
	quint8 numberOfSeats	= panelController()->numberOfSeats();
	do {
		if ( isClockwise  ) {
			nextSeat++;
		}else {
			nextSeat--;
		}
		if ( nextSeat > numberOfSeats ) {
			nextSeat 	-= numberOfSeats;
		}else if ( 0 == nextSeat ) {
			nextSeat 	= numberOfSeats;
		}
	}while( 0 == panelController()->player(nextSeat) );
	return nextSeat;
}
void DJDesktopController::sendGameTrace( quint8 traceID, const QByteArray& traceData, AclProc aclProc, const QVariant& parameters )
{
	QByteArray traceBuf( sizeof(GeneralGameTrace2Head) + traceData.size(), 0 );
	PGeneralGameTrace2Head ptrace = (PGeneralGameTrace2Head)traceBuf.data();
	
	ptrace->chTable = panelController()->tableId();
	ptrace->chSite = panelController()->seatId();
	ptrace->chType = traceID;
	ptrace->chBufLen = traceData.size();
	memcpy(ptrace->chBuf,traceData.data(),traceData.size());
	
	QByteArray buf	= panelController()->makeGameData( DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2,traceBuf );

	DJClientRequest request( buf ,aclProc,parameters);
	panelController()->sendRequest( request );
}
void DJDesktopController::clearAutoDisappearText()
{
}
/*
const DesktopItems& DJDesktopController::seatTypeItems( quint8 seat, uint type ) const
{
	const DesktopTypeItems& typeItems	= m_seatTypeItems.at(seat);
	const DesktopItems& items	= typeItems.value( type );
	return items;
}
DesktopItem* DJDesktopController::seatTypeItem( quint8 seat, uint type, int index ) const
{
	const DesktopTypeItems typeItems	= m_seatTypeItems.at(seat);
	const DesktopItems items	= typeItems.value( type );
	return items.value(index,0);
}
*/
DesktopItems& DJDesktopController::desktopItems( int seat, int type )
{
	DesktopTypeItems& typeItems	= m_seatTypeItems[seat];
	DesktopItems& items	= typeItems[type];
	return items;
}
DesktopItems DJDesktopController::selectedDesktopItems( int seat, int type )
{
	DesktopItems selectedItems;
	DesktopItems& items	= desktopItems( seat, type );
	foreach( DJDesktopItem *item, items ) {
		QGraphicsItem *graphicsItem	= item->graphicsItem();
		DJGraphicsSvgItem *imageItem	= (DJGraphicsSvgItem*)graphicsItem;		
		if ( imageItem->isDJSelected() )
			selectedItems << item;
	}
	return selectedItems;
}
DJDesktopItem* DJDesktopController::createDesktopItem( int seat, int type, int value )
{
	//djDebug()<<"DJDesktopController::createDesktopItem"<<seat<<type<<value;
	return new DJDesktopItem(seat,type,value);
}
void DJDesktopController::appendDesktopItem( int seat, int type, int value )
{
	DJDesktopItem *item	= createDesktopItem( seat, type, value );
	appendDesktopItem( seat, type, item );
}
void DJDesktopController::appendDesktopItem( int seat, int type, DJDesktopItem *item )
{
	DesktopItems& items	= desktopItems( seat, type );
	items.append( item );
}
void DJDesktopController::replaceDesktopItem( int seat, int type, int index, int value )
{
	DJDesktopItem *item	= createDesktopItem( seat, type, value );
	replaceDesktopItem( seat, type, index, item );
}
void DJDesktopController::replaceDesktopItem( int seat, int type, int index, DJDesktopItem *item )
{
	DesktopItems& items	= desktopItems( seat, type );
	if ( index >= items.size() ) {
		items.append( item );
	}else {
		DJDesktopItem *oldItem	= items.at(index);
		items.replace( index, item );
		delete oldItem;
	}
}
void DJDesktopController::moveDesktopItems( int seat, int type1, int type2 )
{
	DesktopItems& items	= desktopItems( seat, type1 );
	DesktopItemsIterator it	= items.begin();
	while( it != items.end() ) {
		DJDesktopItem *item	= *it;
		it	= items.erase( it );
		item->setType( type2 );
		appendDesktopItem( seat, type2, item );
	}
}
DJDesktopItem* DJDesktopController::takeDesktopItem( int seat, int type, int value )
{
	//djDebug()<<"DJDesktopController::takeDesktopItem"<<seat<<type<<value;
	DesktopItems& items	= desktopItems( seat, type );
	DesktopItemsIterator it	= items.begin();
	while( it != items.end() ) {
		DJDesktopItem *item	= *it;
		if ( value == item->value() ) {
			items.erase(it);
			return item;
		}
		it++;
	}
	return 0;
}
void DJDesktopController::clearDesktopItems( int seat, int type )
{
	DesktopTypeItems& typeItems	= m_seatTypeItems[seat];
	if ( 0 == type ) {
		//clear all type
		DesktopTypeItemsIterator it	= typeItems.begin();
		while( it != typeItems.end() ) {
			DesktopItems& items	= *it;
			qDeleteAll( items );
			items.clear();
			it++;
		}
	}else {
		DesktopItems& items	= typeItems[type];
		qDeleteAll( items );
		items.clear();
	}
}
void DJDesktopController::hideDesktopItems( int seat, int type )
{
	DesktopItems& items	= desktopItems( seat, type );
	foreach( DJDesktopItem *item, items ) {
		QGraphicsItem *graphicsItem	= item->graphicsItem();
		if ( 0 != graphicsItem )
			graphicsItem->hide();
	}
}
void DJDesktopController::showDesktopItems( int seat, int type )
{
	DesktopItems& items	= desktopItems( seat, type );
	foreach( DJDesktopItem *item, items ) {
		QGraphicsItem *graphicsItem	= item->graphicsItem();
		if ( 0 != graphicsItem )
			graphicsItem->show();
	}
}
