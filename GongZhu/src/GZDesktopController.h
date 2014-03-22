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

#ifndef GZDESKTOPCONTROLLER_H
#define GZDESKTOPCONTROLLER_H

#include<QtGui>
#include "DJDesktopPokerController.h"
#include "gozhprof.h"

#define POKER_TYPE_LASTROUND_THROWN		0x60

class GZDesktopController : public DJDesktopPokerController
{
	Q_OBJECT
public :
	GZDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent );
	~GZDesktopController();
	
	virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
	virtual void gameTraceModel( const GeneralGameTrace2Head *gameTrace );
	virtual void gameTraceView( const GeneralGameTrace2Head *gameTrace );
	virtual void initUnderGameInfo( const QByteArray& buf );
	virtual void locateAllNonMatrimoveItems();
	virtual void repaintCurrentStatus();
	virtual bool isChuEnabled() const;
	bool isLiangEnabled() const;
	virtual QByteArray createThrowTrace( DesktopItems& items );
private slots:
	void clickThrow();
	void clickLiang();
	void previousPressed();
	void previousRelesed();
private:
	GongzhuCurrent 		m_current;
	QWidget				*m_actionContainer,*m_showContainer;
	QToolButton			*m_showButton;
	QToolButton			*m_previous,*m_throw;
};

#endif
