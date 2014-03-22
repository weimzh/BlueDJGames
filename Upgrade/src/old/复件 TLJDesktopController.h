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

#define DJGAME_POKERDESKTOP_TYPE_HANDCARD			0x50
#define DJGAME_POKERDESKTOP_TYPE_THROWCARD			0x51
#define DJGAME_POKERDESKTOP_TYPE_ATTACHCARD			0x52

#define UPGRADE_MAX_SAVE_ROUND				2
#define DJGAME_UPGRADE_TYPE_RESULT			0x101
#define DJGAME_UPGRADE_TYPE_DESCRIBE		0x102

class TLJDesktopController : public DJDesktopPokerController
{
	Q_OBJECT
public :
	enum PokerClass{ Unknown, Diamond,Club,Heart,Spade,Trump };
	TLJDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent );
	~TLJDesktopController();
	
	virtual void gameInfo( const QByteArray& buf );
	virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
	virtual void gameTraceModel( const GeneralGameTrace2Head *gameTrace );
	virtual void gameTraceView( const GeneralGameTrace2Head *gameTrace );
	virtual void StaticInitDesktop();
	virtual void repaintCurrentStatus();
	
	//virtual void locateCenterPoint( QPoint& point );
	virtual int rankOnSuit( const DJPoker& poker ) const;
	virtual int distanceBetweenPokers( const DJPoker& poker1, const DJPoker& poker2 ) const;
	bool isSameClass( const DJPoker& poker1, const DJPoker& poker2 ) const;
	bool isTrump( const DJPoker& poker ) const;
	PokerClass classOfPoker( const DJPoker& poker ) const;
//public slots:
	//virtual void handleMatrixChanged();
private slots:
	void handleFrameChanged( int currentCardFrame );
	void handleStateChanged( QTimeLine::State newState );
private:
	UpgradeCurrent 	c_current;
	QTimeLine		*m_timeline;
	DJPoker			m_showedPoker,m_levelPoker;
	//index is seat
	QList< QList<unsigned char> > m_dealingCards;	
};

#endif
