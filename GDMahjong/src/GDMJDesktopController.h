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

#ifndef GDMJDESKTOPCONTROLLER_H
#define GDMJDESKTOPCONTROLLER_H

#include<QtGui>
#include "DJDesktopMahjongController.h"
#include "mj2profile.h"

class GDMJDesktopController : public DJDesktopMahjongController
{
	Q_OBJECT
public :
	GDMJDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent );
	~GDMJDesktopController();
	
	virtual int bricksPerWall() const;
	virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
	virtual void gameTraceModel( const GeneralGameTrace2Head *gameTrace );
	virtual void gameTraceView( const GeneralGameTrace2Head *gameTrace );
	virtual QString bonusTarget( const DJGameBonus* bonus );
	virtual void initUnderGameInfo( const QByteArray& buf );
	virtual void locateAllNonMatrimoveItems();
	virtual void repaintHost();
	virtual QString	userItemNameSuffix( DJGameUser* user ) const;
	virtual void repaintCurrentStatus();
	virtual void repaintAll();
	void repaintZhuang();
private:
	MahjongCurrent 			m_current;
	MJCard					m_qiangGangCard;
	DJGraphicsTextItem 		*m_zhuangText;
};

#endif
