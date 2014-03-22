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
#include "DJDesktopPokerController.h"
#include "DJPanelController.h"
#include "DJGraphicsPixmapItem.h"
#include "DJGraphicsSvgItem.h"
#include "DJGraphicsTextItem.h"
#include "DJDesktopItem.h"
#include "DJDesktopPokerItem.h"
#include "DJDesktop.h"
#include "DJDesktopScene.h"
#include "Utility.h"
#include "DJClientRequest.h"
#include "DJPanel.h"

static uint qHash ( DJPoker poker )
{
	return (poker.suit() << 16) | poker.point();
}
static	qreal	Z_SELECT_RECT	=	4000;
static QList<int> allSuits	= QList<int>() 	<< DJPoker::Diamond
											<< DJPoker::Club
											<< DJPoker::Heart
											<< DJPoker::Spade
											<< DJPoker::Joker;
static DJDesktopPokerController		*gPokerController	= 0;

static void HandleThrowAcl( const QByteArray& buf, const QVariant& parameters )
{
	DJDesktopPokerController* pokerController = static_cast<DJDesktopPokerController *>(parameters.value<void*>());
	PDJGameProtocol pacl = (PDJGameProtocol)buf.data();
	pokerController->receiveThrowAcl( pacl->chBuf[0] );
}
static bool DJPokerLessThan( const DJPoker& poker1, const DJPoker& poker2 )
{
	if ( gPokerController ) {
		return gPokerController->isFirstPokerLefter( poker1, poker2 );
	}else
		return false;
}
static bool DJPokerItemLessThan( DJDesktopItem* item1, DJDesktopItem* item2 )
{
	if ( gPokerController ) {
		return gPokerController->isFirstPokerItemLefter( (DJDesktopPokerItem*)item1, (DJDesktopPokerItem*)item2 );
	}else
		return false;
}								
DJDesktopPokerController::DJDesktopPokerController( DJPanelController* panelController, const QSize& size, QWidget *parent )
	:DJDesktopController( panelController, size, parent)
{	
	djDebug() << "DJDesktopPokerController constructor";
	
	setWaitingThrowAcl( false );
	
	setBridgeCard( true );
	setPokerSortMode( SuitPoint );
	//setRankOfSuits( NormalSuitRank );
	//setRankOfPoints( AcePointRank );

	QString firstPoker	= ":/BaseRes/svg/poker/c1.svg";
	QDir dir(":/BaseRes/svg/poker");
	QStringList files	= dir.entryList(QDir::Files);
	foreach( QString file, files ) {
		QString filepath	= dir.filePath(file);
		djDebug()<<"file"<<file<<"filepath"<<filepath;
		QSvgRenderer *renderer	= new QSvgRenderer( filepath, this );
		m_pokerSvgCache.insert( filepath, renderer );
		
		if ( file == "back.svg" ) {
			QPixmap pix	= SvgRender2Pixmap( renderer );
			m_pokerPixCache.insert( filepath, pix );
		}
	}
	
	m_pokerSize	= m_pokerSvgCache[firstPoker]->defaultSize();
	
	for ( int view = 0; view <= 4; ++view ) {
		m_scalesOfHand << 0.7;
		m_scalesOfThrown << 1;
		m_scalesOfAttached << 0.7;
		m_scalesOfWagerChips << 1;
	}
	m_scalesOfHand[1]	= 1.2;
	m_pokerDistance	= 18;
	m_deltaOfNewline	= 40;
	
	QPixmap pix	= QPixmap(":/BaseRes/image/chip/red1.png");
	m_chipSize	= pix.size();
	m_chipDistance	= 20;
	
	setSelectRectEnabled( true );
	setSelectEnabled( true );
	
	m_selectRectItem 	= new QGraphicsRectItem( 0, desktop()->scene() );
	m_selectRectItem->setPen( QPen(Qt::green) );
	m_selectRectItem->setZValue( Z_SELECT_RECT );
	m_selectRectItem->hide();
}
DJDesktopPokerController::~DJDesktopPokerController()
{
	djDebug() << "DJDesktopPokerController destructor";
}
QSvgRenderer* DJDesktopPokerController::pokerSvgCache( const QString& resPath ) const
{
	return m_pokerSvgCache.value( resPath, 0 );
}
QPixmap DJDesktopPokerController::pokerPixCache( const QString& resPath ) const
{
	return m_pokerPixCache.value( resPath, QPixmap() );
}
int DJDesktopPokerController::numberOfPokers(int seat, int type)
{
	DesktopItems &items	= desktopItems( seat, type );
	return items.size();
}
void DJDesktopPokerController::throwAllCards()
{
	djDebug()<<"DJDesktopPokerController::throwAllCards";
	if( isChuEnabled() ) {
		DesktopItems& allItems	= desktopItems( panelController()->seatId(), POKER_TYPE_HAND );
		throwPokerItems( allItems );
	}
}
void DJDesktopPokerController::throwSelectedCards()
{
	djDebug()<<"DJDesktopPokerController::throwSelectedCards";
	if( isChuEnabled() ) {
		djDebug()<<"chu is enabled";
		DesktopItems selectedItems	= selectedDesktopItems( panelController()->seatId(), POKER_TYPE_HAND );
		throwPokerItems( selectedItems );
	}
	djDebug()<<"chu is DISABLED";
}
void DJDesktopPokerController::throwPokerItems( DesktopItems& items )
{
	djDebug()<<"DJDesktopPokerController::throwPokerItems"<<items.size();
	if( !isChuEnabled() ) {
		djDebug()<<"chu is disable";
		return;
	}
	QByteArray trace	= createThrowTrace( items );
	if ( trace.isEmpty() ) {
		djDebug()<<"trace is empty";
		return;
	}
	QByteArray buf	= panelController()->makeGameData( DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2, trace );
	QVariant parameters	= QVariant::fromValue((void*)this);
	DJClientRequest request( buf, HandleThrowAcl, parameters );
	panelController()->sendRequest( request );
	setWaitingThrowAcl( true );
}
QByteArray DJDesktopPokerController::createThrowTrace( DesktopItems& items )
{
	djDebug()<<"DJDesktopPokerController::createThrowTrace";
	return QByteArray();
}
void DJDesktopPokerController::receiveThrowAcl( quint8 status )
{
	djDebug()<<"DJDesktopPokerController::receiveThrowAcl"<<hex<<status;
	setWaitingThrowAcl( false );
	
	if ( DJGAME_FAILED( status ) ) {
		djDebug()<<"failed";
		QMessageBox::information( panelController()->panel(),tr("throw card failed"),tr("plz read game rules"),QMessageBox::Ok );
	}
}
bool DJDesktopPokerController::isChuEnabled() const
{
	return false;
}
void DJDesktopPokerController::locateAll()
{
	djDebug()<<"DJDesktopPokerController::locateAll";
	DJDesktopController::locateAll();
	
	locatePoolSpace( m_poolSpace );
	locateCenterSpace( m_centerSpace );
	
	djDebug()<<"centerPoint"<<centerPoint();
	djDebug()<<"availableSpace"<<availableSpace();
	djDebug()<<"poolSpace"<<poolSpace();
}
void DJDesktopPokerController::locatePoolSpace( QRect& rect )
{
	rect.setBottom( availableSpace().bottom() - static_cast<int>(scaleOfHand(1) * (pokerSize().height() + deltaOfNewline())) );
	rect.setLeft( availableSpace().left() + static_cast<int>(scaleOfHand(2) * pokerSize().height()) );
	rect.setTop( availableSpace().top() + static_cast<int>(scaleOfHand(3) * pokerSize().height()) );
	rect.setRight( availableSpace().right() - static_cast<int>(scaleOfHand(4) * pokerSize().height()) );
}
void DJDesktopPokerController::locateCenterSpace( QRect& rect )
{
	int space	= 20;
	
	rect.setBottom( centerPoint().y() + space );
	rect.setLeft( centerPoint().x() - space );
	rect.setTop( centerPoint().y() - space );
	rect.setRight( centerPoint().x() + space );
}
void DJDesktopPokerController::repaintAll()
{
	DJDesktopController::repaintAll();
	//repaintAllRemainders();
}
void DJDesktopPokerController::handleMatrixChanged()
{
	DJDesktopController::handleMatrixChanged();
}
void DJDesktopPokerController::handleMousePressed( QGraphicsSceneMouseEvent *mouseEvent )
{
	djDebug()<<"DJDesktopPokerController::handleMousePressed"<<mouseEvent->button()<<mouseEvent->buttons();
	
	if( panelController()->isLookingOn() )
		return;
	if ( !isSelectEnabled() )
		return;
	if ( Qt::LeftButton == mouseEvent->button() ) {
		QPointF scenePos	= mouseEvent->scenePos();
		QGraphicsItem *item	= desktop()->desktopScene()->itemAt( scenePos );
	
		if (  isSelectRectEnabled() ) {
			desktop()->desktopScene()->setMouseMoveEventEnabled(true);
			m_startPoint 	= mouseEvent->scenePos();
		}
		
		if ( 0 == item ) {
    		//djDebug()<<"unselect card";
    		selectSelfHandPokers();
    	}else {
    		//djDebug()<<"select card";
    		QVariant var1	= item->data(KEY_OF_SEAT);
    		QVariant var2	= item->data(KEY_OF_TYPE);
    		if( var1.isValid() && var2.isValid() ) {
    			int seat	= var1.toInt();
    			int type	= var2.toInt();
    			if ( POKER_TYPE_HAND == type &&
    				seat == panelController()->seatId() ) {
					if ( DJGraphicsPixmapItem::DJGraphicsPixmapItemType == item->type() ) {
						DJGraphicsPixmapItem *imageItem	= (DJGraphicsPixmapItem*)item;
						imageItem->setDJSelected( !imageItem->isDJSelected() );
    					imageItem->adjustPos( desktop()->graphicsMatrix() );
					}else if ( DJGraphicsSvgItem::DJGraphicsSvgItemType == item->type() ) {
						DJGraphicsSvgItem *imageItem	= (DJGraphicsSvgItem*)item;
						imageItem->setDJSelected( !imageItem->isDJSelected() );
    					imageItem->adjustPos( desktop()->graphicsMatrix() );
					}
    			}
    		}
    	}
	}
}
void DJDesktopPokerController::handleMouseMoving( QGraphicsSceneMouseEvent *mouseEvent )
{
	//djDebug()<<"DJDesktopPokerController::handleMouseMoving"<<mouseEvent->button()<<mouseEvent->buttons();
	
	if(panelController()->isLookingOn())
		return;
	
	if ( !isSelectEnabled() )
		return;
		
	if ( Qt::LeftButton & mouseEvent->buttons() && isSelectRectEnabled() ) {
		//djDebug()<<"left button m_isSelectRectEnabled";
 		m_selectRectItem->setRect( m_startPoint.x(),
 									m_startPoint.y(),
 									mouseEvent->scenePos().x() - m_startPoint.x(),
 									mouseEvent->scenePos().y() - m_startPoint.y() );
		m_selectRectItem->show();
	}
}
void DJDesktopPokerController::handleMouseReleased( QGraphicsSceneMouseEvent *mouseEvent )
{
	djDebug()<<"DJDesktopPokerController::handleMouseReleased"<<mouseEvent->button()<<mouseEvent->buttons();
	
	//QPointF scenePos	= mouseEvent->scenePos();
	//QGraphicsItem *item	= desktop()->desktopScene()->itemAt( scenePos );
	
	if( panelController()->isLookingOn() )
		return;
	
	if ( !isSelectEnabled() )
		return;
		
	if ( Qt::LeftButton == mouseEvent->button() ) {
		//left button released
		if ( m_selectRectItem->isVisible() ) {
			//qt can't handle negative width rect correctly
			//so we convert it to positive width rect
			
			qreal left	= (mouseEvent->scenePos().x() < m_startPoint.x())?mouseEvent->scenePos().x():m_startPoint.x();
 			qreal top	= (mouseEvent->scenePos().y() < m_startPoint.y())?mouseEvent->scenePos().y():m_startPoint.y();
 			qreal w	= mouseEvent->scenePos().x() - m_startPoint.x();
 			qreal h	= mouseEvent->scenePos().y() - m_startPoint.y();
			if ( w < 0 ) {
				w	= -w;
			}
			if ( h < 0 ) {
				h	= -h;
			}
			QRect selectRect	= QRectF( left, top, w, h ).toRect();
			QRegion	region( selectRect );
			//djDebug()<<"selectRect"<<selectRect;
			QList<QGraphicsItem*> selectedItems	= desktop()->desktopScene()->items(selectRect);
			foreach ( QGraphicsItem *item, selectedItems ) {
				//djDebug()<<"zvalue="<<item->zValue();
				QVariant var	= item->data(KEY_OF_SEAT);
				if ( !var.isValid() )
					continue;
				quint8 seat	= var.toInt();
				if ( seat != panelController()->seatId() )
					continue;
					
				var	= item->data(KEY_OF_TYPE);
				if ( !var.isValid() )
					continue;
				int type	= var.toInt();
				if ( type != POKER_TYPE_HAND )
					continue;
				if ( DJGraphicsPixmapItem::DJGraphicsPixmapItemType == item->type() ) {
					DJGraphicsPixmapItem *imageItem	= (DJGraphicsPixmapItem*)item;
					QRect itemRect	= imageItem->sceneBoundingRect().toRect();
					QRect intersectRect	= selectRect.intersect( itemRect );
    				
    				if ( region.intersects( intersectRect ) ) {
     					region	= region.subtracted( intersectRect );
    					imageItem->setDJSelected( true );
    					imageItem->adjustPos( desktop()->graphicsMatrix() );
    				}
				}else if ( DJGraphicsSvgItem::DJGraphicsSvgItemType == item->type() ) {
					DJGraphicsSvgItem *imageItem	= (DJGraphicsSvgItem*)item;
					QRect itemRect	= imageItem->sceneBoundingRect().toRect();
					QRect intersectRect	= selectRect.intersect( itemRect );
    				
    				if ( region.intersects( intersectRect ) ) {
     					region	= region.subtracted( intersectRect );
    					imageItem->setDJSelected( true );
    					imageItem->adjustPos( desktop()->graphicsMatrix() );
    				}
				}
			}
		}
	}else if( Qt::RightButton == mouseEvent->button() ) {
		//right button released
		throwSelectedCards();
    }
    desktop()->desktopScene()->setMouseMoveEventEnabled(false);
	m_selectRectItem->hide();
}
bool DJDesktopPokerController::isFirstPokerLefter( const DJPoker& poker1, const DJPoker& poker2 ) const
{
	//djDebug()<<"DJDesktopPokerController::isFirstPokerLefter";
	int result	= 0;
	for( int i = 0; i < m_sortKeys.size(); ++i ) {
		SortKey	key	= m_sortKeys.at(i);
		result	= comparePokerWithKey( poker1, poker2, key );
		if ( result != 0 )
			break;
	}
	//djDebug()<<"copare result"<<result;
	if ( 1 == result ) {
		return true;
	}else {
		return false;
	}
}
bool DJDesktopPokerController::isFirstPokerItemLefter( DJDesktopPokerItem* item1, DJDesktopPokerItem* item2 ) const
{
	//djDebug()<<"DJDesktopPokerController::isFirstPokerItemLefter"<<item1<<item2;
	
	const DJPoker& poker1	= item1->poker();
	const DJPoker& poker2	= item2->poker();
	return isFirstPokerLefter( poker1, poker2 );
}
int	DJDesktopPokerController::comparePokerWithKey( const DJPoker& poker1, const DJPoker& poker2, SortKey key ) const
{
	//djDebug()<<"DJDesktopPokerController::comparePokerWithKey";
	switch(key) {
	case SortByASCount: {
		//djDebug()<<"SortByASCount";
		int ascount1	= numberOfPokersOnAllSuits( m_sortingPokers, DJPoker(DJPoker::AnySuit,poker1.point()) );
		int ascount2	= numberOfPokersOnAllSuits( m_sortingPokers, DJPoker(DJPoker::AnySuit,poker2.point()) );
		
		//djDebug()<<"ascount1"<<ascount1<<"ascount2"<<ascount2;
		
		if ( ascount1 < ascount2 )
			return -1;
		else if ( ascount1 > ascount2 )
			return 1;
		else
			return 0;
		break;
	}
	case SortBySuit: {
		//djDebug()<<"SortBySuit";
		int suitRank1	= rankOnSuit( poker1 );
		int suitRank2	= rankOnSuit( poker2 );
		
		//djDebug()<<"suitRank1"<<suitRank1<<"suitRank2"<<suitRank2;
		
		if ( suitRank1 < suitRank2 )
			return -1;
		else if ( suitRank1 > suitRank2 )
			return 1;
		else
			return 0;
		break;
	}
	case SortByPoint: {
		//djDebug()<<"SortByPoint";
		int pointRank1	= rankOnPoint( poker1 );
		int pointRank2	= rankOnPoint( poker2 );
		
		//djDebug()<<"pointRank1"<<pointRank1<<"pointRank2"<<pointRank2;
		
		if ( pointRank1 < pointRank2 )
			return -1;
		else if ( pointRank1 > pointRank2 )
			return 1;
		else
			return 0;
		break;
	}
	default:
		return 0;
	}
}
int DJDesktopPokerController::distanceBetweenPokers( const DJPoker& poker1, const DJPoker& poker2 ) const
{
	return 1;
}
//return length of pokers
int DJDesktopPokerController::calculateDistanceOfPokers( DesktopItems& items, qreal scale, bool isFixedDistance )
{
	djDebug()<<"DJDesktopPokerController::calculateDistanceOfPoker";
	int totalLength	= 0;
	int groupLength	= 0;
	DesktopItemsIterator it	= items.begin();
	DesktopItemsIterator groupIt	= it;
	while( it != items.end() ) {
		DJDesktopPokerItem *curItem	= (DJDesktopPokerItem*)*it;
		DesktopItemsIterator nextIt	= it + 1;
		if ( nextIt == items.end() ) {
			//current item is the last poker
			int distance	= 0;
			int length	= pokerSize().width();
			curItem->setDistance( distance );
			curItem->setLength( length );
			totalLength	+= curItem->length();
			groupLength	+= curItem->length();
			DJDesktopPokerItem *groupItem	= (DJDesktopPokerItem*)*groupIt;
			groupItem->setLength( groupLength );
			break;
		}
		DJDesktopPokerItem *nextItem	= (DJDesktopPokerItem*)*nextIt;
		int distanceFactor;
		if ( isFixedDistance ) {
			distanceFactor	= 1;
		}else {
			distanceFactor	= distanceBetweenPokers( curItem->poker(), nextItem->poker() );
		}
		int distance	= pokerDistance() * distanceFactor;
		int length	= pokerSize().width();
		if ( 1 == distanceFactor ) {
			//no group
			curItem->setDistance( distance );
			curItem->setLength( length );
			groupLength	+= curItem->distance();
			totalLength	+= curItem->distance();
		}else {
			//new group
			curItem->setDistance( distance );
			curItem->setLength( length );
			groupLength	+= curItem->length();
			totalLength	+= curItem->distance();
			DJDesktopPokerItem *groupItem	= (DJDesktopPokerItem*)*groupIt;
			groupItem->setLength( groupLength );
			groupIt	= nextIt;
			groupLength	= 0;
		}
		it++;
	}
	return static_cast<int>(totalLength * scale);
}
int DJDesktopPokerController::rankOnSuit( const DJPoker& poker ) const
{
	int suit	= poker.suit();
	if ( suit == DJPoker::Joker )
		return DJPoker::ConstTrump | DJPoker::SuitTrump | DJPoker::Joker;
	return suit;
}
int DJDesktopPokerController::rankOnPoint( const DJPoker& poker ) const
{
	DJPoker::Point point	= poker.point();
	if ( DJPoker::Ace == point )
		return DJPoker::Shift | point;
	return point;
}
int DJDesktopPokerController::numberOfPokersOnAllSuits( const QHash<DJPoker,int> countPokers, const DJPoker& comboPoker ) const
{
	int comboSuit	= comboPoker.suit();
	DJPoker::Point	point	= comboPoker.point();
	int count	= 0;
	foreach( int suit, allSuits ) {
		if ( comboSuit & suit ) {
			count	+= countPokers.value( DJPoker(suit,point) );
		}
	}
	return count;
}
QHash<DJPoker,int> DJDesktopPokerController::createCountPokers( const QList<DJPoker>& handPokers ) const
{
	QHash<DJPoker,int> countPokers;
	foreach( DJPoker poker, handPokers ) {
		int count	= handPokers.count( poker );
		countPokers.insert( poker, count );
	}
	return countPokers;
}
void DJDesktopPokerController::setPokerSortKeys( const QList<SortKey>& sortKeys )
{
	m_sortKeys	= sortKeys;
}
void DJDesktopPokerController::setBridgeCard( bool isBridgeCard )
{
	m_isBridgeCard	= isBridgeCard;
}
void DJDesktopPokerController::sortPokers( QList<DJPoker>& pokers )
{
	//djDebug()<<"DJGamePokerDesktop::sortPokers";
	m_sortingPokers	= createCountPokers( pokers );
	gPokerController	= this;
	qSort( pokers.begin(), pokers.end(), DJPokerLessThan );
}
void DJDesktopPokerController::sortPokers( DesktopItems & pokerItems )
{
	//djDebug()<<"DJDesktopPokerController::sortPokers"<<"size="<<pokerItems.size();
	QList<DJPoker> pokers	= pokersFromDesktopItems( pokerItems );
	//foreach( DJPoker poker, pokers ) {
		//djDebug()<<poker.description();
	//}
	m_sortingPokers	= createCountPokers( pokers );
	gPokerController	= this;
	//djDebug()<<"begin sort";
	qSort( pokerItems.begin(), pokerItems.end(), DJPokerItemLessThan );
	//djDebug()<<"end sort";
}
void DJDesktopPokerController::setPokerSortMode( SortMode mode )
{
	switch( mode ) {
	case ASCountPointSuit: {
		QList<SortKey> sortKeys;
		sortKeys << SortByASCount << SortByPoint << SortBySuit;
		setPokerSortKeys( sortKeys );
		break;
	}
	case PointASCountSuit: {
		QList<SortKey> sortKeys;
		sortKeys << SortByPoint << SortByASCount << SortBySuit;
		setPokerSortKeys( sortKeys );
		break;
	}
	case SuitPoint: {
		QList<SortKey> sortKeys;
		sortKeys << SortBySuit << SortByPoint;
		setPokerSortKeys( sortKeys );
		break;
	}
	case PointSuit: {
		QList<SortKey> sortKeys;
		sortKeys << SortByPoint << SortBySuit;
		setPokerSortKeys( sortKeys );
		break;
	}
	default:
		break;
	}
}

bool DJDesktopPokerController::groupGreaterThan( const QList<DJPoker>& group1,
									const QList<DJPoker>& group2 )
{
	return rankOnPoint(group1.first()) > rankOnPoint(group2.first());
}
bool DJDesktopPokerController::shunGreaterThan( const QList<DJPoker>& shun1,
										const QList<DJPoker>& shun2 )
{
	return rankOnPoint(shun1.last()) > rankOnPoint(shun2.last());
}
//same rank pokers can be united to become group
//e.g. Spade5 Heart5 Club5 Diamond5 => 5555
DJPoker DJDesktopPokerController::parallelPoker( const DJPoker& poker, bool isStrictSuit ) const
{
	if ( isStrictSuit )
		return DJPoker( poker.suit(), poker.point() );
	else
		return DJPoker( DJPoker::AnySuit, poker.point() );
}
//serveral serial pokers can be linked to become shun
//e.g. Spade3 Heart4 Club5
DJPoker DJDesktopPokerController::higherSerialPoker( const DJPoker& poker, bool isStrictSuit ) const
{
	DJPoker::Point point	= poker.point();
	if ( point >= DJPoker::Num2 && point <= DJPoker::Queen ) {
		point	= (DJPoker::Point)(point + 1);
	}else if ( DJPoker::King == point ) {
		point	= DJPoker::Ace;
	}else {
		return DJPoker();
	}
	if ( isStrictSuit )
		return DJPoker( poker.suit(), point );
	else
		return DJPoker( DJPoker::AnySuit, point );
}
DJPoker DJDesktopPokerController::lowerSerialPoker( const DJPoker& poker, bool isStrictSuit ) const
{
	DJPoker::Point point	= poker.point();
	if ( point >= DJPoker::Num3 && point <= DJPoker::King ) {
		point	= (DJPoker::Point)(point - 1);
	}else if ( DJPoker::Ace == point ) {
		point	= DJPoker::King;
	}else {
		return DJPoker();
	}
	if ( isStrictSuit )
		return DJPoker( poker.suit(), point );
	else
		return DJPoker( DJPoker::AnySuit, point );
}
bool DJDesktopPokerController::isHighestSerialPoker( const DJPoker& poker ) const
{
	return false;
}
bool DJDesktopPokerController::isLowestSerialPoker( const DJPoker& poker ) const
{
	return false;
}
QList<DJPoker> DJDesktopPokerController::findGroup( const QHash<DJPoker,int> countPokers,
													const DJPoker& keyPoker,
											 		int width,
											 		bool isStrictSuit,
											 		bool isExactWidth ) const
{
	if ( !keyPoker.isValid() )
    	return QList<DJPoker>();
	//djDebug()<<"DJDesktopPokerController::findGroup";
	if ( isStrictSuit ) {
		return getGroupFromOneOfSuits( countPokers, keyPoker, width, isExactWidth );
	}else {
	 	return getGroupFromAllOfSuits( countPokers, keyPoker, width, isExactWidth );
	}
}
QList<DJPoker> DJDesktopPokerController::findNextGroup( const QHash<DJPoker,int> countPokers,
												const DJPoker& currentKeyPoker, 
												DJPoker& nextKeyPoker,
												int width,
												bool isHigher,
												bool isStrictSuit,
												bool isExactWidth ) const
{
	if ( !currentKeyPoker.isValid() ) {
		return QList<DJPoker>();
	}
	//djDebug()<<"DJDesktopPokerController::findNextGroup"
		//	<<"isHigher"<<isHigher<<"current keyPoker"<<currentKeyPoker.description();
	
	if ( isHigher ) {
		nextKeyPoker	= higherSerialPoker( currentKeyPoker, isStrictSuit );
	}else {
		nextKeyPoker	= lowerSerialPoker( currentKeyPoker, isStrictSuit );
	}
	//djDebug()<<"nextKeyPoker"<<nextKeyPoker.description();
    return findGroup( countPokers, nextKeyPoker, width, isStrictSuit, isExactWidth );
}
QList<DJPoker> DJDesktopPokerController::findShun( const QHash<DJPoker,int> countPokers,
												const DJPoker& keyPoker,
										 		int width, int length, bool isHigher,
										 		bool isStrictSuit,
										 		bool isExactWidth ) const
{
	QList<DJPoker> pokers;
	DJPoker nextKeyPoker	= keyPoker;
	while( length-- ) {
       	QList<DJPoker> nextGroup	= findGroup( countPokers, nextKeyPoker,
       											width, isStrictSuit, isExactWidth );
       	if ( nextGroup.isEmpty() )
       		return QList<DJPoker>();
       	pokers << nextGroup;
       	if ( 0 == length )
       		return pokers;
       	if ( isHigher )
			nextKeyPoker	= higherSerialPoker( nextKeyPoker, isStrictSuit );
		else
			nextKeyPoker	= lowerSerialPoker( nextKeyPoker, isStrictSuit );
		if ( !nextKeyPoker.isValid()
			|| (isHighestSerialPoker( nextKeyPoker ) && 1 != length) )
            return QList<DJPoker>();
    }
    return QList<DJPoker>();
}										 		
QList<DJPoker> DJDesktopPokerController::iterateGroup( const QHash<DJPoker,int>& countPokers,
									int width, DJPoker& keyPoker,
									QSet<DJPoker>& excluder,
									bool isStrictSuit,
									bool isExactWidth ) const
{	
	QHash<DJPoker,int>::const_iterator it	= countPokers.constBegin();
	while( it != countPokers.constEnd() ) {
		keyPoker	= parallelPoker( it.key(), isStrictSuit );
		if ( !excluder.contains( keyPoker ) ) {
			QList<DJPoker> group	= findGroup( countPokers, keyPoker, width, isStrictSuit, isExactWidth );
			if ( !group.isEmpty() ) {
				excluder.insert( keyPoker );
				return group;
			}
		}
		++it;
	}
	return QList<DJPoker>();
}
QList<DJPoker> DJDesktopPokerController::iterateShun( const QHash<DJPoker,int>& countPokers,
						 			int width, int length,
						 			DJPoker& keyPoker,
						 			QSet<DJPoker>& excluder,
						 			bool isStrictSuit,
						 			bool isExactWidth ) const
{
	//djDebug()<<"DJGamePokerDesktop::iterateShun"<<width<<length<<isStrictSuit;
	if ( length <= 0 )
		return QList<DJPoker>();
	if ( 1 == length )
		return iterateGroup( countPokers, width, keyPoker, excluder,
							isStrictSuit, isExactWidth );
		
	QSet<DJPoker> groupExcluder;
	while( true ) {
		QList<DJPoker> firstGroup = iterateGroup( countPokers, width,
												keyPoker, groupExcluder,
												isStrictSuit, isExactWidth );
		
		if ( firstGroup.isEmpty() )
			return QList<DJPoker>();
		//djDebug()<<"firstGroup"<<firstGroup.first().description();
		//djDebug()<<"keyPoker"<<keyPoker.description();
		
		if ( !excluder.contains( keyPoker ) ) {
			//go on find the next groups of shun
			//djDebug()<<"go to find next group";
        	QList<DJPoker> shunPokers;
        	shunPokers << firstGroup;
        	
        	bool foundShun	= true;
        	int count	= length - 1;//still need count group to complete shun
        	
        	DJPoker currentKeyPoker	= keyPoker;
        	while( count > 0 ) {
        		//djDebug()<<"count"<<count;
        		DJPoker nextKeyPoker;
        		QList<DJPoker> nextGroup	= findNextGroup( countPokers, currentKeyPoker,
        										nextKeyPoker, width, true,
        										isStrictSuit, isExactWidth );
        		if ( nextGroup.isEmpty() ) {
        			//djDebug()<<"no next group";
            		foundShun	= false;
            		break;
            	}
            	//djDebug()<<"nextGroup"<<nextGroup.first().description();
				//djDebug()<<"currentKeyPoker"<<currentKeyPoker.description();
				//djDebug()<<"nextKeyPoker"<<nextKeyPoker.description();
				
            	if ( isHighestSerialPoker( nextKeyPoker ) && 1 != count ) {
            		//djDebug()<<"reach the highest";
            		foundShun	= false;
            		break;
            	}
            	count--;
            	currentKeyPoker = nextKeyPoker;
            	shunPokers << nextGroup;
        	}
        	if ( foundShun ) {
    			//found shun, add key poker to excluder to avoid reporting again
    			//djDebug()<<"found shun"<<keyPoker.description();
    			excluder.insert( keyPoker );
    			return shunPokers;
    		}
		}
	}
	return QList<DJPoker>();
}
//indexes should be empty before iteration
//indexed is empty means end
void DJDesktopPokerController::iterateCombination( int total, int length, QList<bool>& indexes ) const
{
	if ( indexes.isEmpty() ) {
		for (int i = 0; i < total; ++i ) {
			if ( i < length )
				indexes.append( true );
			else
				indexes.append( false );
		}
		return;
	}
	
	bool found	= false;
	int i;
	int countFalse	= 0;
	int countTrue	= 0;
	for( i = 0; i < indexes.size() - 1; ++i ) {
		if ( indexes.at(i) && !indexes.at(i+1) ) {
			//found 1 0,exchange
			indexes[i]	= false;
			indexes[i+1]	= true;
			found	= true;
			break;
		}
		if ( indexes.at(i) )
			countTrue++;
		else
			countFalse++;
	}
	if ( found ) {
		//move all 1 before found point to left
		for (int j = 0; j < qMin(countFalse,countTrue); ++j )
			indexes.swap(j,i-j-1);
	}else {
		indexes.clear();
	}
}
bool DJDesktopPokerController::removeSubset( QList<DJPoker>& pokers, const QList<DJPoker>& subset ) const
{
	bool removed	= false;
	foreach( DJPoker poker, subset ) {
		int index	= pokers.indexOf( poker );
		if ( -1 != index ) {
			pokers.removeAt( index );
			removed	= true;
		}
	}
	return removed;
}
void DJDesktopPokerController::selectSelfHandPokers( const QList<DJPoker>& pokersToBeSelected )
{	
	QList<DJPoker> pokers	= pokersToBeSelected;
	DesktopItems& items	= desktopItems( panelController()->seatId(), POKER_TYPE_HAND );
	foreach( DJDesktopItem *pokerItem, items ) {
		QGraphicsItem *item	= pokerItem->graphicsItem();
		if ( !item )
			continue;
		if ( DJGraphicsPixmapItem::DJGraphicsPixmapItemType == item->type() ) {
			DJGraphicsPixmapItem *imageItem	= (DJGraphicsPixmapItem*)item;
			if ( imageItem ) {
				imageItem->setDJSelected( false );
				quint8 card	= pokerItem->value();
				DJPoker poker( card, m_isBridgeCard );
				QList<DJPoker> subset;
				subset	<< poker;
				if ( removeSubset( pokers, subset ) ) {
					imageItem->setDJSelected( true );
				}
				imageItem->adjustPos( desktop()->graphicsMatrix() );
			}
		}else if ( DJGraphicsSvgItem::DJGraphicsSvgItemType == item->type() ) {
			DJGraphicsSvgItem *imageItem	= (DJGraphicsSvgItem*)item;
			if ( imageItem ) {
				imageItem->setDJSelected( false );
				quint8 card	= pokerItem->value();
				DJPoker poker( card, m_isBridgeCard );
				QList<DJPoker> subset;
				subset	<< poker;
				if ( removeSubset( pokers, subset ) ) {
					imageItem->setDJSelected( true );
				}
				imageItem->adjustPos( desktop()->graphicsMatrix() );
			}
		}
	}
}
QList<DJPoker> DJDesktopPokerController::getGroupFromAllOfSuits( const QHash<DJPoker,int> countPokers,
											const DJPoker& keyPoker, int width,
											bool isExactWidth ) const
{	
	//djDebug()<<"DJDesktopPokerController::getGroupFromAllOfSuits";
	
	int comboSuit	= keyPoker.suit();
	DJPoker::Point	point	= keyPoker.point();
	
	QList<DJPoker> pokers;
	int totalCount	= 0;
	foreach( int suit, allSuits ) {
		if ( comboSuit & suit ) {
			DJPoker poker	= DJPoker(suit,point);
			int count = countPokers.value( poker );
			for( int i = 0; i < count; ++i ) {
				pokers << poker;
			}
			totalCount	+= count;
		}
	}
	if ( isExactWidth ) {
		if ( totalCount == width )
			return pokers;
		else
			return QList<DJPoker>();
	}else {
		if ( totalCount >= width ) {
			return pokers.mid(0,width);
		}else
			return QList<DJPoker>();
	}
}
QList<DJPoker> DJDesktopPokerController::getGroupFromOneOfSuits( const QHash<DJPoker,int> countPokers,
											const DJPoker& keyPoker, int width,
											bool isExactWidth ) const
{
	//djDebug()<<"DJDesktopPokerController::getGroupFromOneOfSuits";
	
	//djDebug()<<"keyPoker"<<keyPoker.description();
	//djDebug()<<"width"<<width<<"isExactWidth"<<isExactWidth;
	
	int comboSuit	= keyPoker.suit();
	DJPoker::Point	point	= keyPoker.point();
	//djDebug()<<"comboSuit"<<hex<<comboSuit;
	QList<DJPoker> pokers;
	foreach( int suit, allSuits ) {
		//djDebug()<<"suit"<<hex<<suit;
		if ( comboSuit & suit ) {
			//djDebug()<<"match suit";
			DJPoker poker	= DJPoker(suit,point);
			int count = countPokers.value( poker );
			//djDebug()<<"count"<<count;
			if ( (isExactWidth && count == width)
				|| (!isExactWidth && count >= width) ) {
        		for( int i = 0; i < width; ++i ) {
					pokers << poker;
				}
        		return pokers;
        	}
		}
	}
	return QList<DJPoker>();
}

QString DJDesktopPokerController::pokerResPath(const DJPoker& poker,bool &isSvg) const
{
	/*
	unsigned char chCardIndex;
	
	if(DJGAME_POKER_ISBK(chCard))
        chCardIndex = 54;
    else if(DJGAME_POKER_ISSK(chCard))
        chCardIndex = 53;
	else if(chCard == DJGAME_POKER_BACKGROUND)
		chCardIndex = 55;
    else
        chCardIndex = (DJGAME_POKER_CLASS(chCard)-1)*13 + (DJGAME_POKER_VALUE(chCard));
    
    if(chCardIndex ==0 || chCardIndex > 55)
        chCardIndex = 55;

	QString resPath	= QString(":/BaseRes/svg/poker/%1.svg").arg(chCardIndex,2,10,QLatin1Char('0'));

	isSvg	= true;
	if ( 55 == chCardIndex ) {
#ifdef BACK_POKER_USING_PIXMAP        
		isSvg	= false;
#endif
	}
	return resPath;
	*/
	isSvg	= true;
	if ( !poker.isValid() ) {
#ifdef BACK_POKER_USING_PIXMAP
		isSvg	= false;
#endif       
		return QString(":/BaseRes/svg/poker/back.svg");
	}
	if ( poker.isBigJoker() )
		return QString(":/BaseRes/svg/poker/bigjoker.svg");
	if ( poker.isSmallJoker() )
		return QString(":/BaseRes/svg/poker/smalljoker.svg");
		
	QString suit;
	switch( poker.suit() ) {
	case DJPoker::Diamond:
		suit	= "d";
		break;
	case DJPoker::Club:
		suit	= "c";
		break;
	case DJPoker::Heart:
		suit	= "h";
		break;
	case DJPoker::Spade:
		suit	= "s";
		break;
	default:
		break;
	}
	if ( suit.isEmpty() ) {
#ifdef BACK_POKER_USING_PIXMAP
		isSvg	= false;
#endif       
		return QString(":/BaseRes/svg/poker/back.svg");
	}
	int value	= (int)poker.point();
	return QString(":/BaseRes/svg/poker/%1%2.svg").arg(suit).arg(value,0,16);
}

QList<DJPoker> DJDesktopPokerController::pokersFromDesktopItems( const DesktopItems& items ) const
{
	QList<DJPoker> pokers;
	foreach( DJDesktopItem *item, items ) {
		DJDesktopPokerItem *pokerItem	= (DJDesktopPokerItem *)item;
		DJPoker poker	= pokerItem->poker();
		pokers.append( poker );
	}
	return pokers;
}
QByteArray DJDesktopPokerController::cardsFromPokers( const QList<DJPoker>& pokers ) const
{
	QByteArray buf;
	foreach( DJPoker poker, pokers ) {
		buf.append( poker.toByte( isBridgeCard() ) );
	}
	return buf;
}
void DJDesktopPokerController::createDJGamePokerImage( const QByteArray& cards, DJGamePokerImage& pokerImage )
{
	memset(&pokerImage,0,sizeof(DJGamePokerImage));
	for( int i = 0; i < cards.size(); i++ ) {
		quint8 card	= cards[i];
		pokerImage.chPage[card]++;
	}
}
DJDesktopItem* DJDesktopPokerController::createDesktopItem( int seat, int type, int value )
{
	return new DJDesktopPokerItem(seat,type,value,m_isBridgeCard);
}
DJDesktopItem* DJDesktopPokerController::takeDesktopItem( int seat, int type, int value )
{
	DesktopItems& items	= desktopItems( seat, type );
	DesktopItemsIterator errorItemIt	= items.end();
	DesktopItemsIterator it	= items.begin();
	while( it != items.end() ) {
		DJDesktopItem *item	= *it;
		if ( DJGAME_POKER_ERRORCARD(item->value()) ) {
			errorItemIt	= it;
		}
		if ( value == item->value() ) {
			items.erase(it);
			return item;
		}
		it++;
	}
	if ( errorItemIt != items.end() ) {
		DJDesktopItem *item	= *errorItemIt;
		items.erase( errorItemIt );
		return item;
	}
	return 0;
}
void DJDesktopPokerController::repaintHandPokers( int seat, int type, bool repaintSize, bool repaintPos, bool resort, qreal scale )
{
	djDebug()<<"DJDesktopPokerController::repaintHandPokers"<<seat<<type<<repaintSize<<repaintPos<<resort<<scale;
	
	DesktopItems &items	= desktopItems( seat, type );
	if ( items.isEmpty() )
		return;
	djDebug()<<"items size="<<items.size();
	
	if ( resort ) {
		sortPokers( items );
	}
	quint8 view = seat2View(seat);
	if ( 0 == scale )
		scale	= scaleOfHand(view);
	int len	= calculateDistanceOfPokers( items, scale, false );
	int startx	= 0;
	int starty	= 0;
	qreal z	= 500;
	Qt::Alignment align	= 0;

	switch( view ) {
		case 1 : {
			QPen pen( Qt::green );
			pen.setWidth( 2 );
			QPoint deltaPos( 0, -20 );
			
			startx	= (len > availableSpace().width()) ? availableSpace().left() : centerPoint().x() - (len >> 1);
			starty	= availableSpace().bottom();
			z	+= 100;
			align	= Qt::AlignBottom;
			repaintPokerItems( items, scale, align,
							repaintSize, repaintPos, view,
							startx, starty, z,
							QPoint(1,0), deltaOfNewline(), availableSpace().right(),
							true, pen, deltaPos );
			break;
		}
		case 2 :
			startx	= availableSpace().left() + (pokerSize().height() * scale);
			starty	= (len > poolSpace().height()) ? poolSpace().top() : centerPoint().y() - (len >> 1);
			repaintPokerItems( items, scale, align,
							repaintSize, repaintPos, view,
							startx, starty, z,
							QPoint(0,1), -deltaOfNewline(), poolSpace().bottom() );
			break;
		case 3 :
			startx	= (len > poolSpace().width()) ? poolSpace().left() : centerPoint().x() - (len >> 1);
			starty	= availableSpace().top();
			z	-= 100;
			repaintPokerItems( items, scale, align,
							repaintSize, repaintPos, view,
							startx, starty, z,
							QPoint(1,0), deltaOfNewline(), poolSpace().right() );
			break;
		case 4 :
			startx	= availableSpace().right() - (pokerSize().height() * scale) - 4;
			starty	= (len > poolSpace().height()) ? poolSpace().bottom() : centerPoint().y() + (len >> 1);
			repaintPokerItems( items, scale, align,
							repaintSize, repaintPos, view,
							startx, starty, z,
							QPoint(0,-1), deltaOfNewline(), poolSpace().top() );
			break;
	}
}
void DJDesktopPokerController::repaintThrownPokers( int seat, int type, qreal scale )
{
	djDebug()<<"DJDesktopPokerController::repaintThrownPokers"<<seat;
	
	DesktopItems &items	= desktopItems( seat, type );
	if ( items.isEmpty() )
		return;
	djDebug()<<"items size="<<items.size();
	
	sortPokers( items );
	
	quint8 view = seat2View(seat);
	if ( 0 == scale )
		scale	= scaleOfThrown(view);
	int len	= calculateDistanceOfPokers( items, scale );
	int startx	= 0;
	int starty	= 0;
	qreal z	= 1000;
	Qt::Alignment align	= 0;
	switch( view ) {
		case 1 :
			startx	= (len > availableSpace().width()) ? availableSpace().left() : centerPoint().x() - (len >> 1);
			starty	= centerSpace().bottom();
			z	+= 100;
			repaintPokerItems( items, scale, align,
							false, true, 1,
							startx, starty, z, 
							QPoint(1,0), deltaOfNewline(), availableSpace().right() );
			break;
		case 2 :
			startx	= (len > centerSpace().left() - availableSpace().left()) ? availableSpace().left() : centerSpace().left() - len;
			starty	= centerPoint().y();
			align	= Qt::AlignVCenter;
			repaintPokerItems( items, scale, align,
							false, true, 1,
							startx, starty, z, 
							QPoint(1,0), deltaOfNewline(), centerSpace().left() );
			break;
		case 3 :
			startx	= (len > availableSpace().width()) ? availableSpace().left() : centerPoint().x() - (len >> 1);
			starty	= centerSpace().top();
			z	-= 100;
			align	= Qt::AlignBottom;
			repaintPokerItems( items, scale, align,
							false, true, 1,
							startx, starty, z, 
							QPoint(1,0), deltaOfNewline(), poolSpace().right() );
			break;
		case 4 :
			startx	= (len > availableSpace().right() - centerSpace().right()) ? centerSpace().right() : centerSpace().right();
			starty	= centerPoint().y();
			align	= Qt::AlignVCenter;
			repaintPokerItems(items, scale, align,
							false, true, 1,
							startx, starty, z, 
							QPoint(1,0), deltaOfNewline(), availableSpace().right() );
			break;
	}
}
void DJDesktopPokerController::repaintAttachedPokers( int seat, int type, qreal scale )
{
	djDebug()<<"DJDesktopPokerController::repaintAttachedPokers"<<seat;
	
	DesktopItems &items	= desktopItems( seat, type );
	if ( items.isEmpty() )
		return;
	djDebug()<<"items size="<<items.size();
	
	quint8 view = seat2View(seat);
	if ( 0 == scale )
		scale	= scaleOfAttached(view);
	int len	= calculateDistanceOfPokers( items, scale );
	int startx	= 0;
	int starty	= 0;
	qreal z	= 200;
	Qt::Alignment align	= 0;
	switch( view )
	{
		case 1 :
			startx	= 0;
			starty	= poolSpace().bottom();
			repaintPokerItems( items, scale, align,
							false, true, view,
							startx, starty, z, 
							QPoint(1,0), deltaOfNewline(), poolSpace().left() );
			break;
		case 2 :
			startx	= rectOfSeatShow(seat).right();
			starty	= (len > rectOfSeatShow(seat).top() - availableSpace().top()) ? availableSpace().top() : rectOfSeatShow(seat).top() - len;
			repaintPokerItems( items, scale, align,
							false, true, view,
							startx, starty, z, 
							QPoint(0,1), -deltaOfNewline(), availableSpace().bottom() );
			break;
		case 3 :
			startx	= rectOfSeatShow(seat).right();
			starty	= rectOfSeatShow(seat).top();
			repaintPokerItems( items, scale, align,
							false, true, view,
							startx, starty, z, 
							QPoint(1,0), deltaOfNewline(), availableSpace().right() );
			break;
		case 4 :
			startx	= rectOfSeatShow(seat).left();
			starty	= rectOfSeatShow(seat).top() - 4;
			repaintPokerItems( items, scale, align,
							false, true, view,
							startx, starty, z,
							QPoint(0,-1), deltaOfNewline(), availableSpace().top() );
			break;
	}
}
void DJDesktopPokerController::repaintPokerItems(
	DesktopItems &items, qreal scale, Qt::Alignment align,
	bool repaintSize, bool repaintPos, quint8 view,
	int startx, int starty, qreal z, 
	const QPoint& directionOfNextItem, int deltaOfLine, int border,
	bool hasPen, const QPen& pen, const QPoint& deltaPosBySelection )
{
	djDebug()<<"DJDesktopPokerController::repaintPokerItems";
	
	int x	= startx;
	int y	= starty;
	qreal	angle	= 0;
	switch( view ) {
	case 2:
		angle	= 90;
		break;
	case 4:
		angle	= 270;
		break;
	default:
		break;
	}
	DesktopItemsIterator it	= items.begin();
	while( it != items.end() ) {
	//foreach( DJDesktopItem *item, items ) {
		DJDesktopPokerItem *pokerItem	= (DJDesktopPokerItem*)*it;
		QGraphicsItem *item	= pokerItem->graphicsItem();
		if ( 0 == item ) {
			bool isSvg;
			QString resPath;
			if ( pokerItem->isBack() ) {
				resPath	= pokerResPath( DJPoker(), isSvg );
			}else {
				resPath	= pokerResPath( pokerItem->poker(), isSvg );
			}
			//djDebug()<<"resPath"<<resPath;
			QSvgRenderer *svgRenderer	= pokerSvgCache( resPath );
			if ( isSvg ) {
				DJGraphicsSvgItem *imageItem	= new DJGraphicsSvgItem;
				imageItem->setSharedRenderer( svgRenderer );
				desktop()->scene()->addItem( imageItem );
				
				pokerItem->setGraphicsItem( imageItem );
				imageItem->setVirtualPos( QPointF(x,y) );
				imageItem->setDeltaPos( deltaPosBySelection );
				imageItem->setAlignment( align );
				imageItem->setExternalScale( desktop()->graphicsScale() );
				imageItem->setInternalScale( scale );
				imageItem->setAngleOfRotation( angle );
				imageItem->setZValue(z++);
				imageItem->setDJSelected( false );
				if ( hasPen )
					imageItem->setPen( pen );
				imageItem->adjustPos( desktop()->graphicsMatrix() );
				imageItem->show();
			}else {
				QPixmap pix	= pokerPixCache( resPath );
				DJGraphicsPixmapItem *imageItem	= new DJGraphicsPixmapItem( pix, 0, desktop()->scene() );	
				pokerItem->setGraphicsItem( imageItem );
				imageItem->setVirtualPos( QPointF(x,y) );
				imageItem->setDeltaPos( deltaPosBySelection );
				imageItem->setAlignment( align );
				imageItem->setExternalScale( desktop()->graphicsScale() );
				imageItem->setInternalScale( scale );
				imageItem->setAngleOfRotation( angle );
				imageItem->setZValue(z++);
				imageItem->setDJSelected( false );
				if ( hasPen )
					imageItem->setPen( pen );
				imageItem->adjustPos( desktop()->graphicsMatrix() );
				imageItem->show();
			}
		}else {
			//already load image
			if ( DJGraphicsPixmapItem::DJGraphicsPixmapItemType == item->type() ) {
				DJGraphicsPixmapItem *imageItem	= (DJGraphicsPixmapItem*)item;
				if ( repaintPos ) {
					imageItem->setVirtualPos( QPointF(x,y) );
					imageItem->setDeltaPos( deltaPosBySelection );
					imageItem->setAlignment( align );
				}
				if ( repaintSize ) {
					imageItem->setExternalScale( desktop()->graphicsScale() );
					imageItem->setInternalScale( scale );
				}
				imageItem->setZValue(z++);
				imageItem->setDJSelected( false );
				if ( hasPen )
					imageItem->setPen( pen );
				imageItem->setAngleOfRotation( angle );
				imageItem->adjustPos( desktop()->graphicsMatrix() );
				imageItem->show();
			}else if ( DJGraphicsSvgItem::DJGraphicsSvgItemType == item->type() ) {
				DJGraphicsSvgItem *imageItem	= (DJGraphicsSvgItem*)item;
				if ( repaintPos ) {
					imageItem->setVirtualPos( QPointF(x,y) );
					imageItem->setDeltaPos( deltaPosBySelection );
					imageItem->setAlignment( align );
				}
				if ( repaintSize ) {
					imageItem->setExternalScale( desktop()->graphicsScale() );
					imageItem->setInternalScale( scale );
				}
				imageItem->setZValue(z++);
				imageItem->setDJSelected( false );
				if ( hasPen )
					imageItem->setPen( pen );
				imageItem->setAngleOfRotation( angle );
				imageItem->adjustPos( desktop()->graphicsMatrix() );
				imageItem->show();
			}
		}
		
		//calculate next item's pos
		DesktopItemsIterator nextIt	= it + 1;
		if ( nextIt == items.end() )
			break;
		DJDesktopPokerItem *nextPokerItem	= (DJDesktopPokerItem*)*nextIt;
		
		int nextItemLen	= static_cast<int>((pokerItem->distance() + nextPokerItem->length()) * scale);
		int newlineDelta	= static_cast<int>( deltaOfLine * scale );
		int distance	= static_cast<int>( pokerItem->distance() * scale );
		if ( directionOfNextItem.x() > 0 ) {
			if ( x + nextItemLen > border ) {
				if ( DJGAME_POKER_ERRORCARD(pokerItem->value()) )
					break;
				x	= startx;
				y	+= newlineDelta;
			}else {
				x	+= distance;
			}
		}else if ( directionOfNextItem.x() < 0 ) {
			if ( x - nextItemLen < border ) {
				if ( DJGAME_POKER_ERRORCARD(pokerItem->value()) )
					break;
				x	= startx;
				y	+= newlineDelta;
			}else {
				x	-= distance;
			}
		}else if ( directionOfNextItem.y() > 0 ) {
			if ( y + nextItemLen > border ) {
				if ( DJGAME_POKER_ERRORCARD(pokerItem->value()) )
					break;
				x	+= newlineDelta;
				y	= starty;
			}else {
				y	+= distance;
			}
		}else if ( directionOfNextItem.y() < 0 ) {
			if ( y - nextItemLen < border ) {
				if ( DJGAME_POKER_ERRORCARD(pokerItem->value()) )
					break;
				x	+= newlineDelta;
				y	= starty;
			}else {
				y	-= distance;
			}
		}
		it++;
	}
}
int DJDesktopPokerController::lengthOfChips( DesktopItems& items, qreal scale ) const
{
	int length	= 0;
	
	if ( !items.isEmpty() ) {
		length	= (items.size() - 1) * chipDistance() + chipSize().width();
	}
	return static_cast<int>(length * scale);
}
void DJDesktopPokerController::repaintWagerChips( int seat, int type, DJGraphicsTextItem *textItem, int chipValue, bool repaintSize, bool repaintPos, qreal scale )
{
	djDebug()<<"DJDesktopPokerController::repaintWagerChips"<<seat;
	
	DesktopItems &items	= desktopItems( seat, type );
	if ( items.isEmpty() ) {
		textItem->hide();
		return;
	}
	djDebug()<<"items size="<<items.size();
	
	quint8 view = seat2View(seat);
	if ( 0 == scale )
		scale	= scaleOfWagerChips(view);
	int len	= lengthOfChips( items, scale );
	djDebug()<<"len"<<len;
	int startx	= 0;
	int starty	= 0;
	qreal z	= 1000;
	Qt::Alignment align	= 0;
	int angle	= 0;
	
	int wager	= chipValue * items.size();
	textItem->setPlainText( QString::number(wager) );
	
	switch( view ) {
		case 1 :
			startx	= (len > availableSpace().width()) ? availableSpace().left() : centerPoint().x() - (len >> 1);
			starty	= centerSpace().bottom();
			z	+= 100;
			repaintChipItems( items, scale, align,
							repaintSize, repaintPos, view,
							startx, starty, z,
							QPoint(1,0), deltaOfNewline(), availableSpace().right() );
			textItem->setVirtualPos( QPoint(centerPoint().x(),centerSpace().bottom()+m_chipSize.height()/2) );
			textItem->setAlignment( Qt::AlignCenter );
			break;
		case 2 : {
			startx	= centerSpace().left();
			starty	= (len > poolSpace().height()) ? poolSpace().top() : centerPoint().y() - (len >> 1);
			repaintChipItems( items, scale, align,
							repaintSize, repaintPos, view,
							startx, starty, z,
							QPoint(0,1), -deltaOfNewline(), poolSpace().bottom() );
			int x	= centerSpace().left() - (m_chipSize.width()>>1);
			int y	= centerPoint().y();
			x	+=	static_cast<int>(textItem->boundingRect().height())>>1;
			y	-=	static_cast<int>(textItem->boundingRect().width())>>1;
			textItem->setVirtualPos( QPoint(x,y) );
			textItem->setAlignment( 0 );
			angle	= 90;
			break;
		}
		case 3 :
			startx	= (len > availableSpace().width()) ? availableSpace().left() : centerPoint().x() - (len >> 1);
			starty	= centerSpace().top();
			z	-= 100;
			align	= Qt::AlignBottom;
			repaintChipItems( items, scale, align,
							repaintSize, repaintPos, view,
							startx, starty, z,
							QPoint(1,0), deltaOfNewline(), poolSpace().right() );
			textItem->setVirtualPos( QPoint(centerPoint().x(),centerSpace().top()-m_chipSize.height()/2) );
			textItem->setAlignment( Qt::AlignCenter );
			break;
		case 4 : {
			startx	= centerSpace().right();
			starty	= (len > poolSpace().height()) ? poolSpace().bottom() : centerPoint().y() + (len >> 1);
			repaintChipItems( items, scale, align,
							repaintSize, repaintPos, view,
							startx, starty, z,
							QPoint(0,-1), deltaOfNewline(), poolSpace().top() );
			int x	= centerSpace().right() + (m_chipSize.width()>>1);
			int y	= centerPoint().y();
			x	-=	static_cast<int>(textItem->boundingRect().height())>>1;
			y	+=	static_cast<int>(textItem->boundingRect().width())>>1;
			textItem->setVirtualPos( QPoint(x,y) );
			textItem->setAlignment( 0 );
			angle	= 270;
			break;
		}
		default:
			break;
	}
	textItem->setAngleOfRotation(angle);
	textItem->setExternalScale( desktop()->graphicsScale() );
	textItem->adjustPos( desktop()->graphicsMatrix() );
	textItem->show();
}
void DJDesktopPokerController::repaintChipItems(
	DesktopItems &items, qreal scale, Qt::Alignment align,
	bool repaintSize, bool repaintPos, quint8 view,
	int startx, int starty, qreal z,
	const QPoint& directionOfNextItem, int deltaOfLine, int border,
	bool hasPen, const QPen& pen, const QPoint& deltaPosBySelection )
{
	djDebug()<<"DJDesktopPokerController::repaintChipItems";
	
	int x	= startx;
	int y	= starty;
	qreal	angle	= 0;
	switch( view ) {
	case 2:
		angle	= 90;
		break;
	case 4:
		angle	= 270;
		break;
	default:
		break;
	}
	DesktopItemsIterator it	= items.begin();
	while( it != items.end() ) {
		DJDesktopItem *desktopItem	= (DJDesktopItem*)*it;
		djDebug()<<"desktopItem"<<desktopItem->seat()<<desktopItem->type();
		QGraphicsItem *item	= desktopItem->graphicsItem();
		if ( 0 == item ) {
			int chipValue	= desktopItem->value();
			QPixmap pix	= chipPixmap( chipValue );
			DJGraphicsPixmapItem *imageItem	= new DJGraphicsPixmapItem( pix, 0, desktop()->scene() );	
			desktopItem->setGraphicsItem( imageItem );
			imageItem->setVirtualPos( QPointF(x,y) );
			imageItem->setDeltaPos( deltaPosBySelection );
			imageItem->setAlignment( align );
			imageItem->setExternalScale( desktop()->graphicsScale() );
			imageItem->setInternalScale( scale );
			imageItem->setAngleOfRotation( angle );
			imageItem->setZValue(z++);
			imageItem->setDJSelected( false );
			if ( hasPen )
				imageItem->setPen( pen );
			imageItem->adjustPos( desktop()->graphicsMatrix() );
			imageItem->show();
		}else {
			DJGraphicsPixmapItem *imageItem	= (DJGraphicsPixmapItem*)item;
			if ( repaintPos ) {
				imageItem->setVirtualPos( QPointF(x,y) );
				imageItem->setDeltaPos( deltaPosBySelection );
				imageItem->setAlignment( align );
			}
			if ( repaintSize ) {
				imageItem->setExternalScale( desktop()->graphicsScale() );
				imageItem->setInternalScale( scale );
			}
			imageItem->setZValue(z++);
			imageItem->setDJSelected( false );
			if ( hasPen )
				imageItem->setPen( pen );
			imageItem->setAngleOfRotation( angle );
			imageItem->adjustPos( desktop()->graphicsMatrix() );
			imageItem->show();
		}
		
		//calculate next item's pos
		DesktopItemsIterator nextIt	= it + 1;
		if ( nextIt == items.end() )
			break;
		
		int nextItemLen	= static_cast<int>( (chipDistance() + chipSize().width()) * scale);
		int newlineDelta	= static_cast<int>( deltaOfLine * scale );
		int distance	= static_cast<int>( chipDistance() * scale );
		if ( directionOfNextItem.x() > 0 ) {
			if ( x + nextItemLen > border ) {
				x	= startx;
				y	+= newlineDelta;
			}else {
				x	+= distance;
			}
		}else if ( directionOfNextItem.x() < 0 ) {
			if ( x - nextItemLen < border ) {
				x	= startx;
				y	+= newlineDelta;
			}else {
				x	-= distance;
			}
		}else if ( directionOfNextItem.y() > 0 ) {
			if ( y + nextItemLen > border ) {
				x	+= newlineDelta;
				y	= starty;
			}else {
				y	+= distance;
			}
		}else if ( directionOfNextItem.y() < 0 ) {
			if ( y - nextItemLen < border ) {
				x	+= newlineDelta;
				y	= starty;
			}else {
				y	-= distance;
			}
		}
		it++;
	}
}
QPixmap	DJDesktopPokerController::chipPixmap( int chip ) const
{
	QPixmap pix;
	switch( chip ) {
	case CHIP_RED1:
		pix	= QPixmap(":/BaseRes/image/chip/red1.png");
		break;
	case CHIP_YELLOW1:
		pix	= QPixmap(":/BaseRes/image/chip/yellow1.png");
		break;
	case CHIP_GREEN1:
		pix	= QPixmap(":/BaseRes/image/chip/green1.png");
		break;
	case CHIP_BLUE1:
		pix	= QPixmap(":/BaseRes/image/chip/blue1.png");
		break;
	case CHIP_RED2:
		pix	= QPixmap(":/BaseRes/image/chip/red2.png");
		break;
	case CHIP_YELLOW2:
		pix	= QPixmap(":/BaseRes/image/chip/yellow2.png");
		break;
	case CHIP_GREEN2:
		pix	= QPixmap(":/BaseRes/image/chip/green2.png");
		break;
	case CHIP_BLUE2:
		pix	= QPixmap(":/BaseRes/image/chip/blue2.png");
		break;
	default:
		break;
	}
	return pix;
}