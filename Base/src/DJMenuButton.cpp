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
#include "DJMenuButton.h"
#include "DJGlobal.h"

DJMenuButton::DJMenuButton(QWidget * parent)
	:QPushButton(parent)
{
	//setAttribute( Qt::WA_Hover );
	m_menu	= 0;
	setFlat( true );
	connect( this, SIGNAL(clicked()), SLOT(displayMenu()) );
}
DJMenuButton::~DJMenuButton()
{
}
void DJMenuButton::setMenu( QMenu* menu )
{
	m_menu	= menu;
	//QPushButton::setMenu(menu);
}
/*
bool DJMenuButton::event ( QEvent * e )
{
	if ( QEvent::HoverEnter == e->type() ) {
		djDebug()<<"QEvent::HoverEnter";
		if ( m_menu ) {//&& !m_menu->isVisible() ) {
    		//QList<QMenu *> menus = parent()->findChildren<QMenu *>();
    		//foreach( QMenu *menu, menus ) {
    			//menu->hide();
    		//}
    		int x	= (width()>>2) * 3;
    		int y	= height()>>1;
    		m_menu->popup( mapToGlobal( QPoint(x,y) ) );
    		m_menu->show();
    	}
		return true;
	}else if ( QEvent::HoverLeave == e->type() ) {
		djDebug()<<"QEvent::HoverLeave";
		if ( m_menu )
			m_menu->hide();
		return true;
	}else
		return QPushButton::event( e );
}
*/
void DJMenuButton::enterEvent( QEvent * event )
{
	djDebug()<<"DJMenuButton::enterEvent";
	if ( isVisible() )
		displayMenu();
}
/*
void DJMenuButton::leaveEvent( QEvent * event )
{
	djDebug()<<"DJMenuButton::leaveEvent";
	//if ( m_menu )
		//m_menu->hide();
}
*/
/*
void DJMenuButton::mouseMoveEvent ( QMouseEvent * event )
{
	djDebug()<<"DJMenuButton::moveEvent";
}
*/
void DJMenuButton::displayMenu()
{
	if ( m_menu && !m_menu->isVisible() ) {
		QList<QMenu *> menus = parent()->findChildren<QMenu *>();
		foreach( QMenu *menu, menus ) {
			menu->hide();
		}
		int x	= (width()>>2) * 3;
		int y	= height()>>1;
		m_menu->popup( mapToGlobal( QPoint(x,y) ) );
		m_menu->show();
	}
}