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

#ifndef SUOHADESKTOPCONTROLLER_H
#define SUOHADESKTOPCONTROLLER_H

#include<QtGui>
#include "DJDesktopPokerController.h"
#include "shprofile.h"

class SuohaDesktopController : public DJDesktopPokerController
{
	Q_OBJECT
public :
	SuohaDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent );
	~SuohaDesktopController();
	
	virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
	virtual void gameTraceModel( const GeneralGameTrace2Head *gameTrace );
	virtual void gameTraceView( const GeneralGameTrace2Head *gameTrace );
	virtual void initUnderGameInfo( const QByteArray& buf );
	
	virtual void handleMousePressed( QGraphicsSceneMouseEvent *mouseEvent );
	virtual void handleMouseReleased( QGraphicsSceneMouseEvent *mouseEvent );
	virtual void locatePoolSpace( QRect& rect );
	virtual void locateCenterSpace( QRect& rect );
	virtual void repaintCurrentStatus();
	virtual void repaintAll();
	virtual bool isUserPlaying( quint8 seatId ) const;
	virtual bool isCloseable() const;
	virtual QString userItemNameSuffix( DJGameUser* user ) const;
	virtual void repaintHost();
	virtual void locateAllNonMatrimoveItems();
	bool isLeader( int seat );
	void playerBet( quint8 chips );
	void locateButtons();
	void repaintHandChips();
public slots:
	void clickQi();
	void clickSuo();
	void clickJia();
	void clickGen();
private:
	SuohaCurrent			m_current;
	QList<DJGraphicsTextItem*>	m_wagerTextItems;
	//DJGraphicsTextItem		*m_minItem,*m_maxItem;
	QPushButton 			*m_btnQi,*m_btnSuo,*m_btnJia,*m_btnGen;
	int						m_steps;
	qint16			c_PlayerCounters[DJGAME_SUOHA_MAX_PLAYERS+1];
	qint16			c_PlayerPreCounters[DJGAME_SUOHA_MAX_PLAYERS+1];
	quint8			c_HandCounters;
	quint16			c_CurrentCounters;
};

#endif
