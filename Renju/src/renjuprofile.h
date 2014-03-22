#ifndef __DJGAME_RENJU_PROTOCOL_H__
#define __DJGAME_RENJU_PROTOCOL_H__

#include "protocol.h"
#include "renjurule.h"
//replace RENJU to Uppercase GameName
//replace Renju 


#define RENJU_MAX_SITES								2


//#define DJGAME_PROTOCOL_SUBTYPE_ROOMINFO			0x01

typedef struct __tagRenjuRoom
{	
	RenjuRule			rule;
}RenjuRoom,*PRenjuRoom;


//#define DJGAME_PROTOCOL_SUBTYPE_CURRENT_GAMEINFO		0x17
//pgamehead->chBuf:
typedef struct _tagRenjuCurrent
{
	unsigned char		chWhite;		//
	unsigned char 		chBlack;
	unsigned char 		chExchanges;
	unsigned char 		chXX;
	unsigned short		shTotalGames;
	int 				iScore[RENJU_MAX_SITES+1];
	RenjuRule			rule;
	RenjuRule			whiterule;
	RenjuRule			blackrule;
}RenjuCurrent,*PRenjuCurrent;

///定义游戏的GAMETRACE


//#define DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2			0xFE
#define RENJU_GAMETRACE_ADJUST			0x01


#define RENJU_GAMETRACE_MOVE			0x02
typedef struct __tagRenjuMove
{
	unsigned char x;
	unsigned char y;
}RenjuMove,*PRenjuMove;

#define RENJU_GAMETRACE_SETRULE			0x03
//chBuf = RenjuRule

#define RENJU_GAMETRACE_SURRENDER		0x04
//
#define RENJU_GAMETRACE_REQUESTDRAW		0x05
//

#define RENJU_GAMETRACE_REQUESTRULE		0x06
//chBuf = RenjuRule

#define RENJU_GAMETRACE_BEFORBID			0x07

#define RENJU_GAMETRACE_SELECT				0x08
//client->server chBuf[0]=1/2


#define RENJU_GAMETRACE_EXCHANGE			0x09

#define RENJU_GAMETRACE_REMOVE				0x0A
//5手两打后删除多余的棋,结构等于于 MOVE


#define RENJU_GAMETRACE_DESCRIBE			0x0B
typedef struct __tagRenjuStepDescribe
{
	unsigned char x;
	unsigned char y;
	unsigned char des;
}RenjuStepDescribe,*PRenjuStepDescribe;
#define RENJU_STEP_DESCRIBE_FORBID			0x01
#define RENJU_STEP_DESCRIBE_NOTFORBID		0x02
#define RENJU_STEP_DESCRIBE_WIN			0x03

//////////////////////////////////////////////////////
#define DJGAME_RENJU_TABLE_STATUS_WAIT_MOVE	0x05
#define DJGAME_RENJU_TABLE_STATUS_WAIT_RULE	0x06
#define DJGAME_RENJU_TABLE_STATUS_WAIT_SELECT	0x07


#define RENJU_TIMEOUT_WAIT_SELECT			30
#define RENJU_TIMEOUT_WAIT_RULE			30


//#define RENJU_MAGIC_PICKUP				0x01
#endif
