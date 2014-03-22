#ifndef __DJGAME_SUOHA_PROTOCOL_H__
#define __DJGAME_SUOHA_PROTOCOL_H__

#include "protocol.h"
#include "shrule.h"
//replace SUOHA to Uppercase GameName
//replace Suoha 

#define DJGAME_SUOHA_MAX_PLAYERS						6
#define SUOHA_MAX_SITES								6

#define SUOHA_PUKE_CARDS								28
//8-A

#define SUOHA_PLAYER_MAX_CARDS							5

//#define DJGAME_PROTOCOL_SUBTYPE_ROOMINFO			0x01

typedef struct __tagSuohaRoom
{
	 int 		iMinCounter;//筹码少于此数不能落坐
	unsigned int 		uMinYazhu;		//最小压注,单个筹码的价值
	unsigned char 		chMaxMulti;		//单轮最大压注倍数(相对最小压注)
	unsigned char 		chXX;
	unsigned short 	uGameMaxMulti;		//单局游戏最大被倍数（相对uMinYazhu，玩家全押时使用）
	
	SuohaRule			rule;
	unsigned int 		uGameMaxCounter; //单局游戏最大的Counter =uMinYazhu*chMaxMulti*(SUOHA_PLAYER_MAX_CARDS+1);
}SuohaRoom,*PSuohaRoom;

//#define DJGAME_PROTOCOL_SUBTYPE_CURRENT_GAMEINFO		0x17
//pgamehead->chBuf:
typedef struct _tagSuohaCurrent
{
	unsigned int 		uMinYazhu[SUOHA_MAX_SITES+1];	//每个人的最小压注
	int 				iCounterAddup[SUOHA_MAX_SITES+1];//本次游戏的输赢（筹码）
	unsigned char		chMaster;		//庄家的坐位号
	unsigned char 		chPlayers;		//当前玩家数
	unsigned char 		chMapSites[DJGAME_SUOHA_MAX_PLAYERS+1];
}SuohaCurrent,*PSuohaCurrent;

///定义游戏的GAMETRACE


//#define DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2			0xFE
#define SUOHA_GAMETRACE_PICKUP			0x01
#define SUOHA_GAMETRACE_PICKUP2			0x81
typedef struct __tagSuohaPickup
{
	unsigned char chMapSite;
	unsigned char chCardNumber;
	unsigned char chCard;
}SuohaPickup,*PSuohaPickup;

#define SUOHA_GAMETRACE_BET		0x02
//押注
typedef struct __tagSuohaBet
{
	unsigned char chMapSite;
	unsigned char chMultiple;	//房间最小压注的倍数; <当前已押注是 放弃； ＝当前已押注 跟； > 加注
								//SUOHA_BET_MAX 全押
}SuohaBet,*PSuohaBet;
#define SUOHA_BET_MAX				0xFF
#define SUOHA_BET_FOLLOW				0x0
#define SUOHA_BET_AWAY				0xFE

#define SUOHA_GAMETRACE_RESULT				0x05
//非庄家,庄家不用发数据(因为输赢都是相对庄家进行的)
typedef struct __tagSuohaResult
{
	unsigned char chMapSite;
	unsigned char chXX;
	unsigned short shMultiple;
}SuohaResult,*PSuohaResult;
#define SUOHA_GAMERESULT_WIN		1
#define SUOHA_GAMERESULT_LOSE		0

#define SUOHA_GAMETRACE_SHOWCARDS				0x06
typedef struct __tagSuohaShowCards
{
	unsigned char chMapSite;
	unsigned char chCards;
	unsigned char chCard[1];
}SuohaShowCards,*PSuohaShowCards;

//////////////////////////////////////////////////////
#define DJGAME_SUOHA_TABLE_STATUS_WAIT_BET	0x05

#define SUOHA_TIMEOUT_WAIT_BET			20
#define SUOHA_TIMEOUT_WAIT_MASTER			20
#define SUOHA_TIMEOUT_WAIT_LOCK			35

//#define SUOHA_MAGIC_PICKUP				0x01



#define SUOHA_CARDTYPE_TONGHUASHUN			0x3000
#define SUOHA_CARDTYPE_4						0x2A00
#define SUOHA_CARDTYPE_32						0x2600
#define SUOHA_CARDTYPE_TONGSE					0x2000
#define SUOHA_CARDTYPE_SHUN					0x1A00
#define SUOHA_CARDTYPE_3						0x1600
#define SUOHA_CARDTYPE_22						0x1000
#define SUOHA_CARDTYPE_2						0x0A00
#define SUOHA_CARDTYPE_0						0

#define SUOHA_GETCARDTYPE(__cardtype)			((__cardtype)&0xFF00)
#endif
