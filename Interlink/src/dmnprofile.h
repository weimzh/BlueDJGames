#ifndef __DJGAME_DOMINO_PROTOCOL_H__
#define __DJGAME_DOMINO_PROTOCOL_H__

#include "protocol.h"
#include "dmnrule.h"
//replace DOMINO to Uppercase GameName
//replace Domino 


#define DOMINO_MAX_SITES								4


//#define DJGAME_PROTOCOL_SUBTYPE_ROOMINFO			0x01

typedef struct __tagDominoRoom
{
	 int		xx1;//Player's scroe must >= the value
	 int		xx2;//Player's scroe must <= the value

	unsigned char 		chXX;		//输赢倍数
	
	DominoRule			rule;
}DominoRoom,*PDominoRoom;

//#define DJGAME_PROTOCOL_SUBTYPE_CURRENT_GAMEINFO		0x17
//pgamehead->chBuf:
typedef struct _tagDominoCurrent
{
	unsigned char chBoneyard;
}DominoCurrent,*PDominoCurrent;

///定义游戏的GAMETRACE


//#define DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2			0xFE
#define DOMINO_GAMETRACE_PICKUP			0x01
typedef struct __tagDominoPickup
{
	unsigned char chPages;
	unsigned char chProperties;
	unsigned char chReserves;		//牌池中保留的张数
	unsigned char chCards[1];		//每4bit表示一个数字
}DominoPickup,*PDominoPickup;
#define DOMINO_PICKUP_PROPERTIES_RESET		0
#define DOMINO_PICKUP_PROPERTIES_APPEND		1


#define DOMINO_GAMETRACE_PLACE			0x02
typedef struct __tagDominoPlace
{
	unsigned char chChainID;
	unsigned char chCard;
	unsigned char chDesktopNumber;
}DominoPlace,*PDominoPlace;

#define DOMINO_GAMETRACE_DRAW			0x03
//

#define DOMINO_GAMETRACE_ADJUST			0x05
//chBuf = DominoChains

//////////////////////////////////////////////////////
#define DJGAME_DOMINO_TABLE_STATUS_WAIT_PLACE	0x05
#define DJGAME_DOMINO_TABLE_STATUS_WAIT_DRAW		0x06

#define DOMINO_TIMEOUT_WAIT_PLACE			30
#define DOMINO_TIMEOUT_WAIT_DRAW			10


//#define DOMINO_MAGIC_PICKUP				0x01
#endif
