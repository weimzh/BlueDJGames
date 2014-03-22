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

#include "DJGameSetup.h"
#include "Utility.h"

DJGameSetup::DJGameSetup()
{
	djDebug() << "DJGameSetup default constructor";
	m_slowestSpeed	= 0;
	m_refuseSameIp	= false;
	m_breakRate		= 0;
	m_minScore		= 0;
	m_maxScore		= 0;
	m_maxUnwelcome	= 0;
}
DJGameSetup::DJGameSetup( const NormalUserDes *userDsc )
{
	djDebug() << "DJGameSetup:: user constructor";
	GameNormalSetup	*gameNormalSetup	= (GameNormalSetup*)((char*)&userDsc->general + userDsc->chGeneralUserDesLen + userDsc->chGameNormalDataLen);
	
	m_slowestSpeed	= gameNormalSetup->chLowestSpeed;
	m_refuseSameIp	= gameNormalSetup->chSameIP;
	m_breakRate		= gameNormalSetup->chBreakRate;
	m_minScore		= letoh4( gameNormalSetup->iLowestScore );
	m_maxScore		= letoh4( gameNormalSetup->iHighestScore );
	if ( sizeof(GameNormalSetup) == userDsc->chGameNormalSetupLen )
		m_maxUnwelcome	= letoh2( gameNormalSetup->shMaxUnwelcome );
	else
		m_maxUnwelcome	= 0;
}

DJGameSetup::DJGameSetup( const QString& gameName )
{
	djDebug() << "DJGameSetup:: game constructor";
	GetSettingGameSetup( gameName, m_slowestSpeed, m_refuseSameIp, m_breakRate, m_minScore, m_maxScore, m_maxUnwelcome );
}

DJGameSetup::DJGameSetup( const DJGameSetup& other )
{
	djDebug() << "DJGameSetup:: copy constructor";
	init( other );
}
DJGameSetup&	DJGameSetup::operator=( const DJGameSetup& other )
{
	init( other );
	return *this;
}

DJGameSetup::~DJGameSetup()
{
	djDebug() << "DJGameSetup:: destructor";
}
void DJGameSetup::init( const DJGameSetup& other )
{  	
   	m_slowestSpeed	= other.m_slowestSpeed;
	m_refuseSameIp	= other.m_refuseSameIp;
	m_breakRate	= other.m_breakRate;
	m_minScore	= other.m_minScore;
	m_maxScore	= other.m_maxScore;
	m_maxUnwelcome	= other.m_maxUnwelcome;
}
DJGameSetup::operator GameNormalSetup()
{
	GameNormalSetup setup;
	
	setup.chLowestSpeed	= m_slowestSpeed;
	setup.chSameIP		= m_refuseSameIp;
	setup.chBreakRate	= m_breakRate;
	setle4( &setup.iLowestScore, m_minScore );
	setle4( &setup.iHighestScore, m_maxScore );
	setle2( &setup.shMaxUnwelcome, m_maxUnwelcome );
	return setup;
}
