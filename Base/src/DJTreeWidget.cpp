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

#include <QtGui>
#include "DJTreeWidget.h"

DJTreeWidget::DJTreeWidget(QWidget * parent)
	:QTreeWidget(parent)
{
}

DJTreeWidget::~DJTreeWidget()
{
}
/*
void DJTreeWidget::paintEvent ( QPaintEvent * event )
{
	QTreeWidgetItem * item = itemAt(1,1);
	if(item != NULL)
	{
		QRect rect = visualItemRect(item);
		iItemHigh = rect.height();
	}
	QPalette palette	= this->palette();
	QLinearGradient linearGradient( QPoint(0,0), QPoint(0,iItemHigh) );
	linearGradient.setSpread( QGradient::RepeatSpread );
	linearGradient.setColorAt(0, Qt::white );
	linearGradient.setColorAt(1, QColor(225,225,225) );

	palette.setBrush(QPalette::Base, linearGradient );
	setPalette( palette );
	QTreeWidget::paintEvent(event);
}
*/
void DJTreeWidget::resizeEvent ( QResizeEvent * event )
{
	QTreeWidget::resizeEvent( event );
	
	QTreeWidgetItem * item = itemAt(1,1);
	if(item != NULL)
	{
		QRect rect = visualItemRect(item);
		iItemHigh = rect.height();
	}else
		iItemHigh = 20;
		
	QLinearGradient linearGradient( QPoint(0,0), QPoint(0,iItemHigh) );
	linearGradient.setSpread( QGradient::RepeatSpread );
	linearGradient.setColorAt(0, Qt::white );
	linearGradient.setColorAt(1, QColor(225,225,225) );
		
    QPalette pal = palette();
	pal.setBrush( QPalette::Base, linearGradient );
	setPalette(pal);
}
