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

#include <iostream>
#include "ddzprofile.h"
#include "ddzrule.h"

bool CheckDDZCall(PDoudzhuRule prule,unsigned char chCall,unsigned char chPreCall)
{
	if(chCall <= chPreCall)
		return false;
	int i = 0;
	while(prule->chCall[i] != 0)
	{
		if(prule->chCall[i] == chCall)
			return true;
		i++;
	}
	return false;
}

bool IsMaxCall(PDoudzhuRule prule,unsigned char chCall)
{
	int i = 0;
	while(prule->chCall[i] != 0)
	{
		if(prule->chCall[i] == chCall)
		{
			if(prule->chCall[i+1] == 0)
				return true;
			return false;
		}
		i++;
	}
	return false;;
}

bool IsRocket(PDoudzhuRule prule,unsigned char *buf,unsigned char chLen)
{
	int i;
	if((prule->chCardPages << 1) == chLen)
	{
		for(i=0;i<chLen;i++)
		{
			if(buf[i] != DOUDZHU_PUKE_KING_BIG && buf[i] != DOUDZHU_PUKE_KING_SMALL)
				return false;
		}
		return true;
	}
	return false;
}

bool CheckDDZThrow(PDoudzhuRule prule,unsigned char *pchCards,unsigned char chLen,
							//规则							出牌人手里的牌
							unsigned char *pchThrow,unsigned char chThrowLen,
							//出的牌
							unsigned char *pchCurCardType,unsigned char *pchCurValue,unsigned char *pchCurSerials,
							//用于返回牌形和大小
							unsigned char chPreCardType,unsigned char chPreValue,unsigned char chPreSerials)
							//前面一个人出的牌形和大小
{
	if(IsSubSet(pchCards,chLen,pchThrow,chThrowLen))
	{
		*pchCurCardType = CheckCardTypeCode(prule,pchThrow,chThrowLen,pchCurValue,pchCurSerials);
		if(*pchCurCardType == 0)
			return false;
		if(chPreCardType != 0)
		{
			if(chPreCardType != *pchCurCardType)
			{
				if(ISROCKET(*pchCurCardType) )
					return true;
				if(ISBOMB(prule,*pchCurCardType) )
				{
					if(!ISBOMB(prule,chPreCardType) )
						return true;
					else
					{
						if( (*pchCurCardType &0x0F) > (chPreCardType &0x0F) )
							return true;
						//张数相同的炸弹拥有相同的CardType,不会执行到这里,所以无需判断
					}
				}
				return false;
			}
			if(chPreSerials != 0 && chPreSerials != *pchCurSerials)
				return false;
			return CompareLandlordCard(*pchCurValue,chPreValue);
		}
		return true;
	}
	return false;
}

unsigned char CheckCardTypeCode(PDoudzhuRule prule,unsigned char *pchCards,unsigned char chLen,unsigned char *pchCardMinValue,unsigned char *pchSerials)
{
	unsigned char chCardType = 0;
	unsigned char chValue,chSecPage,chMainPage,chMainPage2,chSerials,chRealCard[17];
	unsigned char chBuf[75];
	int i;
	if(chLen > 60 || chLen == 0)
		return 0;
	memcpy(chBuf,pchCards,chLen);
	
	chMainPage = GetMax(prule->chCardPages,chBuf,chLen,chRealCard,true);
	if(chMainPage == 0)
		return 0;
	if(chMainPage == (prule->chCardPages << 1) && (chRealCard[0]&0x0F) > 0x0D)
	{//火箭
		 if(GetTotalPages(chBuf,chLen) == 0)
			 return DOUDZHU_CARDTYPE_ROCKET;
		 return 0;
	 }
	 if(chMainPage > (prule->chCardPages<<2) )
		 return 0;
	chSerials = 0;
	chValue = chRealCard[0]&0x0F;
	do
	{
		chMainPage2 = GetMax(prule->chCardPages,chBuf,chLen,chRealCard,true);
		if(chMainPage2 == chMainPage)
		{
			if(!IsSerial(chValue,chRealCard[0]&0x0F))
				return 0;
			chValue = chRealCard[0]&0x0F;
		}
		chSerials++;
	}while(chMainPage2 == chMainPage);
	*pchSerials = chSerials;
	if(chSerials > 1 && 
		(prule->chSerialMinPages[chMainPage] == 0 ||chSerials <prule->chSerialMinPages[chMainPage]) )
		return 0;
	if(chMainPage2 != 0)
	{//把最后多选出的一手牌放回去
		int i,j=0;
		for(i=0;i<chLen;i++)
		{
			if(chBuf[i] == 0)
			{
				chBuf[i] = chRealCard[j];
				j++;
				if(j>=chMainPage2)
					break;
			}
		}
	}
//	DEBUG_OUTPUT(("CheckCardTypeCode start to seach secpage : "));
//	DEBUG_BUFFER(((char *)chBuf,chLen));
	chSecPage = GetTotalPages(chBuf,chLen);
	if(chSecPage > 0)
	{
		if(chSecPage%chSerials != 0)
			return 0;
		chSecPage /= chSerials;
		if(chSecPage > 2)
			return 0;
		for(i=0;i<chSerials;i++)
		{
			if(GetSpecialCardType(prule->chCardPages,chBuf,chLen,chSecPage,chRealCard,true) == 0)
				return 0;
		}
	}
	*pchCardMinValue = chValue;
	if(chSecPage == 0 )	//是单纯牌形(包含单纯的顺子)
		return chMainPage;
	chCardType = (chSecPage<<4)+chMainPage;
	if(chSerials > 1)
		chCardType += DOUDZHU_CARDTYPE_SERIAL;
	
	i=0;
	while(prule->chComSupport[i] != 0)
	{//检查规则中是否支持该复合牌形
		if(prule->chComSupport[i] == chCardType)
			return chCardType;
		i++;
	}
	return 0;
}

unsigned char GetMax(unsigned char chRulePages,unsigned char *pchCards,unsigned char chLen,unsigned char *pchRealCard,bool bRemove)
{
	unsigned char chPages[16],chMax=0,chMaxPages=0;
	int i,j;
	memset(chPages,0,16);
	for(i=0;i<chLen;i++)
	{
		if(pchCards[i] != 0)
		{
			chPages[ (pchCards[i]&0x0F) ] ++;
		}
	}
	if(chPages[DOUDZHU_PUKE_KING_SMALL&0x0F] == chRulePages && 
		chPages[DOUDZHU_PUKE_KING_BIG&0x0F] == chRulePages )
	{//火箭
		j=0;
		for(i=0;i<chLen;i++)
		{
			if( pchCards[i] >= DOUDZHU_PUKE_KING_SMALL)
			{
				pchRealCard[j] = pchCards[i];
				if(bRemove)
					pchCards[i] = 0;
				j++;
			}
		}
		return (chRulePages<<1);
	}
		
	for(i=1;i<16;i++)
	{
		if( chPages[i]!=0 && chPages[i] >= chMaxPages)
		{
			if(chPages[i] == chMaxPages)
			{
				if(CompareLandlordCard(i,chMax) )
					chMax = i;
			}else
			{
				chMaxPages = chPages[i];
				chMax = i;
			}
		}
		
	}
	if(chMax == 0)
		return 0;
	
	j=0;
	for(i=0;i<chLen;i++)
	{
		if( (pchCards[i] &0x0F) == chMax)
		{
			pchRealCard[j] = pchCards[i];
			if(bRemove)
				pchCards[i] = 0;
			j++;
		}
	}
	return chMaxPages;
}

unsigned char GetSpecialCardType(unsigned char chRulePages,unsigned char *pchCards,unsigned char chLen,unsigned char chPage,unsigned char *pchRealCard,bool bRemove)
{//return card value
	unsigned char chPages[16],chValue;
	int i,j;

	memset(chPages,0,16);
	for(i=0;i<chLen;i++)
	{
		if(pchCards[i] != 0)
		{
			chPages[ ((*pchCards)&0x0F) ] ++;
		}
	}
	chValue = 0;
	while(chValue == 0 && chPage <= (chRulePages<<2) )
	{
		for(i=1;i<16;i++)
		{
			if(chPages[i] == chPage)
			{
				chValue = i;
			}
		}
		if(chValue == 0)
			chPage ++;
	}
	if(chValue != 0)
	{
		j=0;
		for(i=0;i<chLen;i++)
		{
			if( (pchCards[i] &0x0F) == chValue)
			{
				pchRealCard[j] = pchCards[i];
				if(bRemove)
					pchCards[i] = 0;
				j++;
			}
		}
	}
	return chValue;
}

bool IsSubSet(unsigned char *pchParent,unsigned char chParentLen,
							unsigned char *pchSub,unsigned char chSubLen)
{
	int i,j;
	for(i=0;i<chSubLen;i++)
	{
		if(pchSub[i] == 0)
			continue;
		for(j=0;j<chParentLen;j++)
		{
			if(pchParent[j] == 0)
				continue;
			if(pchParent[j] == pchSub[i])
				break;
		}
		if(j>=chParentLen)
			return false;
	}
	return true;
}

bool RemoveSubSet(unsigned char *pchParent,unsigned char chParentLen,
							unsigned char *pchSub,unsigned char chSubLen)
{
	int i,j;
	for(i=0;i<chSubLen;i++)
	{
		if(pchSub[i] == 0)
			continue;
		for(j=0;j<chParentLen;j++)
		{
			if(pchParent[j] == 0)
				continue;
			if(pchParent[j] == pchSub[i])
			{
				pchParent[j] = 0;
				break;
			}
		}
	}
	return true;
}

bool CompareLandlordCard(unsigned char chValue1,unsigned char chValue2)
{
	chValue1 &= 0x0F;
	chValue2 &= 0x0F;
	if(chValue2>2 && chValue2<=13 && (chValue1 > chValue2 || chValue1<3) )
		return true;
	if(chValue2== 1 && (chValue1==2 || chValue1>13) )
		return true;
	if(chValue2 == 2 && chValue1 > 13)
		return true;
	if(chValue2 > 13 && chValue1>chValue2)
		return true;
	return false;
}
bool IsSerial(unsigned char chValue1,unsigned char chValue2)
{
	if(chValue1 == 1 && chValue2 == 13)
		return true;
	if(chValue1<= 13 && chValue1 > 3 && chValue2 == chValue1-1)
		return true;
	return false;	
}

unsigned char GetTotalPages(unsigned char *chBuf,unsigned char chLen)
{
	int i;
	unsigned char page = 0;
	for(i=0;i<chLen;i++)
	{
		if(chBuf[i] != 0)
			page++;
	}
	return page;
}

//////////////////////////////////////////
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
