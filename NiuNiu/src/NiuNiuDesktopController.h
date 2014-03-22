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

#ifndef NIUNIUDESKTOPCONTROLLER_H
#define NIUNIUDESKTOPCONTROLLER_H

#include<QtGui>
#include "DJDesktopPokerController.h"
#include "nnprofile.h"

class NiuNiuDesktopController : public DJDesktopPokerController
{
	Q_OBJECT
public :
	NiuNiuDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent );
	~NiuNiuDesktopController();
	
	virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
	virtual void gameTraceModel( const GeneralGameTrace2Head *gameTrace );
	virtual void gameTraceView( const GeneralGameTrace2Head *gameTrace );
	virtual void initUnderGameInfo( const QByteArray& buf );
	virtual void locatePoolSpace( QRect& rect );
	virtual void locateCenterSpace( QRect& rect );
	virtual bool isUserPlaying( quint8 seatId ) const;
	virtual void repaintHost();
	virtual void repaintCurrentStatus();
	virtual void repaintAll();
	virtual void locateAllNonMatrimoveItems();
	virtual bool isCloseable() const;
	virtual QString userItemNameSuffix( DJGameUser* user ) const;
	void repaintHandChips();
	void locateButtons();
public slots:
	void clickGen();
	void clickYaZhu();
	void clickJiaBei();
	void clickQi();
private:
	NiuniuCurrent			m_current;
	QList<DJGraphicsTextItem*>	m_wagerTextItems;
	qint16			c_PlayerCounters[DJGAME_NIUNIU_MAX_PLAYERS+1];
	qint16			c_PlayerSelfCounters[DJGAME_NIUNIU_MAX_PLAYERS+1];
	quint8			c_HandCounters;
	QPushButton 	*m_btnGen,*m_btnQi,*m_btnYaZhu,*m_btnJiaBei;
};

#endif
