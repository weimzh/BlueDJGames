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

#include "DJPanel.h"
#include "DJPanelController.h"
#include "DJDesktopController.h"
#include "DJClientRequest.h"
#include "protocol.h"

DJPanel::DJPanel( DJPanelController* panelController )
	:QMainWindow(0,Qt::Window),m_panelController(panelController)
{	
	djDebug() << "DJPanel constructor";
}

DJPanel::~DJPanel()
{
	djDebug() << "DJPanel destructor";
}
void DJPanel::closeEvent( QCloseEvent *e )
{
	djDebug()<<"DJPanel::closeEvent";
	if ( m_panelController->isLookingOn() ) {
		//look on
		QByteArray data;
		data.append( m_panelController->tableId() );
		QByteArray buf	= m_panelController->makeGameData( DJGAME_PROTOCOL_SUBTYPE_QUITVIEW, data );
		DJClientRequest request( buf );
		m_panelController->sendRequest( request );
		e->accept();
		return;
	}
	DJDesktopController *dc	= m_panelController->desktopController();
    if( dc && dc->isCloseable() )
    {
    	QByteArray buf	= m_panelController->makeGameData( DJGAME_PROTOCOL_SUBTYPE_QUIT, QByteArray() );
		DJClientRequest request( buf );
		m_panelController->sendRequest( request );
		e->accept();
    }else {
    	e->ignore();
    }
}
