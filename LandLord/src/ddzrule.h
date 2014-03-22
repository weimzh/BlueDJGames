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

#ifndef __DJGAME_DOUDZHU_RULE_H__
#define __DJGAME_DOUDZHU_RULE_H__


//一手牌使用 1 byte 来描述
//  7   6   5   4   3   2   1   0
//[ 牌形  ][附属张数][  主牌形张数   ]

//所谓主牌形张数,是指同面值牌的重复次数,而不是总数量
//比如333444,主牌形张数为 3,牌形为 DOUDZHU_CARDTYPE_SERIAL
//附属张数也是同样道理,比如3334445566,附属张数为2 ,牌形为 DOUDZHU_CARDTYPE_SERIAL

#define DOUDZHU_CARDTYPE_BASE			0x00	//单纯牌形
#define DOUDZHU_CARDTYPE_SERIAL			0x40	//连续
#define DOUDZHU_CARDTYPE_ROCKET			0xC0	//火箭 ,炸弹属于单纯牌形,无需特殊定义
//火箭只有一中牌形,就是所由猫抓全,因此实际描述该牌形就是0xC0(0x03<<6)

#define DOUDZHU_MAX_CALLLEVEL							6

typedef struct __tagDoudzhuRule
{
		unsigned char 		chCardPages;	//几付牌 1-4
	unsigned char 		chSerialMinPages[17]; //[x]表示几张的顺子,比如是单张顺还是双张顺,==0 表示不能出顺
	unsigned char 		chComSupport[20];		//支持的复合牌形
	unsigned char 		chBombPages;			//炸弹的张数
		unsigned char 		chCall[DOUDZHU_MAX_CALLLEVEL];
}DoudzhuRule,*PDoudzhuRule;

#define ISBOMB(__prule,__cardtype)		( ((__cardtype) & 0xF0) == 0 && (__cardtype) >= (__prule)->chBombPages)
#define ISROCKET(__cardtype)		( (__cardtype) == DOUDZHU_CARDTYPE_ROCKET )

bool CheckDDZCall(PDoudzhuRule prule,unsigned char chCall,unsigned char chPreCall);
bool IsMaxCall(PDoudzhuRule prule,unsigned char chCall);
bool IsRocket(PDoudzhuRule prule,unsigned char *buf,unsigned char chLen);

bool CheckDDZThrow(PDoudzhuRule prule,unsigned char *pchCards,unsigned char chLen,
							//规则							出牌人手里的牌
							unsigned char *pchThrow,unsigned char chThrowLen,
							//出的牌
							unsigned char *pchCurCardType,unsigned char *pchCurValue,unsigned char *pchCurSerials,
							//用于返回牌形和大小
							unsigned char chPreCardType,unsigned char chPreValue,unsigned char chPreSerials);
							//前面一个人出的牌形和大小

bool RemoveSubSet(unsigned char *pchParent,unsigned char chParentLen,	\
							unsigned char *pchSub,unsigned char chSubLen);

unsigned char CheckCardTypeCode(PDoudzhuRule prule,unsigned char *pchCards,unsigned char chLen,unsigned char *pchCardMinValue,unsigned char *pchSerials);

unsigned char GetSpecialCardType(unsigned char chRulePages,unsigned char *pchCards,unsigned char chLen,unsigned char chPage,unsigned char *pchRealCard,bool bRemove);
bool IsSerial(unsigned char chValue1,unsigned char chValue2);
unsigned char GetTotalPages(unsigned char *chBuf,unsigned char chLen);
unsigned char GetMax(unsigned char chRulePages,unsigned char *pchCards,unsigned char chLen,unsigned char *chCardValue,bool bRemove);
bool CompareLandlordCard(unsigned char chValue1,unsigned char chValue2);
bool IsSubSet(unsigned char *pchParent,unsigned char chParentLen,unsigned char *pchSub,unsigned char chSubLen);



bool DJGamePoker_IsSubSet(PDJGamePokerImage ppokerimg,unsigned char *pchild,unsigned char chChilds);
bool DJGamePoker_RemoveCards(PDJGamePokerImage ppokerimg, \
							unsigned char *pchSub,unsigned char chSubLen);
unsigned char DJGamePoker_GetTotalPages(PDJGamePokerImage ppokerimg, \
											unsigned char chClass,unsigned char chValue);
											
void  DJGamePoker_CopyImage(PDJGamePokerImage psrc, \
									unsigned char chClass,unsigned char chValue, \
									PDJGamePokerImage pdes);
unsigned char DJGamePoker_GetSubSet(PDJGamePokerImage ppokerimg, \
									unsigned char chClass,unsigned char chValue, \
									unsigned char *psubset,unsigned char chSubMax,bool bOrder);
									
#endif
