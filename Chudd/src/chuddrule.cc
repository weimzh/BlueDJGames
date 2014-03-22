#include <iostream>
#include "protocol.h"
#include "chuddrule.h"

bool ChuddRule_CheckThrow(PDJGamePokerImage phands,PChuddRuleCardType pcurtype,unsigned char *pcards,unsigned chPages)
{
	if(pcurtype->chCardPages != 0 && chPages != pcurtype->chCardPages)
		return false;
	if(!DJGamePoker_IsSubSet(phands,pcards,chPages))
		return false;
	ChuddRuleCardType cardtype;
	if(ChuddRule_GetCardType(pcards,chPages,&cardtype))
	{
		if(pcurtype->chCardType == 0 || pcurtype->chCardPages ==0 )
		{
			memcpy(pcurtype,&cardtype,sizeof(ChuddRuleCardType));
			return true;
		}
		if(chPages < 5 )
		{
			if(cardtype.chCardType != pcurtype->chCardType)
				return false;
			if(!ChuddRule_Compare2Card(pcurtype->chMaxCard,cardtype.chMaxCard))
			{
				memcpy(pcurtype,&cardtype,sizeof(ChuddRuleCardType));
				return true;
			}
			return false;
		}
		if(cardtype.chCardType > pcurtype->chCardType)
		{
			memcpy(pcurtype,&cardtype,sizeof(ChuddRuleCardType));
			return true;
		}
		if(cardtype.chCardType == pcurtype->chCardType && !ChuddRule_Compare2Card(pcurtype->chMaxCard,cardtype.chMaxCard))
		{
			memcpy(pcurtype,&cardtype,sizeof(ChuddRuleCardType));
			return true;
		}
	}
	return false;
}

bool ChuddRule_Compare2Card(unsigned char chCard1,unsigned char chCard2)
{
	unsigned char chV1,chV2;
	chV1 = DJGAME_POKER_VALUE(chCard1);
	chV2 = DJGAME_POKER_VALUE(chCard2);
	
	if(chV1 == DJGAME_POKER_2 && chV2 != DJGAME_POKER_2)
		return true;
	if(chV2 == DJGAME_POKER_2 && chV1 != DJGAME_POKER_2)
		return false;
	
	if(chV1 == DJGAME_POKER_A && chV2 != DJGAME_POKER_A)
		return true;
	if(chV2 == DJGAME_POKER_A && chV1 != DJGAME_POKER_A)
		return false;
	
	if(chV1 != chV2)
		return chV1>chV2;
	
	return DJGAME_POKER_CLASS(chCard1)>DJGAME_POKER_CLASS(chCard2);
	
}
bool ChuddRule_GetCardType(unsigned char *pcards,unsigned char chPages,PChuddRuleCardType pcardtype)
{
	if(chPages == 0 || chPages > 5)
		return false;
	memset(pcardtype,0,sizeof(ChuddRuleCardType));
	if(chPages == 1 )
	{
		if(!DJGAME_POKER_ERRORCARD(*pcards))
		{
			pcardtype->chCardPages = 1;
			pcardtype->chSerials = 1;
			pcardtype->chCardType = CHUDD_CARDTYPE_1;
			pcardtype->chMaxCard = *pcards;
			return true;
		}
		return false;
	}
	if(chPages == 2 )
	{
		if(!DJGAME_POKER_ERRORCARD(*pcards) && !DJGAME_POKER_ERRORCARD(*(pcards+1)) && 
			DJGAME_POKER_VALUE(*pcards) == DJGAME_POKER_VALUE(*(pcards+1)))
		{
			pcardtype->chCardPages = 2;
			pcardtype->chSerials = 1;
			pcardtype->chCardType = CHUDD_CARDTYPE_2;
			pcardtype->chMaxCard = ((*pcards) > (*(pcards+1)))?(*pcards):(*(pcards+1));
			return true;
		}
		return false;
	}
	if(chPages == 3 )
	{
		if(!DJGAME_POKER_ERRORCARD(*pcards) && !DJGAME_POKER_ERRORCARD(*(pcards+1)) && 
			!DJGAME_POKER_ERRORCARD(*(pcards+2)) &&
			DJGAME_POKER_VALUE(*pcards) == DJGAME_POKER_VALUE(*(pcards+1)) && 
				DJGAME_POKER_VALUE(*pcards) == DJGAME_POKER_VALUE(*(pcards+2)))
		{
			pcardtype->chCardPages = 3;
			pcardtype->chSerials = 1;
			pcardtype->chCardType = CHUDD_CARDTYPE_3;
			pcardtype->chMaxCard = ((*pcards) > (*(pcards+1)))?(*pcards):(*(pcards+1));
			pcardtype->chMaxCard = (pcardtype->chMaxCard > (*(pcards+2)))?(pcardtype->chMaxCard):(*(pcards+2));
			return true;
		}
		return false;
	}
	
	if(chPages == 4 )
	{
		if(!DJGAME_POKER_ERRORCARD(*pcards) && !DJGAME_POKER_ERRORCARD(*(pcards+1)) && 
			!DJGAME_POKER_ERRORCARD(*(pcards+2)) &&
			DJGAME_POKER_VALUE(*pcards) == DJGAME_POKER_VALUE(*(pcards+1)) && 
				DJGAME_POKER_VALUE(*pcards) == DJGAME_POKER_VALUE(*(pcards+2)) && 
				DJGAME_POKER_VALUE(*pcards) == DJGAME_POKER_VALUE(*(pcards+3)))
		{
			pcardtype->chCardPages = 4;
			pcardtype->chSerials = 1;
			pcardtype->chCardType = CHUDD_CARDTYPE_4;
			pcardtype->chMaxCard = ((*pcards) > (*(pcards+1)))?(*pcards):(*(pcards+1));
			pcardtype->chMaxCard = (pcardtype->chMaxCard > (*(pcards+2)))?(pcardtype->chMaxCard):(*(pcards+2));
			pcardtype->chMaxCard = (pcardtype->chMaxCard > (*(pcards+3)))?(pcardtype->chMaxCard):(*(pcards+3));
			return true;
		}
		return false;
	}
	unsigned char chValues[15],ch,chClass = DJGAME_POKER_CLASS(pcards[0]);
	bool b32=false,bT=true;
	unsigned char ch2s=0;
	int i;
	memset(chValues,0,15);
	
	pcardtype->chCardPages = 5;
	pcardtype->chSerials = 1;
	
	for(i=0;i<5;i++)
	{
		if(DJGAME_POKER_ERRORCARD(pcards[i]))
			return false;
		ch = DJGAME_POKER_VALUE(pcards[i]);
		chValues[ch] ++;
		if(chValues[ch] == 3 )
		{
			pcardtype->chMaxCard = DJGAME_POKER_MAKE_CODE(0,DJGAME_POKER_CLASS_S,ch);
			b32=true;
		}
		if(chValues[ch] == 4 )
		{
			pcardtype->chCardType = CHUDD_CARDTYPE_41;
			pcardtype->chMaxCard = DJGAME_POKER_MAKE_CODE(0,DJGAME_POKER_CLASS_S,ch);
			return true;
		}
		if(chValues[ch] == 2)
		{
			ch2s ++;
		}
		if(bT && DJGAME_POKER_CLASS(pcards[i]) != chClass)
			bT = false;
	}
	if(b32 && ch2s == 2)
	{
		pcardtype->chCardType = CHUDD_CARDTYPE_32;
		return true;
	}
	if(ch2s > 0)
		return false;
	if(chValues[DJGAME_POKER_K] == 1)
	{
		chValues[DJGAME_POKER_K+1] = chValues[DJGAME_POKER_A];
		chValues[DJGAME_POKER_A] = 0;
	}
	bool bs = false;
	ch = 0;
	for(i=1;i<= DJGAME_POKER_K+1;i++)
	{
		if(chValues[i] == 1 )
		{
			bs = true;
			ch ++;
			if(ch == 5)
			{
				if(bT)
					pcardtype->chCardType = CHUDD_CARDTYPE_TS;
				else
					pcardtype->chCardType = CHUDD_CARDTYPE_ZS;
				break;
			}
		}
		if(ch == 5)
			break;
		if(chValues[i] == 0 && bs)
		{
			if(bT)
				pcardtype->chCardType = CHUDD_CARDTYPE_T5;
			break;
		}
	}
	if(pcardtype->chCardType != 0)
	{
		ch = 0;
		for(i= DJGAME_POKER_K+1;i>= 5; i--)
		{
			if(chValues[i] != 0)
				break;
		}
		if(i== DJGAME_POKER_K+1)
			ch = DJGAME_POKER_A;
		else 
			ch = i;
		for(i=4;i>=0;i--)
		{
			if(DJGAME_POKER_VALUE(pcards[i]) == ch)
			{
				pcardtype->chMaxCard = pcards[i];
				return true;
			}
		}
	}
	return false;
		
}



///////////////////////////////////////
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
