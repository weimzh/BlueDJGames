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

#ifndef LLKDESKTOPCONTROLLER_H
#define LLKDESKTOPCONTROLLER_H

#include<QtGui>
#include "DJDesktopController.h"
#include "shssprofile.h"

class DJGraphicsLineItem;
class DJGraphicsRectItem;

class LLKDesktopController : public DJDesktopController
{
	Q_OBJECT
public :
	LLKDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent );
	~LLKDesktopController();
	
	virtual void initUnderGameInfo( const QByteArray& buf );
	virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
	virtual void gameTraceModel( const GeneralGameTrace2Head *gameTrace );
	virtual void gameTraceView( const GeneralGameTrace2Head *gameTrace );
	virtual void locateClockPosition( QPoint& point, Qt::Alignment& align );
	virtual bool isCloseable() const;
	virtual void repaintCurrentStatus();
	virtual void locateAllNonMatrimoveItems();
	virtual void repaintAll();
	virtual void handleMatrixChanged();
	
	QPixmap GetCardPixmap(unsigned char chCard);
	QPoint map2VirtualPos( const QPoint& origin, quint8 indexx,quint8 indexy );
	void clearItems( int type );
	void clearMarks();
	void markCard( DJGraphicsPixmapItem *item, bool flag );
	void initCards();
	void repaintCards();
	void deleteCards(const GeneralGameTrace2Head *ptrace);
	void requestAdjust();
	void setWaitingAdjustACL( bool flag ) { m_isWaitingAdjustACL = flag; }
	void repaintLives();
	void repaintScore();
public slots:
	virtual void handleMousePressed( QGraphicsSceneMouseEvent *mouseEvent );
	void handleDeleteTimeout();
	virtual void handleStartClicked();
	void handleResetClicked();
	void handleFrameChanged( int frame );
private:
	ShisenshoCurrent	m_current;
	unsigned char 		m_chMap[SHISENSHO_MAX_MAP+8];
	unsigned char 		m_shSkin;
	QSize				m_cardSize;
	QPoint				m_origin;
	//DJGraphicsPixmapItem	*m_selectedItem;
	int					m_selectedX,m_selectedY;
	ShisenshoDelete		m_del;
	QTimer				*m_deleteTimer;
	bool				m_isWaitingAdjustACL;
	DJGraphicsPixmapItem	*m_cards[SHISENSHO_MAX_MAP+1][SHISENSHO_MAX_MAP+1];
	DJGraphicsLineItem *m_connectLine;
	QPushButton			*m_btnReset;
	
	DJGraphicsRectItem	*m_lifeRectBounder,*m_lifeRect;
	QTimeLine			*m_timeline;
	int					m_score;
	DJGraphicsTextItem	*m_scoreText;
};

#endif
