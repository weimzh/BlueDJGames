//#include <iostream>
#include "bjrule.h"
#include "bjprofile.h"
//#include "jdebug.h"

////////////////////////////
//rule
//
unsigned char Blackjack_GetPoints(unsigned char *pcards,unsigned char chPages)
{
	if(chPages == 0 || chPages >= BLACKJACK_PLAYER_MAX_CARDS)
		return 0;
	unsigned char chAs=0,chTotalValues=0,chFaceValue;
	int i;
	for(i=0;i<BLACKJACK_PLAYER_MAX_CARDS;i++)
	{
		chFaceValue = DJGAME_POKER_VALUE(pcards[i]);
		if(chFaceValue == 1)
		{
			chAs ++;
		}else
			chTotalValues += ((chFaceValue>10)?10:chFaceValue);
	}
	if(chAs > 0)
	{
		if(chTotalValues + 11 + (chAs-1) > 21)
			return chTotalValues+chAs;
		return chTotalValues + 11 + (chAs-1);
	}
	return chTotalValues;
}

//BLACKJACK_GAMERESULT_WIN		1
//BLACKJACK_GAMERESULT_LOST		2
//BLACKJACK_GAMERESULT_DRAW		0
unsigned char Blackjack_Compare(unsigned char chDealerPoint,unsigned char chDealerPages,
									unsigned char chPlayerPoint,unsigned char chPlayerPages)
{
	if(chPlayerPoint > 21)
		return BLACKJACK_GAMERESULT_LOST;
	if(chDealerPoint > 21)
		return BLACKJACK_GAMERESULT_WIN;
	
	if(chDealerPoint == 21 && chDealerPages == 2)
	{
		if(chPlayerPoint == 21 && chPlayerPages == 2)
			return BLACKJACK_GAMERESULT_DRAW;
		return BLACKJACK_GAMERESULT_LOST;
	}
	if(chPlayerPoint == 21 && chPlayerPages == 2)
		return BLACKJACK_GAMERESULT_WIN;
	
	if(chPlayerPoint > chDealerPoint)
		return BLACKJACK_GAMERESULT_WIN;
	else if(chPlayerPoint == chDealerPoint)
	{
		if(chPlayerPages > chDealerPages)
			return BLACKJACK_GAMERESULT_WIN;
		else if(chPlayerPages < chDealerPages)
			return BLACKJACK_GAMERESULT_LOST;
		return BLACKJACK_GAMERESULT_DRAW;
	}
	return BLACKJACK_GAMERESULT_LOST;
}
