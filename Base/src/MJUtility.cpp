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

#include "MJUtility.h"
#include "mjrule.h"

bool MJShunCardLess( MJCard card1, MJCard card2 )
{ 
	return GET_MAHJONG_CARDVALUE(card1) < GET_MAHJONG_CARDVALUE(card2);
}

DJ_BASE_EXPORT MJCardsMultiSet MakeMultiSet( const MJCards& cards )
{		
	MJCardsMultiSet  multiset;
	for( unsigned int i = 0; i < cards.size(); i++ ) {
		multiset.insert( cards[i] );
 	} 
	return multiset;
}
DJ_BASE_EXPORT MJCardsSet MakeSet( const MJCards& cards )
{
	MJCardsSet  set;
	for( unsigned int i = 0; i < cards.size(); i++ ) {
		set.insert( cards[i] );
 	} 
	return set;
}
DJ_BASE_EXPORT void SortHuTypeWrap( PMahjongHUTypeWrap pHuTypeWrap )
{
	if ( 0 == pHuTypeWrap )
		return;
	for ( int i = 0; i < 9; ++i ) {
  		if ( pHuTypeWrap->cardwrap[i].chExist
  			&& MAHJONG_CARDWRAP_NAME_SHUN == pHuTypeWrap->cardwrap[i].chCardWrapName ) {
  			MJCards cards;
  			for (int j = 0; j < 4; j++ ) {
  				MJCard card	= pHuTypeWrap->cardwrap[i].chWrap[j];
  				if ( 0 != card )
  					cards.push_back(card);
  			}
  			sort(cards.begin(),cards.end(),MJShunCardLess );
			for( unsigned int j = 0; j < cards.size(); j++ ) {
				pHuTypeWrap->cardwrap[i].chWrap[j]	= cards[j];
			}
  		}
  	}
}
DJ_BASE_EXPORT void FreeHUTypeWrap( PMahjongHUTypeWrap pHuTypeWrap )
{
  	while(pHuTypeWrap)
	{
		PMahjongHUTypeWrap p=pHuTypeWrap;
		pHuTypeWrap = pHuTypeWrap->pnext;
		free(p);
	}
}

