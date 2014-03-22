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

#include "DJRoomController.h"
#include "DJGameController.h"
#include "DJRoomView.h"
#include "DJRoomCellItem.h"
#include "DJGameUser.h"
#include "DJRoomPlayerItem.h"
#include "DJRoomPlayerAvatarItem.h"
#include "DJRoomPlayerNameItem.h"
#include "DJRoomReadyItem.h"
#include "DJTableController.h"

static	int		ROOM_VIEW_MARGIN_X		= 	20;
static	int		ROOM_VIEW_MARGIN_Y		= 	40;
static	int		ROOM_VIEW_INTERVAL_X	= 	20;
static	int		ROOM_VIEW_INTERVAL_Y	= 	40;
static	qreal	ZVALUE_READY			=	500;
static	qreal	ZVALUE_FOCUS			=	501;

static 	qreal 	ZOOM_STEP				= 0.2;
static 	qreal 	MAX_ZOOM				= 2.0;
static 	qreal 	MIN_ZOOM				= 0.2;

static 	const char*	KEY_GAME_ROOM_VIEW_SCALE		= "GameRoomViewScale";

DJRoomController::DJRoomController( DJGameController* gameController, QWidget *container )
	:QObject(gameController),m_gameController(gameController)
{	
	djDebug() << "DJRoomController constructor";
	
	m_roomView	= new DJRoomView( this, container );
	
    qreal	scale	= roomViewScale();
	QMatrix m( scale, 0, 0, scale, 0, 0 );
	m_roomView->setMatrix( m );
	QSlider *scaleSlider	= gameController->scaleSlider();
	scaleSlider->setTracking( false );
	scaleSlider->setRange( 1, 10 );
	scaleSlider->setSingleStep( 1 );
	scaleSlider->setPageStep( 1 );
	scaleSlider->setValue( qRound(scale * 10) / 2 );
	
	connect( scaleSlider, SIGNAL(valueChanged(int)), SLOT(handleScaleChanged(int)) );
	
	//m_roomView->setContextMenuPolicy( Qt::CustomContextMenu );
	//connect(m_roomView, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(showRoomViewContext(const QPoint&)));
	
	m_selfReadyItem	= new DJRoomReadyItem( 0, m_roomView->scene() );
	m_selfReadyItem->setZValue( ZVALUE_READY );
	m_selfReadyItem->hide();
	connect( m_selfReadyItem, SIGNAL(ready()), SLOT(handleReady()));
	
	m_focusRectItem	= new QGraphicsRectItem( 0, m_roomView->scene() );
	m_focusRectItem->setZValue( ZVALUE_FOCUS );
	m_focusRectItem->hide();
	
	//m_roomViewContextMenu	= new QMenu( m_roomView );
	
	//m_actZoomIn	= new QAction( tr("Zoom in"), m_roomView );
	//m_actZoomIn->setShortcut( QKeySequence(Qt::Key_Plus) );	
	//new QShortcut(QKeySequence(Qt::Key_Plus),m_roomView,SLOT(zoomIn()));
	//connect( m_actZoomIn, SIGNAL(triggered()), SLOT(zoomIn()));
	//m_roomViewContextMenu->addAction(m_actZoomIn);
	
	//m_actRevert	= new QAction( tr("Revert"), m_roomView );
	//connect( m_actRevert, SIGNAL(triggered()), SLOT(revert()));
	//m_roomViewContextMenu->addAction(m_actRevert);
	
	
	//m_actZoomOut	= new QAction( tr("Zoom out"), m_roomView );
	//m_actZoomOut->setShortcut( QKeySequence(Qt::Key_Minus) );
	//new QShortcut(QKeySequence(Qt::Key_Minus),m_roomView,SLOT(zoomOut()));
	//connect( m_actZoomOut, SIGNAL(triggered()), SLOT(zoomOut()));
	//m_roomViewContextMenu->addAction(m_actZoomOut);
}

DJRoomController::~DJRoomController()
{
	djDebug() << "DJRoomController destructor";
	
	delete m_selfReadyItem;
	delete m_roomView;
}
/*
const QSize& DJRoomController::cellSize() const
{
	return m_cellSize;
}
const QPixmap& DJRoomController::readyPix() const
{
	return m_readyPix;
}
*/
const QPixmap& DJRoomController::tablePlayingPix() const
{
	return m_tablePlayingPix;
}
const QPixmap& DJRoomController::tablePix() const
{
	return m_tablePix;
}
const QFont& DJRoomController::tableFont() const
{
	return m_tableFont;
}	
const QColor& DJRoomController::tableColor() const
{
	return m_tableColor;
}	
const QList<QPixmap>& DJRoomController::seatPixs() const
{
	return m_seatPixs;
}
const QList<QPoint>& DJRoomController::seatOffsets() const
{
	return m_seatOffsets;
}
DJRoomView* DJRoomController::roomView() const
{
	return m_roomView;
}
DJTableController* DJRoomController::tableController( quint8 tableId ) const
{
	if ( 0 == tableId || tableId > (uint)m_tableControllers.size() )
		return 0;
	return m_tableControllers.at(tableId-1);
}
DJGameController* DJRoomController::gameController() const
{
	return m_gameController;
}
void DJRoomController::createRoomItems( const QSize& cellSize, const QBrush& backgroundBrush,
								const QPixmap& readyPix, const QPixmap& tablePlayingPix,
								uint numberOfTables,const QPixmap& tablePix, 
    							const QFont& tableFont, const QColor& tableColor,
								const QFont& playerFont, const QColor& playerColor,
    							QList<QPixmap> seatPixs, const QList<QPoint>& seatOffsets )
{
	m_cellSize	= cellSize;
	m_readyPix	= readyPix;
	m_tablePlayingPix	= tablePlayingPix;
	m_tablePix	= tablePix;
	m_tableFont	= tableFont;
	m_tableColor	= tableColor;
	m_playerFont	= playerFont;
	m_playerColor	= playerColor;
	
	m_seatPixs	= seatPixs;
	m_seatOffsets	= seatOffsets;
	
	m_roomView->setBackgroundBrush( backgroundBrush );
	m_roomView->setAlignment( Qt::AlignLeft | Qt::AlignTop );
	
	m_selfReadyItem->setPixmap( readyPix );
	m_focusRectItem->setPen( QPen(Qt::green) );
	m_focusRectItem->setRect( 0,0,m_cellSize.width(), m_cellSize.height() );
	
	for ( quint8 tableId = 1; tableId <= numberOfTables; ++tableId ) {
		DJTableController *tableController	= new DJTableController(tableId,this);
		m_tableControllers << tableController;
	}
}
void DJRoomController::clearRoomItems()
{
	m_cellOrigins.clear();
	qDeleteAll( m_playerItems );
	m_playerItems.clear();
	qDeleteAll( m_tableControllers );
	m_tableControllers.clear();
}
int DJRoomController::calculateColumns(int totalWidth, int cellWidth, int interval ) const
{
	djDebug()<<"calculateColumns"<<totalWidth<<cellWidth<<interval;
	if ( totalWidth < cellWidth )
		return 0;
	int columns	= 2;
	while( (cellWidth * columns) + (interval * (columns - 1)) <= totalWidth ) {
		columns++;
	}
	return columns - 1;
}
void DJRoomController::repaintAll()
{
	int viewWidth	= static_cast<int>(m_roomView->viewport()->width() / m_roomView->matrix().m11());
	int availableWidth	= viewWidth - 2 * ROOM_VIEW_MARGIN_X;
	
	int columns	= calculateColumns( availableWidth, m_cellSize.width(), ROOM_VIEW_INTERVAL_X );
	
	int	eachSpaceX	= 0;
	if ( 0 == columns )  {
		columns = 1;
		eachSpaceX	= 0;
	}else {
		int totalSpaceX	= availableWidth - (m_cellSize.width() * columns) - (ROOM_VIEW_INTERVAL_X * (columns - 1));
		eachSpaceX	= totalSpaceX / columns;
	}
	int rows	= ( m_tableControllers.size() + columns - 1) / columns;
	
	int viewHeight	= (m_cellSize.height() + ROOM_VIEW_INTERVAL_Y) * rows + ROOM_VIEW_MARGIN_Y;
	
	m_roomView->setSceneRect( 0, 0, viewWidth, viewHeight );
	
	int i,j;
	
	m_cellOrigins.clear();
	
	int x	= ROOM_VIEW_MARGIN_X + eachSpaceX/2;
	int y	= ROOM_VIEW_MARGIN_Y;
    for ( i = 0; i < rows; i++ ) {
    	for ( j = 0; j < columns; j++ ) {
    		m_cellOrigins << QPoint( x, y );
    		x	+= m_cellSize.width() + ROOM_VIEW_INTERVAL_X + eachSpaceX;
    	}
    	x	= ROOM_VIEW_MARGIN_X + eachSpaceX/2;
    	y	+= m_cellSize.height() + ROOM_VIEW_INTERVAL_Y;
	}
	for ( int i = 0; i < m_tableControllers.size(); i++ ) {
		QPoint cellOrigin	= m_cellOrigins.at(i);
		DJRoomCellItem *cellItem	= m_tableControllers.at(i)->cellItem();
		cellItem->setPos( cellOrigin );
	}
	QHash<DJGameUser*, DJRoomPlayerItem*>::const_iterator it = m_playerItems.constBegin();
	while ( it != m_playerItems.constEnd() ) {
		DJGameUser *user	= it.key();
        DJRoomPlayerItem* playerItem	= it.value();
        repaintPlayer( user, playerItem );
        ++it;
    }
}
void DJRoomController::addUser( DJGameUser* user )
{
	djDebug()<<"DJRoomController::addUser";
	
	if ( m_playerItems.contains( user ) )
		return;
		
	DJRoomPlayerItem	*playerItem	= new DJRoomPlayerItem( this, user, m_playerFont, m_playerColor, 0, m_roomView->scene() );
	m_playerItems.insert( user, playerItem );
			
	repaintPlayer( user );
	changeTableUsers( user, 0, 0, GAME_STATUS_IDLE );
	
	connect( user, SIGNAL(trinityChanged(DJGameUser*,quint8,quint8,quint8)), SLOT(repaintPlayer(DJGameUser*)) );
	connect( user, SIGNAL(trinityChanged(DJGameUser*,quint8,quint8,quint8)), SLOT(changeTableUsers(DJGameUser*,quint8,quint8,quint8)) );
	connect( user, SIGNAL(reloaded(DJGameUser*)), SLOT(reloadUser(DJGameUser*)) );
	
	connect( playerItem->avatarItem(), SIGNAL(playerClicked(DJGameUser*,Qt::MouseButtons)), SLOT(handlePlayerClicked(DJGameUser*,Qt::MouseButtons)));
	connect( playerItem->avatarItem(), SIGNAL(playerDoubleClicked(DJGameUser*,Qt::MouseButtons)), SLOT(handlePlayerDoubleClicked(DJGameUser*,Qt::MouseButtons)));
}
void DJRoomController::removeUser( DJGameUser* user )
{	
	if ( m_playerItems.contains( user ) ) {
		user->setTrinity( 0, 0, GAME_STATUS_IDLE );
		DJRoomPlayerItem	*playerItem	= m_playerItems.take( user );
		delete playerItem;
	}
}
void DJRoomController::reloadUser( DJGameUser* user )
{
	djDebug()<<"DJRoomController::reloadUser";
	
	if ( m_playerItems.contains( user ) ) {
		DJRoomPlayerItem	*playerItem	= m_playerItems.value( user );
		playerItem->reload();
		repaintPlayer( user );
	}else {
		addUser( user );
	}
}
void DJRoomController::repaintPlayer( DJGameUser* user )
{
	DJRoomPlayerItem	*playerItem	= m_playerItems.value( user, 0 );
	
	repaintPlayer( user, playerItem );
}
void DJRoomController::repaintPlayer( DJGameUser* user, DJRoomPlayerItem *playerItem )
{	
	if ( 0 == playerItem )
		return;
		
	DJRoomPlayerAvatarItem	*avatarItem	= playerItem->avatarItem();
	DJRoomPlayerNameItem	*nameItem	= playerItem->nameItem();
	
	quint8 tableId	= user->tableId();
	quint8 seatId	= user->seatId();
	
	if ( 0 == tableId || 0 == seatId ) {
		avatarItem->hide();
		nameItem->hide();
		if ( user->isSelf() ) {
			m_selfReadyItem->hide();
			setFocusTable( 0 );
		}
		return;
	}
	
	uint tableIndex	= tableId - 1;
	uint seatIndex	= seatId - 1;
		
	QPoint cellOrigin	= m_cellOrigins.at(tableIndex);
    QPoint cellCenter(m_cellSize.width()/2,m_cellSize.height()/2);
    QPoint avatarCenter(avatarItem->boundingRect().toRect().width()/2,avatarItem->boundingRect().toRect().height()/2);
    QPoint avatarPos	= cellOrigin + cellCenter + m_seatOffsets.at(seatIndex) - avatarCenter;
    	
    avatarItem->setPos( avatarPos );
    avatarItem->show();
    
    if ( user->isSelf() ) {
    	if ( user->isDown() ) {
    		QPoint readyCenter(m_selfReadyItem->boundingRect().toRect().width()/2,m_selfReadyItem->boundingRect().toRect().height()/2);
    		QPoint readyPos	= cellOrigin + cellCenter - readyCenter;
    		m_selfReadyItem->setPos( readyPos );
    		m_selfReadyItem->show();
		}else {
			m_selfReadyItem->hide();
		}
		setFocusTable( 0 );
    }
    if ( user->isRobot() ) {
		nameItem->hide();
	}else {
    	QPoint nameCenter(nameItem->boundingRect().toRect().width()/2,nameItem->boundingRect().toRect().height());
    	QPoint namePos	= cellOrigin + cellCenter + m_seatOffsets.at(seatIndex) - nameCenter;
   	 	namePos.ry() -=	avatarItem->boundingRect().toRect().height()/2;
    
    	nameItem->setPos( namePos );
    	nameItem->show();
	}
	playerItem->update();
}
void DJRoomController::handleSeatClicked( quint8 tableId, quint8 seatId, QGraphicsSceneMouseEvent *mouseEvent )
{
	if ( Qt::LeftButton == mouseEvent->button() )	
		m_gameController->requestSitDown( tableId, seatId );
}
void DJRoomController::handleReady()
{
	m_gameController->requestReady();
}
void DJRoomController::handlePlayerClicked( DJGameUser *user, Qt::MouseButtons buttons )
{
	if ( Qt::LeftButton == buttons ) {
		if ( user->isSelf() ) {
			m_gameController->requestStandUp();
		}else {
			m_gameController->userSelected( user );
		}
	}else if ( Qt::RightButton == buttons ) {
		m_gameController->showUserContext( user );
	}
}
void DJRoomController::handlePlayerDoubleClicked( DJGameUser *user, Qt::MouseButtons buttons )
{
	if ( Qt::LeftButton == buttons ) {
		m_gameController->userDoubleClicked( user );
	}
}
void DJRoomController::changeTableUsers( DJGameUser* user, quint8 oldTableId, quint8 oldSeatId, quint8 oldGameStatus )
{
	quint8 tableId	= user->tableId();
	quint8 seatId		= user->seatId();

	DJTableController* oldtc	= 0;
	DJTableController* newtc	= 0;
	
	if ( oldTableId ) {
		oldtc	= tableController(oldTableId);
	}
	if ( tableId ) {
		newtc	= tableController(tableId);
	}
	
	if ( oldTableId != tableId || oldSeatId != seatId ) {
		//table seat changed
    	if ( oldtc ) {
    		oldtc->removeUser( user );
    	}
    	if ( newtc ) {
    		newtc->addUser( user );
    	}
	}
}
/*
void DJRoomController::showRoomViewContext(const QPoint& point)
{
	djDebug()<<"showRoomViewContext";
	qreal scale	= roomViewScale();
	m_actZoomIn->setEnabled( !(scale >= MAX_ZOOM) );
	m_actRevert->setEnabled( !(1 == scale) );
	m_actZoomOut->setEnabled( !(scale <= MIN_ZOOM) );
	m_roomViewContextMenu->exec( QCursor::pos() );
}
void DJRoomController::zoomIn()
{
	djDebug() << "zoomIn";
	qreal scale	= roomViewScale();
	if ( scale > MAX_ZOOM )
		return;
	scale	+= ZOOM_STEP;
	setRoomViewScale( scale );
}
void DJRoomController::zoomOut()
{
	djDebug() << "zoomOut";
	qreal scale	= roomViewScale();
	if ( scale < MIN_ZOOM )
		return;
	scale	-= ZOOM_STEP;
	setRoomViewScale( scale );
}
void DJRoomController::revert()
{
	djDebug() << "revert";
	setRoomViewScale( 1 );
}
*/
void DJRoomController::setFocusTable( quint8 tableId )
{
	m_focusRectItem->hide();
	if ( 0 == tableId )
		return;
	DJTableController *tc	= tableController( tableId );
	if ( 0 == tc )
		return;
	DJRoomCellItem *cell	= tc->cellItem();
    m_roomView->ensureVisible( cell );
    
    QPoint	cellPos	= m_cellOrigins.value( tableId - 1, QPoint() );
    m_focusRectItem->setPos( cellPos );
    m_focusRectItem->show();
}
qreal DJRoomController::roomViewScale() const
{
	QSettings settings;
    settings.beginGroup( m_gameController->gameName() );
    qreal	scale	= settings.value( KEY_GAME_ROOM_VIEW_SCALE, 1 ).toDouble();
    settings.endGroup();
    if ( scale < MIN_ZOOM )
		scale	= MIN_ZOOM;
	else if ( scale > MAX_ZOOM )
    	scale	= MAX_ZOOM;
    return scale;
}
void DJRoomController::setRoomViewScale( qreal scale )
{
	if ( scale < MIN_ZOOM )
		scale	= MIN_ZOOM;
	else if ( scale > MAX_ZOOM )
    	scale	= MAX_ZOOM;
	QMatrix m( scale, 0, 0, scale, 0, 0 );
	m_roomView->setMatrix( m );
	repaintAll();
	
	QSettings settings;
    settings.beginGroup( m_gameController->gameName() );
    settings.setValue( KEY_GAME_ROOM_VIEW_SCALE, scale );
    settings.endGroup();
}
void DJRoomController::handleScaleChanged( int value )
{
	qreal scale	= static_cast<qreal>(value*2) / 10;
	djDebug()<<"handleScaleChanged"<<value<<scale;
	setRoomViewScale( scale );
}
