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

#ifndef __DJGAME_DOMINO_RULE_H__
#define __DJGAME_DOMINO_RULE_H__

#define DOMINO_DOUBLE9_CARDS								55
#define DOMINO_DOUBLE6_CARDS							28

#define DOMINO_CARD_BACK				0xFF
#define DOMINO_CARD_NULL				0xFE
#define DOMINO_CARD_ERRORID				0xEE
#define DOMINO_CARD_ERROR(__id)							(__id>9)

#define DOMINO_MAX_POINTS				60


#define DOMINO_CARD_CREATID(__i,__j)					(((__i)<<4)+__j)
#define DOMINO_CARD_GET1(__card)						((__card<0x9a)?((__card)>>4):DOMINO_CARD_ERRORID)
#define DOMINO_CARD_GET2(__card)						( (__card<0x9a)?((__card)&0x0F):DOMINO_CARD_ERRORID)
#define DOMINO_CARD_REVERSE(__card)						(((__card)>>4) | ((__card & 0x0F)<<4))
#define DOMINO_ISCARD(__card)							(!DOMINO_CARD_ERROR(DOMINO_CARD_GET1(__card)) && !DOMINO_CARD_ERROR(DOMINO_CARD_GET2(__card)) )
typedef struct __tabDominoRule
{
	unsigned short 		shLowest;		//最低分数(不达此分数不允许退出)
	unsigned char 		chCards;		//牌的张数(双6 28张,双9 55张);
	unsigned char 		chInitPages;	//每个玩家的初始牌数
}DominoRule,*PDominoRule;

#define DOMINOCHAINS_CHAIN_ID				0
#define DOMINOCHAINS_CHAIN_CARD				1
typedef struct __tagDominoChains
{
	unsigned char root;					//旋接
	unsigned char chLastCard[6];
	unsigned char chChainPages;
	unsigned char chain[DOMINO_DOUBLE9_CARDS+1][2];		//	DOMINOCHAINS_CHAIN_ID,DOMINOCHAINS_CHAIN_CARD
}DominoChains,*PDominoChains;


bool IsPlacable(PDominoChains pchains,unsigned char *pchCards,unsigned char chPages);
unsigned char GetChainTotalPoints(PDominoChains pchains);
bool AppendCard2Chain(PDominoChains pchains,unsigned char chChainID,unsigned char chCard,unsigned char *pchCards,unsigned char chPages,bool bRemove);
unsigned char GetCurrentDesktopNumbers(PDominoChains pchains);

unsigned char GetCardOnChain(PDominoChains pchains,unsigned char chChainID,char chReqIndex,char *pchStartIndex);

#endif
