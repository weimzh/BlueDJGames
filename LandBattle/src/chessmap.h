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

#ifndef __DJGAME_CHESS_MAP_H__
#define __DJGAME_CHESS_MAP_H__


#ifndef DJGAME_CHESSMAP_MAX_PINS
#define DJGAME_CHESSMAP_MAX_PINS	8		//该宏应放在相关游戏的头文件中定义
#endif

#ifndef DJGAME_CHESSMAP_COMPARE2NODE
#define DJGAME_CHESSMAP_COMPARE2NODE(arg) 	__Compare2Node arg
#endif

#ifndef DJGAME_CHESSMAP_COMPARE2NODEID
#define DJGAME_CHESSMAP_COMPARE2NODEID(arg) 	__Compare2NodeID arg
#endif

#ifndef DJGAME_CHESSMAP_GET2NODEABSPATH
#define DJGAME_CHESSMAP_GET2NODEABSPATH(arg) 	__Get2NodeAbsPath arg
#endif



typedef struct __tagChipPower
{
	short power;
}CHIPPOWER,*PCHIPPOWER;

#define MAP_MAX_F			0x7F7F

typedef struct __tagMapF
{
	short f;
}MAPF,*PMAPF;

typedef struct __tagChip
{
	unsigned short	id;
	unsigned short 	owner;
	CHIPPOWER	power;
	MAPF		f;
}CHIP,*PCHIP;

#define MAPNODETYPE_VOID			0xFFFF
typedef struct __tagNodeType
{
	unsigned short type;
}NODETYPE,*PNODETYPE;

#define MAPNODEDISTRICT_VOID		0xFFFF
typedef struct __tagNodeID
{
	unsigned short district;
	unsigned short id;
	NODETYPE		type;
}NODEID,*PNODEID;
#define IsVoidNode(__pid)				((__pid)->type.type == MAPNODETYPE_VOID ||(__pid)->district == MAPNODEDISTRICT_VOID)

typedef struct __tagPathDes
{
	MAPF	f;
}PATHDES,*PPATHDES;

typedef struct __tagMapPin
{
	unsigned short type;
	struct __tagMapNode	*pnode;
}MapPin,*PMapPin;


#define MAPNODE_STYLE_PK			0x01		//如果node具备此属性,说明该node允许吃子,
												//否则说明该node是先到先占,后来者不能进入
#define MAPNODE_STYLE_SEACHING		0x02
typedef struct __tagMapNode
{
	NODEID			id;
	CHIP			chip;
	MAPF			f;
	unsigned char 	style;
	short			x;
	short 			y;
	void 			*context;
	MapPin 			pin[DJGAME_CHESSMAP_MAX_PINS+1];
	PATHDES 		path[DJGAME_CHESSMAP_MAX_PINS+1][DJGAME_CHESSMAP_MAX_PINS+1];
}MapNode,*PMapNode;

typedef struct __tagChessMapHead
{
	unsigned short shMaxNodes;
	unsigned short shCurNodes;
	unsigned short shVoidNodeIndex;
	
	unsigned char 	chHavePath;
	unsigned int 	iLastLen;
//	unsigned char * ppath;
	unsigned int 	iContext;
	void 			*pContext;
	
	PMapNode		pnode[1];
}ChessMapHead,*PChessMapHead;

PChessMapHead GetChessmapHead(char * pchessmap);
bool InitChessMap(char * pchessmap,int iMapMaxSize,unsigned short shMaxNodes);
PMapNode InsertNode(char * pchessmap,PMapNode pnode,bool bArrange);
bool SetPin(char * pchessmap,PNODEID pnode1id,unsigned char node1pinid,unsigned short pintype,PNODEID pnode2id);
bool Connect2NodeByVoidNode(char * pchessmap,PNODEID pnode1id,unsigned char chPin1, \
								PNODEID pnode2id,unsigned char chPin2,unsigned short pintype);

bool RemovePin(char * pchessmap,PNODEID pnode1id,unsigned char node1pinid,PNODEID pnode2id);
void ArrangeNode(char * pchessmap);

PMapNode CheckPath(char * pchessmap,PCHIPPOWER power,PMapNode pstart,unsigned char *ppath);

unsigned short SearchOwnerNodes(char * pchessmap,unsigned short owner,PMapNode *pnodes,unsigned short shMaxNodes,int minpower);
PMapNode SearchNode(char * pchessmap,PNODEID pid);
bool SearchPath(char * pchessmap,PCHIPPOWER power,PMapNode pstart,PMapNode pend,unsigned char **pppath,int *iTotalF);
PMapNode Point2Node(char * pchessmap,int x,int y,int dx,int dy);

void ClearChessmap(char * pchessmap);

unsigned char GetPinFaceNumber(unsigned char chPin);
PMapNode GetRealNode(PMapNode pnode);

int __Compare2Node(PMapNode pnode1,PMapNode pnode2);
int __Compare2NodeID(PNODEID id1,PNODEID id2);

unsigned int __Get2NodeAbsPath(char *pchessmap,PMapNode pnode1,PMapNode pnode2);

#endif
