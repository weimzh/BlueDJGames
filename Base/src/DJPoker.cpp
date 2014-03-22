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
#include "DJPoker.h"

static QList<int> bridgeSuits	= QList<int>()
									<< 	DJPoker::Club
									<< 	DJPoker::Diamond
									<< 	DJPoker::Heart
									<<	DJPoker::Spade;
static QList<int> normalSuits	= QList<int>()
									<< 	DJPoker::Diamond
									<< 	DJPoker::Club
									<< 	DJPoker::Heart
									<<	DJPoker::Spade;


DJPoker::DJPoker( quint8 card, bool isBridgeCard )
	:m_id(card)
{	
	//djDebug()<<"DJPoker constructor"<<hex<<card<<isBridgeCard;
	
	m_suit	= NoSuit;
	m_point	= NoPoint;
	
	if ( 0x3E == card ) {
		m_suit	= Joker;
		m_point	= SmallJoker;
	}else if  ( 0x3F == card ) {
		m_suit	= Joker;
		m_point	= BigJoker;
	}else {
		quint8 value	= ( card & 0x0F );
		if ( value >= 1 && value <= 13 ) {
			m_point	= static_cast<Point>( value );
			quint8 suitId	= ((card & 0x30) >> 4);	// suitid is 0~3
    		if ( isBridgeCard ) {
    			m_suit	= bridgeSuits.at(suitId);
    		}else {
    			m_suit	= normalSuits.at(suitId);
    		}
    	}
	}
}
DJPoker::DJPoker( int suit, Point point )
	: m_suit(suit), m_point(point)
{
	//djDebug()<<"DJPoker constructor"<<hex<<"suit"<<suit<<"point"<<point;
	m_id	= 0;
}
DJPoker::~DJPoker()
{
}
quint8 DJPoker::toByte( bool isBridgeCard ) const
{
	if ( 0 != m_id )
		return m_id;
	else {
		if ( NoSuit == m_suit || NoPoint == m_point ) {
			return 0x0F;
		}else if ( Joker == m_suit && SmallJoker == m_point ) {
			return 0x3E;
		}else if  ( Joker == m_suit && BigJoker == m_point ) {
			return 0x3F;
		}else {
    		quint8 suit;
    		if ( isBridgeCard ) {
    			suit	= bridgeSuits.indexOf( m_suit );
    		}else {
    			suit	= normalSuits.indexOf( m_suit );
    		}
    		quint8 value	= (quint8)m_point;
    		return suit << 4 | value;
    	}
	}
}
QString DJPoker::description() const
{
	QChar suits[5] = {'-','-','-','-','-'};
	if ( m_suit & Diamond )
		suits[4]	= 'D';
	if ( m_suit & Club )
		suits[3]	= 'C';
	if ( m_suit & Heart )
		suits[2]	= 'H';
	if ( m_suit & Spade )
		suits[1]	= 'S';
	if ( m_suit & Joker )
		suits[0]	= 'J';
	QString text( suits, 5 );
	return QString("DJPOKER: suit:%1 point:%2").arg(text).arg(m_point);
}


