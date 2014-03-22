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

#include "LLKPanelController.h"
#include "LLKDesktopController.h"
#include "DJGameController.h"
#include "DJGameRank.h"
#include "Utility.h"

LLKPanelController::LLKPanelController( DJTableController* tableController, quint8 seatId )
			: DJPanelController( tableController, seatId )
{	
	djDebug() << "RenjuPanelContrller constructor";
	m_rankWidget	= 0;
}
LLKPanelController::~LLKPanelController()
{
	djDebug() << "LLKPanelController destructor";
}
void LLKPanelController::createAccessoryWidgets( QWidget *parent )
{
	createUsersWidget( parent );
	createShowWidget( parent );
	m_rankWidget	= new DJGameRankTabWidget( gameController(), parent,20);
	m_rankWidget->setMinimumHeight(200);
	createChatWidget( parent );
}
DJDesktopController* LLKPanelController::createDesktopController( const QSize& desktopSize, QWidget *parent )
{
	return new LLKDesktopController( this, desktopSize, parent );
}
void LLKPanelController::gameTrace( const GeneralGameTrace2Head *gameTrace)
{
	switch( gameTrace->chType ) 
	{
	case SHISENSHO_GAMETRACE_RANK :
	{
		PShisenshoRank prank =(PShisenshoRank)gameTrace->chBuf;
		m_rankWidget->InsertIntoList(prank->chType,prank->chIndex,QString::fromUtf8(prank->szUserName),letoh4(&prank->userid),letoh4(prank->uiScore),letoh4(prank->timestamp));
		break;
	}
	default:
		break;
	}
	DJPanelController::gameTrace(gameTrace);
}