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

#ifndef DJGAMESETUP_H
#define DJGAMESETUP_H

#include <QtGui>
#include "DJBaseGlobal.h"
#include "protocol.h"

class DJ_BASE_EXPORT DJGameSetup
{
public:
	DJGameSetup();
	DJGameSetup( const NormalUserDes *userDsc );
	DJGameSetup( const QString& gameName );
    DJGameSetup( const DJGameSetup& other );
    virtual ~DJGameSetup();
    DJGameSetup&	operator=( const DJGameSetup& other );
    operator GameNormalSetup();
    
	quint8	slowestSpeed() const { return m_slowestSpeed; }
	bool	refuseSameIp() const { return m_refuseSameIp; } 
	quint8 	breakRate() const { return m_breakRate; }
	qint32	minScore() const { return m_minScore; }
	qint32	maxScore() const { return m_maxScore; }
	quint16 maxUnwelcome() const { return m_maxUnwelcome; }
	
	void	setSlowestSpeed( quint8 speed ) { m_slowestSpeed	= speed; }
	void 	setRefuseSameIp( bool sameip ) { m_refuseSameIp	= sameip; }
	void 	setBreakRate( quint8 breakRate ) { m_breakRate	= breakRate; }
	void 	setMinScore( qint32 minScore ) { m_minScore	= minScore; }
	void 	setMaxScore( qint32 maxScore ) { m_maxScore	= maxScore; }
	void 	setMaxUnwelcome( quint16 maxUnwelcome ) { m_maxUnwelcome	= maxUnwelcome; }
private:
	void init( const DJGameSetup& other );

	quint8	m_slowestSpeed;
	bool	m_refuseSameIp;
	quint8	m_breakRate;
	qint32	m_minScore;
	qint32	m_maxScore;
	quint16 m_maxUnwelcome;
};

#endif
