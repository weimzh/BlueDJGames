#ifndef __DJGAME_MAHJONG_PROTOCOL_H__
#define __DJGAME_MAHJONG_PROTOCOL_H__
#include "protocol.h"
#include "mjrule.h"
#include "mjfans.h"


#define MAHJONG_HU_PAGES			14
#define MAHJONG_MAX_SITES			4

#define MAHJONG_GAMEBONUS_TARGET_FAN		0x10
//bonus.chParam1 是指定的番种

//#define DJGAME_PROTOCOL_SUBTYPE_ROOMINFO			0x01

typedef struct __tagMahjongRoom
{
	 int		iMinCounter;
	 int		xx2;
	unsigned char 		chXX;
	MahjongRule			rule;
}MahjongRoom,*PMahjongRoom;



//#define DJGAME_PROTOCOL_SUBTYPE_CURRENT_GAMEINFO		0x17
//pgamehead->chBuf:
typedef struct _tagMahjongCurrent
{
	unsigned char		chRingOrientation;
	unsigned char 		chXX;					//废弃,仅为向1.x兼容,
	unsigned char		chMaster;
	unsigned char 		chMenOrientation[MAHJONG_MAX_SITES+1];	//1-MAHJONG_MAX_SITES,每个坐位的门风
}MahjongCurrent,*PMahjongCurrent;


//#define DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2			0xFE
#define MAHJONG_GAMETRACE_RESET			0x20
#define MAHJONG_GAMETRACE_INIT			0x81
//ch[0-12/15]init cards(13/16 by rule),[13/16] plutus show [+1] plutus card


#define MAHJONG_GAMETRACE_DRAW			0x21
#define MAHJONG_GAMETRACE_PICKUP		0x83
//chBuf[0]=card

#define MAHJONG_GAMETRACE_OUT			0x04 //in
//chBuf[0]=card
#define MAHJONG_GAMETRACE_EATCOLLISION		0x05 //in
//chBuf[0]-[1] other cards
#define MAHJONG_GAMETRACE_GANGFLOWER		0x06	//in
//chBuf[0]=chCard

#define MAHJONG_GAMETRACE_GAMEOVER		0x08
#define MAHJONG_GAMETRACE_HU			0x09
//chBuf[0] chSite (dian pao),chBuf[1] = last card
//chBuf[2] = card pages ,chBuf[3]--chBuf[chBuf[3]+1] cards
//chBuf[chBuf[3]+2] FanID, chBuf[chBuf[3]+3] iFan
//....
#define MAHJONG_GAMETRACE_TING			0x0A
//chBuf[0] is card 

#define MAHJONG_GAMETRACE_WALLSTART		0x0B

#define MAHJONG_GAMETRACE_MULTITRACE	0x10
//chBufLen表示chBuf中包含的trace数量



#define MAHJONG_GAMETRACE_RESULT2		0x11
typedef struct __tagMJResult2
{
	unsigned char chCards[20];
	short 			shScore[5];
	unsigned char	chDianSite;
	unsigned char	chFans;
	unsigned char 	chLastCard;
	unsigned char  chXX;
	MahjongFanResult fans[1];
}MJResult2,*PMJResult2;



#define MAHJONG_GAMETRACE_PLUTUS			0x0C


//////////////////////////////////////////////////////
#define DJGAME_MAHJONG_TABLE_STATUS_WAIT_EATCOLLISION	0x05
#define DJGAME_MAHJONG_TABLE_STATUS_WAIT_OUT			0x06
#define DJGAME_MAHJONG_TABLE_STATUS_WAIT_LOCK	0x10

#define MAHJONG_TIMEOUT_WAIT_LOCK			35


#define MAHJONG_TIMEOUT_WAIT_EATCOLLISION	20
#define MAHJONG_TIMEOUT_WAIT_OUT			20


#define MAHJONG_MAGIC_PICKUP				0x01
#endif
