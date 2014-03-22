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
#include "dmnrule.h"
#include "dmnprofile.h"
//#include "jdebug.h"


bool IsPlacable(PDominoChains pchains,unsigned char *pchCards,unsigned char chPages)
{
	unsigned char root1,root2;
	root1 = DOMINO_CARD_GET1(pchains->root);
	root2 = DOMINO_CARD_GET2(pchains->root);
	//DEBUG_OUTPUT(("IsPlacable pages=%d\n",chPages));
	//DEBUG_BUFFER(((char *)pchains,sizeof(DominoChains)));
	if(chPages == 0 || pchCards == NULL)
		return false;
	//DEBUG_OUTPUT(("cards : "));
	//DEBUG_BUFFER(((char *)pchCards,chPages));
	if(DOMINO_CARD_ERROR(root1) || DOMINO_CARD_ERROR(root2))
		return true;
	unsigned char  i,j=(root1==root2)?4:2;
	unsigned char  num=0,chNumbers[5]={DOMINO_CARD_ERRORID,DOMINO_CARD_ERRORID,DOMINO_CARD_ERRORID,DOMINO_CARD_ERRORID,DOMINO_CARD_ERRORID};
	unsigned char ch1,ch2;
	for(i=1;i<=j;i++)
	{
		ch2 = DOMINO_CARD_GET2(pchains->chLastCard[i]);
		if(!DOMINO_CARD_ERROR(ch2))
		{
			chNumbers[num] = ch2;
			num ++;
		}else
		{
			if(i==1)
			{
				chNumbers[num] = root1;
				num ++;
				j = 2;
			}else if(i==2)
			{
				chNumbers[num] = root2;
				num ++;
				j = 2;
			}else
			{
				chNumbers[num] = root2;
				num ++;
			}
		}
	}
	
	for(j=0;j<chPages;j++)
	{
		ch1 = DOMINO_CARD_GET1(*(pchCards+j));
		ch2 = DOMINO_CARD_GET2(*(pchCards+j));
		for(i=0;i<num;i++)
		{
			if(ch1 == chNumbers[i] || ch2 == chNumbers[i])
				return true;
		}
	}
	
	return false;
}

unsigned char GetCurrentDesktopNumbers(PDominoChains pchains)
{
	unsigned char chRet = 0,ch1,ch2,i;
	unsigned char root1,root2,chMaxChains=2;
	root1 = DOMINO_CARD_GET1(pchains->root);
	root2 = DOMINO_CARD_GET2(pchains->root);
	if(root1 == root2 && DOMINO_ISCARD(pchains->chLastCard[1]) && DOMINO_ISCARD(pchains->chLastCard[2]) )
		chMaxChains=4;
	if(!DOMINO_ISCARD(pchains->chLastCard[1]))
	{
		if(root1 == root2)
			chRet = (root1<<1);
		else
			chRet = root1;
		if(DOMINO_ISCARD(pchains->chLastCard[2]))
		{
			ch1 = DOMINO_CARD_GET1(pchains->chLastCard[2]);
			ch2 = DOMINO_CARD_GET2(pchains->chLastCard[2]);
			if(ch1 == ch2)
				chRet += (ch1<<1);
			else
				chRet += ch2;
		}
		return chRet;
	}
	
	if(!DOMINO_ISCARD(pchains->chLastCard[2]))
	{
		if(root1 == root2)
			chRet = (root1<<1);
		else
			chRet = root2;
	
		ch1 = DOMINO_CARD_GET1(pchains->chLastCard[1]);
		ch2 = DOMINO_CARD_GET2(pchains->chLastCard[1]);
		if(ch1 == ch2)
			chRet += (ch1<<1);
		else
			chRet += ch2;
		
		return chRet;
	}
	for(i=1;i<=	chMaxChains;i++)
	{
		if(DOMINO_ISCARD(pchains->chLastCard[i]))
		{
			ch1 = DOMINO_CARD_GET1(pchains->chLastCard[i]);
			ch2 = DOMINO_CARD_GET2(pchains->chLastCard[i]);
			if(ch1 == ch2)
				chRet += (ch1<<1);
			else
				chRet += ch2;
		}
	}
	return chRet;
	
}
bool AppendCard2Chain(PDominoChains pchains,unsigned char chChainID,unsigned char chCard,unsigned char *pchCards,unsigned char chPages,bool bRemove)
{
	//是自己手里的牌
	unsigned char chPos=chPages+1, i,ch1,ch2;
	
	if(pchCards == NULL || chPages == 0)
		return false;
	for(i=0;i<chPages;i++)
	{
		if(chCard == *(pchCards+i) || DOMINO_CARD_REVERSE(chCard) == *(pchCards+i))
		{
			chPos = i;
			break;
		}
	}
	if(chPos>= chPages )
	{
		//DEBUG_OUTPUT(("is't hand-card\n"));
		return false;
	}
	
	unsigned char root1,root2;
	root1 = DOMINO_CARD_GET1(pchains->root);
	root2 = DOMINO_CARD_GET2(pchains->root);
	if(DOMINO_CARD_ERROR(root1) || DOMINO_CARD_ERROR(root2))
	{//放置第一张牌
		unsigned char chMaxCard=DOMINO_CARD_ERRORID;
		char chMax=-1;
		bool bDouble = false;
		for(i=0;i<chPages;i++)
		{
			ch1 = DOMINO_CARD_GET1(*(pchCards+i));
			ch2 = DOMINO_CARD_GET2(*(pchCards+i));
			if(ch1 == ch2 && (!bDouble || (ch1<<1) > chMax) )
			{
				bDouble = true;
				chMax = (ch1<<1);
				chMaxCard = *(pchCards+i);
			}
			if(ch1 != ch2 && !bDouble && ch1+ch2 > chMax)
			{
				chMax = ch1+ch2;
				chMaxCard = *(pchCards+i);
			}
		
		}
		if(chCard != chMaxCard)
			return false;
		pchains->root = chCard;
		if(bRemove)
		{
			*(pchCards+chPos) = *(pchCards+(chPages-1));
			*(pchCards+(chPages-1)) = DOMINO_CARD_ERRORID;
		}
		pchains->chChainPages = 0;
		return true;
	}
	
	//是否能接上
	if(chChainID > 4 || chChainID == 0)
	{
		//DEBUG_OUTPUT(("error chain id %d \n",chChainID));
		return false;
	}
	ch1 = DOMINO_CARD_GET1(chCard);
	ch2 = DOMINO_CARD_GET2(chCard);
	unsigned chLast2 = DOMINO_CARD_GET2(pchains->chLastCard[chChainID] );
	if(DOMINO_CARD_ERROR(chLast2))
	{
		if(chChainID > 2)
		{
			if(root1==root2 && DOMINO_ISCARD(pchains->chLastCard[1]) && DOMINO_ISCARD(pchains->chLastCard[2]))
			{
				chLast2 = root1;
			}else
				return false;
		}else if(chChainID == 1)
			chLast2 = root1;
		else
			chLast2 = root2;
	}
	if(ch1 == chLast2)
	{
		;
	}else if(ch2 == chLast2)
	{
		chCard = DOMINO_CARD_REVERSE(chCard);
	}else
		return false;
	pchains->chLastCard[chChainID] = chCard;
	pchains->chain[pchains->chChainPages][DOMINOCHAINS_CHAIN_ID] = chChainID;
	pchains->chain[pchains->chChainPages][DOMINOCHAINS_CHAIN_CARD] = chCard;
	pchains->chChainPages ++;
	if(bRemove)
	{
		*(pchCards+chPos) = *(pchCards+(chPages-1));
		*(pchCards+(chPages-1)) = DOMINO_CARD_ERRORID;
	}
	
	if(root1 != root2 && ch1==ch2)
	{//顶替root成为旋接牌
		DominoChains tempc;
		unsigned char chTempCard,chNewChainIndex=0,chNewID = (chChainID==1)?2:1;
		memset(&tempc,DOMINO_CARD_ERRORID,sizeof(DominoChains) );
		tempc.root = chCard;
		char chCurIndex = pchains->chChainPages+1;
		do
		{
			chTempCard = GetCardOnChain(pchains,chChainID,-1,&chCurIndex);
			if(DOMINO_ISCARD(chTempCard))
			{
				chTempCard = DOMINO_CARD_REVERSE(chTempCard);
				tempc.chain[chNewChainIndex][DOMINOCHAINS_CHAIN_ID] = chNewID;
				tempc.chain[chNewChainIndex][DOMINOCHAINS_CHAIN_CARD] = chTempCard;
				tempc.chLastCard[chNewID] = chTempCard;
				chNewChainIndex ++;
			}else
				break;
		}while(chCurIndex >= 0);
		tempc.chain[chNewChainIndex][DOMINOCHAINS_CHAIN_ID] = chNewID;
		if(chChainID == 1)
			tempc.chain[chNewChainIndex][DOMINOCHAINS_CHAIN_CARD] = pchains->root;
		else
			tempc.chain[chNewChainIndex][DOMINOCHAINS_CHAIN_CARD] = DOMINO_CARD_REVERSE(pchains->root);
		tempc.chLastCard[chNewID] = DOMINO_CARD_REVERSE(pchains->root);
		chNewChainIndex ++;
		
		chCurIndex = 0;
		do
		{
			chTempCard = GetCardOnChain(pchains,chNewID,1,&chCurIndex);
			if(DOMINO_ISCARD(chTempCard))
			{
				tempc.chain[chNewChainIndex][DOMINOCHAINS_CHAIN_ID] = chNewID;
				tempc.chain[chNewChainIndex][DOMINOCHAINS_CHAIN_CARD] = chTempCard;
				tempc.chLastCard[chNewID] = chTempCard;
				chNewChainIndex ++;
			}else
				break;
		}while(chCurIndex <= pchains->chChainPages);
		
		tempc.chChainPages = chNewChainIndex;
		memcpy(pchains,&tempc,sizeof(DominoChains));
	}
	return true;
}

unsigned char GetCardOnChain(PDominoChains pchains,unsigned char chChainID,char chReqIndex,char *pchStartIndex)
{
	if(chChainID == 0)
	{
		return pchains->root;
	}
	if(chChainID > 4 || pchains->chChainPages == 0 || pchains->chChainPages > DOMINO_DOUBLE9_CARDS)
		return DOMINO_CARD_ERRORID;
	if(abs(chReqIndex) > pchains->chChainPages || pchStartIndex < 0)
		return DOMINO_CARD_ERRORID;

	int start,index;
	if(chReqIndex < 0)
	{
		if(*pchStartIndex < pchains->chChainPages)
			start = *pchStartIndex;
		else
			start = pchains->chChainPages-1;
		index = -1;
		while(start>=0)
		{
			if(pchains->chain[start][DOMINOCHAINS_CHAIN_ID] == chChainID)
			{
				if(index == chReqIndex)
				{
					*pchStartIndex = start-1;
					return pchains->chain[start][DOMINOCHAINS_CHAIN_CARD];
				}
				index --;
			}
			start --;
		}
		return DOMINO_CARD_ERRORID;
	}else
	{
		if(*pchStartIndex > pchains->chChainPages)
			return DOMINO_CARD_ERRORID;
	
		start = (*pchStartIndex);
		index = 1;
		while(start<pchains->chChainPages)
		{
			if(pchains->chain[start][DOMINOCHAINS_CHAIN_ID] == chChainID)
			{
		//		printf("find %d chain %d card %02x\n",chChainID,index,pchains->chain[start][DOMINOCHAINS_CHAIN_CARD]);
				if(index == chReqIndex)
				{
					*pchStartIndex = start+1;
					return pchains->chain[start][DOMINOCHAINS_CHAIN_CARD];
				}
				index ++;
			}
			start ++;
		}
		return DOMINO_CARD_ERRORID;
	}
	return DOMINO_CARD_ERRORID;

}

unsigned char GetChainTotalPoints(PDominoChains pchains)
{
	unsigned char root1,root2;
	root1 = DOMINO_CARD_GET1(pchains->root);
	root2 = DOMINO_CARD_GET2(pchains->root);
	if(DOMINO_CARD_ERROR(root1) || DOMINO_CARD_ERROR(root2))
		return DOMINO_MAX_POINTS + 10;
	
	unsigned char i,chPoints=0,ch1,ch2,j=(root1==root2)?4:2;

	for(i=1;i<=j;i++)
	{
		ch1 = DOMINO_CARD_GET1(pchains->chLastCard[i]);
		ch2 = DOMINO_CARD_GET2(pchains->chLastCard[i]);
		if(!DOMINO_CARD_ERROR(ch2))
		{
			if(ch1 == ch2)
				chPoints += (ch2<<1);
			else
				chPoints += ch2;
		}else if(i<=2)
		{
			if(root1 ==root2)
				chPoints += (root1<<1);
			else
			{
				if(i == 1)
					chPoints += root1;
				else
					chPoints += root2;
			}
			j = 2;
		}
	}
	
	return chPoints;
	
}
