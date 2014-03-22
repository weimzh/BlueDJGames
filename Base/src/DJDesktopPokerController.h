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

#ifndef DJDESKTOPPOKERCONTROLLER_H
#define DJDESKTOPPOKERCONTROLLER_H

#include <QtGui>
#include <QtSvg>
#include "DJBaseGlobal.h"
#include "DJDesktopController.h"
#include "protocol.h"
#include "DJPoker.h"

#define	BACK_POKER_USING_PIXMAP

#define POKER_TYPE_HAND					0x50
#define POKER_TYPE_THROWN				0x51
#define POKER_TYPE_ATTACHED				0x52

#define CHIP_TYPE_WAGER					0x55
#define CHIP_TYPE_HAND					0x56

#define	CHIP_RED1						0x01
#define	CHIP_YELLOW1					0x02
#define	CHIP_GREEN1						0x03
#define	CHIP_BLUE1						0x04
#define	CHIP_RED2						0x11
#define	CHIP_YELLOW2					0x12
#define	CHIP_GREEN2						0x13
#define	CHIP_BLUE2						0x14

class DJDesktopItem;
class DJDesktopPokerItem;
class DJ_BASE_EXPORT DJDesktopPokerController : public DJDesktopController
{
    Q_OBJECT 
public:
	enum SortKey{
		SortByASCount,
		SortBySuit,
		SortByPoint
	};
	enum SortMode{
		ASCountPointSuit,
		PointASCountSuit,
		SuitPoint,
		PointSuit
	};

	DJDesktopPokerController( DJPanelController* panelController, const QSize& size, QWidget *parent );
    virtual ~DJDesktopPokerController();

	//virtual void remainderPositions( QList<QPoint>& points, QList<Qt::Alignment>& aligns, QList<int> &modes );
	
	bool isWaitingThrowAcl() const { return m_isWaitingThrowAcl; }
	void setWaitingThrowAcl( bool isWaitingThrowAcl ) { m_isWaitingThrowAcl = isWaitingThrowAcl; }
	
	QSvgRenderer* pokerSvgCache( const QString& resPath ) const;
	QPixmap pokerPixCache( const QString& resPath ) const;	
	bool isBridgeCard() const { return m_isBridgeCard; }
	QRect poolSpace() const { return m_poolSpace; }
	QRect centerSpace() const { return m_centerSpace; }
	qreal scaleOfHand( int view ) const { return m_scalesOfHand.value(view,1); }
	void setScaleOfHand( int view, qreal scale ) { m_scalesOfHand[view]	= scale; }
	qreal scaleOfThrown( int view ) const { return m_scalesOfThrown.value(view,1); }
	void setScaleOfThrown( int view, qreal scale ) { m_scalesOfThrown[view]	= scale; }
	qreal scaleOfAttached( int view ) const { return m_scalesOfAttached.value(view,1); }
	void setScaleOfAttached( int view, qreal scale ) { m_scalesOfAttached[view]	= scale; }
	qreal scaleOfWagerChips( int view ) const { return m_scalesOfWagerChips.value(view,1); }
	void setScaleOfWagerChips( int view, qreal scale ) { m_scalesOfWagerChips[view]	= scale; }
	QSize pokerSize() const { return m_pokerSize; }
	void setPokerSize( const QSize& pokerSize ) { m_pokerSize = pokerSize; }
	int pokerDistance() const { return m_pokerDistance; }
	void setPokerDistance( int distance ) { m_pokerDistance = distance; } 
	int deltaOfNewline() const { return m_deltaOfNewline; }
	int chipDistance() const { return m_chipDistance; }
	void setChipDistance( int distance ) { m_chipDistance = distance; }
	QSize chipSize() const { return m_chipSize; }
	void setChipSize( const QSize& chipSize ) { m_chipSize = chipSize; }
	int lengthOfChips( DesktopItems& items, qreal scale ) const;
	bool isSelectRectEnabled() const { return m_isSelectRectEnabled; }
	void setSelectRectEnabled( bool enabled ) { m_isSelectRectEnabled = enabled; }
	bool isSelectEnabled() const { return m_isSelectEnabled; }
	void setSelectEnabled( bool enabled ) { m_isSelectEnabled = enabled; }
	int numberOfPokers(int seat, int type);
	
	void throwAllCards();
	virtual void throwSelectedCards();
	
	virtual QByteArray createThrowTrace( DesktopItems& items );
	virtual void receiveThrowAcl( quint8 status );
	virtual bool isChuEnabled() const;
	
	virtual void locateAll();
	virtual void locatePoolSpace( QRect& rect );
	virtual void locateCenterSpace( QRect& rect );
	
	virtual void repaintAll();
	
	QString pokerResPath( const DJPoker& poker, bool &isSvg ) const;
	QPixmap chipPixmap( int chip ) const;
	QList<DJPoker> pokersFromDesktopItems( const DesktopItems& items ) const;
	QByteArray cardsFromPokers( const QList<DJPoker>& pokers ) const;
	void createDJGamePokerImage( const QByteArray& cards, DJGamePokerImage& pokerImage );
	
	virtual DJDesktopItem* createDesktopItem( int seat, int type, int value );
	virtual DJDesktopItem* takeDesktopItem( int seat, int type, int value );
	
	virtual int distanceBetweenPokers( const DJPoker& poker1, const DJPoker& poker2 ) const;
	virtual int rankOnSuit( const DJPoker& poker ) const;
	virtual int rankOnPoint( const DJPoker& poker ) const;
	virtual bool isFirstPokerLefter( const DJPoker& poker1, const DJPoker& poker2 ) const;
	virtual bool isFirstPokerItemLefter( DJDesktopPokerItem* item1, DJDesktopPokerItem* item2 ) const;
	
	virtual void repaintHandPokers( int seat, int type, bool repaintSize, bool repaintPos, bool resort, qreal scale = 0 );
	virtual void repaintThrownPokers( int seat, int type, qreal scale = 0 );
	virtual void repaintAttachedPokers( int seat, int type, qreal scale = 0 );
	virtual void repaintPokerItems( DesktopItems &items, qreal scale, Qt::Alignment align,
									bool repaintSize, bool repaintPos, quint8 view,
									int startx, int starty, qreal z, 
									const QPoint& directionOfNextItem, int deltaOfLine, int border,
									bool hasPen = false, const QPen& pen = QPen(),
									const QPoint& deltaPosBySelection = QPoint() );			
	virtual void repaintWagerChips( int seat, int type, DJGraphicsTextItem *textItem, int chipValue, bool repaintSize, bool repaintPos, qreal scale = 0 );
	virtual void repaintChipItems(
				DesktopItems &items, qreal scale, Qt::Alignment align,
				bool repaintSize, bool repaintPos, quint8 view,
				int startx, int starty, qreal z,
				const QPoint& directionOfNextItem, int deltaOfLine, int border,
				bool hasPen = false, const QPen& pen = QPen(),
				const QPoint& deltaPosBySelection = QPoint()  );
	int calculateDistanceOfPokers( DesktopItems& items, qreal scale, bool isFixedDistance = true );
	int numberOfPokersOnAllSuits( const QHash<DJPoker,int> countPokers, const DJPoker& comboPoker ) const;
	QHash<DJPoker,int> createCountPokers( const QList<DJPoker>& handPokers ) const;
	void setPokerSortKeys( const QList<SortKey>& sortKeys );
	void setPokerSortMode( SortMode mode );
	void setBridgeCard( bool isBridgeCard );
	
	void sortPokers( QList<DJPoker>& pokers );
	void sortPokers( DesktopItems & pokerItems );	
	int	comparePokerWithKey( const DJPoker& poker1, const DJPoker& poker2, SortKey key ) const;
	
	virtual bool groupGreaterThan( const QList<DJPoker>& group1,
									const QList<DJPoker>& group2 );
	virtual bool shunGreaterThan( const QList<DJPoker>& shun1,
										const QList<DJPoker>& shun2 );
	virtual DJPoker parallelPoker( const DJPoker& poker, bool isStrictSuit ) const;	
	virtual DJPoker higherSerialPoker( const DJPoker& poker, bool isStrictSuit ) const;
	virtual DJPoker lowerSerialPoker( const DJPoker& poker, bool isStrictSuit ) const;
	//subclass should implemented the following two methods only for loop serial
	//e.g. Q K A, A 2 3, A can be the highest and the lowest
	//when A is the highest,stop iterate higherSerialPoker
	//when A is the lowest,stop iterate lowerSerialPoker
	virtual bool isHighestSerialPoker( const DJPoker& poker ) const;
	virtual bool isLowestSerialPoker( const DJPoker& poker ) const;
	
	QList<DJPoker> findGroup( const QHash<DJPoker,int> countPokers,
								const DJPoker& keyPoker,
						 		int width,
						 		bool isStrictSuit,
						 		bool isExactWidth ) const;
	QList<DJPoker> findNextGroup( const QHash<DJPoker,int> countPokers,
								const DJPoker& currentKeyPoker,
								DJPoker& nextKeyPoker,
						 		int width, bool isHigher,
						 		bool isStrictSuit,
						 		bool isExactWidth ) const;
	QList<DJPoker> findShun( const QHash<DJPoker,int> countPokers,
								const DJPoker& keyPoker,
						 		int width, int length, bool isHigher,
						 		bool isStrictSuit,
						 		bool isExactWidth ) const;
	QList<DJPoker> iterateGroup( const QHash<DJPoker,int>& countPokers,
							 	int width, DJPoker& keyPoker,
							 	QSet<DJPoker>& excluder,
							 	bool isStrictSuit,
							 	bool isExactWidth ) const;
	QList<DJPoker> iterateShun( const QHash<DJPoker,int>& countPokers,
							 	int width, int length,
							 	DJPoker& keyPoker,
							 	QSet<DJPoker>& excluder,
							 	bool isStrictSuit,
							 	bool isExactWidth ) const;
	void iterateCombination( int total, int length, QList<bool>& indexes ) const;
	bool removeSubset( QList<DJPoker>& pokers, const QList<DJPoker>& subset ) const;
	void selectSelfHandPokers( const QList<DJPoker>& pokersToBeSelected = QList<DJPoker>() );
	void throwPokerItems( DesktopItems& items );
public slots:
	virtual void handleMatrixChanged();
	virtual void handleMousePressed( QGraphicsSceneMouseEvent *mouseEvent );
	virtual void handleMouseMoving( QGraphicsSceneMouseEvent *mouseEvent );
	virtual void handleMouseReleased( QGraphicsSceneMouseEvent *mouseEvent );
private:
	QList<DJPoker> getGroupFromAllOfSuits( const QHash<DJPoker,int> countPokers,
													const DJPoker& keyPoker,
											 		int width, bool isExactWidth ) const;
	QList<DJPoker> getGroupFromOneOfSuits( const QHash<DJPoker,int> countPokers,
													const DJPoker& keyPoker,
										 			int width, bool isExactWidth ) const;
	
	QHash<DJPoker,int>			m_sortingPokers;//poker,count
	bool						m_isBridgeCard;
	QList<SortKey>				m_sortKeys;

	QHash<QString,QSvgRenderer*>	m_pokerSvgCache;
	QHash<QString,QPixmap>			m_pokerPixCache;
	//index is view
	QList<qreal>				m_scalesOfHand;
	QList<qreal>				m_scalesOfThrown;
	QList<qreal>				m_scalesOfAttached;
	QList<qreal>				m_scalesOfWagerChips;
	
	QSize						m_pokerSize,m_chipSize;
	int							m_pokerDistance,m_chipDistance;
	int							m_deltaOfNewline;
	
	QRect						m_poolSpace;
	QRect						m_centerSpace;
	
	QPointF						m_startPoint;
	QGraphicsRectItem			*m_selectRectItem;
	bool						m_isSelectRectEnabled;
	bool						m_isSelectEnabled;
	bool						m_isWaitingThrowAcl;
};

#endif
