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

#ifndef DJPANELUSERITEM_H
#define DJPANELUSERITEM_H

#include <QtGui>
#include "DJBaseGlobal.h"

class DJGameUser;
class DJPanelController;

class DJ_BASE_EXPORT DJPanelUserItem : public QObject, public QTreeWidgetItem
{
	Q_OBJECT
public:
    DJPanelUserItem( DJGameUser* user, DJPanelController* panelController, QTreeWidget* parent, int type = Type );
    virtual ~DJPanelUserItem();
    
    virtual bool operator< ( const QTreeWidgetItem & other ) const;
    
    DJGameUser*	user() const { return m_user; }
    
    void setName( const QString& name );
    void setNickName( const QString& nickname );
    void setSeat( quint8 seatId );
	void setPanelScore( int score );
	void deltaPanelScore( int delta );
	void setPanelChips( int chips );
	void deltaPanelChips( int delta );
	void setDoorWind( quint8 wind );
	void setRingWind( quint8 wind );
public slots:
	void setScore();
	void setChips();
private:
	DJGameUser*			m_user;
	DJPanelController*	m_panelController;
};

#endif
