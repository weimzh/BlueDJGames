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

#ifndef DJPOKER_H
#define DJPOKER_H

#include <QtCore>
#include "DJBaseGlobal.h"

class DJ_BASE_EXPORT DJPoker
{
public:
	enum Suit{
		NoSuit		= 0,
		Diamond		= 0x01,
		Club		= 0x02,
		Heart		= 0x04,
		Spade		= 0x08,
		Joker		= 0x10,
		//Exclusive	= 0x20,//used by key poker(may be combo suits), 
		//exclusive means need only one of suits, otherwise means all suits
		SuitTrump	= 0x20,
		ConstTrump	= 0x40,
		//SHCD		= Diamond | Club | Heart | Spade,
		AnySuit		= Diamond | Club | Heart | Spade | Joker
	};
	enum Point{
		NoPoint	= 0,
		Ace,
		Num2,
		Num3,
		Num4,
		Num5,
		Num6,
		Num7,
		Num8,
		Num9,
		Num10,
		Jack,
		Queen,
		King,
		Shift	= 0x10,
		SmallJoker = 0x21,
		BigJoker
	};
    DJPoker( quint8 card, bool isBridgeCard );
    DJPoker( int suit = NoSuit, Point point = NoPoint );
    virtual ~DJPoker();
    
    QString description() const;
    bool isValid() const { return NoSuit != m_suit && NoPoint != m_point; }
    bool isBigJoker() const { return Joker == m_suit && BigJoker == m_point; }
    bool isSmallJoker() const { return Joker == m_suit && SmallJoker == m_point; }
    int suit() const { return m_suit; }
    Point point() const { return m_point; }
    quint8 toByte( bool isBridgeCard ) const;
    //bool isEqualPoint( const DJPoker & other ) const { return m_point == other.m_point; }
    bool operator== ( const DJPoker & other ) const { return (m_suit == other.m_suit) && (m_point == other.m_point); }
private:	
   	int 		m_suit;
   	Point		m_point;
   	quint8		m_id;
};

#endif
