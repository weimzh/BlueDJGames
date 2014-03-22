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

#ifndef XQDESKTOPCONTROLLER_H
#define XQDESKTOPCONTROLLER_H

#include<QtGui>
#include "DJDesktopController.h"
#include "xqprofile.h"

class DJPanelController;
class DJGraphicsPixmapItem;
class DJGraphicsChessTimerItem;

class XQDesktopController : public DJDesktopController
{
	Q_OBJECT
public :
	XQDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent );
	~XQDesktopController();
	
	virtual QString	userItemNameSuffix( DJGameUser* user );
	virtual void initUnderGameInfo( const QByteArray& buf );
	virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
	virtual void gameTraceModel( const GeneralGameTrace2Head *gameTrace );
	virtual void gameTraceView( const GeneralGameTrace2Head *gameTrace );
	virtual void repaintCurrentStatus();
	virtual void locateAllNonMatrimoveItems();
	virtual void locateStartButton();
	
	void GetNodeXY(unsigned char x,unsigned char y,int *px,int *py);
	void DrawChip(unsigned char x,unsigned char y);
	void ClearChip(unsigned char x,unsigned char y);
	bool IsJiangJun(unsigned char chEnd);
	void StaticGameTrace(GeneralGameTrace2Head *gameTrace);
	void SetPlayerTimer(quint8 chOwner,PDJGameChessTimer rtimer);
	void setWaitingMoveACL( bool flag ) { m_isWaitingMoveACL = flag; }
	void rule2ChessTimer( const XiangqiRule& rule, DJGameChessTimer& ctimer );
public slots:
	virtual void handleMatrixChanged();
	virtual void handleMousePressed( QGraphicsSceneMouseEvent *mouseEvent );
	void clickQiuHe();
	void clickRenShu();
private slots:
	void handleCountdown(unsigned char countdown);
private:
	void init();
	
	unsigned char 					m_shSkin;
	XiangqiCurrent 					m_current;
	XiangQiBoard					m_board;
	DJGraphicsPixmapItem			*m_boardItem;
	DJGraphicsPixmapItem			*m_fromPosItem,*m_toPosItem;
	DJGraphicsChessTimerItem		*m_blackTimer,*m_redTimer;
	QPushButton						*m_qiuhe,*m_renshu;
	QHash<quint16,DJGraphicsPixmapItem*>	m_chipItems;
	int								m_x,m_y;
	unsigned char 					m_chStartPoint;
	bool 							m_isWaitingMoveACL;
	bool							m_hasRequestDraw;
};

#endif
