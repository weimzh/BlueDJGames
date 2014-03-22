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

#ifndef DJTOOLBUTTON_H
#define DJTOOLBUTTON_H

#include <QtGui>
#include "DJBaseGlobal.h"

class DJ_BASE_EXPORT DJToolButton : public QToolButton
{
	Q_OBJECT
public :
	DJToolButton( QWidget * parent = 0 );
	virtual ~DJToolButton();
	
	QIcon backgroundIcon() const { return m_icon; }
	void setBackgroundIcon( const QIcon& icon );
//protected:
	//virtual void paintEvent( QPaintEvent *event );
private:
	QIcon		m_icon;
};
#endif
