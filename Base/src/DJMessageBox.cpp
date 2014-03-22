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

#include "DJMessageBox.h"

DJMessageBox::DJMessageBox( int second, const QString & caption, const QString & text, Icon icon, int button0, int button1, int button2, QWidget * parent )
	: QMessageBox( caption, text, icon, button0, button1, button2, parent )
{
	djDebug() << "DJMessageBox constructor";
	QTimer::singleShot( second * 1000, this, SLOT(accept()));
}
int DJMessageBox::information( int second, QWidget * parent, const QString & caption, const QString & text, int button0, int button1, int button2 )
{
	DJMessageBox mb( second, caption, text, Information, button0, button1, button2, parent);
#ifdef Q_WS_MAC
    mb.setFixedSize(mb.sizeHint());
#endif	
	return mb.exec();
}