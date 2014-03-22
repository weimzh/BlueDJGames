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

#ifndef RENJUDESKTOPCONTROLLER_H
#define RENJUDESKTOPCONTROLLER_H

#include<QtGui>
#include "DJDesktopController.h"
#include "renjuprofile.h"

class RenjuDesktopController;

class RenjuChip : public QObject
{
public :
	RenjuChip(RenjuDesktopController *dc,quint8 chOwner,int x,int y,quint16 chStep);
	~RenjuChip();
		
	void show();
	void hide();
	void setNumberBase(quint16 base);
	quint16 number() const { return m_number; }
	quint8	color() const { return m_owner; }
	bool isCurrent() const { return m_isCurrent; }
	
	quint16 	currentNumber() const;
	void setDisplayNumber(bool b);
	void setCurrentChip(bool b);
	bool isMe( QGraphicsItem *item ) const;
	
	void setCurrentImage(quint8 ch);
	
private :
	DJGraphicsPixmapItem	*m_itemChip;
	DJGraphicsTextItem		*m_itemNumber;
	DJGraphicsPixmapItem  	*m_itemCurrent;
	
	bool		m_isCurrent,m_isNumber;
	quint8		m_owner;
	quint16		m_number,m_numberBase;
	RenjuDesktopController *m_dc;
};
class DJGraphicsChessTimerItem;

class RenjuDesktopController : public DJDesktopController
{
	Q_OBJECT
public :
	RenjuDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent );
	~RenjuDesktopController();
	
	virtual void initUnderGameInfo( const QByteArray& buf );
	virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
	virtual void gameTraceModel( const GeneralGameTrace2Head *gameTrace );
	virtual void gameTraceView( const GeneralGameTrace2Head *gameTrace );
	virtual void handleMousePressed( QGraphicsSceneMouseEvent *mouseEvent );
	virtual void repaintCurrentStatus();
	virtual void locateAllNonMatrimoveItems();
	virtual void locateStartButton();
	void SetPlayerTimer(quint8 chOwner,PDJGameChessTimer rtimer);
	void setWaitingMoveACL( bool flag ) { m_isWaitingMoveACL = flag; }
	QPoint boardPos() const { return m_boardPos; }
public slots:
	void clickJinshou();
	void clickJiaohuan();
	void clickQiuhe();
	void clickRenshu();
	void handleCountdown(unsigned char countdown);
private:
	void init();
	void createChip(quint8 chOwner,quint8 x,quint8 y,quint8 chStep);
	void clearChip(quint8 x,quint8 y);
	void signYN();
	void signChips(bool flag);
	void signT0T1();
	void setChipBaseNumber(quint16 base);
	
	RenjuCurrent 				m_current;
	QPoint						m_boardPos;
	DJGraphicsPixmapItem		*m_itemBoard;
	bool						m_isWaitingMoveACL;
	RenjuBoard					m_board;
	RenjuStepDescribe			m_describe;
	
	quint16						m_shSteps;
	quint8						m_lastOwner;
	quint8						m_stepNumbers;
	quint8						m_steps[300][2];
	RenjuChip 					*m_chips[DJGAME_RENJU_WIDTH+1][DJGAME_RENJU_WIDTH+1];
	
	DJGraphicsPixmapItem		*m_itemSignYN;
	DJGraphicsPixmapItem		*m_itemSignT0T1;
	bool 						m_bSignYN;
	quint8						m_chSignT0;
	QPushButton					*m_btnJinshou,*m_btnJiaohuan,*m_btnQiuhe,*m_btnRenshu;
	DJGraphicsChessTimerItem	*m_blackTimer,*m_whiteTimer;
	bool						m_hasRequestDraw;
};

#endif
