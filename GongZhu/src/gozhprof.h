#ifndef __DJGAME_GONGZHU_PROTOCOL_H__
#define __DJGAME_GONGZHU_PROTOCOL_H__

#include "protocol.h"
#include "gozhrule.h"
//replace GONGZHU to Uppercase GameName
//replace Gongzhu 

#define GONGZHU_DOUBLE9_CARDS								55
#define GONGZHU_DOUBLE6_CARDS							28

#define GONGZHU_MAX_SITES								4


//#define DJGAME_PROTOCOL_SUBTYPE_ROOMINFO			0x01

typedef struct __tagGongzhuRoom
{
	GongzhuRule			rule;
	int 				iMaxScore;
	int 				iMinScore;
	unsigned short		shBaseScore;
}GongzhuRoom,*PGongzhuRoom;

//#define DJGAME_PROTOCOL_SUBTYPE_CURRENT_GAMEINFO		0x17
//pgamehead->chBuf:
typedef struct _tagGongzhuCurrent
{
	int					iScore[GONGZHU_MAX_SITES+1];
	unsigned short		shTotalGames;
	GongzhuRule			rule;
}GongzhuCurrent,*PGongzhuCurrent;

///定义游戏的GAMETRACE


//#define DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2			0xFE
#define GONGZHU_GAMETRACE_RESET				0x01
#define GONGZHU_GAMETRACE_PICKUP			0x81


#define GONGZHU_GAMETRACE_SHOW			0x02
//GongzhuShow (gozhurule.h)

#define GONGZHU_GAMETRACE_THROW			0x03
//

#define GONGZHU_GAMETRACE_ROUNDRESULT			0x04
typedef struct __tagGongzhuRoundResult
{
	unsigned char chPages;
	unsigned char chCards[1];
}GongzhuRoundResult,*PGongzhuRoundResult;

#define GONGZHU_GAMETRACE_CLEARDESKTOP			0x05

//////////////////////////////////////////////////////
#define DJGAME_GONGZHU_TABLE_STATUS_WAIT_SHOW		0x05
#define DJGAME_GONGZHU_TABLE_STATUS_WAIT_THROW		0x06

#define GONGZHU_TIMEOUT_WAIT_SHOW			20
#define GONGZHU_TIMEOUT_WAIT_THROW			25


#endif
