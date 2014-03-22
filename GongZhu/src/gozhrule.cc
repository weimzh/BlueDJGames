#include <iostream>
#include "protocol.h"
#include "gozhrule.h"

#define PLAYER_MAX_PAGES				28


unsigned char GongzhuRule_CheckThrow(PDJGamePokerImage ppokerimg,
									unsigned char *pthrow,unsigned char chThrowPages,
									unsigned char chFirPlayerThrow,unsigned char chFirstPages,
									unsigned char chClassRound,unsigned char *pchCurMax)
{
	unsigned char chPages,chCards[PLAYER_MAX_PAGES+2];
	if(chFirstPages > 2 || chThrowPages > 2)
		return GONGZHU_RULE_FAILED;
	if(chFirPlayerThrow == 0 || chFirstPages == 0)
	{//本轮第一家出牌
		if(DJGAME_POKER_ERRORCARD(*pthrow))
			return GONGZHU_RULE_FAILED;
		if(!GongzhuRule_IsDouble(pthrow,chThrowPages))
			return GONGZHU_RULE_FAILED;

		if(DJGamePoker_IsSubSet(ppokerimg,pthrow,chThrowPages) )
		{
			if(chClassRound == 0)
			{
				if(ppokerimg->chSign[*pthrow] & GONGZHU_POKER_SIGN_SHOW)
				{
					if(GongzhuRule_GetNotShowPages(ppokerimg,DJGAME_POKER_CLASS(*pthrow)) != 0)
						return GONGZHU_RULE_FAILED;
				}
			}
			return GONGZHU_RULE_SUCCESS_WITH_MAX;
		}
		return GONGZHU_RULE_FAILED;
	}else
	{
		if( chThrowPages != chFirstPages)
			return GONGZHU_RULE_FAILED;
		if(DJGAME_POKER_ERRORCARD(*pthrow))
			return 	GONGZHU_RULE_FAILED;
		if(chFirstPages == 2 && DJGAME_POKER_ERRORCARD(*(pthrow+1)))
			return 	GONGZHU_RULE_FAILED;
		if(!DJGamePoker_IsSubSet(ppokerimg,pthrow,chFirstPages))
			return GONGZHU_RULE_FAILED;
//		chPages = DJGamePoker_GetTotalPages(ppokerimg,DJGAME_POKER_CLASS(chFirPlayerThrow),0);
		chPages = DJGamePoker_GetSubSet(ppokerimg,DJGAME_POKER_CLASS(chFirPlayerThrow),0,chCards,PLAYER_MAX_PAGES,true);
		if(chPages != 0)
		{
			unsigned char chIncludeDoubles = GongzhuRule_IncludeDoubles(ppokerimg,DJGAME_POKER_CLASS(chFirPlayerThrow),chFirstPages,true);
			if(chPages == chFirstPages)
			{
				memcpy(pthrow,chCards,chFirstPages);
			}else if(chPages < chFirstPages)
			{//只有一种情况 chPages = 1,chFirstPages = 2
				if(chCards[0] != *pthrow && chCards[0] != *(pthrow+1))
					*pthrow = chCards[0];
				return GONGZHU_RULE_SUCCESS;
			}else
			{
				for(int i=0;i<chFirstPages;i++)
				{
					if(DJGAME_POKER_CLASS(*(pthrow+i)) != DJGAME_POKER_CLASS(chFirPlayerThrow))
						return GONGZHU_RULE_FAILED;
				}
				bool bDouble = GongzhuRule_IsDouble(pthrow,chFirstPages);
				if( !bDouble && chIncludeDoubles > 0)
					return GONGZHU_RULE_FAILED;
				
				if(chClassRound == 0 && DJGAME_POKER_CLASS(*pthrow) == DJGAME_POKER_CLASS(chFirPlayerThrow))
				{
					char chIncludeShow = 0;
					if( ppokerimg->chSign[*pthrow] & GONGZHU_POKER_SIGN_SHOW)
						chIncludeShow ++;
					if(chFirstPages == 2 && (ppokerimg->chSign[*(pthrow+1)] & GONGZHU_POKER_SIGN_SHOW))
						chIncludeShow ++;
					if(chIncludeShow != 0)
					{
					 	if(chFirstPages == 1)
						{
							if(GongzhuRule_GetNotShowPages(ppokerimg,DJGAME_POKER_CLASS(chFirPlayerThrow))
										>= 1)
								return 	GONGZHU_RULE_FAILED;
						}else
						{
							if(bDouble && chIncludeDoubles>1)
								return GONGZHU_RULE_FAILED;
							if(!bDouble && GongzhuRule_GetNotShowPages(ppokerimg,DJGAME_POKER_CLASS(chFirPlayerThrow))
										> 1)
							{
								return GONGZHU_RULE_FAILED;
							}
						}
					}//if(chIncludeShow != 0)
				}//if(chClassRound == 0)
			}//chPages > chFirstPages
			
			
			if(GongzhuRule_Compare2Hand(pchCurMax,pthrow,chFirstPages) < 0)
				return GONGZHU_RULE_SUCCESS_WITH_MAX;
			return GONGZHU_RULE_SUCCESS;
		}else//chPages != 0
		{
			if(DJGamePoker_IsSubSet(ppokerimg,pthrow,chFirstPages))
			{
				return GONGZHU_RULE_SUCCESS;
			}
			return GONGZHU_RULE_FAILED;
		}
	}
	return GONGZHU_RULE_FAILED;
}

int GongzhuRule_Compare2Hand(unsigned char *pch1,unsigned char *pch2,unsigned char chPages)
{
	bool b1,b2;
	b1 = GongzhuRule_IsDouble(pch1,chPages);
	b2 = GongzhuRule_IsDouble(pch2,chPages);
	if(b1 != b2)
	{
		if(DJGAME_POKER_CLASS(*pch1) == DJGAME_POKER_CLASS_D)
			return b1?1:-1;
		return b2?1:-1;
	}
	unsigned char chMax1 = *pch1;
	unsigned char chMax2 = *pch2;
	if(!b1)
	{
		if(GongzhuRule_Compare2Card(chMax1,*(pch1+1)) < 0)
			chMax1 = *(pch1+1);
		if(GongzhuRule_Compare2Card(chMax2,*(pch2+1)) < 0)
			chMax2 = *(pch2+1);
	}
	
	return GongzhuRule_Compare2Card(chMax1,chMax2);
}

int GongzhuRule_Compare2Card(unsigned char chCard1,unsigned char chCard2)
{
	if(DJGAME_POKER_CLASS(chCard1) != DJGAME_POKER_CLASS(chCard2))
		return 1;
	unsigned char chValue1 = DJGAME_POKER_VALUE(chCard1);
	unsigned char chValue2 = DJGAME_POKER_VALUE(chCard2);
	if(chValue1 == chValue2)
		return 1;
	if(chValue1 == DJGAME_POKER_A)
		return 1;
	if(chValue2 == DJGAME_POKER_A)
		return -1;
	
	return (chValue1>chValue2)?1:-1;
}


unsigned char GongzhuRule_IncludeDoubles(PDJGamePokerImage ppokerimg,unsigned char chClass,
										unsigned char chNeedPages,bool bUp)
{
	unsigned char chDoubles = 0;
	int iS,iE;
	if(chClass == 0 || chClass > DJGAME_POKER_CLASS_S)
	{
		iS = 1;
		iE = 0x3F;
	}else
	{
		iS = DJGAME_POKER_MAKE_CODE(0,chClass,1);
		iE = DJGAME_POKER_MAKE_CODE(0,chClass,DJGAME_POKER_K);
	}
	for(int i=iS;i<=iE;i++)
	{
		if(ppokerimg->chPage[i] == chNeedPages)
			chDoubles ++;
		else if(bUp && ppokerimg->chPage[i] > chNeedPages)
			chDoubles += ppokerimg->chPage[i]/chNeedPages;
	}
	return chDoubles;
}
bool GongzhuRule_IsDouble(unsigned char *pcards,unsigned char chPages)
{
	if(chPages == 0 || DJGAME_POKER_ERRORCARD(*pcards))
		return false;
	if(chPages == 1 )
		return true;
	
	if(DJGAME_POKER_CARD(*(pcards+1)) != DJGAME_POKER_CARD(*pcards))
		return false;
	return true;
}

unsigned char GongzhuRule_GetNotShowPages(PDJGamePokerImage ppokerimg,unsigned char chClass)
{
	int iS,iE;
	unsigned char chRet = 0;
	if(chClass == 0 || chClass > DJGAME_POKER_CLASS_S)
	{
		iS = 1;
		iE = 0x3F;
	}else
	{
		iS = DJGAME_POKER_MAKE_CODE(0,chClass,1);
		iE = DJGAME_POKER_MAKE_CODE(0,chClass,DJGAME_POKER_K);
	}
	for(int i=iS; i<=iE;i++)
	{
		if(ppokerimg->chPage[i] >0 && !(ppokerimg->chSign[i] & GONGZHU_POKER_SIGN_SHOW))
			chRet += ppokerimg->chPage[i];
	}
	return chRet;
}

unsigned char gs_gongzhu_order[DJGAME_POKER_CLASS_S+1][2][13] = 
{
	{{0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0}},
	{{2,3,4,5,6,7,8,9,DJGAME_POKER_J,DJGAME_POKER_Q,DJGAME_POKER_K,DJGAME_POKER_A,10},
		{10,DJGAME_POKER_A,DJGAME_POKER_K,DJGAME_POKER_Q,DJGAME_POKER_J,9,8,7,6,5,4,3,2}},
	{{DJGAME_POKER_A,DJGAME_POKER_K,DJGAME_POKER_Q,10,9,8,7,6,5,4,3,2,DJGAME_POKER_J},
		{10,9,8,7,6,5,4,3,2,DJGAME_POKER_Q,DJGAME_POKER_K,DJGAME_POKER_A,DJGAME_POKER_J}},
	{{2,3,4,5,6,7,8,9,10,DJGAME_POKER_J,DJGAME_POKER_Q,DJGAME_POKER_K,DJGAME_POKER_A},
		{DJGAME_POKER_A,DJGAME_POKER_K,DJGAME_POKER_Q,DJGAME_POKER_J,10,9,8,7,6,5,4,3,2}},
	{{2,3,4,5,6,7,8,9,10,DJGAME_POKER_J,DJGAME_POKER_K,DJGAME_POKER_A,DJGAME_POKER_Q},
		{DJGAME_POKER_Q,DJGAME_POKER_A,DJGAME_POKER_K,DJGAME_POKER_J,10,9,8,7,6,5,4,3,2}}
};
		
		
unsigned char GongzhuRule_Following(PDJGamePokerImage ppokerimg,unsigned char chClass,
								bool bFollowing,unsigned char chReqPages,unsigned char *pret)
{
	int i,j;
	unsigned char *pchOrder,chTemp;
	unsigned char chCardOrder[PLAYER_MAX_PAGES+2],chCards[PLAYER_MAX_PAGES+2];
	memset(chCards,0,PLAYER_MAX_PAGES+2);
	if(bFollowing)
	{
		pchOrder = gs_gongzhu_order[chClass][0];
	}else
		pchOrder = gs_gongzhu_order[chClass][1];
	
	unsigned char chPages = DJGamePoker_GetSubSet(ppokerimg,chClass,0,chCards,PLAYER_MAX_PAGES,(pchOrder[6]>pchOrder[5]));
	if(chPages <= chReqPages)
	{
		//DEBUG_OUTPUT(("GongzhuRule_Following chReqPages=%d,chPages=%d : \n",chReqPages,chPages));
		if(chPages > 0)
			memcpy(pret,chCards,chPages);
		return chPages;
	}
	//DEBUG_OUTPUT(("GongzhuRule_Following cards : \n"));
	//DEBUG_BUFFER(((char *)chCards,chPages));
	unsigned char chValue;
	for(i=0;i<chPages;i++)
	{
		chCardOrder[i] = 16;
		chValue = DJGAME_POKER_VALUE(chCards[i]);
		for(j=0;j<13;j++)
		{
			if(chValue == pchOrder[j])
				chCardOrder[i] = j;
		}
	}
	//DEBUG_OUTPUT(("GongzhuRule_Following order : \n"));
	//DEBUG_BUFFER(((char *)chCardOrder,chPages));
	do
	{
		j = 0;
		for(i=0;i<chPages-1;i++)
		{
			if(chCardOrder[i] > chCardOrder[i+1])
			{
				chTemp = chCardOrder[i];
				chCardOrder[i] = chCardOrder[i+1];
				chCardOrder[i+1] = chTemp;
				chTemp = chCards[i];
				chCards[i] = chCards[i+1];
				chCards[i+1] = chTemp;
				j ++;
			}
		}
	}while(j > 0);
	//DEBUG_OUTPUT(("GongzhuRule_Following after order : \n"));
	//DEBUG_BUFFER(((char *)chCards,chPages));
	if(chReqPages == 2 )
	{
		for(i=0;i<chPages-1;i++)
		{
			if(chCards[i] != 0 && chCardOrder[i] == chCardOrder[i+1])
			{
				memcpy(pret,&(chCards[i]),2);
				return 2;
			}
		}
		memcpy(pret,chCards,2);
		return 2;
	}

	for(i=0;i<chPages;i++)
	{
		if( chCards[i] != 0 && (i == chPages-1 || chCardOrder[i] != chCardOrder[i+1]) )
		{
			*pret = chCards[i];
			return 1;
		}
		if(chCardOrder[i] == chCardOrder[i+1])
			i++;
	}
	*pret = chCards[0];
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////
bool DJGamePoker_IsSubSet(PDJGamePokerImage ppokerimg,unsigned char *pchild,unsigned char chChilds)
{
	if(chChilds == 0)
		return false;
	DJGamePokerImage imgTemp;
	memcpy(&imgTemp,ppokerimg,sizeof(DJGamePokerImage));
	
	int i;
	for(i=0;i<chChilds;i++)
	{
		if(DJGAME_POKER_ERRORCARD(*(pchild+i)))
			return false;
		if(imgTemp.chPage[*(pchild+i)] < 1)
			return false;
		imgTemp.chPage[*(pchild+i)] --;
	}
	return true;
	
}

void DJGamePoker_CopyImage(PDJGamePokerImage psrc,
									unsigned char chClass,unsigned char chValue,
									PDJGamePokerImage pdes)
{
	memset(pdes,0,sizeof(DJGamePokerImage));
	int iS,iE,d=1;
	if(chClass == 0 || chClass > DJGAME_POKER_CLASS_S)
	{
		iS = 1;
		iE = 0x3F;
	}else
	{
		iS = DJGAME_POKER_MAKE_CODE(0,chClass,1);
		iE = DJGAME_POKER_MAKE_CODE(0,chClass,DJGAME_POKER_K);
	}
	if(chValue != 0)
	{
		d = 0x10;
		iS += (chValue-1);
	}
	for(int i=iS; i<=iE;i+=d)
	{
		pdes->chPage[i] = psrc->chPage[i];
	}
}

unsigned char DJGamePoker_GetSubSet(PDJGamePokerImage ppokerimg,
									unsigned char chClass,unsigned char chValue,
									unsigned char *psubset,unsigned char chSubMax,bool bOrder)
{
	unsigned char chRet = 0;
	int iS,iE,d=1;
	if(chClass == 0 || chClass > DJGAME_POKER_CLASS_S)
	{
		iS = 1;
		iE = 0x3F;
	}else
	{
		iS = DJGAME_POKER_MAKE_CODE(0,chClass,1);
		iE = DJGAME_POKER_MAKE_CODE(0,chClass,DJGAME_POKER_K);
	}
	if(chValue != 0)
	{
		d = 0x10;
		iS += (chValue-1);
	}
	int i,j;
	if(bOrder)
	{
		for(i=iS; i<=iE;i+=d)
		{
			if( ppokerimg->chPage[i] > 0 )
			{
				for(j=0;j<ppokerimg->chPage[i];j++)
				{
					if(chRet < chSubMax)
						*(psubset+chRet) = i;
					chRet ++;
				}
			}
		}
	}else 
	{
		for(i=iE; i>=iS;i-=d)
		{
			if( ppokerimg->chPage[i] > 0 )
			{
				for(j=0;j<ppokerimg->chPage[i];j++)
				{
					if(chRet < chSubMax)
						*(psubset+chRet) = i;
					chRet ++;
				}
			}
		}
	}
	return chRet;
}

unsigned char DJGamePoker_GetTotalPages(PDJGamePokerImage ppokerimg,
											unsigned char chClass,unsigned char chValue)
{
	int iS,iE,d=1;
	unsigned char chRet = 0;
	
	if(chClass == 0 || chClass > DJGAME_POKER_CLASS_S)
	{
		iS = 1;
		iE = 0x3F;
	}else
	{
		if( (chValue >0 && chValue <= 13) || (
					chClass == DJGAME_POKER_CLASS_S && chValue > 0 && chValue <0x10) )
			return ppokerimg->chPage[DJGAME_POKER_MAKE_CODE(0,chClass,chValue)];
		
		iS = DJGAME_POKER_MAKE_CODE(0,chClass,1);
		iE = DJGAME_POKER_MAKE_CODE(0,chClass,DJGAME_POKER_K);
	}
	if(chValue != 0)
	{
		d = 0x10;
		iS += (chValue-1);
	}
	for(int i=iS; i<=iE;i+=d)
	{
		chRet += ppokerimg->chPage[i];
	}
	return chRet;
}

bool DJGamePoker_RemoveCards(PDJGamePokerImage ppokerimg,
							unsigned char *pchSub,unsigned char chSubLen)
{
	if(	pchSub == NULL || chSubLen == 0)
		return true;
	int i;
	for(i=0;i<chSubLen;i++)
	{
		if(DJGAME_POKER_ERRORCARD(pchSub[i]))
			continue;
		if(ppokerimg->chPage[pchSub[i]] > 0)
			ppokerimg->chPage[pchSub[i]] --;
	}
	return true;
}
