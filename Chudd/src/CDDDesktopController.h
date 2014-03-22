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

#ifndef CDDDESKTOPCONTROLLER_H
#define CDDDESKTOPCONTROLLER_H

#include<QtGui>
#include "DJDesktopPokerController.h"
#include "chuddprof.h"

class CDDDesktopController : public DJDesktopPokerController
{
	Q_OBJECT
public :
	CDDDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent );
	~CDDDesktopController();
	
	virtual void gameWait( quint16 mask, quint8 status ,quint16 timeout);
	virtual void gameTraceModel( const GeneralGameTrace2Head *gameTrace );
	virtual void gameTraceView( const GeneralGameTrace2Head *gameTrace );
	virtual void initUnderGameInfo( const QByteArray& buf );
	virtual void locateAllNonMatrimoveItems();
	virtual bool isChuEnabled() const;
	bool isQiEnabled() const;
	virtual QString bonusTarget( const DJGameBonus* bonus );
	virtual void repaintCurrentStatus();
	virtual QByteArray createThrowTrace( DesktopItems& items );
	virtual void fillPlayerInfo();
private slots:
	void clickTip();
	void clickThrow();
	void clickArrange();
	void clickPass();
private:
	ChuddCurrent		m_current;
	quint8				m_seatOfLastThrow;
	QWidget				*m_actionContainer;
	QToolButton			*m_throw,*m_arrange,*m_pass,*m_tip;
	int					m_styleOfArrange;
	ChuddRuleCardType	m_tipOtherType,m_tipSelfType;
	QByteArray			m_tipHandCards;
	QList<bool>			m_tipHandIndexes;
	unsigned char		m_tipSelectedCards[15];
};

#endif
