#ifndef __DJGAME_XIANGQI_PROTOCOL_H__
#define __DJGAME_XIANGQI_PROTOCOL_H__

#include "protocol.h"
#include "xqrule.h"
//replace XIANGQI to Uppercase GameName
//replace Xiangqi 


//#define DJGAME_PROTOCOL_SUBTYPE_ROOMINFO			0x01

typedef struct __tagXiangqiRoom
{
	 int		iMinCounter;
	 int		xx2;
	unsigned char chXX;
	XiangqiRule			rule;
}XiangqiRoom,*PXiangqiRoom;

typedef struct __tagXiangqiUserGameExtra
{
	unsigned int 	uiDuels;
}XiangqiUserGameExtra,*PXiangqiUserGameExtra;
#define XIANGQI_GET_USERGAME_EXTRA(__pusergame)			(PXiangqiUserGameExtra)((__pusergame)->gamecustom)


//#define DJGAME_PROTOCOL_SUBTYPE_CURRENT_GAMEINFO		0x17
//pgamehead->chBuf:
typedef struct _tagXiangqiCurrent
{
	unsigned char		chRed;		//
	unsigned char 		chBlack;
	XiangqiRule			rule;
	XiangqiRule			redrule;
	XiangqiRule			blackrule;
}XiangqiCurrent,*PXiangqiCurrent;

///定义游戏的GAMETRACE


//#define DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2			0xFE
#define XIANGQI_GAMETRACE_ADJUST			0x01
//chBuf[] = board

#define XIANGQI_GAMETRACE_MOVE		0x02
typedef struct __tagXiangQiMove
{
	unsigned char chStart;
	unsigned char chEnd;
	unsigned char chBoardTurn;
	unsigned char chBuf[1];	//server reserve
}XiangQiMove,*PXiangQiMove;

#define XIANGQI_GAMETRACE_SETRULE		0x03
//chBuf = XiangqiRule

#define XIANGQI_GAMETRACE_SURRENDER		0x04
//
#define XIANGQI_GAMETRACE_REQUESTDUELS		0x05
//

#define XIANGQI_GAMETRACE_REQUESTRULE		0x06
//chBuf = XiangqiRule

#define XIANGQI_GAMETRACE_REPEAT			0x08
//chBuf[0] = times

//////////////////////////////////////////////////////
#define DJGAME_XIANGQI_TABLE_STATUS_WAIT_RULE	0x06
#define XIANGQI_TIMEOUT_WAIT_RULE				60

#define DJGAME_XIANGQI_TABLE_STATUS_WAIT_MOVE	0x05


#endif
