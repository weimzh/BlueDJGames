#ifndef __DJGAME_RENJU_RULE_H__
#define __DJGAME_RENJU_RULE_H__

#include "protocol.h"
#define DJGAME_RENJU_WIDTH				15
#define DJGAME_RENJU_OWNER_BLACK		1
#define DJGAME_RENJU_OWNER_WHITE		2

#define RENJU_RULE_TIMER_FASTEST				10

#define RENJU_RULEMASK_BASETIME					0x01
#define RENJU_RULEMASK_TIMEAFTERBASETIME			0x02
#define RENJU_RULEMASK_TIMES						0x04
#define RENJU_RULEMASK_TIMEAFTERTIMEOUT			0x08
#define RENJU_RULEMASK_STEPS						0x10

#define RENJU_RULEMASK_RULESET						0x20

#define RENJU_RULESET_HASFORBID					0x10
#define RENJU_RULESET_EXCHANGE						0x20
#define RENJU_RULESET_DOUBLE						0x40

typedef struct __tabRenjuRule
{
	unsigned char chRuleMask;
	unsigned char chMinGames;
	unsigned char chRuleSet;
	unsigned char chXX;
	
	DJGameChessTimer	timer;
	
}RenjuRule,*PRenjuRule;

typedef struct __tabRenjuBoard
{
	unsigned char chBuf[((DJGAME_RENJU_WIDTH+1)*(DJGAME_RENJU_WIDTH+1))>>2];
}RenjuBoard,*PRenjuBoard;

bool RenjuRule_SetNodeChip(PRenjuBoard pboard,unsigned char x,unsigned char y,unsigned char chOwner);
char RenjuRule_GetNodeChip(PRenjuBoard pboard,unsigned char x,unsigned char y);

unsigned char RenjuRule_GetSerials(PRenjuBoard pboard,unsigned char x,unsigned char y,
						int dx,int dy);

bool RenjuRule_GetFirstBlank(PRenjuBoard pboard,unsigned char *x,unsigned char *y, \
							int dx,int dy,unsigned char chRange,unsigned char chOwner);

bool RenjuRule_GetOwnerHead(PRenjuBoard pboard,unsigned char *x,unsigned char *y,int dx,int dy);
unsigned char RenjuRule_GetRowStatus(PRenjuBoard pboard,unsigned char x,unsigned char y, \
						int dx,int dy,unsigned char chRange,unsigned char chOwner, \
						unsigned char *pchChips,unsigned char *pchBlanks, \
						unsigned char *pchMaxSerials,unsigned char *pchMaxBlankSerials);

unsigned char RenjuRule_CheckRule(PRenjuRule prule,PRenjuBoard pboard,unsigned char x,unsigned char y,unsigned char chOwner);

unsigned char RenjuRule_IsForbid(PRenjuBoard pboard,unsigned char x,unsigned char y);

#define DJGAME_RENJU_RULE_SUCCESS				0
#define DJGAME_RENJU_RULE_FORBID_NOT			0
#define DJGAME_RENJU_RULE_WIN					2
#define DJGAME_RENJU_RULE_FORBID_NOT_WITH_A4	3

#define DJGAME_RENJU_RULE_FAILED				0x80
#define DJGAME_RENJU_RULE_FORBID_33			0x81
#define DJGAME_RENJU_RULE_FORBID_44			0x82
#define DJGAME_RENJU_RULE_FORBID_6				0x83

#define RENJU_STATUS_SUCCESS(__sta)			!(__sta & 0x80)

#endif
