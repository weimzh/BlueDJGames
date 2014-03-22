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
#include "DJToolButton.h"

//static	QSize	ICON_SIZE	= QSize(32,32);

DJToolButton::DJToolButton( QWidget * parent )
	:QToolButton(parent)
{
	djDebug()<<"DJToolButton constructor";
}
DJToolButton::~DJToolButton()
{
	djDebug()<<"DJToolButton destructor";
}
void DJToolButton::setBackgroundIcon( const QIcon& icon )
{
	m_icon	= icon;
}
#if 0
void DJToolButton::paintEvent( QPaintEvent *event )
{	

	QWidget *parentWidget = qobject_cast<QWidget*>(parent());
	if ( parentWidget ) {
		QPalette::ColorRole parentRole	= parentWidget->backgroundRole();
			
		QPalette parentPalette = parentWidget->palette();
		//QRect rect = parentWidget->rect();
		QBrush parentBrush = parentPalette.brush( parentRole );
		//QPixmap parentPix(prect.width(),prect.height());
		//QPainter p(&pix);
		//p.fillRect(0,0,pix.width(),pix.height(),parentbrush);
		
		QPalette pal = palette();
		pal.setBrush(QPalette::Button,parentBrush);
		setPalette(pal);
	}
	
	//QRect qrect = tbw->geometry();
		//		QRect prect = parent->rect();
	
				
	/*
	QPalette pal = palette();
	//pal.setBrush(QPalette::Background,Qt::red);
	pal.setBrush(QPalette::Button,Qt::green);
	//pal.setBrush(QPalette::Base,Qt::blue);
	setPalette(pal);
	*/
	QToolButton::paintEvent( event );
}
#endif
