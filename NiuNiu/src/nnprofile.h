#ifndef __DJGAME_NIUNIU_PROTOCOL_H__
#define __DJGAME_NIUNIU_PROTOCOL_H__

#include "protocol.h"
#include "nnrule.h"
//replace NIUNIU to Uppercase GameName
//replace Niuniu 

#define DJGAME_NIUNIU_MAX_PLAYERS						6
#define NIUNIU_MAX_SITES								6

#define NIUNIU_PUKE_CARDS								52

#define NIUNIU_PLAYER_MAX_CARDS							5

#define NIUNIU_PUKE_KING_BIG							0x3F
#define NIUNIU_PUKE_KING_SMALL							0x3E
#define NIUNIU_PUKE_BACKGROUND							0x0F

//#define DJGAME_PROTOCOL_SUBTYPE_ROOMINFO			0x01

typedef struct __tagNiuniuRoom
{
	 int		xx1;//Player's scroe must >= the value
	 int		xx2;//Player's scroe must <= the value
	 int 		iMinCounter;
	unsigned int 		uMinYazhu;		//最小压注,单个筹码的价值
	unsigned char 		chMaxMulti;		//最大压注倍数(相对最小压注)
	unsigned char 		chXX;
	
	NiuniuRule			rule;
	unsigned int 		uGameMaxCounter; //单局游戏最大的Counter =uMinYazhu*chMaxMulti*(NIUNIU_PLAYER_MAX_CARDS+1);
}NiuniuRoom,*PNiuniuRoom;

//#define DJGAME_PROTOCOL_SUBTYPE_CURRENT_GAMEINFO		0x17
//pgamehead->chBuf:
typedef struct _tagNiuniuCurrent
{
	unsigned int 		uMinYazhu[NIUNIU_MAX_SITES+1];	//每个人的最小压注
	unsigned char		chMaster;		//庄家的坐位号
	unsigned char 		chPlayers;		//当前玩家数
	unsigned char 		chMapSites[DJGAME_NIUNIU_MAX_PLAYERS+1];
}NiuniuCurrent,*PNiuniuCurrent;

///定义游戏的GAMETRACE


//#define DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2			0xFE
#define NIUNIU_GAMETRACE_PICKUP			0x01
#define NIUNIU_GAMETRACE_PICKUP2			0x81
typedef struct __tagNiuniuPickup
{
	unsigned char chMapSite;
	unsigned char chCardNumber;
	unsigned char chCard;
}NiuniuPickup,*PNiuniuPickup;

#define NIUNIU_GAMETRACE_YAZHU		0x02
typedef struct __tagNiuniuYazhu
{
	unsigned char chMapSite;
	unsigned char chMultiple;	//房间最小压注的倍数;
}NiuniuYazhu,*PNiuniuYazhu;

#define NIUNIU_GAMETRACE_MASTER		0x03
//chBuf[0] == 0 认输; 否则 跟;

#define NIUNIU_GAMETRACE_RESULT				0x05
//非庄家,庄家不用发数据(因为输赢都是相对庄家进行的)
typedef struct __tagNiuniuResult
{
	unsigned char chMapSite;
	unsigned char chResult;
	unsigned char chMultiple;
}NiuniuResult,*PNiuniuResult;
#define NIUNIU_GAMERESULT_WIN		1
#define NIUNIU_GAMERESULT_LOSE		0

#define NIUNIU_GAMETRACE_SHOWCARDS				0x06
typedef struct __tagNiuniuShowCards
{
	unsigned char chMapSite;
	unsigned char chCards;
	unsigned char chCard[1];
}NiuniuShowCards,*PNiuniuShowCards;

//////////////////////////////////////////////////////
#define DJGAME_NIUNIU_TABLE_STATUS_WAIT_YAZHU	0x05
#define DJGAME_NIUNIU_TABLE_STATUS_WAIT_MASTER	0x06

#define NIUNIU_TIMEOUT_WAIT_YAZHU			20
#define NIUNIU_TIMEOUT_WAIT_MASTER			20

//#define NIUNIU_MAGIC_PICKUP				0x01
#endif
