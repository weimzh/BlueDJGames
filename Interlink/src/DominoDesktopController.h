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

#ifndef DOMINODESKTOPCONTROLLER_H
#define DOMINODESKTOPCONTROLLER_H

#include<QtGui>
#include "DJDesktopController.h"
#include "dmnprofile.h"

#define DOMINO_TYPE_DESKCARD				0x60
#define DOMINO_TYPE_HANDCARD				0x61

class DominoDesktopController : public DJDesktopController
{
	Q_OBJECT
public :
	DominoDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent );
	~DominoDesktopController();
	virtual void initUnderGameInfo( const QByteArray& buf );
	virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
	virtual void gameTraceModel( const GeneralGameTrace2Head *gameTrace );
	virtual void gameTraceView( const GeneralGameTrace2Head *gameTrace );
	virtual void repaintCurrentStatus();
	void RepaintLink();
	bool RepaintSingleChain(unsigned char chChainID,int x,int y,unsigned char chOri,QRect & rect);
	DJGraphicsPixmapItem* DrawBone(int x,int y,unsigned char chCard,unsigned char chChainID,unsigned char chOri);
	void clearDeskCards();
	void clearHandCardsMark();
	void repaintSum(int sum);
	void repaintRemains(int remains );
	QPixmap cardPixmap( quint8 card, quint8 direction );
	void repaintHandCards( int seat, int type, bool repaintSize, bool repaintPos, qreal scale );
	void repaintCardItems(
		DesktopItems &items, qreal scale, Qt::Alignment align,
		bool repaintSize, bool repaintPos, quint8 view,
		int startx, int starty, qreal z,
		const QPoint& directionOfNextItem, int deltaOfLine, int border );
	virtual DJDesktopItem* takeDesktopItem( int seat, int type, int value );
	void CheckPosition(unsigned char chCard,unsigned char *pchCards,unsigned char chPages,bool bSend);
	virtual void handleMousePressed( QGraphicsSceneMouseEvent *mouseEvent );
	virtual void handleMouseDoubleClicked( QGraphicsSceneMouseEvent *mouseEvent );
	QByteArray handCardsFromDesktopItems();
	void setWaitingPlaceACL( bool flag ) { m_isWaitingPlaceACL = flag; }
	quint8 selectedHandCard();
	void SendPlace(unsigned char chCard,unsigned char chChainID,unsigned char *pchCards,unsigned char chPages);
	virtual void locateAvailableSpace( QRect& rect );
	virtual void locateClockPosition( QPoint& point, Qt::Alignment& align );
	virtual void fillPlayerInfo();
	virtual void locateAllNonMatrimoveItems();
	void init();
public slots :
	void clickZhua();
private:
	DominoCurrent		m_current;
	DominoChains		m_chains;
	bool				m_isWaitingPlaceACL;
	
	int					m_cardDistance;
	QSize				m_cardSize;
	QList<int>			m_score;
	QPushButton			*m_btnZhua;
	DJGraphicsTextItem	*m_sumTextItem,*m_remainTextItem;
};

#endif
