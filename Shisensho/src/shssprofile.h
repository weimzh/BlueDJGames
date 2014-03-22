//****************************************************************************
//Copyright (C) 2005-2006 Beijing BlueDJ Technology Co.,Ltd. All rights reserved.

//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with this program (in the file LICENSE.GPL); if not, write to the Free Software
//Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

//Please visit http://www.bluedj.com for more infomation about us
//and contact us at ggwizard@gmail.com or darkdong@gmail.com
//****************************************************************************/

#ifndef __DJGAME_DOMINO_PROTOCOL_H__
#define __DJGAME_DOMINO_PROTOCOL_H__

#include "protocol.h"
#include "shssrule.h"
//replace SHISENSHO to Uppercase GameName
//replace Shisensho 

#define SHISENSHO_MAX_SITES								1
#define SHISENSHO_MIN_RECORD							200


//Must set to SHISENSHO_MAX_MAP+8 and zero memory

//#define DJGAME_PROTOCOL_SUBTYPE_ROOMINFO			0x01

typedef struct __tagShisenshoRoom
{
	 int		iPlayerLowestScore;//Player's scroe must >= the value
	 int		iPlayerHighestScore;//Player's scroe must <= the value
	
	ShisenshoRule			rule;
}ShisenshoRoom,*PShisenshoRoom;

//#define DJGAME_PROTOCOL_SUBTYPE_CURRENT_GAMEINFO		0x17
//pgamehead->chBuf:
typedef struct _tagShisenshoCurrent
{
	ShisenshoRule	rule;
	unsigned short	shRemain;	//剩余时间,rule中的timeout是总共的时间,此处是当前剩余
	unsigned int 	iScore;		//当前分数
}ShisenshoCurrent,*PShisenshoCurrent;

///定义游戏的GAMETRACE


//#define DJGAME_PROTOCOL_SUBTYPE_GAMETRACE2			0xFE
#define SHISENSHO_GAMETRACE_ADJUST			0x01
typedef struct __tagShisenshoAdjust
{/*如果rule中的width height过大,有可能单个包(蓝迪游戏限制单个协议包最大尺寸是
	DJGAME_MAX_PROTOCOL_SIZE) 无法完成传输,因此需要再此处支持多包传输初始化状态
	*/
	unsigned char chStartLine;
	unsigned char chEndLine;
	unsigned char chBuf[1];
}ShisenshoAdjust,*PShisenshoAdjust;

#define SHISENSHO_GAMETRACE_DELETE			0x02
typedef struct __tagShisenshoDelete
{
	unsigned char chX[6]; // for width
	unsigned char chY[6];	// for height
	/*
	在标准情况下,(chX[0],chY[0])表示了第一张牌,
	(chX[1],chY[1])表示了两张牌连线上的第一个拐点,
	(chX[2],chY[2])表示了两张牌连线上的第二个拐点,
	(chX[3],chY[3])表示了第二张牌,
	(chX[4],chY[4])为0,表示路径描述结束
	但[2]及[3]有可能为0(即两张牌间少于两个拐点)
	*/
	unsigned char		chWah;			//去掉2张牌后,随机发送的奖励分(以100分为单位)
}ShisenshoDelete,*PShisenshoDelete;

#define SHISENSHO_GAMETRACE_RESET			0x03
//玩家要求重新排列(减少一次生命) 服务器同意后将其剩余生命发到客户端
//chBuf[0] = 剩余生命


#define SHISENSHO_GAMETRACE_GATESCORE			0x04
//每关(局)结束后的奖励分;
typedef struct __tagShisenshoGateScore
{
	unsigned short shGateScore;
	unsigned int   uiTotalScore;
}ShisenshoGateScore,*PShisenshoGateScore;

#define SHISENSHO_GAMETRACE_FORCEOVER			0x05
//结束游戏

#define SHISENSHO_GAMETRACE_CONTINUE			0x06

#define SHISENSHO_GAMETRACE_RANK			0x10

//////////////////////////////////////////////////////
#define DJGAME_SHISENSHO_TABLE_STATUS_WAIT_DELETE	0x05
#define DJGAME_SHISENSHO_TABLE_STATUS_WAIT_CONTINUE	0x06

#define DJGAME_SHISENSHO_TIMEOUT_WAIT_CONTINUE			300



#define SHISENSHO_RANK_TYPE_DAILY				0x01
#define SHISENSHO_RANK_TYPE_WEEKLY				0x02
#define SHISENSHO_RANK_TYPE_MONTHLY				0x04
#define SHISENSHO_RANK_TYPE_YEARLY				0x08

typedef struct __tagShisenshoRank
{
	unsigned char chType;
	unsigned char chIndex;
	int	uiScore;
	int 			timestamp;
	USERID			userid;
	char szUserName[DJGAME_USERNAME_LEN+2]; //服务器发送时,长度等于具体用户名的长度
}ShisenshoRank,*PShisenshoRank;
#endif
