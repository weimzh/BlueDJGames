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

#include "DJProgressDialog.h"
#include "DJSocket.h"
#include "DJGlobal.h"

DJProgressDialog::DJProgressDialog( DJSocket *socket, QWidget *parent, Qt::WFlags f )
	:QDialog( parent, f ),m_socket(socket)
{
	djDebug() << "DJProgressDialog constructor" << isModal();
	setMinimumSize( 160, 80 );
	setWindowTitle( tr("please wait") );
	QVBoxLayout *layout	= new QVBoxLayout( this );
	
	m_text	= new QLabel;
	layout->addWidget( m_text );
	
	QPushButton	*button	= new QPushButton( tr("Cancel") );
	layout->addWidget( button );
	connect( button, SIGNAL(clicked()), socket, SLOT(cancel()));
	
	m_timer	= new QTimer( this );
	m_timer->setSingleShot( true );
	connect(m_timer, SIGNAL(timeout()), socket, SLOT(timeout()));
	
	//adjustSize();
}
DJProgressDialog::~DJProgressDialog()
{
	djDebug() << "DJProgressDialog destructor";
}
void DJProgressDialog::moveToCenter()
{
	QDesktopWidget desktop;
	QPoint	center	= desktop.availableGeometry().center();
	
	int	dx	= size().width() >> 1;
	int dy	= size().height() >> 1;
	move( center - QPoint(dx,dy) );
}
void DJProgressDialog::startProgress( int seconds, const QString& text )
{
	if ( -1 != seconds )
		m_timer->start( seconds * 1000 );
	else
		m_timer->stop();
	m_text->setText( text );
	//moveToCenter();
	adjustSize();
	show();
}
void DJProgressDialog::stopProgress()
{
	m_timer->stop();
	hide();
}


