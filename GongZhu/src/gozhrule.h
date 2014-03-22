#ifndef __DJGAME_GONGZHU_RULE_H__
#define __DJGAME_GONGZHU_RULE_H__

#include "protocol.h"

#define CARD_ZHU					DJGAME_POKER_MAKE_CODE(0,DJGAME_POKER_CLASS_S,DJGAME_POKER_Q)
#define CARD_YANG		DJGAME_POKER_MAKE_CODE(0,DJGAME_POKER_CLASS_D,DJGAME_POKER_J)
#define CARD_BIAN		DJGAME_POKER_MAKE_CODE(0,DJGAME_POKER_CLASS_C,DJGAME_POKER_10)
#define CARD_HA			DJGAME_POKER_MAKE_CODE(0,DJGAME_POKER_CLASS_H,DJGAME_POKER_A)
#define GONGZHU_ZHU(__card)			((DJGAME_POKER_CARD(__card))==CARD_ZHU)
#define GONGZHU_YANG(__card)		((DJGAME_POKER_CARD(__card))==CARD_YANG)
#define GONGZHU_BIAN(__card)		((DJGAME_POKER_CARD(__card))==CARD_BIAN)
#define GONGZHU_HA(__card)		((DJGAME_POKER_CARD(__card))==CARD_HA)

#define GONGZHU_ZHU_FENSHU			(-100)
#define GONGZHU_YANG_FENSHU			100
#define GONGZHU_BIAN_BEISHU			2
#define GONGZHU_HA_FENSHU			(-50)
#define GONGZHU_HK_FENSHU			(-40)
#define GONGZHU_HQ_FENSHU			(-30)
#define GONGZHU_HJ_FENSHU			(-20)
#define GONGZHU_H10_FENSHU			(-10)
#define GONGZHU_H9_FENSHU			(-10)
#define GONGZHU_H8_FENSHU			(-10)
#define GONGZHU_H7_FENSHU			(-10)
#define GONGZHU_H6_FENSHU			(-10)
#define GONGZHU_H5_FENSHU			(-10)
#define GONGZHU_H4_FENSHU			0
#define GONGZHU_H3_FENSHU			0
#define GONGZHU_H2_FENSHU			0

#define GONGZHU_QUANHONG_FENSHU		200

typedef struct __tabGongzhuRule
{
	unsigned char chCardPages;
	unsigned char chTeams;
	unsigned char chXX;
}GongzhuRule,*PGongzhuRule;

typedef struct __tagGongzhuShowCard
{
	unsigned char chPages;
	unsigned char chCard;
}GongzhuShow,*PGongzhuShow;

#define GONGZHU_RULE_FAILED				0x80
#define GONGZHU_RULE_SUCCESS			0x00
#define GONGZHU_RULE_SUCCESS_WITH_MAX	0x01

#define GONGZHU_POKER_SIGN_SHOW				0x01


unsigned char GongzhuRule_CheckThrow(PDJGamePokerImage ppokerimg, \
									unsigned char *pthrow,unsigned char chThrowPages, \
									unsigned char chFirPlayerThrow,unsigned char chFirstPages, \
									unsigned char chClassRound,unsigned char *pchCurMax);

unsigned char GongzhuRule_Following(PDJGamePokerImage ppokerimg,unsigned char chClass, \
								bool bFollowing,unsigned char chReqPages,unsigned char *pret);

int GongzhuRule_Compare2Hand(unsigned char *pch1,unsigned char *pch2,unsigned char chPages);
int GongzhuRule_Compare2Card(unsigned char chCard1,unsigned char chCard2);

bool GongzhuRule_IsDouble(unsigned char *pcards,unsigned char chPages);
unsigned char GongzhuRule_GetNotShowPages(PDJGamePokerImage ppokerimg,unsigned char chClass);

unsigned char GongzhuRule_IncludeDoubles(PDJGamePokerImage ppokerimg,unsigned char chClass, \
										unsigned char chNeedPages,bool bUp);
										
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
