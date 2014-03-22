#ifndef __DJGAME_DOUDZHU_PROTOCOL_H__
#define __DJGAME_DOUDZHU_PROTOCOL_H__
#include "protocol.h"
#include "ddzrule.h"

#define DOUDZHU_PUKE_CARDS								54
#define DOUDZHU_MAX_SITES								4


#define DOUDZHU_GAMEBONUS_TARGET_LASTCARD		0x10
//最后一手牌包含是指定数字（比如3）,chParam1 = 数字

#define DOUDZHU_PUKE_KING_BIG							0x3F
#define DOUDZHU_PUKE_KING_SMALL							0x3E

//#define DJGAME_PROTOCOL_SUBTYPE_ROOMINFO			0x01

typedef struct __tagDoudzhuRoom
{
	 unsigned int		iMinCounter;
	 unsigned int 		uMaxGameCounters;
	unsigned char 		chXX;
	unsigned char 		chHands;		//底牌数量
	
	DoudzhuRule			rule;

}DoudzhuRoom,*PDoudzhuRoom;

//#define DJGAME_PROTOCOL_SUBTYPE_CURRENT_GAMEINFO		0x17
//pgamehead->chBuf:
typedef struct _tagDoudzhuCurrent
{
	unsigned char		chMaster;		//地主的坐位号
	unsigned char 		chFirstCall;	//第一个叫牌的坐位号
}DoudzhuCurrent,*PDoudzhuCurrent;


//#define DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2			0xFE
//#define DOUDZHU_GAMETRACE_RESET			0x12
#define DOUDZHU_GAMETRACE_DEAL			0x01			//Player Initialize cards (unorder,dial order)
#define DOUDZHU_GAMETRACE_PICKUP		0x81

#define DOUDZHU_GAMETRACE_CALL		0x02
//chBuf[0] = 叫的分数

#define DOUDZHU_GAMETRACE_THROW				0x03
#define DOUDZHU_GAMETRACE_COMEBACK			0x04

#define DOUDZHU_GAMETRACE_HAND				0x05
//#define DOUDZHU2_GAMETRACE_HAND				

//////////////////////////////////////////////////////
#define DJGAME_DOUDZHU_TABLE_STATUS_WAIT_CALL	0x05
#define DJGAME_DOUDZHU_TABLE_STATUS_WAIT_THROW	0x06


#define DOUDZHU_TIMEOUT_WAIT_CALL			15
#define DOUDZHU_TIMEOUT_WAIT_THROW			32

//#define DOUDZHU_MAGIC_PICKUP				0x01
#endif
