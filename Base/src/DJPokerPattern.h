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

#ifndef DJPOKERPATTERN_H
#define DJPOKERPATTERN_H

#include <QtGui>
#include "DJBaseGlobal.h"
#include "DJPoker.h"

class DJDesktopPokerController;

class DJ_BASE_EXPORT DJPokerPattern
{
public:
	DJPokerPattern( int width, int length, const DJPoker& keyPoker = DJPoker(), const QList<DJPoker>& pokers = QList<DJPoker>() );
	static QList<DJPokerPattern> fromPokers( DJDesktopPokerController* desktop, const QList<DJPoker>& pokers, bool isStrict );
	
	bool isValid() const { return 0 != m_width && 0 != m_length; }
	int width() const { return m_width; }
	int length() const { return m_length; }
	DJPoker keyPoker() const { return m_keyPoker; }
	QList<DJPoker> pokers() const { return m_pokers; }
	bool operator< ( const DJPokerPattern & other ) const;
	QList<DJPoker> takeLength( int length );
	QString description() const;
private:
	int				m_width;
	int				m_length;
	DJPoker			m_keyPoker;
	QList<DJPoker>	m_pokers;
};

#endif
