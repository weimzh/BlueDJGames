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

#ifndef DJDESKTOPMAHJONGCONTROLLER_H
#define DJDESKTOPMAHJONGCONTROLLER_H

#include <QtGui>
#include <QtSvg>
#include "DJBaseGlobal.h"
#include "DJDesktopController.h"
#include "protocol.h"
#include "mjtype.h"

#define	MAHJONG_TYPE_STANDING_HAND	0x10
#define	MAHJONG_TYPE_LYING_HAND		0x20
#define MAHJONG_MAX_LYING_HAND		4
#define	MAHJONG_TYPE_FLOWER			0x30
#define	MAHJONG_TYPE_TOP_WALL		0x40
#define	MAHJONG_TYPE_BOTTOM_WALL	0x50
#define	MAHJONG_TYPE_POOL			0x60
#define	MAHJONG_TYPE_HU				0x70

class DJGraphicsSvgItem;

class DJ_BASE_EXPORT DJDesktopMahjongController : public DJDesktopController
{
    Q_OBJECT 
public:
	enum Pose {
		Standing,
		Lying,
		Wall,
	};
	DJDesktopMahjongController( DJPanelController* panelController, const QSize& size, QWidget *parent );
    virtual ~DJDesktopMahjongController();
    	
	virtual int bricksPerWall() const;
    virtual void initUnderGameInfo( const QByteArray& buf );
	
	virtual void locateAll();
	virtual void locateWallSpace( QRect& rect );
	virtual void locatePoolSpace( QRect& rect );
	virtual void locateCenterSpace( QRect& rect );
	virtual void locateAllNonMatrimoveItems();
	
	virtual DJDesktopItem* takeDesktopItem( int seat, int type, int value );

	MJCards& selfStandingCards() { return m_selfStandingCards; };
	MahjongHUTypeWrap& selfLyingCards() { return m_selfLyingCards; };
	void syncSelfStandingCards();
	void syncSelfLyingCards();
	MJCardsGroups testChi() const;
	MJCards testPeng() const;
	MJCardsGroups testZimoGang() const;
	MJCards testDianGang() const;
	int testHu( MJCard qiangGangCard ) const;
	MJCard testHua() const;
	void setSupportedHuStyle( quint16 mask ) { m_supportedHuStyle = mask; }
	QString fanName( int fan ) const;
	
	QRect wallSpace() const { return m_wallSpace; }
	QRect poolSpace() const { return m_poolSpace; }
	QRect centerSpace() const { return m_centerSpace; }
	int	selfEdge() const { return m_selfEdge; }
	
	qreal scaleOfViewStandingKnownHand( int view ) const { return m_scaleOfViewStandingKnownHand.value(view,1); }
	qreal scaleOfViewStandingUnknownHand( int view ) const { return m_scaleOfViewStandingUnknownHand.value(view,1); }
	qreal scaleOfViewLyingHand( int view ) const { return m_scaleOfViewLyingHand.value(view,1); }
	qreal scaleOfViewFlower( int view ) const { return m_scaleOfViewFlower.value(view,1); }
	qreal scaleOfViewWall( int view ) const { return m_scaleOfViewWall.value(view,1); }
	qreal scaleOfViewPool( int view ) const { return m_scaleOfViewPool.value(view,1); }
	qreal scaleOfViewChu( int view ) const { return m_scaleOfViewChu.value(view,1); }
	void setScaleOfViewStandingKnowHand( const QList<qreal>& scales ) { m_scaleOfViewStandingKnownHand	= scales; }
	void setScaleOfViewStandingUnknowHand( const QList<qreal>& scales ) { m_scaleOfViewStandingUnknownHand	= scales; }
	void setScaleOfViewLyingHand( const QList<qreal>& scales ) { m_scaleOfViewLyingHand	= scales; }
	void setScaleOfViewFlower( const QList<qreal>& scales ) { m_scaleOfViewFlower	= scales; }
	void setScaleOfViewWall( const QList<qreal>& scales ) { m_scaleOfViewWall	= scales; }
	void setScaleOfViewPool( const QList<qreal>& scales ) { m_scaleOfViewPool	= scales; }
	void setScaleOfViewChu( const QList<qreal>& scales ) { m_scaleOfViewChu	= scales; }
	
	QSize sizeOfViewStandingKnownHand( int view ) const { return m_sizeOfViewStandingKnownHand.value(view); }
	QSize sizeOfViewStandingUnknownHand( int view ) const { return m_sizeOfViewStandingUnknownHand.value(view); }
	QSize sizeOfViewLyingHand( int view ) const { return m_sizeOfViewLyingHand.value(view); }
	QSize sizeOfViewWall( int view ) const { return m_sizeOfViewWall.value(view); }
	
	int distanceOfViewStandingKnownHand( int view ) const { return m_distanceOfViewStandingKnownHand.value(view,0); }
	int distanceOfViewStandingUnknownHand( int view ) const { return m_distanceOfViewStandingUnknownHand.value(view,0); }
	int distanceOfViewLyingHand( int view ) const { return m_distanceOfViewLyingHand.value(view,0); }
	int distanceOfViewWall( int view ) const { return m_distanceOfViewWall.value(view,0); }
	void setDistanceOfViewStandingKnownHand( const QList<int>& distances ) { m_distanceOfViewStandingKnownHand = distances; }
	void setDistanceOfViewStandingUnknownHand( const QList<int>& distances ) { m_distanceOfViewStandingUnknownHand = distances; }
	void setDistanceOfViewLyingHand( const QList<int>& distances ) { m_distanceOfViewLyingHand = distances; }
	void setDistanceOfViewWall( const QList<int>& distances ) { m_distanceOfViewWall = distances; }
	
	int deltaOfViewPoolNewLine( int view ) const { return m_deltaOfViewPoolNewLine.value(view,0); }
	QPointF deltaOfViewHandGang( int view ) const { return m_deltaOfViewHandGang.value(view,QPointF(0,0)); }
	void setDeltaOfViewPoolNewLine( const QList<int>& deltas ) { m_deltaOfViewPoolNewLine = deltas; }
	void setDeltaOfViewHandGang( const QList<QPointF>& deltas ) { m_deltaOfViewHandGang = deltas; }
	
	bool isLastTraceGang() const { return m_isLastTraceGang; }
	void setLastTraceGang( bool flag ) { m_isLastTraceGang	= flag; }
	quint8 lastChuSeat() const { return m_lastChuSeat; }
	MJCard lastChuCard() const { return m_lastChuCard; }
	void setLastChuSeat( quint8 seat ) { m_lastChuSeat = seat; }
	void setLastChuCard( MJCard card ) { m_lastChuCard = card; }
	
	int bricksRemovedFromHead() const { return m_bricksRemovedFromHead; }
	int bricksRemovedFromTail() const { return m_bricksRemovedFromTail; }
	
	QString cardResPath( quint8 card, Pose pose, quint8 view ) const;
	void removeFromStandingHand( int seat, MJCard card );
	void chipeng( int seat, const MJCards& cards );
	bool gangflower( int seat, MJCard card );
	void chu( int seat, MJCard card );
	void zhua( int seat, MJCard card );
	
	int lengthOfHand( int seat );
	int lengthOfWall( int view );
	
	void validateView( int& view );
	void getWallTailViewIndex( int& view, int& index );
	void setWallHeadTail( int seat, int skips );
	void removeFromWall( bool fromHead );
	void repaintWall( int wallStartSeat, int wallStartSkips );
	void repaintViewWall( int view );
	void repaintViewWall(
				int view, DesktopItems &items, qreal scale, 
				int x, int y, qreal z, 
				int dx, int dy, int dz,
				int eachItemWidth, int distance, Qt::Alignment align );
	void repaintSeatHand( int seat, bool repaintSize, bool repaintPos, bool resort, bool isLastPickup = false );
	void repaintSeatViewHand(
				int seat, int view, 
				bool repaintSize, bool repaintPos, bool isLastPickup,
				int& x, int& y, qreal& z, 
				int dx, int dy, int dz,
				Qt::Alignment align );
	void repaintViewHand(
				int view, DesktopItems &items, 
				bool repaintSize, bool repaintPos,
				Pose pose, qreal scale, 
				int& x, int& y, qreal& z, 
				int dx, int dy, int dz,
				int eachItemWidth, int distance, Qt::Alignment align );
	void repaintSeatPool( int seat, bool repaintSize, bool repaintPos );
	void repaintViewPool( int view, DesktopItems &items, 
				qreal scale, bool repaintSize, bool repaintPos,
				int startx, int starty, qreal z,
				int dx, int dy, int dz,
				int eachItemWidth, int distance, Qt::Alignment align,
				int deltaOfLine, int border );
	void repaintSeatFlower( int seat, bool repaintSize, bool repaintPos );
	void repaintChu( bool show );
	void repaintHu( int seatOfHu, const unsigned char *buf, quint8 len );
	void clearHuText();
	int doMJSelect( const MJCardsGroups& groups );
	QSvgRenderer* cardRenderer( const QString& resPath );
	
	QWidget* chiWidget() const { return m_chi; }
	QWidget* chuWidget() const { return m_chu; }
	QWidget* gangWidget() const { return m_gang; }
	QWidget* huWidget() const { return m_hu; }
	QWidget* pengWidget() const { return m_peng; }
	QWidget* qiWidget() const { return m_qi; }
	QWidget* tingWidget() const { return m_ting; }
	
	virtual quint8 gameTraceIDOfChiPeng() const;
	virtual quint8 gameTraceIDOfChu() const;
	virtual quint8 gameTraceIDOfGangFlower() const;
	virtual quint8 gameTraceIDOfHu() const;
	virtual quint8 gameTraceIDOfTing() const;
	virtual quint8 gameWaitIDOfChiPeng() const;
	virtual quint8 gameWaitIDOfChu() const;
	
	//bool isChuEnabled() const;
	//bool isQiEnabled() const;
	void throwSelectedCard();
	bool try2Chu();
	bool try2Qi();
public slots:
	virtual void handleMousePressed( QGraphicsSceneMouseEvent *mouseEvent );
	virtual void handleMouseReleased( QGraphicsSceneMouseEvent *mouseEvent );
	virtual void handleMouseDoubleClicked( QGraphicsSceneMouseEvent *mouseEvent );
	
	virtual void clickChi();
	virtual void clickChu();
	virtual void clickGang();
	virtual void clickHu();
	virtual void clickPeng();
	virtual void clickQi();
	virtual void clickTing();
private:
   	QHash<QString,QSvgRenderer*>	m_mahjongSvgCache;
	DJGraphicsSvgItem 				*m_chuItem;
	DJGraphicsTextItem				*m_huTitle;
	QList< DJGraphicsTextItem* >	m_huFanNames;
	QList< DJGraphicsTextItem* >	m_huFanValues;
	

	//index is view
	QList<qreal>				m_scaleOfViewStandingKnownHand;
	QList<qreal>				m_scaleOfViewStandingUnknownHand;
	QList<qreal>				m_scaleOfViewLyingHand;
	QList<qreal>				m_scaleOfViewWall;
	QList<qreal>				m_scaleOfViewFlower;
	QList<qreal>				m_scaleOfViewPool;
	QList<qreal>				m_scaleOfViewChu;
	
	// width and height of view 2 4 are exchanged from original pic size
	QList< QSize >				m_sizeOfViewStandingKnownHand;
	QList< QSize >				m_sizeOfViewStandingUnknownHand;
	QList< QSize >				m_sizeOfViewLyingHand;
	QList< QSize >				m_sizeOfViewWall;
	
	QList< int >				m_distanceOfViewStandingKnownHand;
	QList< int >				m_distanceOfViewStandingUnknownHand;
	QList< int >				m_distanceOfViewLyingHand;
	QList< int >				m_distanceOfViewWall;
	
	QList< int >				m_deltaOfViewPoolNewLine;
	QList< QPointF >			m_deltaOfViewHandGang;
	
	QRect						m_wallSpace;
	QRect						m_poolSpace;
	QRect						m_centerSpace;
	int							m_selfEdge;
	
	//view,index
	QPair<int,int>				m_wallHead;
	QPair<int,int>				m_wallTail;
	int							m_bricksRemovedFromHead;
	int							m_bricksRemovedFromTail;
	
	bool						m_isLastTraceGang;
	quint8						m_lastChuSeat;
	MJCard 						m_lastChuCard;
	
	MJCards						m_selfStandingCards;
	MahjongHUTypeWrap			m_selfLyingCards;
	
	quint16						m_supportedHuStyle;
	
	QWidget						*m_actionContainer;
	QToolButton 				*m_chi,*m_peng,*m_chu,*m_qi,*m_gang,*m_hu,*m_ting;
};

#endif
