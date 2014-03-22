#ifndef __DJGAME_CHUDD_RULE_H__
#define __DJGAME_CHUDD_RULE_H__

typedef struct __tabChuddRule
{
	unsigned char chXX;
}ChuddRule,*PChuddRule;

#define CHUDD_CARDTYPE_FAILED			0
#define CHUDD_CARDTYPE_1			0x01
#define CHUDD_CARDTYPE_2			0x02
#define CHUDD_CARDTYPE_3			0x03
#define CHUDD_CARDTYPE_4			0x04

#define CHUDD_CARDTYPE_ZS			0x05
#define CHUDD_CARDTYPE_T5			0x07
#define CHUDD_CARDTYPE_32			0x08
#define CHUDD_CARDTYPE_41			0x09
#define CHUDD_CARDTYPE_TS			0x10

typedef struct __tagChuddRuleCardType
{
	unsigned char chCardPages;
	unsigned char chCardType;
	unsigned char chSerials;
	unsigned char chMaxCard;
}ChuddRuleCardType,*PChuddRuleCardType;


bool ChuddRule_CheckThrow(PDJGamePokerImage phands,PChuddRuleCardType pcurtype,unsigned char *pcards,unsigned chPages);
bool ChuddRule_Compare2Card(unsigned char chCard1,unsigned char chCard2);
bool ChuddRule_GetCardType(unsigned char *pcards,unsigned char chPages,PChuddRuleCardType pcardtype);

bool DJGamePoker_IsSubSet(PDJGamePokerImage ppokerimg,unsigned char *pchild,unsigned char chChilds);
void DJGamePoker_CopyImage(PDJGamePokerImage psrc, \
									unsigned char chClass,unsigned char chValue, \
									PDJGamePokerImage pdes);

unsigned char DJGamePoker_GetSubSet(PDJGamePokerImage ppokerimg, \
									unsigned char chClass,unsigned char chValue, \
									unsigned char *psubset,unsigned char chSubMax,bool bOrder);
unsigned char DJGamePoker_GetTotalPages(PDJGamePokerImage ppokerimg, \
											unsigned char chClass,unsigned char chValue);
bool DJGamePoker_RemoveCards(PDJGamePokerImage ppokerimg,
							unsigned char *pchSub,unsigned char chSubLen);


#endif
