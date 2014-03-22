#ifndef __DJGAME_NIUNIU_RULE_H__
#define __DJGAME_NIUNIU_RULE_H__

typedef struct __tabNiuniuRule
{
	unsigned char chRule;
}NiuniuRule,*PNiuniuRule;

unsigned char NiuniuGetCardsPoints(unsigned char *pcards);
bool NiuniuCompare2Cards(unsigned char *pcards1,unsigned char *pcards2,unsigned char *p1,unsigned char *p2);
int NiuniuCompare2Card(unsigned char chCard1,unsigned char chCard2);
unsigned char NiuniuGetMaxCard(unsigned char *pcards,bool bRemove);

#endif
