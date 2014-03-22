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

#ifndef TLJDESKTOPCONTROLLER_H
#define TLJDESKTOPCONTROLLER_H

#include<QtGui>
#include "DJDesktopPokerController.h"
#include "ug2profile.h"

class DJPanelController;
class DJGraphicsTextItem;
class DJGraphicsPixmapItem;
class DJPoker;
class DJPokerPattern;

#define POKER_TYPE_LASTROUND_THROWN		0x60
#define POKER_TYPE_SUBTRACT				0x61
#define NUMBER_OF_SHOW_BUTTONS			5

class TLJDesktopController : public DJDesktopPokerController
{
	Q_OBJECT
public :
	enum PokerClass{ Unknown, Diamond,Club,Heart,Spade,Trump };
	TLJDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent );
	~TLJDesktopController();
	
	virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
	virtual void gameTraceModel( const GeneralGameTrace2Head *gameTrace );
	virtual void gameTraceView( const GeneralGameTrace2Head *gameTrace );
	virtual void initUnderGameInfo( const QByteArray& buf );
	virtual QString	userItemNameSuffix( DJGameUser* user ) const;
	virtual QByteArray createThrowTrace( DesktopItems& items );
	virtual bool isChuEnabled() const;
	virtual void locateAll();
		
	void locateSuitPositions( QList<QPoint>& points, QList<Qt::Alignment>& aligns );
	virtual void locateAllNonMatrimoveItems();
	virtual void repaintAll();
	virtual void repaintHost();
	virtual void fillPlayerInfo();
	//virtual void repaintPlayerInfo();
	void repaintShowedSuit();
	void repaintShowedSuitItem( quint8 seat );
	virtual void repaintCurrentStatus();
	virtual int rankOnSuit( const DJPoker& poker ) const;
	virtual int distanceBetweenPokers( const DJPoker& poker1, const DJPoker& poker2 ) const;
	bool isSameClass( const DJPoker& poker1, const DJPoker& poker2 ) const;
	bool isTrump( const DJPoker& poker ) const;
	PokerClass classOfPoker( const DJPoker& poker ) const;
	QList<DJPoker> handPokersOnClass( int pokerClass, QList<DJPoker>& nonSameClassPokers );
	int findMatchingWidth( QList<DJPokerPattern>& patterns, int width );
	QList<DJPokerPattern>::iterator findMatchingPattern( QList<DJPokerPattern>& patterns, int width, int length, int mode );
	virtual DJPoker higherSerialPoker( const DJPoker& poker, bool isStrict ) const;
	virtual DJPoker lowerSerialPoker( const DJPoker& poker, bool isStrict ) const;
private slots:
	void handleFrameChanged( int currentCardFrame );
	void showNT();
	void showSpade();
	void showHeart();
	void showClub();
	void showDiamond();
	
	void clickTip();
	void clickThrow();
	void previousPressed();
	void previousRelesed();
	void subtractReleased();
	void subtractPressed();
private:
	void dealCards( int numberOfShouldHaveDealedCards );
	void repaintZhuang();
	void repaintScore();
	void checkShowStatus();
	quint8 makeShowCard( int numberOfShowPokers, const DJPoker& poker );
	bool isFollowing() const;
	
	UpgradeCurrent 	m_current;
	QTimeLine		*m_timeline;
	DJPoker			m_showedPoker,m_levelPoker;
	int				m_numberOfShowedPokers;
	int				m_seatOfShow;
	int				m_currentScore;
	
	quint8			m_seatOfFirstThrow;
	int				m_numberOfFirstThrownCards;
	quint8 			m_masterSeat;
	
	//index is seat
	QList< QList<unsigned char> > m_undealedCards;
	
	DJGraphicsTextItem		*m_resultText;
	DJGraphicsTextItem 		*m_zhuangText;
	DJGraphicsTextItem 		*m_scoreText;
	
	QWidget			*m_actionContainer,*m_showContainer;
	QToolButton		*m_showButtons[NUMBER_OF_SHOW_BUTTONS];
	quint8			m_checkShowResult[NUMBER_OF_SHOW_BUTTONS];
	bool			m_showIsOver;
	QToolButton		*m_previous,*m_subtract,*m_tip, *m_throw;
	
	QHash<int,int>	m_suit2ShowButton;
	QList<DJPoker::Point>		m_series;
		
	QList<DJGraphicsPixmapItem*>	m_suitItems;
	QList<QPoint>					m_suitPoints;
	QList<Qt::Alignment>			m_suitAligns;
};

#endif
