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
//#include "jdebug.h"
#include "chessmap.h"
#include "bufalloc.h"

PMapNode SearchNodeFromList(PNODEID pid,PMapNode *ppfirstnode,unsigned short shNodes);
bool __SearchPath(char *pchessmap,PCHIPPOWER power,PMapNode pcur,PMapNode pend,unsigned char *ppath,bool bHighBit,int *iTotalF);

bool InitChessMap(char * pchessmap,int iMapMaxSize,unsigned short shMaxNodes)
{
	PChessMapHead phead;
	InitBufferForAlloc(pchessmap,iMapMaxSize);
	phead = (PChessMapHead)BufferAlloc(pchessmap,(shMaxNodes+2)*sizeof(PMapNode)+sizeof(ChessMapHead),true);
	phead->shMaxNodes = shMaxNodes;
	phead->shCurNodes = 0;
	phead->chHavePath = 0;
	phead->shVoidNodeIndex = 1;
	return true;
}

PChessMapHead GetChessmapHead(char * pchessmap)
{
	return (PChessMapHead)GetFirstBlock(pchessmap);
}
PMapNode InsertNode(char * pchessmap,PMapNode pnode,bool bArrange)
{
	unsigned short shSize;
	PMapNode p;
	int i;
	
	PChessMapHead phead  = (PChessMapHead)GetFirstBlock(pchessmap);
	if(phead == NULL || phead->shCurNodes >= phead->shMaxNodes)
		return NULL;
	
	shSize = sizeof(MapNode);
	
	p = (PMapNode)BufferAlloc(pchessmap,shSize,false);
	memcpy(p,pnode,shSize);

	for(i=0;i<phead->shMaxNodes;i++)
	{
		if(phead->pnode[i] == NULL)
		{
			phead->pnode[i] = p;
			phead->shCurNodes ++;
			if(bArrange)
			{
				ArrangeNode(pchessmap);
			}
			return p;
		}
	}
	BufferFree(pchessmap,(char *)p);
	return NULL;
}

bool Connect2NodeByVoidNode(char * pchessmap,PNODEID pnode1id,unsigned char chPin1,
								PNODEID pnode2id,unsigned char chPin2,unsigned short pintype)
{
	MapNode node,*pvn1,*pvn2,*pnode;
	unsigned char chOutputPin,chIdlePin[DJGAME_CHESSMAP_MAX_PINS];
	PChessMapHead phead  = (PChessMapHead)GetFirstBlock(pchessmap);
	int i;
	
	if(DJGAME_CHESSMAP_MAX_PINS < 4)
		return false;
	memset(&(chIdlePin[0]),0,DJGAME_CHESSMAP_MAX_PINS);
	memset(&node,0,sizeof(MapNode));
	node.id.type.type	 = MAPNODETYPE_VOID;
	node.id.district = MAPNODEDISTRICT_VOID;
	node.id.id = phead->shVoidNodeIndex;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	pvn1 = InsertNode(pchessmap,&node,true);
	pnode = SearchNode(pchessmap,pnode2id);
	pvn1->x = pnode->x;
	pvn1->y = pnode->y;
	pvn1->pin[0].pnode = pnode;
	
	chOutputPin = chPin1%(DJGAME_CHESSMAP_MAX_PINS>>1);
	chOutputPin = (chOutputPin==0)?(DJGAME_CHESSMAP_MAX_PINS>>1):chOutputPin;
	chIdlePin[chOutputPin] = 1;
	phead->shVoidNodeIndex ++;
	
	node.id.id = phead->shVoidNodeIndex;
	pvn2 = InsertNode(pchessmap,&node,true);
	pnode = SearchNode(pchessmap,pnode1id);
	pvn2->x = pnode->x;
	pvn2->y = pnode->y;
	pvn2->pin[0].pnode = pnode;
	
	chOutputPin = chPin2%(DJGAME_CHESSMAP_MAX_PINS>>1);
	chOutputPin = (chOutputPin==0)?(DJGAME_CHESSMAP_MAX_PINS>>1):chOutputPin;
	chIdlePin[chOutputPin] = 1;
	phead->shVoidNodeIndex ++;
	
	chOutputPin = 0;
	for(i=1;i<=(DJGAME_CHESSMAP_MAX_PINS>>1);i++)
	{
		if(chIdlePin[i] == 0)
		{
			chOutputPin = i;
			break;
		}
	}
	if(chOutputPin == 0)
		return false;
	SetPin(pchessmap,pnode1id,chPin1,pintype,&pvn1->id);
	SetPin(pchessmap,pnode2id,chPin2,pintype,&pvn2->id);
	SetPin(pchessmap,&pvn1->id,chOutputPin,pintype,&pvn2->id);
	chPin1 = GetPinFaceNumber(chPin1);
	chPin2 = GetPinFaceNumber(chPin2);
	pvn1->path[chPin1][chOutputPin].f.f = 0;
	pvn1->path[chOutputPin][chPin1].f.f = 0;
	chOutputPin += (DJGAME_CHESSMAP_MAX_PINS>>1);
	pvn2->path[chPin2][chOutputPin].f.f = 0;
	pvn2->path[chOutputPin][chPin2].f.f = 0;
	return true;
}

PMapNode GetRealNode(PMapNode pnode)
{
	if(pnode != NULL && pnode->id.type.type == MAPNODETYPE_VOID)
	{
		return pnode->pin[0].pnode;
	}
	return pnode;
}

bool SetPin(char * pchessmap,PNODEID pnode1id,unsigned char node1pinid,unsigned short pintype,PNODEID pnode2id)
{
	PMapNode pnode1,pnode2;
	unsigned char node2pinid;
	node2pinid = node1pinid+(DJGAME_CHESSMAP_MAX_PINS>>1);
	node2pinid %= DJGAME_CHESSMAP_MAX_PINS;
	node2pinid = (node2pinid == 0)?DJGAME_CHESSMAP_MAX_PINS:node2pinid;
	
	pnode1 = SearchNode(pchessmap,pnode1id);
	if(pnode1 == NULL)
		return false;
	pnode2 = SearchNode(pchessmap,pnode2id);
	if(pnode2 == NULL)
		return false;
	pnode1->pin[node1pinid].pnode = pnode2;
	pnode1->pin[node1pinid].type  = pintype;
	pnode2->pin[node2pinid].pnode = pnode1;
	pnode2->pin[node2pinid].type  = pintype;
	return true;
}

bool RemovePin(char * pchessmap,PNODEID pnode1id,unsigned char node1pinid,PNODEID pnode2id)
{
	return false;
}

unsigned short SearchOwnerNodes(char * pchessmap,unsigned short owner,PMapNode *pnodes,unsigned short shMaxNodes,int minpower)
{
	PChessMapHead phead  = (PChessMapHead)GetFirstBlock(pchessmap);
	if(phead == NULL || phead->shCurNodes == 0)
		return 0;
	int i;
	unsigned short shNodes = 0;
	for(i=0;i<phead->shCurNodes;i++)
	{
		if(phead->pnode[i]->chip.id != 0 && phead->pnode[i]->chip.owner == owner && phead->pnode[i]->chip.power.power >minpower )
		{
			if(shNodes <= shMaxNodes)
			{
				pnodes[shNodes] = phead->pnode[i];
			}
			shNodes ++;
		}
	}
	return shNodes;
}

void ClearChessmap(char * pchessmap)
{
	PChessMapHead phead  = (PChessMapHead)GetFirstBlock(pchessmap);
	if(phead == NULL || phead->shCurNodes == 0)
		return ;
	int i;
	for(i=0;i<phead->shCurNodes;i++)
	{
		memset(&(phead->pnode[i]->chip),0,sizeof(CHIP));
	}
}

PMapNode SearchNode(char * pchessmap,PNODEID pid)
{
	PChessMapHead phead  = (PChessMapHead)GetFirstBlock(pchessmap);
	if(phead == NULL || phead->shCurNodes == 0)
		return NULL;
	return SearchNodeFromList(pid,&(phead->pnode[0]),phead->shCurNodes);
}

PMapNode Point2Node(char * pchessmap,int x,int y,int dx,int dy)
{
	PChessMapHead phead  = (PChessMapHead)GetFirstBlock(pchessmap);
	if(phead == NULL || phead->shCurNodes == 0)
		return NULL;
	int i;
	for(i=0;i<phead->shCurNodes;i++)
	{
		if(phead->pnode[i]->x >= x-dx && phead->pnode[i]->x <= x+dx && 
			phead->pnode[i]->y >= y-dy && phead->pnode[i]->y <= y+dy)
			return phead->pnode[i];
	}
	return NULL;
}

PMapNode SearchNodeFromList(PNODEID pid,PMapNode *ppfirstnode,unsigned short shNodes)
{//以2分法查找节点
	int i,j,iRet;
	if(shNodes == 0)
		return NULL;
	if(shNodes < 3)
	{
		for(i=0;i<shNodes;i++)
		{
			if(*ppfirstnode != NULL)
			{
				if(DJGAME_CHESSMAP_COMPARE2NODEID((pid,&( (*ppfirstnode)->id))) == 0)
					return *ppfirstnode;
			}
			ppfirstnode ++;
		}
		return NULL;
	}
	j = (shNodes>>1);
	iRet = DJGAME_CHESSMAP_COMPARE2NODEID((pid,&( (*(ppfirstnode+j))->id)));
	switch(iRet)
	{
		case 0 :
			return *(ppfirstnode+j);
		case 1 :
			return SearchNodeFromList(pid,ppfirstnode,j);
		case -1 :
			return SearchNodeFromList(pid,ppfirstnode+j+1,shNodes-j-1);
	}
	return NULL;
}
PMapNode CheckPath(char * pchessmap,PCHIPPOWER power,PMapNode pstart,unsigned char *ppath)
{
//	PChessMapHead phead  = (PChessMapHead)GetFirstBlock(pchessmap);
	unsigned char chInputPin,chOutputPin;
	int 			iTotalF = 0;
	PMapNode		pnode;
	bool 			bHighBit;
//	PMapNode pend;
	
	chInputPin = 0;
	pnode = pstart;
	bHighBit = false;
	while(iTotalF <= power->power && pnode != NULL)
	{
		if(bHighBit)
		{
			chOutputPin = (((*ppath)&0xF0)>>4);
		}else
		{
			chOutputPin = ((*ppath)&0x0F);
			ppath ++;
		}
		if(chOutputPin == 0)
		{
			//DEBUG_OUTPUT(("Success\n"));
			return pnode;
		}
		if(chInputPin != 0)
			iTotalF += pnode->chip.f.f;
		iTotalF += pnode->path[chInputPin][chOutputPin].f.f ;
		pnode = pnode->pin[chOutputPin].pnode;
		//DEBUG_OUTPUT(("%04X->",pnode->id.id));
		chInputPin = (chOutputPin+(DJGAME_CHESSMAP_MAX_PINS>>1))%DJGAME_CHESSMAP_MAX_PINS;
		chInputPin = (chInputPin == 0)?DJGAME_CHESSMAP_MAX_PINS:chInputPin;
		bHighBit = !bHighBit;
		
	}
	return NULL;
}
bool SearchPath(char * pchessmap,PCHIPPOWER power,PMapNode pstart,PMapNode pend,unsigned char **pppath,int *iTotalF)
{
	bool bRet;
	int i;
	PChessMapHead phead  = (PChessMapHead)GetFirstBlock(pchessmap);
	if(phead == NULL || phead->shCurNodes == 0)
		return false;
	if(phead->chHavePath != 0 )
		BufferSetAllocTotalSize(pchessmap,phead->iLastLen);
	*iTotalF = 0;
	phead->iLastLen = BufferGetAllocTotalSize(pchessmap);
	*pppath = (unsigned char *)BufferAlloc(pchessmap,phead->shMaxNodes,true);
	phead->chHavePath = 1;
	bRet = __SearchPath(pchessmap,power,pstart,pend,*pppath,false,iTotalF);
	//清除所有MAPNODE_STYLE_SEACHING标志
	for(i=0;i<phead->shMaxNodes;i++)
	{
		if(phead->pnode[i] != NULL)
			phead->pnode[i]->style &= (~MAPNODE_STYLE_SEACHING);
	}
	return bRet;
}

bool __SearchPath(char *pchessmap,PCHIPPOWER power,PMapNode pcur,PMapNode pend,unsigned char *ppath,bool bHighBit,int *iTotalF)
{
	int i,j;
	int iCurF;
	unsigned int nAbsPath[DJGAME_CHESSMAP_MAX_PINS+1];
	unsigned char chMask,chInputPin;
	unsigned char *ppathnext;
	
	if( DJGAME_CHESSMAP_COMPARE2NODEID((&(pcur->id),&(pend->id))) == 0)
		return true;
	if(pcur->style & MAPNODE_STYLE_SEACHING)
		return false;
	pcur->style |= MAPNODE_STYLE_SEACHING;
	if(bHighBit)
		chInputPin = (*(ppath-1))&0x0F;
	else
		chInputPin = ((*ppath)&0xF0)>>4;
	if(chInputPin != 0)
		chInputPin = GetPinFaceNumber(chInputPin);
	for(i=1;i<=DJGAME_CHESSMAP_MAX_PINS;i++)
	{
		if(pcur->pin[i].type != 0 && pcur->pin[i].pnode != NULL &&
			!(pcur->pin[i].pnode->style & MAPNODE_STYLE_SEACHING))
		{
			j = *iTotalF + pcur->path[chInputPin][i].f.f;
			if(chInputPin != 0)
				j += pcur->chip.f.f;
			if( j <=power->power  )
			{
				nAbsPath[i] = DJGAME_CHESSMAP_GET2NODEABSPATH((pchessmap,pcur->pin[i].pnode,pend));
				if(nAbsPath[i] == 0)
					nAbsPath[i] =1;
			}else
			{
				//DEBUG_OUTPUT(("not enough power to pin %d\n",i));
				nAbsPath[i] = 0;
			}
		}else
			nAbsPath[i] = 0;
	}
	
	do
	{
		j = 0;
		for(i=1;i<=DJGAME_CHESSMAP_MAX_PINS;i++)
		{
			if((nAbsPath[i] != 0 && j== 0) || (j!=0 && nAbsPath[i]!=0 && nAbsPath[i]< nAbsPath[j]) )
			{
				j = i;
			}
		}
		if(j == 0)
			return false;
		chMask = j;
		if(bHighBit)
		{
			chMask <<= 4;
			*ppath = chMask;
			ppathnext = ppath;
		}else
		{
			*ppath &= 0xF0;
			*ppath += chMask;
			*(ppath+1) = 0;
			ppathnext = ppath+1;
		}
		iCurF = (*iTotalF) + pcur->path[chInputPin][j].f.f;
		if(__SearchPath(pchessmap,power,pcur->pin[j].pnode,pend,ppathnext,(bHighBit)?false:true,&iCurF))
		{
			*iTotalF = iCurF;
			return true;
		}
		nAbsPath[j] = 0;
	}while(j>0);
	return false;	
		
}


void ArrangeNode(char * pchessmap)
{
	int i,j;
	PChessMapHead phead  = (PChessMapHead)GetFirstBlock(pchessmap);
	PMapNode ptemp;
	if(phead != NULL && phead->shCurNodes > 1)
	{
		do
		{
			j = 0;
			for(i=0;i<phead->shMaxNodes-1;i++)
			{
				if(DJGAME_CHESSMAP_COMPARE2NODE((phead->pnode[i+1],phead->pnode[i])) == 1)
				{
					j ++;
					ptemp =phead->pnode[i+1];
					phead->pnode[i+1] = phead->pnode[i];
					phead->pnode[i] = ptemp;
				}
			}
		}while(j!=0);
	}
}

int __Compare2Node(PMapNode pnode1,PMapNode pnode2)
{
	if(pnode1 == NULL && pnode2 == NULL)
		return 0;
	if(pnode1 != NULL && pnode2 == NULL)
		return 1;
	if(pnode1 == NULL && pnode2 != NULL)
		return -1;

	return DJGAME_CHESSMAP_COMPARE2NODEID((&(pnode1->id),&(pnode2->id)));
}

int __Compare2NodeID(PNODEID pid1,PNODEID pid2)
{
	if(pid1->district > pid2->district)
		return -1;
	if(pid1->district < pid2->district)
		return 1;
	
	if(pid1->id > pid2->id)
		return -1;
	if(pid1->id < pid2->id)
		return 1;
	
	return 0;
}

unsigned int __Get2NodeAbsPath(char *pchessmap,PMapNode pnode1,PMapNode pnode2)
{
	return abs(pnode1->x-pnode2->x)+abs(pnode1->y-pnode2->y);
}

//////////////////////////////////////////

unsigned char GetPinFaceNumber(unsigned char chPin)
{
	chPin += (DJGAME_CHESSMAP_MAX_PINS>>1);
	chPin %= DJGAME_CHESSMAP_MAX_PINS;
	return (chPin == 0)?DJGAME_CHESSMAP_MAX_PINS:chPin;
}

////////////////////////////////////////////////////
