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

#include <iostream>

#include "xqrule.h"

unsigned char XiangQi_SetNodeByID(PXiangQiBoard pboard,unsigned char chNodeID,unsigned char chip,unsigned char owner)
{
	if(chNodeID > XIANGQI_MAX_X*XIANGQI_MAX_Y || chNodeID == 0)
		return XIANGQI_RULE_SUCCESS;
	
	unsigned char chByteIndex,chChip=(chip&0x07)|owner,chMask;
	chByteIndex = chNodeID>>1;
	if(chNodeID%2 == 1)
	{
		chMask = 0x0F;
		chChip <<= 4;
	}else
	{
		chMask = 0xF0;
	}
	pboard->node[chByteIndex] &= chMask;
	pboard->node[chByteIndex] |= chChip;
	return XIANGQI_RULE_SUCCESS;
}

unsigned char XiangQi_GetNodeByID(PXiangQiBoard pboard,unsigned char chNodeID)
{
	unsigned char chByteIndex,chChip;
	chByteIndex = chNodeID>>1;
	chChip = pboard->node[chByteIndex];
	if(chNodeID%2 == 1)
	{
		chChip >>= 4;
	}else
	{
		chChip &= 0x0F;
	}
	return chChip;
}
	
unsigned char XiangQi_SetNode(PXiangQiBoard pboard,unsigned char x,unsigned char y,unsigned char chip,unsigned char owner)
{
	if(x> XIANGQI_MAX_X || x==0 || y>XIANGQI_MAX_Y || y==0)
		return XIANGQI_RULE_SUCCESS;
	return XiangQi_SetNodeByID(pboard,XIANGQI_RULE_GETNODEID(x,y),chip,owner);
}

unsigned char XiangQi_GetNode(PXiangQiBoard pboard,unsigned char x,unsigned char y)
{
	if(x> XIANGQI_MAX_X || x==0 || y>XIANGQI_MAX_Y || y==0)
		return 0;
	return XiangQi_GetNodeByID(pboard,XIANGQI_RULE_GETNODEID(x,y));
}

unsigned char XiangQi_KillCommander(PXiangQiBoard pboard,unsigned char owner,unsigned char *chEnd)
{
	unsigned char chCom = (owner==DJGAME_XIANGQI_RED)?DJGAME_XIANGQI_BLACK:DJGAME_XIANGQI_RED;
	
	bool bFind=false;
	unsigned char i=8,x,y,chChip,chJiangPos;
	if(chCom == DJGAME_XIANGQI_RED)
	{
		i = 1;
	}
	//先找到老将的位置
	for(x=4;x<=6;x++)
	{
		for(y=i;y<=i+2;y++)
		{
			chChip = XiangQi_GetNode(pboard,x,y);
			if(chChip == (XIANGQI_CHIP_JIANG | chCom) )
			{
				bFind = true;
				break;
			}
		}
		if(bFind)
			break;
	}
	//检查老将是否能吃子求变
	if(bFind)
	{
		chJiangPos = XIANGQI_RULE_GETNODEID(x,y);
		for(x=1;x<=XIANGQI_MAX_X;x++)
		{
			for(y=1;y<=XIANGQI_MAX_Y;y++)
			{
				chChip = XiangQi_GetNode(pboard,x,y);
				if(chChip !=0 &&  XIANGQI_GETOWNER(chChip) == owner )
				{
					if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(x,y),chJiangPos) ) )
					{
						*chEnd = chJiangPos;
						return XIANGQI_RULE_GETNODEID(x,y);
					}
				}
			}
		}
	}
	*chEnd = 0;
	return 0;
}

unsigned char XiangQi_Move(PXiangQiBoard pboard,unsigned char owner,unsigned char chStartNode,unsigned char chEndNode)
{
	if(chStartNode == chEndNode || 
				chEndNode > XIANGQI_MAX_X*XIANGQI_MAX_Y || chEndNode == 0 ||
				chStartNode > XIANGQI_MAX_X*XIANGQI_MAX_Y || chStartNode == 0)
		return XIANGQI_RULE_FAILED;
	unsigned char sx,sy,ex,ey;
	sx = XIANGQI_RULE_GETX(chStartNode);
	sy = XIANGQI_RULE_GETY(chStartNode);
	
	ex = XIANGQI_RULE_GETX(chEndNode);
	ey = XIANGQI_RULE_GETY(chEndNode);
//	DEBUG_OUTPUT(("Move : chStartNode=%d chEndNode=%d sx=%d,sy=%d,ex=%d,ey=%d\n",chStartNode,chEndNode,sx,sy,ex,ey));

	if(sx > 9 || sx<1 || ex>9 || ex<1 || sy> 10 || sy<1 || ey>10 || ey<1)
	{
	//	DEBUG_OUTPUT(("Error\n"));
		return XIANGQI_RULE_FAILED;
	}
	
	unsigned char chChip = XiangQi_GetNodeByID(pboard,chStartNode);
	if(chChip == 0)
		return XIANGQI_RULE_FAILED;
	if( XIANGQI_GETOWNER(chChip) != owner)
		return XIANGQI_RULE_FAILED;
	unsigned char chEndChip = XiangQi_GetNodeByID(pboard,chEndNode);
	if(chEndChip != 0 && XIANGQI_GETOWNER(chEndChip) == owner)
		return XIANGQI_RULE_FAILED;

	
	unsigned char chRet = XIANGQI_RULE_SUCCESS;
	switch(chChip & 0x07)
	{
		case XIANGQI_CHIP_ZU :
			chRet = XiangQi_ZuMove(pboard,owner,sx,sy,ex,ey,chEndChip);
			break;
		case XIANGQI_CHIP_SHI :
			chRet = XiangQi_ShiMove(pboard,owner,sx,sy,ex,ey,chEndChip);
			break;
		case XIANGQI_CHIP_XIANG :
			chRet = XiangQi_XiangMove(pboard,owner,sx,sy,ex,ey,chEndChip);
			break;
		case XIANGQI_CHIP_PAO :
			chRet =XiangQi_PaoMove(pboard,owner,sx,sy,ex,ey,chEndChip);
			break;
		case XIANGQI_CHIP_MA :
			chRet =XiangQi_MaMove(pboard,owner,sx,sy,ex,ey,chEndChip);
			break;
		case XIANGQI_CHIP_JU :
			chRet =XiangQi_JuMove(pboard,owner,sx,sy,ex,ey,chEndChip);
			break;
		case XIANGQI_CHIP_JIANG :
			chRet =XiangQi_JiangMove(pboard,owner,sx,sy,ex,ey,chEndChip);
			break;
		default :
			return XIANGQI_RULE_FAILED;
	}

	if(XIANGQI_SUCCESS(chRet) &&  (chEndChip & 0x07) ==XIANGQI_CHIP_JIANG)
	{
		return XIANGQI_RULE_SUCCESS_WIN;
	}
	return chRet;
}

unsigned char XiangQi_ZuMove(PXiangQiBoard pboard,unsigned char owner,unsigned char sx,unsigned char sy,unsigned char ex,unsigned char ey,unsigned char chEndChip)
{
	if(owner == DJGAME_XIANGQI_RED)
	{
		if(ey < sy)
			return XIANGQI_RULE_FAILED;
		if(ey == sy && sy<6)
			return XIANGQI_RULE_FAILED;
		if(abs(ex-sx)+abs(ey-sy) != 1)
			return XIANGQI_RULE_FAILED;
		return XIANGQI_RULE_SUCCESS;
	}else if(owner == DJGAME_XIANGQI_BLACK)
	{
		if(ey > sy)
			return XIANGQI_RULE_FAILED;
		if(ey == sy && sy>5)
			return XIANGQI_RULE_FAILED;
		if(abs(ex-sx)+abs(ey-sy) != 1)
			return XIANGQI_RULE_FAILED;
		return XIANGQI_RULE_SUCCESS;
	}
	return XIANGQI_RULE_FAILED;
}

unsigned char XiangQi_ShiMove(PXiangQiBoard pboard,unsigned char owner,unsigned char sx,unsigned char sy,unsigned char ex,unsigned char ey,unsigned char chEndChip)
{
//	if(sx > 9 || sx<1 || ex>9 || ex<1 || sy> 10 || sy<1 || ey>10 || ey<1)
//		return XIANGQI_RULE_FAILED;
	if(owner == DJGAME_XIANGQI_RED)
	{
		if(ex <4 || ex > 6 || sx < 4 || sx >6 || sy>3 || ey>3)
			return XIANGQI_RULE_FAILED;
		if(abs(ex-sx) != 1 || abs(ey-sy) != 1)
			return XIANGQI_RULE_FAILED;
		return XIANGQI_RULE_SUCCESS;
	}else if(owner == DJGAME_XIANGQI_BLACK)
	{
		if(ex <4 || ex > 6 || sx < 4 || sx >6 || sy<8 || ey<8)
			return XIANGQI_RULE_FAILED;
		if(abs(ex-sx) != 1 || abs(ey-sy) != 1)
			return XIANGQI_RULE_FAILED;
		return XIANGQI_RULE_SUCCESS;
	}
	return XIANGQI_RULE_FAILED;
}

unsigned char XiangQi_XiangMove(PXiangQiBoard pboard,unsigned char owner,unsigned char sx,unsigned char sy,unsigned char ex,unsigned char ey,unsigned char chEndChip)
{
//	if(sx > 9 || sx<1 || ex>9 || ex<1 || sy> 10 || sy<1 || ey>10 || ey<1)
//		return XIANGQI_RULE_FAILED;
	if(owner == DJGAME_XIANGQI_RED)
	{
		if(ey > 5 || sy > 5)
			return XIANGQI_RULE_FAILED;
	}else if(owner == DJGAME_XIANGQI_BLACK)
	{
		if(ey < 6 || sy < 6)
			return XIANGQI_RULE_FAILED;
	}else
		return XIANGQI_RULE_FAILED;
	if(abs(ex-sx) != 2 || abs(ey-sy) != 2)
		return XIANGQI_RULE_FAILED;
	unsigned char xyx,xyy;
	if(ex > sx)
		xyx = sx+1;
	else
		xyx = sx-1;
	if(ey > sy)
		xyy = sy + 1;
	else
		xyy = sy - 1;
	if(XiangQi_GetNode(pboard,xyx,xyy) != 0)
		return XIANGQI_RULE_FAILED;
	return XIANGQI_RULE_SUCCESS;
}

unsigned char XiangQi_PaoMove(PXiangQiBoard pboard,unsigned char owner,unsigned char sx,unsigned char sy,unsigned char ex,unsigned char ey,unsigned char chEndChip)
{
//	if(sx > 9 || sx<1 || ex>9 || ex<1 || sy> 10 || sy<1 || ey>10 || ey<1)
//		return XIANGQI_RULE_FAILED;
	if(owner != DJGAME_XIANGQI_RED && owner != DJGAME_XIANGQI_BLACK)
		return XIANGQI_RULE_FAILED;
	unsigned char  x,y,chChip,chips=0;
	int d=0;
	if(ex == sx)
	{
		if(ey < sy)
			d= -1;
		else if(ey > sy)
			d = 1;			
		y = sy;
		while(y != ey)
		{
			y += d;
			if(y == ey)
				break;
			chChip = XiangQi_GetNode(pboard,ex,y);
			if(chChip != 0)
				chips ++;
		}
		if(chEndChip == 0)
		{
			if(chips != 0)
				return XIANGQI_RULE_FAILED;
		}else
		{
			if(chips != 1)
				return XIANGQI_RULE_FAILED;
		}
		return XIANGQI_RULE_SUCCESS;
	}
	if(ey == sy)
	{
		if(ex < sx)
			d= -1;
		else if(ex > sx)
			d = 1;			
		x = sx;
		while(x != ex)
		{
			x += d;
			if(x == ex)
				break;
			chChip = XiangQi_GetNode(pboard,x,ey);
			if(chChip != 0)
				chips ++;
		}
		if(chEndChip == 0)
		{
			if(chips != 0)
				return XIANGQI_RULE_FAILED;
		}else
		{
			if(chips != 1)
				return XIANGQI_RULE_FAILED;
		}
		return XIANGQI_RULE_SUCCESS;
	}
	return XIANGQI_RULE_FAILED;
	
}

unsigned char XiangQi_MaMove(PXiangQiBoard pboard,unsigned char owner,unsigned char sx,unsigned char sy,unsigned char ex,unsigned char ey,unsigned char chEndChip)
{
	if(sx > 9 || sx<1 || ex>9 || ex<1 || sy> 10 || sy<1 || ey>10 || ey<1)
		return XIANGQI_RULE_FAILED;
	if(owner != DJGAME_XIANGQI_RED && owner != DJGAME_XIANGQI_BLACK)
		return XIANGQI_RULE_FAILED;
	unsigned char dx,dy;
	dx = abs(sx-ex);
	dy = abs(sy-ey);
	if ( (dx == 2 && dy == 1 ) || (dy == 2 && dx == 1))
	{//检查马腿
		unsigned char x,y;
		if(dx == 2)
		{
			y = sy;
			if(ex<sx)
				x = sx-1;
			else
				x = sx+1;
		}else
		{
			x = sx;
			if(ey<sy)
				y = sy-1;
			else
				y = sy + 1;
		}
	//	DEBUG_OUTPUT(("MaPos=%d,%d MaTui=%d,%d",sx,sy,x,y));
		if(XiangQi_GetNode(pboard,x,y) != 0)
		{
	//		DEBUG_OUTPUT(("bie ma tui"));
			return XIANGQI_RULE_FAILED;
		}
	//	DEBUG_OUTPUT(("\n"));
		return XIANGQI_RULE_SUCCESS;
	}
	return XIANGQI_RULE_FAILED;
}

unsigned char XiangQi_JuMove(PXiangQiBoard pboard,unsigned char owner,unsigned char sx,unsigned char sy,unsigned char ex,unsigned char ey,unsigned char chEndChip)
{
//	if(sx > 9 || sx<1 || ex>9 || ex<1 || sy> 10 || sy<1 || ey>10 || ey<1)
//		return XIANGQI_RULE_FAILED;
	
	if(owner != DJGAME_XIANGQI_RED && owner != DJGAME_XIANGQI_BLACK)
		return XIANGQI_RULE_FAILED;
	unsigned char  x,y,chChip;
	int d=0;
	if(ex == sx)
	{
		if(ey < sy)
			d= -1;
		else if(ey > sy)
			d = 1;			
		y = sy;
		while(y != ey)
		{
			y += d;
			if(y == ey)
				break;
			chChip = XiangQi_GetNode(pboard,ex,y);
			if(chChip != 0)
				break;
		}
		if(y != ey)
		{
			return XIANGQI_RULE_FAILED;
		}
		return XIANGQI_RULE_SUCCESS;
	}
	if(ey == sy)
	{
		if(ex < sx)
			d= -1;
		else if(ex > sx)
			d = 1;			
		x = sx;
		while(x != ex)
		{
			x += d;
			if(x == ex)
				break;
			chChip = XiangQi_GetNode(pboard,x,ey);
			if(chChip != 0)
				break;
		}
		if(x != ex)
		{
			return XIANGQI_RULE_FAILED;
		}
		return XIANGQI_RULE_SUCCESS;
	}
	return XIANGQI_RULE_FAILED;
	
}

unsigned char XiangQi_JiangMove(PXiangQiBoard pboard,unsigned char owner,unsigned char sx,unsigned char sy,unsigned char ex,unsigned char ey,unsigned char chEndChip)
{
	if(abs(ey-sy)>1)
	{//是否是老将照面
		if(sx != ex)
			return XIANGQI_RULE_FAILED;
		unsigned char chChip = XiangQi_GetNode(pboard,ex,ey);
		if(chChip != 0 && XIANGQI_GETOWNER(chChip) != owner && (chChip&0x07) == XIANGQI_CHIP_JIANG)
		{
			int i;
			if(ey>sy)
				i = 1;
			else
				i = -1;
			sy+= i;
			while(sy != ey)
			{
				chChip = XiangQi_GetNode(pboard,ex,sy);
				if(chChip != 0)
					return XIANGQI_RULE_FAILED;
				sy += i;
			}
			return XIANGQI_RULE_SUCCESS;
		}
		return XIANGQI_RULE_FAILED;
	}
		
	if(owner == DJGAME_XIANGQI_RED)
	{
		if(ex <4 || ex > 6 || sx < 4 || sx >6 || sy>3 || ey>3 || sy< 1 || ey <1)
			return XIANGQI_RULE_FAILED;
		if(abs(ex-sx) + abs(ey-sy) != 1)
			return XIANGQI_RULE_FAILED;
		return XIANGQI_RULE_SUCCESS;
	}else if(owner == DJGAME_XIANGQI_BLACK)
	{
		if(ex <4 || ex > 6 || sx < 4 || sx >6 || sy<8 || ey<8 || sy>10 || ey >10)
			return XIANGQI_RULE_FAILED;
		if(abs(ex-sx) + abs(ey-sy) != 1)
			return XIANGQI_RULE_FAILED;
		return XIANGQI_RULE_SUCCESS;
	}
	return XIANGQI_RULE_FAILED;
}

bool XiangQi_JiangIsMovable(PXiangQiBoard pboard,unsigned char owner)
{
	bool bFind=false;
	unsigned char i=8,x,y,j,chChip,chJiangPos;
	if(owner == DJGAME_XIANGQI_RED)
	{
		i = 1;
	}
	//先找到老将的位置
	for(x=4;x<=6;x++)
	{
		for(y=i;y<=i+2;y++)
		{
			chChip = XiangQi_GetNode(pboard,x,y);
			if(chChip == (XIANGQI_CHIP_JIANG | owner) )
			{
				bFind = true;
				break;
			}
		}
		if(bFind)
			break;
	}
	//检查老将是否能吃子求变
	if(bFind)
	{
	//	DEBUG_OUTPUT(("jiang pos = %d,%d\n",x,y));
		if(y+1 <= i+2)
		{
			chChip = XiangQi_GetNode(pboard,x,y+1);
			if(chChip != 0 && XIANGQI_GETOWNER(chChip) != owner )
			{
			//	DEBUG_OUTPUT(("ret 1\n"));
				return true;
			}
		}
		if(y-1 >= i)
		{
			chChip = XiangQi_GetNode(pboard,x,y-1);
			if(chChip != 0 && XIANGQI_GETOWNER(chChip) != owner )
			{
			//	DEBUG_OUTPUT(("ret 2\n"));
				return true;
			}
		}
		if(x+1 <= 6)
		{
			chChip = XiangQi_GetNode(pboard,x+1,y);
			if(chChip != 0 && XIANGQI_GETOWNER(chChip) != owner )
			{
			//	DEBUG_OUTPUT(("ret 3\n"));
				return true;
			}
		}
		if(x-1 >= 4)
		{
			chChip = XiangQi_GetNode(pboard,x-1,y);
			if(chChip != 0 && XIANGQI_GETOWNER(chChip) != owner )
			{
			//	DEBUG_OUTPUT(("ret 4\n"));
				return true;
			}
		}
	}else
	{
	//	DEBUG_OUTPUT(("Error : Can't find the JIANG owner=%02x\n",owner));
		return true;
	}
	//检查对方是否在长将
	chJiangPos = XIANGQI_RULE_GETNODEID(x,y);
	for(i = 1;i<=9;i++)
	{
		for(j=1;j<=10;j++)
		{
			chChip = XiangQi_GetNode(pboard,i,j);
			if(chChip != 0 && XIANGQI_GETOWNER(chChip) != owner ) 
			{
				if(XIANGQI_SUCCESS(XiangQi_Move(pboard,XIANGQI_GETOWNER(chChip),XIANGQI_RULE_GETNODEID(i,j),chJiangPos) ) )
					return false;
			}
		}
	}
	//			DEBUG_OUTPUT(("ret 5\n"));
	return true;
}

bool XiangQi_IsMovable(PXiangQiBoard pboard,unsigned char owner)
{
	
	int i,j;
	unsigned char chChip;
	for(i = 1;i<=9;i++)
	{
		for(j=1;j<=10;j++)
		{
			chChip = XiangQi_GetNode(pboard,i,j);
			if(chChip != 0 && (chChip&0x08) == owner )
			{
				switch(chChip & 0x07)
				{
					case XIANGQI_CHIP_ZU:
					{
						return true;
					}
					case XIANGQI_CHIP_SHI:
					{
						if(i>1 && j> 1)
							if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i-1,j-1)) ) )
								return true;
						if(j>1)
							if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i+1,j-1)) ) )
								return true;
						if(i>1)
							if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i-1,j+1)) ) )
								return true;
						if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i+1,j+1)) ) )
								return true;
						break;
					}
					case XIANGQI_CHIP_XIANG:
					{
						if(i>2 && j> 2)
							if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i-2,j-2)) ) )
								return true;
						if(j>2)
							if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i+2,j-2)) ) )
								return true;
						if(i>2)
							if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i-2,j+2)) ) )
								return true;
						if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i+2,j+2)) ) )
								return true;
						break;
					}
					case XIANGQI_CHIP_PAO:
					{
						if(i>1)
							if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i-1,j)) ) )
								return true;
						if(j>1)
							if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i,j-1)) ) )
								return true;
						if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i,j+1)) ) )
								return true;
						if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i+1,j)) ) )
								return true;
						break;
					}
					case XIANGQI_CHIP_MA:
					{
						if(i>2)
						{
							if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i-2,j+1)) ) )
								return true;
							if(j>1)
								if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i-2,j-1)) ) )
									return true;
						}
						if(j>2)
						{
							if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i+1,j-2)) ) )
								return true;
							if(i>1)
								if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i-1,j-2)) ) )
									return true;
						}
						if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i+2,j+1)) ) )
								return true;
						if(j>1)
							if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i+2,j-1)) ) )
								return true;
							
						if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i+1,j+2)) ) )
								return true;
						if(i>1)
							if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i-1,j+2)) ) )
								return true;
						break;
					}
					case XIANGQI_CHIP_JU:
					{
						return true;
					}
					case XIANGQI_CHIP_JIANG:
					{
						if(i>1)
							if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i-1,j)) ) )
								return true;
						if(j>1)
							if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i,j-1)) ) )
								return true;
						if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i,j+1)) ) )
								return true;
						if(XIANGQI_SUCCESS(XiangQi_Move(pboard,owner,XIANGQI_RULE_GETNODEID(i,j),XIANGQI_RULE_GETNODEID(i+1,j)) ) )
								return true;
						break;
					}
				}
			}
		}
	}
	return false;
	
}

void InitBoard(PXiangQiBoard pboard)
{
	memset(pboard,0,sizeof(XiangQiBoard));
	//摆放红棋
	XiangQi_SetNode(pboard,1,1,XIANGQI_CHIP_JU,DJGAME_XIANGQI_RED);
	XiangQi_SetNode(pboard,2,1,XIANGQI_CHIP_MA,DJGAME_XIANGQI_RED);
	XiangQi_SetNode(pboard,3,1,XIANGQI_CHIP_XIANG,DJGAME_XIANGQI_RED);
	XiangQi_SetNode(pboard,4,1,XIANGQI_CHIP_SHI,DJGAME_XIANGQI_RED);
	XiangQi_SetNode(pboard,5,1,XIANGQI_CHIP_JIANG,DJGAME_XIANGQI_RED);
	XiangQi_SetNode(pboard,6,1,XIANGQI_CHIP_SHI,DJGAME_XIANGQI_RED);
	XiangQi_SetNode(pboard,7,1,XIANGQI_CHIP_XIANG,DJGAME_XIANGQI_RED);
	XiangQi_SetNode(pboard,8,1,XIANGQI_CHIP_MA,DJGAME_XIANGQI_RED);
	XiangQi_SetNode(pboard,9,1,XIANGQI_CHIP_JU,DJGAME_XIANGQI_RED);
	
	XiangQi_SetNode(pboard,2,3,XIANGQI_CHIP_PAO,DJGAME_XIANGQI_RED);
	XiangQi_SetNode(pboard,8,3,XIANGQI_CHIP_PAO,DJGAME_XIANGQI_RED);
	
	XiangQi_SetNode(pboard,1,4,XIANGQI_CHIP_ZU,DJGAME_XIANGQI_RED);
	XiangQi_SetNode(pboard,3,4,XIANGQI_CHIP_ZU,DJGAME_XIANGQI_RED);
	XiangQi_SetNode(pboard,5,4,XIANGQI_CHIP_ZU,DJGAME_XIANGQI_RED);
	XiangQi_SetNode(pboard,7,4,XIANGQI_CHIP_ZU,DJGAME_XIANGQI_RED);
	XiangQi_SetNode(pboard,9,4,XIANGQI_CHIP_ZU,DJGAME_XIANGQI_RED);
	
	//摆放黑棋
	XiangQi_SetNode(pboard,1,10,XIANGQI_CHIP_JU,DJGAME_XIANGQI_BLACK);
	XiangQi_SetNode(pboard,2,10,XIANGQI_CHIP_MA,DJGAME_XIANGQI_BLACK);
	XiangQi_SetNode(pboard,3,10,XIANGQI_CHIP_XIANG,DJGAME_XIANGQI_BLACK);
	XiangQi_SetNode(pboard,4,10,XIANGQI_CHIP_SHI,DJGAME_XIANGQI_BLACK);
	XiangQi_SetNode(pboard,5,10,XIANGQI_CHIP_JIANG,DJGAME_XIANGQI_BLACK);
	XiangQi_SetNode(pboard,6,10,XIANGQI_CHIP_SHI,DJGAME_XIANGQI_BLACK);
	XiangQi_SetNode(pboard,7,10,XIANGQI_CHIP_XIANG,DJGAME_XIANGQI_BLACK);
	XiangQi_SetNode(pboard,8,10,XIANGQI_CHIP_MA,DJGAME_XIANGQI_BLACK);
	XiangQi_SetNode(pboard,9,10,XIANGQI_CHIP_JU,DJGAME_XIANGQI_BLACK);
	
	XiangQi_SetNode(pboard,2,8,XIANGQI_CHIP_PAO,DJGAME_XIANGQI_BLACK);
	XiangQi_SetNode(pboard,8,8,XIANGQI_CHIP_PAO,DJGAME_XIANGQI_BLACK);
	
	XiangQi_SetNode(pboard,1,7,XIANGQI_CHIP_ZU,DJGAME_XIANGQI_BLACK);
	XiangQi_SetNode(pboard,3,7,XIANGQI_CHIP_ZU,DJGAME_XIANGQI_BLACK);
	XiangQi_SetNode(pboard,5,7,XIANGQI_CHIP_ZU,DJGAME_XIANGQI_BLACK);
	XiangQi_SetNode(pboard,7,7,XIANGQI_CHIP_ZU,DJGAME_XIANGQI_BLACK);
	XiangQi_SetNode(pboard,9,7,XIANGQI_CHIP_ZU,DJGAME_XIANGQI_BLACK);
}
