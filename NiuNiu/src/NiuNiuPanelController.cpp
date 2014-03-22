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

#include "NiuNiuPanelController.h"
#include "NiuNiuDesktopController.h"
#include "DJGameController.h"
#include "Utility.h"

NiuNiuPanelController::NiuNiuPanelController( DJTableController* tableController, quint8 seatId )
			: DJPanelController( tableController, seatId )
{	
	djDebug() << "RenjuPanelContrller constructor";
}
NiuNiuPanelController::~NiuNiuPanelController()
{
	djDebug() << "NiuNiuPanelController destructor";
}
DJDesktopController* NiuNiuPanelController::createDesktopController( const QSize& desktopSize, QWidget *parent )
{
	return new NiuNiuDesktopController( this, desktopSize, parent );
}
