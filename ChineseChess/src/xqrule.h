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

//Please visit http://www.bluedj.com for more infomation about us.
//Contact us at ggwizard@gmail.com or darkdong@gmail.com.
//****************************************************************************/

#ifndef __DJGAME_XIANGQI_RULE_H__
#define __DJGAME_XIANGQI_RULE_H__

#define DJGAME_XIANGQI_RED			0x00
#define DJGAME_XIANGQI_BLACK		0x08

#define XIANGQI_CHIP_ZU				1
#define XIANGQI_CHIP_SHI			2
#define XIANGQI_CHIP_XIANG			3
#define XIANGQI_CHIP_PAO			4
#define XIANGQI_CHIP_MA				5
#define XIANGQI_CHIP_JU				6
#define XIANGQI_CHIP_JIANG			7

#define XIANGQI_RULE_TIMER_FASTEST					10

#define XIANGQI_RULEMASK_BASETIME					0x01
#define XIANGQI_RULEMASK_TIMEAFTERBASETIME			0x02
#define XIANGQI_RULEMASK_TIMES						0x04
#define XIANGQI_RULEMASK_TIMEAFTERTIMEOUT			0x08
#define XIANGQI_RULEMASK_RANG						0x10

typedef struct __tabXiangqiRule
{
	unsigned char chRuleMask;
	unsigned char chRangs[3];		//最多让三个子
	unsigned short shBaseTime;		//基本时间
	unsigned char chTimeAfterBaseTime;	//基本时间用光后每步的时间
	unsigned char chTimes;		//基本时间用光后,允许的超时次数
	unsigned char chTimeAfterTimeout;	//基本时间用光后,每次超时的最长时间
}XiangqiRule,*PXiangqiRule;

typedef struct __tagXiangQiBoard
{
	unsigned char node[50];
}XiangQiBoard,*PXiangQiBoard;

#define XIANGQI_MAX_X							9
#define XIANGQI_MAX_Y							10

#define XIANGQI_RULE_SUCCESS					0
#define XIANGQI_RULE_SUCCESS_WIN				1

#define XIANGQI_RULE_FAILED						0x80

#define XIANGQI_SUCCESS(__status)				(!((__status)&0x80))

#define XIANGQI_RULE_GETNODEID(__x,__y)			((unsigned char)(((__y)-1)*XIANGQI_MAX_X+(__x)))
#define XIANGQI_RULE_GETX(__nodeid)				((unsigned char)(((__nodeid-1)%XIANGQI_MAX_X)+1))
#define XIANGQI_RULE_GETY(__nodeid)				((unsigned char)(((__nodeid-1)/XIANGQI_MAX_X)+1))
#define XIANGQI_GETOWNER(__chip)				((__chip)&0x08)

unsigned char XiangQi_ZuMove(PXiangQiBoard pboard,unsigned char owner,unsigned char sx,unsigned char sy,unsigned char ex,unsigned char ey,unsigned char chEndChip);
unsigned char XiangQi_ShiMove(PXiangQiBoard pboard,unsigned char owner,unsigned char sx,unsigned char sy,unsigned char ex,unsigned char ey,unsigned char chEndChip);
unsigned char XiangQi_XiangMove(PXiangQiBoard pboard,unsigned char owner,unsigned char sx,unsigned char sy,unsigned char ex,unsigned char ey,unsigned char chEndChip);
unsigned char XiangQi_PaoMove(PXiangQiBoard pboard,unsigned char owner,unsigned char sx,unsigned char sy,unsigned char ex,unsigned char ey,unsigned char chEndChip);
unsigned char XiangQi_MaMove(PXiangQiBoard pboard,unsigned char owner,unsigned char sx,unsigned char sy,unsigned char ex,unsigned char ey,unsigned char chEndChip);
unsigned char XiangQi_JuMove(PXiangQiBoard pboard,unsigned char owner,unsigned char sx,unsigned char sy,unsigned char ex,unsigned char ey,unsigned char chEndChip);
unsigned char XiangQi_JiangMove(PXiangQiBoard pboard,unsigned char owner,unsigned char sx,unsigned char sy,unsigned char ex,unsigned char ey,unsigned char chEndChip);

bool XiangQi_JiangIsMovable(PXiangQiBoard pboard,unsigned char owner);
bool XiangQi_IsMovable(PXiangQiBoard pboard,unsigned char owner);

void InitBoard(PXiangQiBoard pboard);
unsigned char XiangQi_SetNodeByID(PXiangQiBoard pboard,unsigned char chNodeID,unsigned char chip,unsigned char owner);
unsigned char XiangQi_GetNodeByID(PXiangQiBoard pboard,unsigned char chNodeID);
unsigned char XiangQi_SetNode(PXiangQiBoard pboard,unsigned char x,unsigned char y,unsigned char chip,unsigned char owner);
unsigned char XiangQi_GetNode(PXiangQiBoard pboard,unsigned char x,unsigned char y);
unsigned char XiangQi_Move(PXiangQiBoard pboard,unsigned char owner,unsigned char chStartNode,unsigned char chEndNode);


unsigned char XiangQi_KillCommander(PXiangQiBoard pboard,unsigned char owner,unsigned char *chEnd);

#endif
