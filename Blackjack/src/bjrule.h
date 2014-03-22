#ifndef __DJGAME_BLACKJACK_RULE_H__
#define __DJGAME_BLACKJACK_RULE_H__

typedef struct __tabBlackjackRule
{
	unsigned char chRule;
}BlackjackRule,*PBlackjackRule;

unsigned char Blackjack_GetPoints(unsigned char *pcards,unsigned char chPages);

unsigned char Blackjack_Compare(unsigned char chDealerPoint,unsigned char chDealerPages,
									unsigned char chPlayerPoint,unsigned char chPlayerPages);
#endif
