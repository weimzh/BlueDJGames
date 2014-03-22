#ifndef __DJGAME_SUOHA_RULE_H__
#define __DJGAME_SUOHA_RULE_H__

typedef struct __tabSuohaRule
{
	unsigned char chRule;
}SuohaRule,*PSuohaRule;

unsigned short Suoha_GetCardsType(unsigned char *pcards);
bool Suoha_Compare2Cards(unsigned char *pcards1,unsigned char *pcards2,unsigned char *p1,unsigned char *p2);
bool Suoha_Compare2Card(unsigned char chCard1,unsigned char chCard2);

#endif
