#include <iostream>
#include "chessmap.h"
#include "4country.h"
//#include "jdebug.h"

void Insert1Board(char *pchessmap,unsigned short board);
void CreatPinFor2Stop(char * pchessmap,unsigned short board1,unsigned short id1,unsigned char pin,
						unsigned short board2,unsigned short id2,unsigned short pintype);
void InitJunQiNodeXY(char *pchessmap,unsigned char chCountrys,unsigned char chFirstSite);

void InitChips()
{
}

void SetChip(char * pchessmap,unsigned char chArea,unsigned char id,PJunQiChip pchip)
{
	PMapNode pnode;
	NODEID			nodeid;
	nodeid.district = 0;
	nodeid.id = GET_NODEID(chArea,0,id);
	pnode = SearchNode(pchessmap,&nodeid);
	if(pnode == NULL)
		return;
	pnode->chip.id = pchip->id;
	pnode->chip.power.power = pchip->power;
	pnode->chip.f.f = MAP_MAX_F;
	pnode->chip.owner = pchip->owner;
}

bool MoveChip(char * pchessmap,unsigned char chAreaSrc,unsigned char idsrc,
								unsigned char chAreaDis,unsigned char iddis,
								unsigned char **pppath)
{
	PMapNode pstart,pend;
	NODEID			nodeid;
	int 			iTotalF;
	bool 			bRet;
	PChessMapHead phead  = (PChessMapHead)GetChessmapHead(pchessmap);
	
	nodeid.district = 0;
	nodeid.id = GET_NODEID(chAreaSrc,0,idsrc);
	pstart = SearchNode(pchessmap,&nodeid);
	if(pstart == NULL || pstart->chip.id == 0 || pstart->chip.power.power == 0)
		return false;
	nodeid.district = 0;
	nodeid.id = GET_NODEID(chAreaDis,0,iddis);
	pend = SearchNode(pchessmap,&nodeid);
	if(pend == NULL)
		return false;
	if(pend->id.type.type == DJGAME_JUNQI_NODETYPE_STATION && pend->chip.id != 0)
		return false;
	bRet = SearchPath(pchessmap,&(pstart->chip.power),pstart,pend,pppath,&iTotalF);
//	if(bRet)
//	{
//		DEBUG_BUFFER(((char *)*pppath,phead->shMaxNodes-1));
//	}
	return bRet;
	
}

unsigned char CheckNodesMovable(PMapNode *pnodes,unsigned char chNodes,unsigned char *unions,unsigned char chUnions)
{
	if(chNodes == 0 || chNodes > 110)
		return 0;
	PMapNode pcopynodes[110],pnextnode;
	memcpy(pcopynodes,pnodes,sizeof(PMapNode)*chNodes);
	int i,j;
	unsigned char chMovableNodes = 0;
	for(i=0;i<chNodes;i++)
	{
		if(pcopynodes[i]->chip.id != 0)
		{
			for(j=1;j<=DJGAME_CHESSMAP_MAX_PINS;j++)
			{
				if(pcopynodes[i]->pin[j].pnode != NULL && pcopynodes[i]->pin[j].type != 0)
				{
					pnextnode = GetRealNode(pcopynodes[i]->pin[j].pnode);
					if(pnextnode != NULL && pcopynodes[i]->path[0][j].f.f <= pcopynodes[i]->chip.power.power && 
							(pnextnode->chip.id == 0 || (!IsFriendlyChip(unions,chUnions,pcopynodes[i],pnextnode) && !IsStation(GET_NodeX(pnextnode->id.id),GET_NodeY(pnextnode->id.id) ) )) )
					{//该子可移动
						pnodes[chMovableNodes] = pcopynodes[i];
						chMovableNodes ++;
						break;
					}
				}
			}//for(j=1;j<=DJGAME_CHESSMAP_MAX_PINS;j++)
		}//if(pcopynodes[i]->chip.id != 0)
	}//i
	return chMovableNodes;
}

bool JunqiCheckArrange(PJunqiTraceInitChip pori,PJunqiTraceInitChip parrange)
{
	char buffer[200];
	int i,j;
	int iDilei=0,iHighDilei=0;
	int iZhadan = 0,iFirstZha=0;
	PJunqiTraceInitChip	pcopy;
	if( pori == NULL || parrange == NULL)
		return false;
	if(pori->chChips != parrange->chChips)
		return false;
	memcpy(buffer,pori,sizeof(JunqiTraceInitChip) + (pori->chChips) * sizeof(JunqiTraceChip));
	pcopy = (PJunqiTraceInitChip)buffer;
	for(i=0;i<pcopy->chChips;i++)
	{
		for(j=0;j<pcopy->chChips;j++)
		{
			if(pcopy->chip[j].chip.id == 0)
				continue;
			if(IsStation( (parrange->chip[i].xy >> 4),(parrange->chip[i].xy&0x0F)) )
				return false;
			if(parrange->chip[i].chip.id == DJGAME_JUNQI_RANK_JUNQI &&
					!IsCamp( (parrange->chip[i].xy >> 4),(parrange->chip[i].xy&0x0F)) )
				return false;
			if(parrange->chip[i].chip.id == DJGAME_JUNQI_RANK_DILEI)
			{
				iDilei ++;
				if( (parrange->chip[i].xy >> 4) > 2)
					iHighDilei ++;
			}
			
			if(parrange->chip[i].chip.id == DJGAME_JUNQI_RANK_ZHADAN)
			{
				iZhadan ++;
				if( (parrange->chip[i].xy >> 4) == 6)
					iFirstZha ++;
			}
			
			if(parrange->chip[i].chip.id == pcopy->chip[j].chip.id)
			{
				parrange->chip[i].chip.owner = pcopy->chip[j].chip.owner;
				parrange->chip[i].chip.power = pcopy->chip[j].chip.power;
				pcopy->chip[j].chip.id = 0;
				break;
			}
		}
		if(j>= pcopy->chChips)
			return false;
	}
	if(iFirstZha > 0 && iZhadan < 4)
		return false;
	if(iHighDilei > 0 && iDilei <=5)
		return false;
	return true;
}
bool IsFriendlyChip(unsigned char *unions,unsigned char chUnions,PMapNode pstart,PMapNode pend)
{
	return IsFriendlyPlayer(unions,chUnions,pstart->chip.owner,pend->chip.owner);
}
bool IsFriendlyPlayer(unsigned char *unions,unsigned char chUnions,unsigned char chP1,unsigned char chP2)
{
	unsigned char ch1,ch2;
	int i;
	if(chP1 == chP2)
		return true;
	if(unions != NULL && chUnions > 0)
	{
		ch1 = (chP1 << 4) + chP2;
		ch2 = (chP2 << 4) + chP1;
		for(i=0;i<chUnions;i++)
		{
			if(unions[i] == ch1 || unions[i] == ch2)
				return true;
		}
	}
	return false;
}

bool IsStation(unsigned char x,unsigned char y)
{
	if( (x==3 || x==5) && (y==2 || y== 4) )
		return true;
	if( x== 4 && y== 3)
		return true;
	return false;
}

bool IsCamp(unsigned char x,unsigned char y)
{
	if(x==1 && (y==2 || y==4) )
		return true;
	return false;
}

void Init2CountryChessmap(char *pchessmap,unsigned int iMaxSize,unsigned char chFirstSite)
{
	NODEID			nodeid,nodeid2;
	InitChessMap(pchessmap,iMaxSize,70);
	PChessMapHead phead  = (PChessMapHead)GetChessmapHead(pchessmap);
	phead->iContext = 2;
	Insert1Board(pchessmap,1);
	Insert1Board(pchessmap,2);
	InitJunQiNodeXY(pchessmap,2,chFirstSite);
	//利用虚节点连接2个棋盘
	nodeid.district = 0;
	nodeid2.district = 0;
	nodeid.id = GET_NODEID(1,6,1);
	nodeid2.id = GET_NODEID(2,6,5);
	Connect2NodeByVoidNode(pchessmap,&nodeid,1,&nodeid2,1,DJGAME_JUNQI_PINTYPE_RAIL);
	
	nodeid.id = GET_NODEID(1,6,5);
	nodeid2.id = GET_NODEID(2,6,1);
	Connect2NodeByVoidNode(pchessmap,&nodeid,1,&nodeid2,1,DJGAME_JUNQI_PINTYPE_RAIL);
	
	nodeid.id = GET_NODEID(1,6,3);
	nodeid2.id = GET_NODEID(2,6,3);
	Connect2NodeByVoidNode(pchessmap,&nodeid,1,&nodeid2,1,DJGAME_JUNQI_PINTYPE_RAIL);
}

void Init3CountryChessmap(char *pchessmap,unsigned int iMaxSize,unsigned char chFirstSite)
{
	MapNode node;
	NODEID			nodeid,nodeid2;
	InitChessMap(pchessmap,iMaxSize,106);
	PChessMapHead phead  = (PChessMapHead)GetChessmapHead(pchessmap);
	phead->iContext = 3;
	Insert1Board(pchessmap,1);
	Insert1Board(pchessmap,2);
	Insert1Board(pchessmap,3);
	
	//创建中心节点
	memset(&node,0,sizeof(MapNode));
	node.id.district = 0;
	node.id.id		 = GET_NODEID(DJGAME_JUNQI_CONNECTER_AREA,0,DJGAME_JUNQI_CONNECTER_AREA_CENTER);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][2].f.f  = 1;
	node.path[0][3].f.f  = 1;
	node.path[1][2].f.f  = 15;
	node.path[1][3].f.f  = 15;
	node.path[2][1].f.f  = 15;
	node.path[2][3].f.f  = 15;
	node.path[3][1].f.f  = 15;
	node.path[3][2].f.f  = 15;
	InsertNode(pchessmap,&node,true);
	
	InitJunQiNodeXY(pchessmap,3,chFirstSite);
	//利用虚节点连接3个棋盘
	nodeid.district = 0;
	nodeid2.district = 0;
	nodeid.id = GET_NODEID(1,6,1);
	nodeid2.id = GET_NODEID(2,6,5);
	Connect2NodeByVoidNode(pchessmap,&nodeid,8,&nodeid2,2,DJGAME_JUNQI_PINTYPE_RAIL);
	
	nodeid.id = GET_NODEID(2,6,1);
	nodeid2.id = GET_NODEID(3,6,5);
	Connect2NodeByVoidNode(pchessmap,&nodeid,8,&nodeid2,2,DJGAME_JUNQI_PINTYPE_RAIL);
	
	nodeid.id = GET_NODEID(3,6,1);
	nodeid2.id = GET_NODEID(1,6,5);
	Connect2NodeByVoidNode(pchessmap,&nodeid,8,&nodeid2,2,DJGAME_JUNQI_PINTYPE_RAIL);
	
	//使用虚节点将三个棋盘和中心节点连接
	nodeid.id = GET_NODEID(1,6,3);
	Connect2NodeByVoidNode(pchessmap,&nodeid,1,&node.id,1,DJGAME_JUNQI_PINTYPE_RAIL);
	nodeid.id = GET_NODEID(2,6,3);
	Connect2NodeByVoidNode(pchessmap,&nodeid,1,&node.id,2,DJGAME_JUNQI_PINTYPE_RAIL);
	nodeid.id = GET_NODEID(3,6,3);
	Connect2NodeByVoidNode(pchessmap,&nodeid,1,&node.id,3,DJGAME_JUNQI_PINTYPE_RAIL);
}

void Init4CountryChessmap(char *pchessmap,unsigned int iMaxSize,unsigned char chFirstSite)
{
	MapNode node,*pcenter;
	NODEID			nodeid,nodeid2;
	InitChessMap(pchessmap,iMaxSize,164);
	PChessMapHead phead  = (PChessMapHead)GetChessmapHead(pchessmap);
	phead->iContext = 4;
	Insert1Board(pchessmap,1);
	Insert1Board(pchessmap,2);
	Insert1Board(pchessmap,3);
	Insert1Board(pchessmap,4);
	
	//创建连接区
			//中心点
	memset(&node,0,sizeof(MapNode));
	node.id.district = 0;
	node.id.id		 = GET_NODEID(DJGAME_JUNQI_CONNECTER_AREA,0,DJGAME_JUNQI_CONNECTER_AREA_CENTER);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][3].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[0][7].f.f  = 1;
	node.path[1][5].f.f  = 1;
	node.path[5][1].f.f  = 1;
	node.path[3][7].f.f  = 1;
	node.path[7][3].f.f  = 1;
	
	node.path[1][3].f.f  = 15;
	node.path[3][1].f.f  = 15;
	node.path[1][7].f.f  = 15;
	node.path[7][1].f.f  = 15;
	node.path[5][3].f.f  = 15;
	node.path[3][5].f.f  = 15;
	node.path[5][7].f.f  = 15;
	node.path[7][5].f.f  = 15;
	pcenter = InsertNode(pchessmap,&node,true);
		//外围
	node.id.id		 = GET_NODEID(DJGAME_JUNQI_CONNECTER_AREA,0,0x03);//0011
	InsertNode(pchessmap,&node,true);
	node.id.id		 = GET_NODEID(DJGAME_JUNQI_CONNECTER_AREA,0,0x01);//0001
	InsertNode(pchessmap,&node,true);
	node.id.id		 = GET_NODEID(DJGAME_JUNQI_CONNECTER_AREA,0,0x09);//1001
	InsertNode(pchessmap,&node,true);
	node.id.id		 = GET_NODEID(DJGAME_JUNQI_CONNECTER_AREA,0,0x02);//0010
	InsertNode(pchessmap,&node,true);
	node.id.id		 = GET_NODEID(DJGAME_JUNQI_CONNECTER_AREA,0,0x08);//1000
	InsertNode(pchessmap,&node,true);
	node.id.id		 = GET_NODEID(DJGAME_JUNQI_CONNECTER_AREA,0,0x06);//0110
	InsertNode(pchessmap,&node,true);
	node.id.id		 = GET_NODEID(DJGAME_JUNQI_CONNECTER_AREA,0,0x04);//0100
	InsertNode(pchessmap,&node,true);
	node.id.id		 = GET_NODEID(DJGAME_JUNQI_CONNECTER_AREA,0,0x0c);//1100
	InsertNode(pchessmap,&node,true);
	
	CreatPinFor2Stop(pchessmap,DJGAME_JUNQI_CONNECTER_AREA,0x03,3,DJGAME_JUNQI_CONNECTER_AREA,0x01,
					DJGAME_JUNQI_PINTYPE_RAIL);
	CreatPinFor2Stop(pchessmap,DJGAME_JUNQI_CONNECTER_AREA,0x03,1,DJGAME_JUNQI_CONNECTER_AREA,0x02,
					DJGAME_JUNQI_PINTYPE_RAIL);
	CreatPinFor2Stop(pchessmap,DJGAME_JUNQI_CONNECTER_AREA,0x01,1,DJGAME_JUNQI_CONNECTER_AREA,0xFF,
					DJGAME_JUNQI_PINTYPE_RAIL);
	CreatPinFor2Stop(pchessmap,DJGAME_JUNQI_CONNECTER_AREA,0x01,3,DJGAME_JUNQI_CONNECTER_AREA,0x09,
					DJGAME_JUNQI_PINTYPE_RAIL);
	CreatPinFor2Stop(pchessmap,DJGAME_JUNQI_CONNECTER_AREA,0x08,7,DJGAME_JUNQI_CONNECTER_AREA,0xFF,
					DJGAME_JUNQI_PINTYPE_RAIL);
	CreatPinFor2Stop(pchessmap,DJGAME_JUNQI_CONNECTER_AREA,0x08,5,DJGAME_JUNQI_CONNECTER_AREA,0x09,
					DJGAME_JUNQI_PINTYPE_RAIL);
	CreatPinFor2Stop(pchessmap,DJGAME_JUNQI_CONNECTER_AREA,0xFF,1,DJGAME_JUNQI_CONNECTER_AREA,0x04,
					DJGAME_JUNQI_PINTYPE_RAIL);
	CreatPinFor2Stop(pchessmap,DJGAME_JUNQI_CONNECTER_AREA,0xFF,7,DJGAME_JUNQI_CONNECTER_AREA,0x02,
					DJGAME_JUNQI_PINTYPE_RAIL);
	CreatPinFor2Stop(pchessmap,DJGAME_JUNQI_CONNECTER_AREA,0x06,3,DJGAME_JUNQI_CONNECTER_AREA,0x04,
					DJGAME_JUNQI_PINTYPE_RAIL);
	CreatPinFor2Stop(pchessmap,DJGAME_JUNQI_CONNECTER_AREA,0x06,5,DJGAME_JUNQI_CONNECTER_AREA,0x02,
					DJGAME_JUNQI_PINTYPE_RAIL);
	CreatPinFor2Stop(pchessmap,DJGAME_JUNQI_CONNECTER_AREA,0x0c,7,DJGAME_JUNQI_CONNECTER_AREA,0x04,
					DJGAME_JUNQI_PINTYPE_RAIL);
	CreatPinFor2Stop(pchessmap,DJGAME_JUNQI_CONNECTER_AREA,0x0c,5,DJGAME_JUNQI_CONNECTER_AREA,0x08,
					DJGAME_JUNQI_PINTYPE_RAIL);
					
	InitJunQiNodeXY(pchessmap,4,chFirstSite);
	//使用虚节点使4个棋盘互相连接
	nodeid.district = 0;
	nodeid2.district = 0;
	nodeid.id = GET_NODEID(1,6,1);
	nodeid2.id = GET_NODEID(2,6,5);
	Connect2NodeByVoidNode(pchessmap,&nodeid,8,&nodeid2,2,DJGAME_JUNQI_PINTYPE_RAIL);
	
	nodeid.id = GET_NODEID(2,6,1);
	nodeid2.id = GET_NODEID(3,6,5);
	Connect2NodeByVoidNode(pchessmap,&nodeid,8,&nodeid2,2,DJGAME_JUNQI_PINTYPE_RAIL);
	
	nodeid.id = GET_NODEID(3,6,1);
	nodeid2.id = GET_NODEID(4,6,5);
	Connect2NodeByVoidNode(pchessmap,&nodeid,8,&nodeid2,2,DJGAME_JUNQI_PINTYPE_RAIL);
	
	nodeid.id = GET_NODEID(4,6,1);
	nodeid2.id = GET_NODEID(1,6,5);
	Connect2NodeByVoidNode(pchessmap,&nodeid,8,&nodeid2,2,DJGAME_JUNQI_PINTYPE_RAIL);
					
	//使用虚节点将4个棋盘连接到中心区
	nodeid.id = GET_NODEID(1,6,1);
	nodeid2.id = GET_NODEID(DJGAME_JUNQI_CONNECTER_AREA,0,0x03);
	Connect2NodeByVoidNode(pchessmap,&nodeid,1,&nodeid2,5,DJGAME_JUNQI_PINTYPE_RAIL);
	nodeid.id = GET_NODEID(1,6,3);
	nodeid2.id = GET_NODEID(DJGAME_JUNQI_CONNECTER_AREA,0,0x01);
	Connect2NodeByVoidNode(pchessmap,&nodeid,1,&nodeid2,5,DJGAME_JUNQI_PINTYPE_RAIL);
	nodeid.id = GET_NODEID(1,6,5);
	nodeid2.id = GET_NODEID(DJGAME_JUNQI_CONNECTER_AREA,0,0x09);
	Connect2NodeByVoidNode(pchessmap,&nodeid,1,&nodeid2,5,DJGAME_JUNQI_PINTYPE_RAIL);
	
	nodeid.id = GET_NODEID(2,6,1);
	nodeid2.id = GET_NODEID(DJGAME_JUNQI_CONNECTER_AREA,0,0x06);
	Connect2NodeByVoidNode(pchessmap,&nodeid,1,&nodeid2,7,DJGAME_JUNQI_PINTYPE_RAIL);
	nodeid.id = GET_NODEID(2,6,3);
	nodeid2.id = GET_NODEID(DJGAME_JUNQI_CONNECTER_AREA,0,0x02);
	Connect2NodeByVoidNode(pchessmap,&nodeid,1,&nodeid2,7,DJGAME_JUNQI_PINTYPE_RAIL);
	nodeid.id = GET_NODEID(2,6,5);
	nodeid2.id = GET_NODEID(DJGAME_JUNQI_CONNECTER_AREA,0,0x03);
	Connect2NodeByVoidNode(pchessmap,&nodeid,1,&nodeid2,7,DJGAME_JUNQI_PINTYPE_RAIL);
	
	nodeid.id = GET_NODEID(3,6,1);
	nodeid2.id = GET_NODEID(DJGAME_JUNQI_CONNECTER_AREA,0,0x0c);
	Connect2NodeByVoidNode(pchessmap,&nodeid,1,&nodeid2,1,DJGAME_JUNQI_PINTYPE_RAIL);
	nodeid.id = GET_NODEID(3,6,3);
	nodeid2.id = GET_NODEID(DJGAME_JUNQI_CONNECTER_AREA,0,0x04);
	Connect2NodeByVoidNode(pchessmap,&nodeid,1,&nodeid2,1,DJGAME_JUNQI_PINTYPE_RAIL);
	nodeid.id = GET_NODEID(3,6,5);
	nodeid2.id = GET_NODEID(DJGAME_JUNQI_CONNECTER_AREA,0,0x06);
	Connect2NodeByVoidNode(pchessmap,&nodeid,1,&nodeid2,1,DJGAME_JUNQI_PINTYPE_RAIL);
	
	nodeid.id = GET_NODEID(4,6,1);
	nodeid2.id = GET_NODEID(DJGAME_JUNQI_CONNECTER_AREA,0,0x09);
	Connect2NodeByVoidNode(pchessmap,&nodeid,1,&nodeid2,3,DJGAME_JUNQI_PINTYPE_RAIL);
	nodeid.id = GET_NODEID(4,6,3);
	nodeid2.id = GET_NODEID(DJGAME_JUNQI_CONNECTER_AREA,0,0x08);
	Connect2NodeByVoidNode(pchessmap,&nodeid,1,&nodeid2,3,DJGAME_JUNQI_PINTYPE_RAIL);
	nodeid.id = GET_NODEID(4,6,5);
	nodeid2.id = GET_NODEID(DJGAME_JUNQI_CONNECTER_AREA,0,0x0c);
	Connect2NodeByVoidNode(pchessmap,&nodeid,1,&nodeid2,3,DJGAME_JUNQI_PINTYPE_RAIL);
	
}

void Init4CountryRowChessmap(char *pchessmap,unsigned int iMaxSize,unsigned char chFirstSite)
{
//	NODEID			nodeid,nodeid2;
	InitChessMap(pchessmap,iMaxSize,150);
	PChessMapHead phead  = (PChessMapHead)GetChessmapHead(pchessmap);
	phead->iContext = 0x84;
	Insert1Board(pchessmap,1);
	Insert1Board(pchessmap,2);
	Insert1Board(pchessmap,3);
	Insert1Board(pchessmap,4);
	InitJunQiNodeXY(pchessmap,0x84,chFirstSite);
	//连接友军
	
	//连接敌军
	
}

void CreatPinFor2Stop(char * pchessmap,unsigned short board1,unsigned short id1,unsigned char pin,
						unsigned short board2,unsigned short id2,unsigned short pintype)
{
	NODEID			nodeid1,nodeid2;
	nodeid1.district = 0;
	nodeid2.district = 0;
	nodeid1.id = (board1<<8)+id1;
	nodeid2.id = (board2<<8)+id2;
	SetPin(pchessmap,&nodeid1,pin,pintype,&nodeid2);
}

void Insert1Board(char *pchessmap,unsigned short board)
{
	MapNode node,*pcurnode;
	NODEID			nodeid,nodeid2;
	
	memset(&node,0,sizeof(MapNode));
	node.id.district = 0;
	node.id.id		 = GET_NODEID(board,1,1);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][3].f.f  = 1;
	InsertNode(pchessmap,&node,true);
	
	node.id.id		 = GET_NODEID(board,1,2);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_CAMP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
//	node.path[0][1].f.f  = 1;
//	node.path[0][3].f.f  = 1;
//	node.path[0][7].f.f  = 1;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.district = 0;
	nodeid.id 		= GET_NODEID(board,1,1);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_WAY,&(node.id));
	
	node.id.id		 = GET_NODEID(board,1,3);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][3].f.f  = 1;
	node.path[0][7].f.f  = 1;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,1,2);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_WAY,&(node.id));
	
	node.id.id		 = GET_NODEID(board,1,4);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_CAMP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
//	node.path[0][1].f.f  = 1;
//	node.path[0][3].f.f  = 1;
//	node.path[0][7].f.f  = 1;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,1,3);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_WAY,&(node.id));
	
	node.id.id		 = GET_NODEID(board,1,5);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][7].f.f  = 1;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,1,4);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_WAY,&(node.id));
	/////////////////////////////////////////////////////////////////////////////////
	//第一行棋盘完成
	//
	// +---+   +---+   +---+   +---+   +---+
	// |1,1|---|1,2|---|1,3|---|1,4|---|1,5|
	// +---+   +---+   +---+   +---+   +---+
	// 兵站     大本营    兵站     大本营    兵站
	//////////////////////////////////////////////////////////////////////////////////
	
	node.id.id		 = GET_NODEID(board,2,1);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][2].f.f  = 1;
	node.path[0][3].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[1][3].f.f  = 15;
	node.path[3][1].f.f  = 15;
	InsertNode(pchessmap,&node,true);
	
	node.id.id		 = GET_NODEID(board,2,2);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_CAMP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][3].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[0][7].f.f  = 1;
	node.path[3][7].f.f  = 1;
	node.path[7][3].f.f  = 1;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,2,1);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_RAIL,&(node.id));
	
	node.id.id		 = GET_NODEID(board,2,3);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][2].f.f  = 1;
	node.path[0][3].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[0][7].f.f  = 1;
	node.path[0][8].f.f  = 1;
	node.path[3][7].f.f  = 1;
	node.path[7][3].f.f  = 1;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,2,2);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_RAIL,&(node.id));
	
	node.id.id		 = GET_NODEID(board,2,4);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_CAMP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][3].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[0][7].f.f  = 1;
	node.path[3][7].f.f  = 1;
	node.path[7][3].f.f  = 1;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,2,3);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_RAIL,&(node.id));
	
	node.id.id		 = GET_NODEID(board,2,5);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[0][7].f.f  = 1;
	node.path[0][8].f.f  = 1;
	node.path[1][7].f.f  = 15;
	node.path[7][1].f.f  = 15;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,2,4);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_RAIL,&(node.id));
	/////////////////////////////////////////////////////////////////////////////////
	//第二行棋盘完成
	//
	// +---+   +---+   +---+   +---+   +---+
	// |2,1|===|2,2|===|2,3|===|2,4|===|2,5|
	// +---+   +---+   +---+   +---+   +---+
	// 兵站     兵站      兵站     兵站     兵站
	//////////////////////////////////////////////////////////////////////////////////
	
	//连接1,2行
	nodeid2.district = 0;
	nodeid.id 		= GET_NODEID(board,2,1);
	nodeid2.id 		= GET_NODEID(board,1,1);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	
	nodeid.id 		= GET_NODEID(board,2,2);
	nodeid2.id 		= GET_NODEID(board,1,2);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	
	nodeid.id 		= GET_NODEID(board,2,3);
	nodeid2.id 		= GET_NODEID(board,1,3);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	
	nodeid.id 		= GET_NODEID(board,2,4);
	nodeid2.id 		= GET_NODEID(board,1,4);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	
	nodeid.id 		= GET_NODEID(board,2,5);
	nodeid2.id 		= GET_NODEID(board,1,5);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	
	//////////////////////////////////////////////////////////
	
	node.id.id		 = GET_NODEID(board,3,1);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][3].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[1][5].f.f  = 1;
	node.path[5][1].f.f  = 1;
	InsertNode(pchessmap,&node,true);
	
	node.id.id		 = GET_NODEID(board,3,2);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STATION;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][2].f.f  = 1;
	node.path[0][3].f.f  = 1;
	node.path[0][4].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[0][6].f.f  = 1;
	node.path[0][7].f.f  = 1;
	node.path[0][8].f.f  = 1;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,3,1);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_WAY,&(node.id));
	
	node.id.id		 = GET_NODEID(board,3,3);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][3].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[0][7].f.f  = 1;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,3,2);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_WAY,&(node.id));
	
	node.id.id		 = GET_NODEID(board,3,4);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STATION;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][2].f.f  = 1;
	node.path[0][3].f.f  = 1;
	node.path[0][4].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[0][6].f.f  = 1;
	node.path[0][7].f.f  = 1;
	node.path[0][8].f.f  = 1;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,3,3);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_WAY,&(node.id));
	
	node.id.id		 = GET_NODEID(board,3,5);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[0][7].f.f  = 1;
	node.path[1][5].f.f  = 1;
	node.path[5][1].f.f  = 1;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,3,4);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_WAY,&(node.id));
	/////////////////////////////////////////////////////////////////////////////////
	//第三行棋盘完成
	//
	// +---+   +---+   +---+   +---+   +---+
	// |3,1|---|3,2|---|3,3|---|3,4|---|3,5|
	// +---+   +---+   +---+   +---+   +---+
	// 兵站     行营      兵站     行营     兵站
	//////////////////////////////////////////////////////////////////////////////////
	//
	//连接2,3行
	nodeid.id	= GET_NODEID(board,3,1);
	nodeid2.id	= GET_NODEID(board,2,1);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_RAIL,&nodeid2);
	
	nodeid.id	= GET_NODEID(board,3,2);
	SetPin(pchessmap,&nodeid,6,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	nodeid2.id	= GET_NODEID(board,2,2);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	nodeid2.id	= GET_NODEID(board,2,3);
	SetPin(pchessmap,&nodeid,4,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	
	nodeid.id	= GET_NODEID(board,3,3);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	
	nodeid.id	= GET_NODEID(board,3,4);
	SetPin(pchessmap,&nodeid,6,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	nodeid2.id	= GET_NODEID(board,2,4);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	nodeid2.id	= GET_NODEID(board,2,5);
	SetPin(pchessmap,&nodeid,4,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	
	nodeid.id	= GET_NODEID(board,3,5);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_RAIL,&nodeid2);
	
	/////////////////////////////////////////////////////////////////////////////
	
	node.id.id		 = GET_NODEID(board,4,1);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][2].f.f  = 1;
	node.path[0][3].f.f  = 1;
	node.path[0][4].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[1][5].f.f  = 1;
	node.path[5][1].f.f  = 1;
	InsertNode(pchessmap,&node,true);
	
	node.id.id		 = GET_NODEID(board,4,2);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][3].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[0][7].f.f  = 1;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,4,1);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_WAY,&(node.id));
	
	node.id.id		 = GET_NODEID(board,4,3);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STATION;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][2].f.f  = 1;
	node.path[0][3].f.f  = 1;
	node.path[0][4].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[0][6].f.f  = 1;
	node.path[0][7].f.f  = 1;
	node.path[0][8].f.f  = 1;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,4,2);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_WAY,&(node.id));
	
	node.id.id		 = GET_NODEID(board,4,4);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][3].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[0][7].f.f  = 1;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,4,3);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_WAY,&(node.id));
	
	node.id.id		 = GET_NODEID(board,4,5);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[0][6].f.f  = 1;
	node.path[0][7].f.f  = 1;
	node.path[0][8].f.f  = 1;
	node.path[1][5].f.f  = 1;
	node.path[5][1].f.f  = 1;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,4,4);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_WAY,&(node.id));
	/////////////////////////////////////////////////////////////////////////////////
	//第四行棋盘完成
	//
	// +---+   +---+   +---+   +---+   +---+
	// |4,1|---|4,2|---|4,3|---|4,4|---|4,5|
	// +---+   +---+   +---+   +---+   +---+
	// 兵站     兵站      行营     兵站     兵站
	//////////////////////////////////////////////////////////////////////////////////
	//
	//连接3,4行
	nodeid.id	= GET_NODEID(board,4,1);
	nodeid2.id	= GET_NODEID(board,3,1);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_RAIL,&nodeid2);
	nodeid2.id	= GET_NODEID(board,3,2);
	SetPin(pchessmap,&nodeid,4,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	
	nodeid.id	= GET_NODEID(board,4,2);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	
	nodeid.id	= GET_NODEID(board,4,3);
	SetPin(pchessmap,&nodeid,6,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	nodeid2.id	= GET_NODEID(board,3,3);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	nodeid2.id	= GET_NODEID(board,3,4);
	SetPin(pchessmap,&nodeid,4,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	
	nodeid.id	= GET_NODEID(board,4,4);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	
	nodeid.id	= GET_NODEID(board,4,5);
	SetPin(pchessmap,&nodeid,6,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	nodeid2.id	= GET_NODEID(board,3,5);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_RAIL,&nodeid2);
	
	/////////////////////////////////////////////////////////////////////
	
	node.id.id		 = GET_NODEID(board,5,1);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][3].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[1][5].f.f  = 1;
	node.path[5][1].f.f  = 1;
	InsertNode(pchessmap,&node,true);
	
	node.id.id		 = GET_NODEID(board,5,2);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STATION;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][2].f.f  = 1;
	node.path[0][3].f.f  = 1;
	node.path[0][4].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[0][6].f.f  = 1;
	node.path[0][7].f.f  = 1;
	node.path[0][8].f.f  = 1;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,5,1);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_WAY,&(node.id));
	
	node.id.id		 = GET_NODEID(board,5,3);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][3].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[0][7].f.f  = 1;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,5,2);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_WAY,&(node.id));
	
	node.id.id		 = GET_NODEID(board,5,4);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STATION;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][2].f.f  = 1;
	node.path[0][3].f.f  = 1;
	node.path[0][4].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[0][6].f.f  = 1;
	node.path[0][7].f.f  = 1;
	node.path[0][8].f.f  = 1;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,5,3);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_WAY,&(node.id));
	
	node.id.id		 = GET_NODEID(board,5,5);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[0][7].f.f  = 1;
	node.path[1][5].f.f  = 1;
	node.path[5][1].f.f  = 1;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,5,4);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_WAY,&(node.id));
	/////////////////////////////////////////////////////////////////////////////////
	//第五行棋盘完成
	//
	// +---+   +---+   +---+   +---+   +---+
	// |5,1|---|5,2|---|5,3|---|5,4|---|5,5|
	// +---+   +---+   +---+   +---+   +---+
	// 兵站     行营      兵站     行营     兵站
	//////////////////////////////////////////////////////////////////////////////////
	//
	//连接4,5行
	nodeid.id	= GET_NODEID(board,5,1);
	nodeid2.id	= GET_NODEID(board,4,1);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_RAIL,&nodeid2);
	
	nodeid.id	= GET_NODEID(board,5,2);
	SetPin(pchessmap,&nodeid,6,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	nodeid2.id	= GET_NODEID(board,4,2);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	nodeid2.id	= GET_NODEID(board,4,3);
	SetPin(pchessmap,&nodeid,4,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	
	nodeid.id	= GET_NODEID(board,5,3);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	
	nodeid.id	= GET_NODEID(board,5,4);
	SetPin(pchessmap,&nodeid,6,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	nodeid2.id	= GET_NODEID(board,4,4);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	nodeid2.id	= GET_NODEID(board,4,5);
	SetPin(pchessmap,&nodeid,4,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	
	nodeid.id	= GET_NODEID(board,5,5);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_RAIL,&nodeid2);
	
	////////////////////////////////////////////////////////////////////////
	
	node.id.id		 = GET_NODEID(board,6,1);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][3].f.f  = 1;
	node.path[0][4].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[0][8].f.f  = 1;
	node.path[1][5].f.f  = 1;
	node.path[5][1].f.f  = 1;
	node.path[8][5].f.f  = 1;
	node.path[5][8].f.f  = 1;
	node.path[3][5].f.f  = 15;
	node.path[5][3].f.f  = 15;
	node.path[3][1].f.f  = 15;
	node.path[1][3].f.f  = 15;
	node.path[8][3].f.f  = 15;
	node.path[3][8].f.f  = 15;
	InsertNode(pchessmap,&node,true);
	
	node.id.id		 = GET_NODEID(board,6,2);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][3].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[0][7].f.f  = 1;
	node.path[3][7].f.f  = 1;
	node.path[7][3].f.f  = 1;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,6,1);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_RAIL,&(node.id));
	
	node.id.id		 = GET_NODEID(board,6,3);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][3].f.f  = 1;
	node.path[0][4].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[0][6].f.f  = 1;
	node.path[0][7].f.f  = 1;
	node.path[3][7].f.f  = 1;
	node.path[7][3].f.f  = 1;
	node.path[1][3].f.f  = 15;
	node.path[3][1].f.f  = 15;
	node.path[7][1].f.f  = 15;
	node.path[1][7].f.f  = 15;
	
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,6,2);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_RAIL,&(node.id));
	
	node.id.id		 = GET_NODEID(board,6,4);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][3].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[0][7].f.f  = 1;
	node.path[3][7].f.f  = 1;
	node.path[7][3].f.f  = 1;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,6,3);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_RAIL,&(node.id));
	
	node.id.id		 = GET_NODEID(board,6,5);
	node.id.type.type	 = DJGAME_JUNQI_NODETYPE_STOP;
	memset(node.path,0x7F,sizeof(PATHDES)*(DJGAME_CHESSMAP_MAX_PINS+1)*(DJGAME_CHESSMAP_MAX_PINS+1));
	node.path[0][1].f.f  = 1;
	node.path[0][2].f.f  = 1;
	node.path[0][5].f.f  = 1;
	node.path[0][6].f.f  = 1;
	node.path[0][7].f.f  = 1;
	node.path[1][5].f.f  = 1;
	node.path[5][1].f.f  = 1;
	node.path[2][5].f.f  = 1;
	node.path[5][2].f.f  = 1;
	node.path[7][5].f.f  = 15;
	node.path[5][7].f.f  = 15;
	node.path[7][1].f.f  = 15;
	node.path[1][7].f.f  = 15;
	node.path[2][7].f.f  = 15;
	node.path[7][2].f.f  = 15;
	pcurnode = InsertNode(pchessmap,&node,true);
	
	nodeid.id 		= GET_NODEID(board,6,4);
	SetPin(pchessmap,&nodeid,3,DJGAME_JUNQI_PINTYPE_WAY,&(node.id));
	/////////////////////////////////////////////////////////////////////////////////
	//第六行棋盘完成
	//
	// +---+   +---+   +---+   +---+   +---+
	// |6,1|---|6,2|---|6,3|---|6,4|---|6,5|
	// +---+   +---+   +---+   +---+   +---+
	// 兵站     兵站      兵站     兵站     兵站
	//////////////////////////////////////////////////////////////////////////////////
	//
	//连接5,6行
	nodeid.id	= GET_NODEID(board,6,1);
	nodeid2.id	= GET_NODEID(board,5,1);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_RAIL,&nodeid2);
	nodeid2.id	= GET_NODEID(board,5,2);
	SetPin(pchessmap,&nodeid,4,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	
	nodeid.id	= GET_NODEID(board,6,2);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	
	nodeid.id	= GET_NODEID(board,6,3);
	SetPin(pchessmap,&nodeid,6,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	nodeid2.id	= GET_NODEID(board,5,3);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	nodeid2.id	= GET_NODEID(board,5,4);
	SetPin(pchessmap,&nodeid,4,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	
	nodeid.id	= GET_NODEID(board,6,4);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	
	nodeid.id	= GET_NODEID(board,6,5);
	SetPin(pchessmap,&nodeid,6,DJGAME_JUNQI_PINTYPE_WAY,&nodeid2);
	nodeid2.id	= GET_NODEID(board,5,5);
	SetPin(pchessmap,&nodeid,5,DJGAME_JUNQI_PINTYPE_RAIL,&nodeid2);
}

//////////////////////////////////////////////////////////////
short g_jqmapxy_2[][4]=
{
{2,0x15 , 22,11},	{2,0x14 , 77,11},	{2,0x13 , 132,11},	{2,0x12 , 187,11},	{2,0x11 , 242,11},
{2,0x25 , 22,66},	{2,0x24 , 77,66},	{2,0x23 , 132,66},	{2,0x22 , 187,66},	{2,0x21 , 242,66},
{2,0x35 , 22,121},	{2,0x34 , 77,121},	{2,0x33 , 132,121},	{2,0x32 , 187,121},	{2,0x31 , 242,121},
{2,0x45 , 22,176},	{2,0x44 , 77,176},	{2,0x43 , 132,176},	{2,0x42 , 187,176},	{2,0x41 , 242,176},
{2,0x55 , 22,231},	{2,0x54 , 77,231},	{2,0x53 , 132,231},	{2,0x52 , 187,231},	{2,0x51 , 242,231},
{2,0x65 , 22,286},	{2,0x64 , 77,286},	{2,0x63 , 132,286},	{2,0x62 , 187,286},	{2,0x61 , 242,286},

{1,0x61 , 22,418},	{1,0x62 , 77,418},	{1,0x63 , 132,418},	{1,0x64 , 187,418},	{1,0x65 , 242,418},
{1,0x51 , 22,473},	{1,0x52 , 77,473},	{1,0x53 , 132,473},	{1,0x54 , 187,473},	{1,0x55 , 242,473},
{1,0x41 , 22,528},	{1,0x42 , 77,528},	{1,0x43 , 132,528},	{1,0x44 , 187,528},	{1,0x45 , 242,528},
{1,0x31 , 22,583},	{1,0x32 , 77,583},	{1,0x33 , 132,583},	{1,0x34,  187,583},	{1,0x35 , 242,583},
{1,0x21 , 22,638},	{1,0x22 , 77,638},	{1,0x23 , 132,638},	{1,0x24 , 187,638},	{1,0x25 , 242,638},
{1,0x11 , 22,693},	{1,0x12 , 77,693},	{1,0x13 , 132,693},	{1,0x14 , 187,693},	{1,0x15 , 242,693},
{0,0,0,0}
};

short g_jqmapxy_3[][4]=
{
{DJGAME_JUNQI_CONNECTER_AREA,0xFF,473,132},
{1,0x61 , 363,319},{1,0x62 , 418,319},{1,0x63 , 473,319},{1,0x64 , 528,319},{1,0x65 , 583,319},
{1,0x51 , 363,374},{1,0x52 ,  418,374},{1,0x53 , 473,374},{1,0x54 , 528,374},{1,0x55 , 583,374},
{1,0x41 , 363,429},{1,0x42 ,  418,429},{1,0x43 , 473,429},{1,0x44 , 528,429},{1,0x45 , 583,429},
{1,0x31 , 363,484},{1,0x32 ,  418,484},{1,0x33 , 473,484},{1,0x34 , 528,484},{1,0x35 , 583,484},
{1,0x21 , 363,539},{1,0x22 ,  418,539},{1,0x23 , 473,539},{1,0x24 , 528,539},{1,0x25 , 583,539},
{1,0x11 , 363,594},{1,0x12 ,  418,594},{1,0x13 , 473,594},{1,0x14 , 528,594},{1,0x15 , 583,594},
	
{2,0x61 , 286,22},	{2,0x62 , 286,77},	{2,0x63 , 286,132},	{2,0x64 , 286,187},	{2,0x65 , 286,242},
{2,0x51 , 231,22},	{2,0x52 , 231,77},	{2,0x53 , 231,132},	{2,0x54 , 231,187},	{2,0x55 , 231,242},
{2,0x41 , 176,22},	{2,0x42 , 176,77},	{2,0x43 , 176,132},	{2,0x44 , 176,187},	{2,0x45 , 176,242},
{2,0x31 , 121,22},	{2,0x32 , 121,77},	{2,0x33 , 121,132},	{2,0x34 , 121,187},	{2,0x35 , 121,242},
{2,0x21 , 66,22},	{2,0x22 , 66,77},	{2,0x23 , 66,132},	{2,0x24 , 66,187},	{2,0x25 , 66,242},
{2,0x11 , 11,22},	{2,0x12 , 11,77},	{2,0x13 , 11,132},	{2,0x14 , 11,187},	{2,0x15 , 11,242},

{3,0x61 , 660,242},{3,0x62 , 660,187},{3,0x63 , 660,132},{3,0x64 , 660,77},{3,0x65 , 660,22},
{3,0x51 , 715,242},{3,0x52 , 715,187},{3,0x53 , 715,132},{3,0x54 , 715,77},{3,0x55 , 715,22},
{3,0x41 , 770,242},{3,0x42 , 770,187},{3,0x43 , 770,132},{3,0x44 , 770,77},	{3,0x45 , 770,22},
{3,0x31 , 825,242},{3,0x32 , 825,187},{3,0x33 , 825,132},{3,0x34 , 825,77},	{3,0x35 , 825,22},
{3,0x21 , 880,242},{3,0x22 , 880,187},{3,0x23 , 880,132}, {3,0x24 , 880,77},	{3,0x25 , 880,22},
{3,0x11 , 935,242},{3,0x12 , 935,187}, {3,0x13 , 935,132}, {3,0x14 , 935,77},	{3,0x15 , 935,22},
	{0,0,0,0}
};

short g_jqmapxy_4[][4]=
{
{DJGAME_JUNQI_CONNECTER_AREA,0xFF,473,473},
{DJGAME_JUNQI_CONNECTER_AREA,0x01 , 473,583},
{DJGAME_JUNQI_CONNECTER_AREA,0x02 , 363,473},
{DJGAME_JUNQI_CONNECTER_AREA,0x04 , 473,363},
{DJGAME_JUNQI_CONNECTER_AREA,0x08 , 583,473},
{DJGAME_JUNQI_CONNECTER_AREA,0x03 , 363,583},
{DJGAME_JUNQI_CONNECTER_AREA,0x06 , 363,363},
{DJGAME_JUNQI_CONNECTER_AREA,0x0C , 583,363},
{DJGAME_JUNQI_CONNECTER_AREA,0x09 , 583,583},

{1,0x61 , 363,660},{1,0x62 , 418,660},{1,0x63 , 473,660},{1,0x64 , 528,660},{1,0x65 , 583,660},
{1,0x51 , 363,715},{1,0x52 , 418,715},{1,0x53 , 473,715},{1,0x54 , 528,715},{1,0x55 , 583,715},
{1,0x41 , 363,770},{1,0x42 , 418,770},{1,0x43 , 473,770},{1,0x44 , 528,770},{1,0x45 , 583,770},
{1,0x31 , 363,825},{1,0x32 , 418,825},{1,0x33 , 473,825},{1,0x34 , 528,825},{1,0x35 , 583,825},
{1,0x21 , 363,880},{1,0x22 , 418,880},{1,0x23 , 473,880},{1,0x24 , 528,880},{1,0x25 , 583,880},
{1,0x11 , 363,935},{1,0x12 , 418,935},{1,0x13 , 473,935},{1,0x14 , 528,935},{1,0x15 , 583,935},

{2,0x61 , 286,363},{2,0x62 , 286,418},{2,0x63 , 286,473},{2,0x64 , 286,528},{2,0x65 , 286,583},
{2,0x51 , 231,363},{2,0x52 , 231,418},{2,0x53 , 231,473},{2,0x54 , 231,528},{2,0x55 , 231,583},
{2,0x41 , 176,363},{2,0x42 , 176,418},{2,0x43 , 176,473},{2,0x44 , 176,528},{2,0x45 , 176,583},
{2,0x31 , 121,363}, {2,0x32 , 121,418}, {2,0x33 , 121,473}, {2,0x34 , 121,528},	{2,0x35 , 121,583},
{2,0x21 , 66,363}, {2,0x22 , 66,418}, {2,0x23 , 66,473}, {2,0x24 , 66,528},	{2,0x25 , 66,583},
{2,0x11 , 11,363},  {2,0x12 ,11,418,}, {2,0x13 ,11,473},	 {2,0x14 ,11,528},	{2,0x15 ,11,583},

{3,0x61 , 583,286},{3,0x62 , 528,286},{3,0x63 , 473,286},{3,0x64 , 418,286},{3,0x65 , 363,286},
{3,0x51 , 583,231},{3,0x52 , 528,231},{3,0x53 , 473,231},{3,0x54 , 418,231},{3,0x55 , 363,231},
{3,0x41 , 583,176},{3,0x42 , 528,176},{3,0x43 , 473,176},{3,0x44 , 418,176},{3,0x45 , 363,176},
{3,0x31 , 583,121}, {3,0x32 , 528,121}, {3,0x33 , 473,121}, {3,0x34 , 418,121},	{3,0x35 , 363,121},
{3,0x21 , 583,66}, {3,0x22 , 528,66}, {3,0x23 , 473,66}, {3,0x24 , 418,66},	{3,0x25 , 363,66},
{3,0x11 , 583,11},  {3,0x12 , 528,11},  {3,0x13 , 473,11},	 {3,0x14 , 418,11},	{3,0x15 , 363,11},

{4,0x61 , 660,583},{4,0x62 , 660,528},{4,0x63 , 660,473},{4,0x64 , 660,418},{4,0x65 , 660,363},
{4,0x51 , 715,583},{4,0x52 , 715,528},{4,0x53 , 715,473},{4,0x54 , 715,418},{4,0x55 , 715,363},
{4,0x41 , 770,583},{4,0x42 , 770,528},{4,0x43 , 770,473},{4,0x44 , 770,418},{4,0x45 , 770,363},
{4,0x31 , 825,583},{4,0x32 , 825,528},{4,0x33 , 825,473},{4,0x34 , 825,418},{4,0x35 , 825,363},
{4,0x21 , 880,583},{4,0x22 , 880,528},{4,0x23 , 880,473},{4,0x24 , 880,418},{4,0x25 , 880,363},
{4,0x11 , 935,583},{4,0x12 , 935,528},{4,0x13 , 935,473},{4,0x14 , 935,418},{4,0x15 , 935,363},
	{0,0,0,0}
};


void InitJunQiNodeXY(char *pchessmap,unsigned char chCountrys,unsigned char chFirstSite)
{
	short *xy=NULL;
	short id,area;
	int i;
	PMapNode pnode;
	NODEID	nodeid;
	
	if(chFirstSite < 1 || chFirstSite > chCountrys)
		chFirstSite = 1;
	switch(chCountrys)
	{
		case 2 : 
			xy = &(g_jqmapxy_2[0][0]);
			break;
		case 3 :
			xy = &(g_jqmapxy_3[0][0]);
			break;
		case 4 :
			xy = &(g_jqmapxy_4[0][0]);
			break;
		default :
			xy = NULL;
	}
	if(xy != NULL)
	{
		nodeid.district = 0;
		while(*xy != 0)
		{
			id = *(xy+1);
			area = *xy;
			//DEBUG_OUTPUT(("default node : %d-%02x ",area,id));
			if(area != DJGAME_JUNQI_CONNECTER_AREA)
			{
				area = (area+(chFirstSite - 1))%chCountrys;
				if(area == 0)
					area = chCountrys;
			}else if(chFirstSite != 1)
			{
				if(id != 0xFF)
				{
					for(i=0;i<chFirstSite-1;i++)
					{
						id <<= 1;
						if(id & 0x10)
						{
							id |= 0x01;
							id &= 0x0F;
						}
					}
				}
			}
			//DEBUG_OUTPUT(("turn to : %d-%02x \n",area,id));
			nodeid.id = GET_NODEID(area,0,id);
			pnode = SearchNode(pchessmap,&nodeid);
			if(pnode != NULL)
			{
				pnode->x = *(xy+2);
				pnode->y = *(xy+3);
			}else
			{
				//DEBUG_OUTPUT(("NO SUCH NODE : %d-%02x\n",*xy,*(xy+1)));
			}
			xy += 4;
		}
	}
	
}

/*
/////////////////////////////////////////////
void __debug_junqi_arrange(PGeneralGameTrace2Head ptrace)
{
	int i;
	char *pname[]={"NULL",
					"DJGAME_JUNQI_RANK_BING",
					"DJGAME_JUNQI_RANK_PAI",
					"DJGAME_JUNQI_RANK_LIAN",
					"DJGAME_JUNQI_RANK_YING",
					"DJGAME_JUNQI_RANK_TUAN",
					"DJGAME_JUNQI_RANK_LVE",
					"DJGAME_JUNQI_RANK_SHI",
					"DJGAME_JUNQI_RANK_JUN",
					"DJGAME_JUNQI_RANK_SILING",
					"DJGAME_JUNQI_RANK_DILEI",
					"DJGAME_JUNQI_RANK_JUNQI",
					"DJGAME_JUNQI_RANK_ZHADAN",
					"DJGAME_JUNQI_RANK_WEIZHI"
	};
	PJunqiTraceInitChip psetchips = (PJunqiTraceInitChip)ptrace->chBuf;
	DEBUG_OUTPUT(("JUNQI_ARRANGE : chSite = %02x ,chBufLen =%02x , mapsite=%02x , chips=%d\n",ptrace->chSite,ptrace->chBufLen,psetchips->chMapSite,psetchips->chChips));
	for(i=0;i<psetchips->chChips;i++)
	{
		DEBUG_OUTPUT(("chip%d : pos=%02x id=%s owner=%d power=%d\n",i,psetchips->chip[i].xy,pname[psetchips->chip[i].chip.id],psetchips->chip[i].chip.owner,psetchips->chip[i].chip.power));
	}
}

void __debug_junqi_status(char *pchessmap)
{
	PChessMapHead phead  = (PChessMapHead)GetChessmapHead(pchessmap);
	if(phead == NULL || phead->shCurNodes == 0)
		return ;
	int i;
	char *pname[]={"NULL",
					"DJGAME_JUNQI_RANK_BING",
					"DJGAME_JUNQI_RANK_PAI",
					"DJGAME_JUNQI_RANK_LIAN",
					"DJGAME_JUNQI_RANK_YING",
					"DJGAME_JUNQI_RANK_TUAN",
					"DJGAME_JUNQI_RANK_LVE",
					"DJGAME_JUNQI_RANK_SHI",
					"DJGAME_JUNQI_RANK_JUN",
					"DJGAME_JUNQI_RANK_SILING",
					"DJGAME_JUNQI_RANK_DILEI",
					"DJGAME_JUNQI_RANK_JUNQI",
					"DJGAME_JUNQI_RANK_ZHADAN",
					"DJGAME_JUNQI_RANK_WEIZHI"
	};
	for(i=0;i<phead->shCurNodes;i++)
	{
		if(phead->pnode[i]->chip.id != 0)
		{
			DEBUG_OUTPUT(("%04x = %s\n",phead->pnode[i]->id.id,pname[phead->pnode[i]->chip.id]));
		}
	}
}
*/