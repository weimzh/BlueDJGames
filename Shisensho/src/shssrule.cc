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

#include <iostream>
#include "shssrule.h"
//#include "shssprofile.h"
//#include "jdebug.h"

/*
连连看的map就是一个字节块,每个位置一个字节,但建议使用
ShisenshoRule_SetPoint 来构造, 一个map有两个重要参数 width,height
使用 ShisenshoRule_GetPoint 可以得到一个位置的地址, 例如
p = ShisenshoRule_GetPoint(....)
那么*p即可得到该位置的图片索引(如果是0,则表示该位置为空)

map内存块的尺寸应该等于 width*height+8,并初始化为0
所有虚拟点(外围一圈)的地址都被影射在最后8个字节中

ShisenshoRule_SearchPath 函数用来检查2个点之间是否可以连接,
参数 [*pxs,*pys]是第一个点,[*(pxs+1),*(pys+1)]是第二个点,
pxs , pys 应该各自指向5个字节,因为如果成功(返回true),则函数要在其中返回路径描述
(起点,第一个拐点,第二个拐点,终点,0)

客户端可以将 ShisenshoRule_SearchPath 成功后返回的 路径描述 发送给服务器
服务器使用 ShisenshoRule_Check 来核查

游戏开始时需要初始化map, ShisenshoRule_InitializeMap
如果是单机游戏,那么直接调用即可,服务器为了提高效率可以事先创见好所有牌的集合 : ShisenshoRule_CreateAllCards

当玩家使用生命时,调用 ShisenshoRule_ResetMap 即可对残余牌进行重新排列
(在完全相同的格局下,交换牌的位置)

*/
bool ShisenshoRule_Check(unsigned char *pmap,unsigned char chWidth,unsigned char chHeight,
							unsigned char *pxs,unsigned char *pys)
{//核查[路径描述]是否合法
	unsigned char x,y,chCard,*p,step,sx,sy;
	p = ShisenshoRule_GetPoint(pmap,chWidth,chHeight,*pxs,*pys);
	if(p == NULL || *p == 0)
		return false;
	chCard = *p;
	sx = *pxs;
	sy = *pys;
	for(step=0;step<4;step++)
	{
		x = *pxs;
		y = *pys;
		pxs++;
		pys++;
		if(*pxs == 0 && *pys == 0)
			return false;
		p = ShisenshoRule_GetPoint(pmap,chWidth,chHeight,*pxs,*pys);
		if(p == NULL)
			return false;
		if(!ShisenshoRule_CheckPipe(pmap,chWidth,chHeight,x,y,*pxs,*pys))
			return false;
		if(*p == chCard)
		{
			*(pxs+1) = 0;
			*(pys+1) = 0;
			return true;
		}
		if(*p != 0)
			return false;
	}
	return false;
}

void ShisenshoRule_DeleteCard(unsigned char *pmap,unsigned char chWidth,unsigned char chHeight,
								unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2,
								unsigned char chAdjustOri,
								void *pcallback, 
								void (*SHSSRule_DelCallBack)(void * pcallback,unsigned char *pmap,unsigned char chWidth,unsigned char chHeight,unsigned char x,unsigned char y,char dx,char dy))
{//SHSSRule_DelCallBack 是客户端的回调函数(服务器端无需回调,NULL即可),当一张牌被删除后,周围牌的位置可能会发生变化,
	//(chAdjustOri决定了周围牌的调整方式)
	//因此用户界面需要修正,完成这个功能的函数被当做参数传输进来,
	//回调函数的参数: [x,y]表示位置发生改变的牌的原始位置,[dx,dy]表示位置的增量,例如[0,1]或者[-1,0]
	//[dx,dy]=[0,0],说明这张牌被删除
	bool bQuit1 = ShisenshoRule_SetPoint(pmap,chWidth,chHeight,x1,y1,0);
	bool bQuit2 = ShisenshoRule_SetPoint(pmap,chWidth,chHeight,x2,y2,0);
	if(!bQuit1 && !bQuit2)
		return;
	if(SHSSRule_DelCallBack != NULL)
	{
		SHSSRule_DelCallBack(pcallback,pmap,chWidth,chHeight,x1,y1,0,0);
		SHSSRule_DelCallBack(pcallback,pmap,chWidth,chHeight,x2,y2,0,0);
	}
	if(chAdjustOri == SHISENSHO_ADJUST_NULL || chAdjustOri > SHISENSHO_ADJUST_MAX)
		return;
	char dx=0,dy=0;
	switch(chAdjustOri)
	{
		case SHISENSHO_ADJUST_BOTTOM :
		{
			dx =0; dy = 1;
			break;
		}
		case SHISENSHO_ADJUST_TOP :
		{
			dx =0; dy = -1;
			break;
		}
		case SHISENSHO_ADJUST_LEFT :
		{
			dx =-1; dy = 0;
			break;
		}
		case SHISENSHO_ADJUST_RIGHT :
		{
			dx =1; dy = 0;
			break;
		}
		default :
			return;
	}
	unsigned char *p;
	do
	{
		x1 += (-dx);
		y1 += (-dy);
		//printf("rule move : (%d,%d)->(%d,%d)\n",x1,y1,x1+dx,y1+dy);
		p = ShisenshoRule_GetPoint(pmap,chWidth,chHeight,x1,y1);
		if(p == NULL)
			return;
		ShisenshoRule_SetPoint(pmap,chWidth,chHeight,x1+dx,y1+dy,*p);
		ShisenshoRule_SetPoint(pmap,chWidth,chHeight,x1,y1,0);
		if(x1 == x2 && y1 == y2)
		{
			x2 += dx;
			y2 += dy;
		}
		if(SHSSRule_DelCallBack != NULL)
			SHSSRule_DelCallBack(pcallback,pmap,chWidth,chHeight,x1,y1,dx,dy);
	}while(x1+(-dx)>0 && x1+(-dx)<= chWidth && y1+(-dy)>0 && y1+(-dy) <= chHeight);
	
	do
	{
		x2 += (-dx);
		y2 += (-dy);
		p = ShisenshoRule_GetPoint(pmap,chWidth,chHeight,x2,y2);
		if(p == NULL)
			return;
		ShisenshoRule_SetPoint(pmap,chWidth,chHeight,x2+dx,y2+dy,*p);
		ShisenshoRule_SetPoint(pmap,chWidth,chHeight,x2,y2,0);
		if(SHSSRule_DelCallBack != NULL)
			SHSSRule_DelCallBack(pcallback,pmap,chWidth,chHeight,x2,y2,dx,dy);
	}while(x2+(-dx)>0 && x2+(-dx)<= chWidth && y2+(-dy)>0 && y2+(-dy) <= chHeight);
	
}

bool ShisenshoRule_IsNull(unsigned char *pmap,unsigned char chWidth,unsigned char chHeight)
{
	int i,j;
	unsigned char *p;
	for(i=1;i<=chWidth;i++)
	{
		for(j=1;j<=chHeight;j++)
		{
			p = ShisenshoRule_GetPoint(pmap,chWidth,chHeight,i,j);
			if(p != NULL && *p != 0)
				return false;
		}
	}
	return true;
}
//检查直线上是否有连通路(不检查2个端点)
bool ShisenshoRule_CheckPipe(unsigned char *pmap,unsigned char chWidth,unsigned char chHeight,
							unsigned char sx,unsigned char sy,unsigned char ex,unsigned char ey)
{
	if(sx != ex && sy != ey)
		return false;
	if(sx == ex && sy == ey)
		return false;
	
	unsigned char *ps,*pe;
	ps = ShisenshoRule_GetPoint(pmap,chWidth,chHeight,sx,sy);
	if(ps == NULL)
		return false;
	pe = ShisenshoRule_GetPoint(pmap,chWidth,chHeight,ex,ey);
	if(pe == NULL)
		return false;
	
	char dx=0,dy=0;
	if(sx != ex)
	{
		dx = (sx<ex)?1:-1;
	}
	if(sy != ey)
	{
		dy = (sy<ey)?1:-1;
	}
	do
	{
		sx += dx;
		sy += dy;
		if(sx == ex && sy == ey)
			break;
		ps = ShisenshoRule_GetPoint(pmap,chWidth,chHeight,sx,sy);
		if(ps == NULL)
			return false;
		if(*ps != 0)
			return false;
	}while(1);
	return true;
}

bool ShisenshoRule_SearchPath(unsigned char *pmap,unsigned char chWidth,unsigned char chHeight,
							unsigned char *pxs,unsigned char *pys)
{
	unsigned char x = *pxs, y=*pys;
	unsigned char ex = *(pxs+1), ey = *(pys+1);	
	unsigned char spx[100],spy[100],epx[100],epy[100],en=0,sn=0,*p,*ps,*pe;
	
	ps = ShisenshoRule_GetPoint(pmap,chWidth,chHeight,x,y);
	if(ps == NULL || *ps == 0)
		return false;
	pe = ShisenshoRule_GetPoint(pmap,chWidth,chHeight,ex,ey);
	if(pe == NULL || pe == ps || *pe != *ps)
		return false;
	//先找出起始点四周所有直线可到达的空位,如果某方向的空位以[ex,ey]为终点,说明2点可直连,直接返回true即可;
	//ShisenshoRule_SearchNull 返回值是 空位终点 的地址
	//如果空位到达边缘,则返回NULL
	p = ShisenshoRule_SearchNull(pmap,chWidth,chHeight,x,y,SHISENSHO_RULE_SEARCH_ORI_UP,&(spx[sn]),&(spy[sn]),&sn);
	if(p != NULL)
	{
		if(p == pe)
			return true;
	}
	p = ShisenshoRule_SearchNull(pmap,chWidth,chHeight,x,y,SHISENSHO_RULE_SEARCH_ORI_DOWN,&(spx[sn]),&(spy[sn]),&sn);
	if(p != NULL)
	{
		if(p == pe)
			return true;
	}
	p = ShisenshoRule_SearchNull(pmap,chWidth,chHeight,x,y,SHISENSHO_RULE_SEARCH_ORI_LEFT,&(spx[sn]),&(spy[sn]),&sn);
	if(p != NULL)
	{
		if(p == pe)
			return true;
	}
	p = ShisenshoRule_SearchNull(pmap,chWidth,chHeight,x,y,SHISENSHO_RULE_SEARCH_ORI_RIGHT,&(spx[sn]),&(spy[sn]),&sn);
	if(p != NULL)
	{
		if(p == pe)
			return true;
	}
	if(sn == 0)
		return false;
	ShisenshoRule_SearchNull(pmap,chWidth,chHeight,ex,ey,SHISENSHO_RULE_SEARCH_ORI_UP,&(epx[en]),&(epy[en]),&en);
	ShisenshoRule_SearchNull(pmap,chWidth,chHeight,ex,ey,SHISENSHO_RULE_SEARCH_ORI_DOWN,&(epx[en]),&(epy[en]),&en);
	ShisenshoRule_SearchNull(pmap,chWidth,chHeight,ex,ey,SHISENSHO_RULE_SEARCH_ORI_LEFT,&(epx[en]),&(epy[en]),&en);
	ShisenshoRule_SearchNull(pmap,chWidth,chHeight,ex,ey,SHISENSHO_RULE_SEARCH_ORI_RIGHT,&(epx[en]),&(epy[en]),&en);
	if(en == 0)
		return false;
	
	int i,j;
	for(i=0;i<sn;i++)
	{
		for(j=0;j<en;j++)
		{
			if(spx[i] == epx[j] && spy[i] == epy[j])
			{//经一个拐点 到达
				*(pxs+2) = *(pxs+1);
				*(pys+2) = *(pys+1);
				*(pxs+1) = spx[i];
				*(pys+1) = spy[i];
				*(pxs+3) = 0;
				*(pys+3) = 0;
				
				return true;
			}
			if(ShisenshoRule_CheckPipe(pmap,chWidth,chHeight,spx[i],spy[i],epx[j],epy[j]))
			{//经2个拐点 到达
				*(pxs+3) = *(pxs+1);
				*(pys+3) = *(pys+1);
				*(pxs+1) = spx[i];
				*(pys+1) = spy[i];
				*(pxs+2) = epx[j];
				*(pys+2) = epy[j];
				*(pxs+4) = 0;
				*(pys+4) = 0;
				return true;
			}
		}
	}
	
	return false;
}

unsigned char * ShisenshoRule_SearchNull(unsigned char *pmap,unsigned char chWidth,unsigned char chHeight,
							unsigned char x,unsigned char y ,unsigned char chOri,unsigned char *pxs,unsigned char *pys,unsigned char *pnumbers)
{
	char dx=0,dy=0;
	unsigned char *p;

	switch(chOri)
	{
		case SHISENSHO_RULE_SEARCH_ORI_UP :
			dy = -1;
			break;
		case SHISENSHO_RULE_SEARCH_ORI_DOWN :
			dy = 1;
			break;
		case SHISENSHO_RULE_SEARCH_ORI_LEFT :
			dx = -1;
			break;
		case SHISENSHO_RULE_SEARCH_ORI_RIGHT :
			dx = 1;
			break;
	}
	if(dx == 0 && dy == 0)
		return NULL;
	do
	{
		x += dx;
		y += dy;
		p = ShisenshoRule_GetPoint(pmap,chWidth,chHeight,x,y);
		if(p == NULL)
			return NULL;
		if(*p != 0)
			return p;
		*pxs = x;
		*pys = y;
		pxs++;
		pys++;
		(*pnumbers)++;
	}while(1);
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////
//从map中copy出指定行的数据(主要用于服务器发送当前状态,对于大的map,需要分包)
bool ShisenshoRule_CopyLines(unsigned char *pmap,unsigned char chWidth,unsigned char chHeight,
				unsigned char chStartLine,unsigned char chEndLine,unsigned char *pbuffer)
{
	if(chStartLine == 0 || chEndLine == 0 || chEndLine < chStartLine || chStartLine > chHeight)
		return false;
	unsigned short shSize = ((chEndLine-chStartLine)+1)*chWidth;
	unsigned char *ps = ShisenshoRule_GetPoint(pmap,chWidth,chHeight,chStartLine,1);
	if(ps == NULL)
		return false;
	memcpy(pbuffer,ps,shSize);
	return true;
}

//设置指定行的数据,(主要用于客户端收到服务器的ADJUST后重置map)
bool ShisenshoRule_SetLines(unsigned char *pmap,unsigned char chWidth,unsigned char chHeight,
				unsigned char chStartLine,unsigned char chEndLine,unsigned char *pbuffer)
{
	if(chStartLine == 0 || chEndLine == 0 || chEndLine < chStartLine || chStartLine > chHeight)
		return false;
	unsigned short shSize = ((chEndLine-chStartLine)+1)*chWidth;
	unsigned char *ps = ShisenshoRule_GetPoint(pmap,chWidth,chHeight,chStartLine,1);
	if(ps == NULL)
		return false;
	memcpy(ps,pbuffer,shSize);
	return true;
}

unsigned char * ShisenshoRule_GetPoint(unsigned char *pmap,unsigned char chWidth,unsigned char chHeight,
											unsigned char x,unsigned char y)
{
	if(pmap == NULL || ( (x<1 || x>chWidth) && (y<1 || y>chHeight)) )
		return NULL;
	if(x>chWidth +1 || y > chHeight+1)
		return NULL;
	if( x<1 || x>chWidth || y<1 || y>chHeight)
		return pmap+(chHeight*chWidth+1);
	return pmap+(((y-1)*chWidth)+(x-1));
}

bool ShisenshoRule_SetPoint(unsigned char *pmap,unsigned char chWidth,unsigned char chHeight,
											unsigned char x,unsigned char y,unsigned char chCard)
{
	if( x<1 || x>chWidth || y<1 || y>chHeight)
		return false;
	unsigned char *ps = ShisenshoRule_GetPoint(pmap,chWidth,chHeight,x,y);
	if(ps == NULL)
		return false;
	*ps = chCard;
	return true;
}

//////////////////////////////////////////////////////////////////////
//map初始化函数

void ShisenshoRule_CreateAllCards(unsigned char *pcardlist,unsigned short shMax)
{//此函数是为了生成所有牌的集合,这是为了服务器提高效率(使用时memcpy即可,无需每次生成)
	int i;
	if(shMax >SHISENSHO_MAX_MAP)
		shMax = SHISENSHO_MAX_MAP;
	for(i=0;i<shMax;i++)
	{
		pcardlist[i] = (i>>3)+1;
	}
}

bool ShisenshoRule_InitializeMap(unsigned char *pcardlist,
								unsigned char *pmap,unsigned char chWidth,unsigned char chHeight)
{
	unsigned char cardlist[SHISENSHO_MAX_MAP+8];
	unsigned short shSize = chWidth * chHeight;
	if(pmap == NULL || chWidth<5 || chHeight<5 || shSize >= SHISENSHO_MAX_MAP)
		return false;
	memset(pmap,0,shSize+8);
	if(pcardlist == NULL)
	{
		ShisenshoRule_CreateAllCards(cardlist,shSize);
	}else
	{
		memcpy(cardlist,pcardlist,shSize);
	}		
	int i,j;
	unsigned char chCardOrder;
	for(i=1;i<=chWidth;i++)
	{
		for(j=1;j<=chHeight;j++)
		{
			chCardOrder = rand()%shSize;
			ShisenshoRule_SetPoint(pmap,chWidth,chHeight,i,j,cardlist[chCardOrder]);
			cardlist[chCardOrder] = cardlist[shSize-1];
			shSize --;
		}
	}
	return true;	
}

void ShisenshoRule_ResetMap(unsigned char *pmap,unsigned char chWidth,unsigned char chHeight)
{//重新排列,每调用一次本函数,生命值应该减一
	unsigned char cardlist[SHISENSHO_MAX_MAP+8],*p;
	unsigned short shSize = 0;
	int i,j;
	unsigned char chCardOrder;
	for(i=1;i<=chWidth;i++)
	{
		for(j=1;j<=chHeight;j++)
		{
			p = ShisenshoRule_GetPoint(pmap,chWidth,chHeight,i,j);
			if(p != NULL && *p != 0)
			{
				cardlist[shSize] = *p;
				shSize ++;
			}
		}
	}
	if(shSize == 0)
		return;
	for(i=1;i<=chWidth;i++)
	{
		for(j=1;j<=chHeight;j++)
		{
			p = ShisenshoRule_GetPoint(pmap,chWidth,chHeight,i,j);
			if(p != NULL && *p != 0)
			{
				chCardOrder = rand()%shSize;
				ShisenshoRule_SetPoint(pmap,chWidth,chHeight,i,j,cardlist[chCardOrder]);
				cardlist[chCardOrder] = cardlist[shSize-1];
				shSize --;
				if(shSize == 0)
					return;
			}
		}
	}
}
