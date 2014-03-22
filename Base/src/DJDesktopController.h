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

#ifndef DJDESKTOPCONTROLLER_H
#define DJDESKTOPCONTROLLER_H

#include <QtGui>
#include "DJBaseGlobal.h"
#include "protocol.h"

class DJGameController;
class DJPanelController;
class DJDesktop;
class DJShowView;
class DJGameUser;
class DJGraphicsTextItem;
class DJGraphicsTimerItem;
class DJGraphicsPixmapItem;
class DJDesktopItem;
class DJWidget;

typedef QList< DJDesktopItem* >			DesktopItems;
typedef DesktopItems::iterator 			DesktopItemsIterator;

typedef QHash< int, DesktopItems >		DesktopTypeItems;
typedef DesktopTypeItems::iterator		DesktopTypeItemsIterator;

typedef QList< DesktopTypeItems >		DesktopSeatTypeItems;
typedef DesktopSeatTypeItems::iterator	DesktopSeatTypeItemsIterator;
	
class DJ_BASE_EXPORT DJDesktopController : public QObject
{
    Q_OBJECT 
public:
	DJDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent );
    virtual ~DJDesktopController();

	virtual void gameInfo( const QByteArray& buf );
	virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
	virtual void gameTrace( const GeneralGameTrace2Head *gameTraceHead );
	virtual void gameTraceModel( const GeneralGameTrace2Head *gameTraceHead );
	virtual void gameTraceView( const GeneralGameTrace2Head *gameTraceHead );
	virtual void initUnderGameInfo( const QByteArray& buf );
	virtual void repaintCurrentStatus();
    virtual void magicResult( const MagicResult* magicResult );
    virtual void gameOver( const DJGameOver* gameOver );
    virtual QString bonusTarget( const DJGameBonus* bonus );
    virtual void bonusCondition( const DJGameBonus* condition );
	virtual void bonusFulfilled( const DJGameBonusFulfill* fulfill );
	virtual bool isCloseable() const;
	
	QPoint centerPoint() const { return m_centerPoint; }
	void setCenterPoint( const QPoint& point );
	QRect availableSpace() const { return m_availableSpace; }
	void setAvailableSpace( const QRect& availableSpace );
	void setShowPositions( const QList<QPoint>& points, const QList<Qt::Alignment>& aligns );
	void setNamePositions( const QList<QPoint>& points, const QList<Qt::Alignment>& aligns );
	void setClockPosition( const QPoint& point, Qt::Alignment align );
	void setHostPositions( const QList<QPoint>& points, const QList<Qt::Alignment>& aligns );

	DJShowView* playerShowView( quint8 seat ) const;
	DJGraphicsTextItem* playerNameItem( quint8 seat ) const;
	DJGraphicsTextItem* playerInfoItem( quint8 seat ) const;
	QRect rectOfSeatShow( quint8 seat ) const;
	QRect rectOfViewShow( quint8 view ) const;
	QRect rectOfSeatName( quint8 seat ) const;
	QRect rectOfViewName( quint8 view ) const;
	QRect rectOfSeatInfo( quint8 seat ) const;
	QRect rectOfViewInfo( quint8 view ) const;
	
	DJGameController* gameController() const;
	DJPanelController* panelController() const { return m_panelController; }
	DJDesktop* desktop() const { return m_desktop; }
	
	virtual QList<quint8> seatIdsOfPlayingUsers() const;
	
	QList<quint8> mask2Seat(quint16 mask);
	
	virtual void locateAll();
	virtual void locateAllNonMatrimoveItems();
	virtual void locateCenterPoint( QPoint& point );
	virtual void locateShowPositions( QList<QPoint>& points, QList<Qt::Alignment>& aligns );
	virtual void locateNamePositions( QList<QPoint>& points, QList<Qt::Alignment>& aligns );
	virtual void locateInfoPositions( QList<QPoint>& points, QList<Qt::Alignment>& aligns );
	virtual void locateClockPosition( QPoint& point, Qt::Alignment& align );
	virtual void locateHostPositions( QList<QPoint>& points, QList<Qt::Alignment>& aligns );
	
	virtual void locateAvailableSpace( QRect& rect );
	virtual void locateStartButton();
	virtual void locateBonusPosition();
	
	virtual void repaintAll();
	virtual void reloadPlayer( DJGameUser *player );
    void repaintAllPlayers();
    void repaintPlayer( quint8 seat );
    void repaintShowView( quint8 seat );
    void repaintNameItem( quint8 seat );
    void repaintClock();
    void setClockTimeout( quint16 mask, int seconds );
    virtual void repaintHost();
    virtual void fillPlayerInfo();
    virtual void repaintPlayerInfo();
    void repaintPlayerInfoItem( quint8 seat );
    void repaintHostItem( quint8 seat );
    //void setHostEnabled( bool enabled );
	void hideHost();
    virtual quint8 seat2View(quint8 seat) const;
    virtual quint8 view2Seat(quint8 view) const;
    virtual bool isUserPlaying( quint8 seatId ) const;
    virtual QString	userItemNameSuffix( DJGameUser* user ) const;
    
    quint8	gameWaitStatus() const { return m_gameWaitStatus; }
    bool isWaitingForMe() const;
    int gameProcessStatus() const;
    void setSavable( bool flag );
    void playWave( const QString& waveName, const QString& gameName = QString() ) const;
    quint8 prevPlayerSeatId( quint8 seatId, bool isClockwise = false ) const;
    quint8 nextPlayerSeatId( quint8 seatId, bool isClockwise = false ) const;
    void sendGameTrace(quint8 traceID, const QByteArray& data = QByteArray(), AclProc aclProc = 0, const QVariant& parameters = QVariant() );
	
	QPushButton* startButton() const { return m_btnStart; }
	DJGraphicsTimerItem* clockItem() const { return m_clockItem; }
    //DJWidget* bonusWidget() const { return m_bonusWidget; }
	//QLabel* bonusTargetWidget() const { return m_bonusTargetWidget; }
	//QLabel* bonusResultWidget() const { return m_bonusResultWidget; }
	
	void clearAutoDisappearText();
	//const DesktopItems& seatTypeItems( quint8 seat, uint type ) const;
	//DesktopItem* desktopItem( int seat, int type, int index ) const;
	DesktopItems& desktopItems( int seat, int type );
	DesktopItems selectedDesktopItems( int seat, int type );
	virtual DJDesktopItem* createDesktopItem( int seat, int type, int value );
	void appendDesktopItem( int seat, int type, int value );
	void appendDesktopItem( int seat, int type, DJDesktopItem *item );
	void replaceDesktopItem( int seat, int type, int index, int value );
	void replaceDesktopItem( int seat, int type, int index, DJDesktopItem *item );
	void moveDesktopItems( int seat, int type1, int type2 );
	virtual DJDesktopItem* takeDesktopItem( int seat, int type, int value );
	void clearDesktopItems( int seat, int type );
	void hideDesktopItems( int seat, int type );
	void showDesktopItems( int seat, int type );
public slots:
	virtual void handleStartClicked();
	virtual void handleMatrixChanged();
	virtual void handleMousePressed( QGraphicsSceneMouseEvent *mouseEvent );
	virtual void handleMouseReleased( QGraphicsSceneMouseEvent *mouseEvent );
	virtual void handleMouseDoubleClicked( QGraphicsSceneMouseEvent *mouseEvent );
	virtual void handleMouseMoving( QGraphicsSceneMouseEvent *mouseEvent );
private:
	DJPanelController			*m_panelController;
	DJDesktop					*m_desktop;
	
	quint8						m_gameWaitStatus;
	quint16						m_gameWaitMask;
	quint16						m_gameWaitTimeout;
	
	QPoint						m_centerPoint;
	QRect						m_availableSpace;
	
	QList<DJShowView*>			m_showViews;
	QList<QPoint>				m_showPoints;
	QList<Qt::Alignment>		m_showAligns;
	
	QList<DJGraphicsTextItem*>	m_nameItems;
	QList<QPoint>				m_namePoints;
	QList<Qt::Alignment>		m_nameAligns;
	
	QList<DJGraphicsTextItem*>	m_infoItems;
	QList<QPoint>				m_infoPoints;
	QList<Qt::Alignment>		m_infoAligns;
		
	DJGraphicsTimerItem			*m_clockItem;
	QPoint						m_clockPoint;
	Qt::Alignment				m_clockAlign;
	
	DJGraphicsPixmapItem		*m_hostItem;
	QList<QPoint>				m_hostPoints;
	QList<Qt::Alignment>		m_hostAligns;
	//bool						m_hostEnabled;
	
	QPushButton					*m_btnStart;
	
	DJGraphicsTextItem			*m_bonusTargetText,*m_bonusAwardText;
	
	DesktopSeatTypeItems		m_seatTypeItems;	
};

#endif
