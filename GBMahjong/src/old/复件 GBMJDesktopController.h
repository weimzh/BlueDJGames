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

#ifndef GBMJDESKTOPCONTROLLER_H
#define GBMJDESKTOPCONTROLLER_H

#include<QtGui>
#include "DJDesktopMJController.h"
#include "mj2profile.h"

class DJPanelController;
class DJGraphicsTextItem;
class DJGraphicsPixmapItem;

class GBMJDesktopController : public DJDesktopMJController
{
	Q_OBJECT
public :
	GBMJDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent );
	~GBMJDesktopController();
	
	virtual void gameInfo( const QByteArray& buf );
	virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
	virtual void gameTraceModel( const GeneralGameTrace2Head *gameTrace );
	virtual void gameTraceView( const GeneralGameTrace2Head *gameTrace );
	virtual QString	userItemNameSuffix( DJGameUser* user );
	virtual void StaticInitDesktop();
	virtual quint8 GetMahjongThrowTraceID(quint8 card);
	
	void DisplayHu(unsigned char chSite,const unsigned char *data,unsigned char chLen);
public slots:
	void ClickStyle();
	void ClickQi();
	void ClickChu();
	void ClickChi();
	void ClickPeng();
	void ClickGang();
	void ClickTing();
	void ClickHu();
private:
	MahjongCurrent 	c_current;
	quint8 			m_lastMGangCard;
	QWidget			*m_toolbar;
	quint8			m_lastThrowSeat;
	QToolButton 	*m_chi,*m_peng,*m_chu,*m_qi,*m_gang,*m_hu,*m_style,*m_ting;
	bool 			c_bReqTing,m_bTingAutoGang,m_bTingZimo;
};

#endif
