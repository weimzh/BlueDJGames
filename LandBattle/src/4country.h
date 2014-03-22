#ifndef __DJGAME_CHESS_4COUNTRY_MAP_H__
#define __DJGAME_CHESS_4COUNTRY_MAP_H__

#include "protocol.h"
#include "chessmap.h"
//#include "jdebug.h"
//void __debug_junqi_arrange(PGeneralGameTrace2Head ptrace);
//void __debug_junqi_status(char *pchessmap);

#ifdef __DEBUG__
#define JUNQI_DEBUG_OUTPUT_ARRANGE(arg)			__debug_junqi_arrange arg
#define JUNQI_DEBUG_OUTPUT_STATUS(arg)				__debug_junqi_status arg;
#else
#define JUNQI_DEBUG_OUTPUT_ARRANGE(arg)	
#define JUNQI_DEBUG_OUTPUT_STATUS(arg)	
#endif

#define DJGAME_JUNQI_NODETYPE_STOP			1	//兵站
#define DJGAME_JUNQI_NODETYPE_STATION		2	//行营
#define DJGAME_JUNQI_NODETYPE_CAMP			3	//大本营

#define DJGAME_JUNQI_PINTYPE_WAY			1	//公路
#define DJGAME_JUNQI_PINTYPE_RAIL			2	//铁路

#define DJGAME_JUNQI_CONNECTER_AREA			8	//中心连接区
#define DJGAME_JUNQI_CONNECTER_AREA_CENTER	0xFF	//中心兵站的ID

#define JUNQI_STEP_POWER					1
#define JUNQI_DIRECT_POWER					13
#define JUNQI_CONER_POWER					15
#define JUNQI_MAX_POWER						200

#define GET_BoardOfNode(__pid)		(((__pid)->id)>>8)
#define IsConnecterPoint(__pid)			(GET_BoardOfNode(__pid)==DJGAME_JUNQI_CONNECTER_AREA)

#define GET_NodeX(__pid)					(((__pid)&0xF0)>>4)
#define GET_NodeY(__pid)					((__pid)&0x0F)


#define GET_NODEID(__d,__x,__y)			((((unsigned short)(__d))<<8)+((__x)<<4)+(__y))

#define DJGAME_JUNQI_RANK_WEIZHI				13

#define DJGAME_JUNQI_RANK_BING					1
#define DJGAME_JUNQI_RANK_PAI					2
#define DJGAME_JUNQI_RANK_LIAN					3
#define DJGAME_JUNQI_RANK_YING					4
#define DJGAME_JUNQI_RANK_TUAN					5
#define DJGAME_JUNQI_RANK_LVE					6
#define DJGAME_JUNQI_RANK_SHI					7
#define DJGAME_JUNQI_RANK_JUN					8
#define DJGAME_JUNQI_RANK_SILING				9
#define DJGAME_JUNQI_RANK_DILEI					10
#define DJGAME_JUNQI_RANK_JUNQI					11
#define DJGAME_JUNQI_RANK_ZHADAN				12



typedef struct __tagJunQiChip
{
	unsigned char id;
	unsigned char power;
	unsigned char owner;		//by MapSite
}JunQiChip,*PJunQiChip;

typedef struct __tagJunqiRoom
{
	 int		iMinCounter;
	 int		xx2;
	unsigned char 		chXX;
	unsigned char 		chRule;		//正常还是乱战,必需做满才开始游戏(固定游戏人数),还是随时开始(随机2-4人参与游戏)
	unsigned char 		chBingConer;	//工兵是否允许拐弯,0为最大拐弯数,1为禁止拐弯,>1允许拐 n-1次
	unsigned char 		chMap2;		//二人对战时使用的棋盘ID
	unsigned char 		chMap3;		//三人对战时使用的棋盘ID
	unsigned char 		chMap4;		//四人对战时使用的棋盘ID
}JunqiRoom,*PJunqiRoom;
#define DJGAME_JUNQI_RULE_LUAN			0x01			//4国乱战(随机棋子)			
#define DJGAME_JUNQI_RULE_RAND			0x02			//任意人数开始游戏(2-4人均可游戏)		
#define DJGAME_JUNQI_RULE_4TEAM			0x04			//4国混战(不结盟)
#define DJGAME_JUNQI_RULE_FRIENDVISIBLE		0x08		//友军可见
#define DJGAME_JUNQI_RULE_FRIENDROW			0x10		//友军并肩(12,34结盟)

#define DJGAME_JUNQI_MAX_PLAYERS			4
typedef struct _tagJunqiCurrent
{
	unsigned char chPlayers;	//2-4
	unsigned char chMapID;		//使用哪个棋盘进行游戏
	unsigned short shSteps;
	unsigned char chMapSites[DJGAME_JUNQI_MAX_PLAYERS+1];
}JunqiCurrent,*PJunqiCurrent;
#define JUNQI_MAP_NORMAL2					0x02
#define JUNQI_MAP_NORMAL3					0x03
#define JUNQI_MAP_NORMAL4					0x04
#define JUNQI_MAP_4ROW						0x05

#define JUNQI_GAMETRACE_ARRANGECHIP				0x81
#define JUNQI_GAMETRACE_INITCHIP				0x01
typedef struct _tagJunqiTraceChip
{
	unsigned char xy;
	JunQiChip		chip;
}JunqiTraceChip,*PJunqiTraceChip;
typedef struct __tagJunqiTraceInitChip
{
	unsigned char chMapSite;			//棋子所在MapSite
	unsigned char chChips;
	JunqiTraceChip	chip[1];
}JunqiTraceInitChip,*PJunqiTraceInitChip;

#define JUNQI_GAMETRACE_MOVE				0x02
typedef struct __tagJunqiTraceMove
{
	unsigned char chMapSite;	//动作人的MapSite
	unsigned char chStartMapSite;////棋子所在MapSite,起点 (终点取决于path)
	unsigned char sxy;
	unsigned char chResult;	//见后定义
	unsigned short shStep;	//第几步
	unsigned char path[1];
}JunqiTraceMove,*PJunqiTraceMove;
#define JUNQI_MOVERESULT_REMOVE			0xFF		//移动到目标点后被吃掉
#define JUNQI_MOVERESULT_SET			0x01		//目标点上原来的子被吃掉(如果该点原来有子)
#define JUNQI_MOVERESULT_CLEAR			0x02		//双双阵亡

#define JUNQI_GAMETRACE_UNION				0x03
//buf[0]  union info
#define JUNQI_GAMETRACE_UNIONOVER			0x05
//buf[0]  union info

#define JUNQI_GAMETRACE_SHOWARRANGE			0x06

#define JUNQI_GAMETRACE_SETCHIP				0x04
typedef struct __tagJunqiSetChip
{
	unsigned char chMapSite;				//棋子所在mapsite
	JunqiTraceChip	chip;
}JunqiSetChip,*PJunqiSetChip;

#define JUNQI_GAMETRACE_SETCHIPS			0x08
typedef struct __tagJunqiSetChips
{
	unsigned char chChips;
	JunqiSetChip chip[1];
}JunqiSetChips,*PJunqiSetChips;

#define JUNQI_GAMETRACE_WEATHER			0x10
typedef struct __tagJunqiWeather
{
	unsigned char chMapSite;
	unsigned char chDay;
	unsigned char weather;
	unsigned char bori;
	unsigned char bpower;
}JunqiWeather,*PJunqiWeather;

#define JUNQI_GAMETRACE_OVER			0x20
//参考DJGAME_PROTOCOL_SUBTYPE_GAMEOVER2数据结构

#define DJGAME_JUNQI_TABLE_STATUS_WAIT_ARRANGE	0x05
#define DJGAME_JUNQI_TABLE_STATUS_WAIT_MOVE		0x06


#define JUNQI_TIMEOUT_WAIT_ARRANGE			120
#define JUNQI_TIMEOUT_WAIT_MOVE				28


void Init2CountryChessmap(char *pchessmap,unsigned int iMaxSize,unsigned char chFirstSite);
void Init3CountryChessmap(char *pchessmap,unsigned int iMaxSize,unsigned char chFirstSite);
void Init4CountryChessmap(char *pchessmap,unsigned int iMaxSize,unsigned char chFirstSite);

void Init4CountryRowChessmap(char *pchessmap,unsigned int iMaxSize,unsigned char chFirstSite);

void SetChip(char * pchessmap,unsigned char chArea,unsigned char id,PJunQiChip pchip);
bool MoveChip(char * pchessmap,unsigned char chAreaSrc,unsigned char idsrc,
								unsigned char chAreaDis,unsigned char iddis,
								unsigned char **pppath);
bool IsStation(unsigned char x,unsigned char y);
bool IsCamp(unsigned char x,unsigned char y);
bool JunqiCheckArrange(PJunqiTraceInitChip pori,PJunqiTraceInitChip parrange);

unsigned char CheckNodesMovable(PMapNode *pnodes,unsigned char chNodes,unsigned char *unions,unsigned char chUnions);
bool IsFriendlyChip(unsigned char *unions,unsigned char chUnions,PMapNode pstart,PMapNode pend);
bool IsFriendlyPlayer(unsigned char *unions,unsigned char chUnions,unsigned char chP1,unsigned char chP2);
#endif
