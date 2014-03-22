#ifndef __DJGAME_BLACKJACK_PROTOCOL_H__
#define __DJGAME_BLACKJACK_PROTOCOL_H__

#include "protocol.h"
#include "bjrule.h"
//replace BLACKJACK to Uppercase GameName
//replace Blackjack 

#define DJGAME_BLACKJACK_MAX_PLAYERS						4
#define BLACKJACK_MAX_SITES								4

#define BLACKJACK_PUKE_CARDS								52

#define BLACKJACK_PLAYER_MAX_CARDS							15


//#define DJGAME_PROTOCOL_SUBTYPE_ROOMINFO			0x01

typedef struct __tagBlackjackRoom
{
	 int 		iMinCounter;
	unsigned int 		uChipFaceValue;		//单个筹码的面值
	unsigned char 		chNormalChips;		//标准押注数
	unsigned char 		chDouble;		//!=0 允许加倍; ==0不允许
	BlackjackRule			rule;
	unsigned int 		uGameMaxValues; //单局游戏最大的Counter =uChipFaceValue*chNormalChips*(chDouble+1);
}BlackjackRoom,*PBlackjackRoom;

//#define DJGAME_PROTOCOL_SUBTYPE_CURRENT_GAMEINFO		0x17
//pgamehead->chBuf:
typedef struct _tagBlackjackCurrent
{
	unsigned char		chDealer;		//庄家的坐位号
	unsigned char 		chPlayers;		//当前玩家数
	unsigned char 		chMapSites[DJGAME_BLACKJACK_MAX_PLAYERS+1];
}BlackjackCurrent,*PBlackjackCurrent;

///定义游戏的GAMETRACE


//#define DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2			0xFE
#define BLACKJACK_GAMETRACE_PICKUP			0x01
#define BLACKJACK_GAMETRACE_PICKUP2			0x81	//只有庄家首张牌会发
typedef struct __tagBlackjackPickup
{
	unsigned char chMapSite;
	unsigned char chCardNumber;
	unsigned char chCard;
}BlackjackPickup,*PBlackjackPickup;

#define BLACKJACK_GAMETRACE_BET		0x02
//每次都是押一个标准注数（根据允许的Double次数决定可以押注几次）

#define BLACKJACK_GAMETRACE_DRAW		0x03
//player请求抓牌，如果超过21点自动判负，等于21点自动停牌

#define BLACKJACK_GAMETRACE_STOP		0x04
//player请求停牌，等待比较大小

#define BLACKJACK_GAMETRACE_RESULT				0x05
//非庄家,庄家不用发数据(因为输赢都是相对庄家进行的)
typedef struct __tagBlackjackResult
{//每个玩家(非庄家）发送一个结果
	unsigned char chMapSite;
	unsigned char chResult;
	unsigned char chChips;
}BlackjackResult,*PBlackjackResult;
#define BLACKJACK_GAMERESULT_WIN		1
#define BLACKJACK_GAMERESULT_LOST		2
#define BLACKJACK_GAMERESULT_DRAW		0

#define BLACKJACK_GAMETRACE_SHOWCARDS				0x06
typedef struct __tagBlackjackShowCards
{//庄家亮牌
	unsigned char chMapSite;
	unsigned char chCards;
	unsigned char chCard[1];
}BlackjackShowCards,*PBlackjackShowCards;

//////////////////////////////////////////////////////
#define DJGAME_BLACKJACK_TABLE_STATUS_WAIT_PLAYER	0x05


#define BLACKJACK_TIMEOUT_WAIT_PLAYER			20


//#define BLACKJACK_MAGIC_PICKUP				0x01
#endif
