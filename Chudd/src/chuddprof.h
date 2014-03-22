#ifndef __DJGAME_CHUDD_PROTOCOL_H__
#define __DJGAME_CHUDD_PROTOCOL_H__

#include "protocol.h"
#include "chuddrule.h"
//replace CHUDD to Uppercase GameName
//replace Chudd 

#define CHUDD_CARDS								52
#define CHUDD_MAX_SITES								4

#define CHUDD_GAMEBONUS_TARGET_MAX			0xFF
//全关，所有人未出牌
#define CHUDD_GAMEBONUS_TARGET_13				0xA0
//关一家 ，（有一家未出牌）
#define CHUDD_GAMEBONUS_TARGET_LASTCARD		0x10
//最后一手牌包含是指定数字（比如3）,chParam1 = 数字


//#define DJGAME_PROTOCOL_SUBTYPE_ROOMINFO			0x01

typedef struct __tagChuddRoom
{
	unsigned int iMinCounter;
	unsigned int uMaxGameCounters;
	ChuddRule			rule;
}ChuddRoom,*PChuddRoom;

//#define DJGAME_PROTOCOL_SUBTYPE_CURRENT_GAMEINFO		0x17
//pgamehead->chBuf:
typedef struct _tagChuddCurrent
{
	int					iScore[CHUDD_MAX_SITES+1];
	unsigned char 		chMaster;
	unsigned char 		chThrowCard;	//出牌时必须包含的牌，只对第一局第一个出牌人有效
	ChuddRule			rule;
}ChuddCurrent,*PChuddCurrent;

///定义游戏的GAMETRACE


//#define DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2			0xFE
#define CHUDD_GAMETRACE_PICKUP			0x01
#define CHUDD_GAMETRACE_PICKUP2			0x81
typedef struct __tagChuddPickup
{
	unsigned char chPages;
	unsigned char chReserves;
	unsigned char chCards[1];
}ChuddPickup,*PChuddPickup;

#define CHUDD_GAMETRACE_THROW			0x03


#define CHUDD_GAMETRACE_SHOW			0x04
//ChuddPickup


//////////////////////////////////////////////////////
#define DJGAME_CHUDD_TABLE_STATUS_WAIT_THROW		0x06


#define CHUDD_TIMEOUT_WAIT_THROW			30

#endif
