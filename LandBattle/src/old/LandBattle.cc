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

#include "DJGameController.h"
#include "DJClientRequest.h"
#include "DJGameUser.h"
#include "DJGameRoom.h"
#include "DJTreeWidget.h"
#include "Utility.h"
#include "DJGameImage.h"
#include "DJGameDesktop.h"
#include "DJAutoDisappearText.h"
#include "bufalloc.h"
#include "LandBattle.h"
#include "4country.h"
#include "DJTreeItemDelegate.h"
#include "DJMatrixCanvasTimer.h"
#include "DJToolButton.h"
#include "DJSchemeEvent.h"
#include "DJGroupChat.h"
#include "DJTextBrowser.h"
#include "DJGlobal.h" 

void  HandleArrangeACL( const QByteArray& buf, const QVariant& parameters );
void  HandleMoveACL( const QByteArray& buf, const QVariant& parameters );

void  aclRequestDrawProc( const QByteArray& buf, const QVariant& parameters )
{
	const DJGameProtocol *protocol	= reinterpret_cast<const DJGameProtocol *>(buf.data());
	quint8 status	= protocol->chBuf[0];
	djDebug() << "aclRequestDrawProc" << hex << status;
	QPushButton* button = static_cast<QPushButton *>(parameters.value<void*>());
	if ( DJGAME_SUCCESS(status) ) {
		button->setEnabled( false );
	}
}

void InitSaveFileHead(DJGamePanel *panel,QString &name,unsigned short shGameID,unsigned char chType);
void AppendData2SaveFile(QString &name,char *data,unsigned int shLen);
char * GetSaveData(QString &name,unsigned short shGameID,unsigned char chType,unsigned int *iLen);


/////////////////////////////////////////////////////////////////////////////////////////
LandBattleDesktop::LandBattleDesktop( QWidget* parent,DJGamePanel *panel,quint32 w,quint32 h)
			: DJGameDesktop(parent,panel,w,h)
{	
	memset(&c_current,0,sizeof(JunqiCurrent));
	
	c_LandBattlePanel = (LandBattlePanel *)panel;
	c_chUnions = 0;
	memset(c_chUnion,0,5);
	
	c_itemBoard = NULL;
	RepaintChessmap(numberOfPlayers());
	
	QMatrix m = GetCurrentMatrix();
	
	QSize iconSize( 19 * 4, 19 );
	c_pbSave = new QPushButton(this);	//Save Scene
	QPixmap pix = QPixmap(":/BaseRes/image/desktop/pushbutton/BaoCunBuJu.png");
	c_pbSave->setIcon( pix );
	c_pbSave->setIconSize( iconSize );
	c_pbSave->adjustSize();
	connect(c_pbSave,SIGNAL(clicked()),this,SLOT(ClickSave()));
	c_pbSave->hide();
	
	c_pbLoad = new QPushButton(this);
	pix = QPixmap(":/BaseRes/image/desktop/pushbutton/ZhuangRuBuJu.png");
	c_pbLoad->setIcon( pix );
	c_pbLoad->setIconSize( iconSize );
	c_pbLoad->adjustSize();
	connect(c_pbLoad,SIGNAL(clicked()),this,SLOT(ClickLoad()));
	c_pbLoad->hide();
	
	c_pbSend = new QPushButton(this);
	pix = QPixmap(":/BaseRes/image/desktop/pushbutton/ChuZheng.png");
	c_pbSend->setIcon( pix );
	c_pbSend->setIconSize( iconSize );
	c_pbSend->adjustSize();
	connect(c_pbSend,SIGNAL(clicked()),this,SLOT(ClickSend()));
	c_pbSend->hide();
	
	c_pbSurrender= new QPushButton(this);
	pix = QPixmap(":/BaseRes/image/desktop/pushbutton/RenShu.png");
	c_pbSurrender->setIcon( pix );
	c_pbSurrender->setIconSize( pix.size() );
	c_pbSurrender->adjustSize();
	connect(c_pbSurrender,SIGNAL(clicked()),this,SLOT(ClickSurrender()));
	
	c_pbDuce= new QPushButton(this);	
	pix = QPixmap(":/BaseRes/image/desktop/pushbutton/QiuHe.png");
	c_pbDuce->setIcon( pix );
	c_pbDuce->setIconSize( pix.size() );
	c_pbDuce->adjustSize();
	connect(c_pbDuce,SIGNAL(clicked()),this,SLOT(ClickDuce()));
	
	c_timerExchange = new QTimer();
	c_timerExchange->setSingleShot(true);
    connect(c_timerExchange,SIGNAL(timeout()),this,SLOT(HandleExchangeTimeout()));
	c_MoveTimer = new QTimer(this);
	c_MoveTimer->setSingleShot(true);
    connect(c_MoveTimer,SIGNAL(timeout()),this,SLOT(HandleMoveTimeout()));
	
	StaticInitDesktop();	
}
LandBattleDesktop::~LandBattleDesktop()
{
}
void LandBattleDesktop::layoutButtons()
{
	djDebug()<<"LandBattleDesktop::layoutButtons";
	QMatrix	m	= GetCurrentMatrix();
	
	unsigned char chPlayers = numberOfPlayers();
	djDebug()<<"chPlayers="<<chPlayers;
	int x,y;
	switch(chPlayers)
	{
		case 2 :
		{
			x = 320+c_X;
			break;
		}
		default :
		{
			x = 700+c_X;
			break;
		}
	}
	
	y = GetRealHeight()-10;
	int tx,ty;
	m.map(x,y,&tx,&ty);
	
	djDebug()<<"x="<<x<<"y="<<y<<"tx="<<tx<<"ty="<<ty;
	
	c_pbSave->move(tx,ty-c_pbSave->height());
	c_pbSurrender->move(tx,ty-c_pbSurrender->height());
	
	c_pbLoad->move(tx,ty-c_pbSave->height()-c_pbLoad->height()-10);
	c_pbDuce->move(tx,ty-c_pbSurrender->height()-c_pbDuce->height()-10);
	
	c_pbSend->move(tx,ty-c_pbSave->height()-c_pbLoad->height()-c_pbSend->height()-20);	
}
void LandBattleDesktop::handleMatrixChanged(const QMatrix &m)
{
	DJGameDesktop::handleMatrixChanged(m);
	
	layoutButtons();
}

void LandBattleDesktop::userArrived( quint32 userId, quint8 seatId )
{
	DJGameDesktop::userArrived( userId, seatId );
	
	layoutButtons();
	
//	AppendBlackOrRedSuffix();	
}
void LandBattleDesktop::gameInfo( const unsigned char *buf )
{
	DJGameDesktop::gameInfo( buf );
		
	memcpy(&c_current,buf,sizeof(JunqiCurrent));
	
	djDebug()<<"gameinfo::chPlayers="<<c_current.chPlayers;
	
	if ( 2 == c_current.chPlayers )
		setRealSize(750,750);
	else
		setRealSize(950,950);
	
	StaticInitDesktop();
	
	if(c_LandBattlePanel->isLookingOn())
	{
		c_pbDuce->setEnabled(false);
		c_pbSurrender->setEnabled(false);
	}else
	{
		c_pbDuce->setEnabled(true);
		c_pbSurrender->setEnabled(true);
	}
	ClearImage(0,0,DESKTOP_IMAGE_TYPE_CHIP);
	RepaintChessmap(c_current.chPlayers);
	memset(c_chessmap,0,LANDBATTLEPANEL_CHESSMAP_MAXSIZE);
	switch(c_current.chMapID)
	{
		case JUNQI_MAP_NORMAL2 :
			DEBUG_OUTPUT(("JUNQI_MAP_NORMAL2\n"));
			Init2CountryChessmap(c_chessmap,LANDBATTLEPANEL_CHESSMAP_MAXSIZE,GetMapSiteOfSeat(c_LandBattlePanel->selfSeatId()));
			break;
		case JUNQI_MAP_NORMAL3 :
			DEBUG_OUTPUT(("JUNQI_MAP_NORMAL3\n"));
			Init3CountryChessmap(c_chessmap,LANDBATTLEPANEL_CHESSMAP_MAXSIZE,GetMapSiteOfSeat(c_LandBattlePanel->selfSeatId()));
			break;
		case JUNQI_MAP_NORMAL4 : 
			DEBUG_OUTPUT(("JUNQI_MAP_NORMAL4\n"));
			Init4CountryChessmap(c_chessmap,LANDBATTLEPANEL_CHESSMAP_MAXSIZE,GetMapSiteOfSeat(c_LandBattlePanel->selfSeatId()));
			break;
		case JUNQI_MAP_4ROW :
			break;
	}
	layoutButtons();
}

void LandBattleDesktop::DrawNodeChip(PMapNode pnode,bool bBig)
{
	int iIndex;
	char chBoard;
	if(pnode == NULL)
		return;
	if(pnode->chip.id == 0 || pnode->chip.owner == 0)
		return;
	if(pnode->chip.id == DJGAME_JUNQI_RANK_WEIZHI)
		iIndex = 0;
	else
	{
		iIndex = pnode->chip.id;
	}

	chBoard = GetPhysicalBoard(pnode);
	
	char szName[255];
	sprintf(szName,":/LandBattleRes/image/jqchip%d_%d_%02X.png",chBoard,pnode->chip.owner,iIndex);
	DEBUG_OUTPUT((szName));
	DEBUG_OUTPUT(("\n"));
	QPixmap pix = QPixmap(QString(szName));
	DJGameImageItem *item;
	QImage img;
	
	if(!pix.isNull())
	{
		img = pix.convertToImage(); 
		QMatrix m = GetCurrentMatrix();
		if(bBig)
			img = img.smoothScale(pix.width()+5,pix.height()+5);
		item = new DJGameImageItem(img,canvas(),m,pnode->id.id,pnode->chip.owner,DESKTOP_IMAGE_TYPE_CHIP,false);
		item->setVAlignment(Qt::AlignVCenter);
		item->setHAlignment(Qt::AlignHCenter);
		item->move(pnode->x+c_X,pnode->y+c_Y);
		item->show();
		item->setMatrix(m);
		item->setZ(500);
		if(bBig)
			item->setMark(true);
	}
	
}

char LandBattleDesktop::GetPhysicalBoard(PMapNode pnode)
{
	PChessMapHead phead  = (PChessMapHead)GetChessmapHead(c_chessmap);
	if(phead->iContext == 2)
		return 1;
	
	char iBoard = GET_BoardOfNode(&(pnode->id));
	if(iBoard == DJGAME_JUNQI_CONNECTER_AREA)
		return 1;
	if(phead->iContext == 4)
	{
		if(iBoard%2 == GetMapSiteOfSeat(c_LandBattlePanel->selfSeatId())%2)
			return 1;
		return 2;
	}
	if(phead->iContext == 3)
	{
		if(iBoard == GetMapSiteOfSeat(c_LandBattlePanel->selfSeatId()))
			return 1;
		return 2;
	}
	return 1;
}


void LandBattleDesktop::ClearSeatChips(quint8 seat)
{
	ClearImage(0,GetMapSiteOfSeat(seat),DESKTOP_IMAGE_TYPE_CHIP);
}

void LandBattleDesktop::ClearMapSiteChips(quint8 mapsite)
{
	ClearImage(0,mapsite,DESKTOP_IMAGE_TYPE_CHIP);
}


void LandBattleDesktop::ClearNodeChip(PMapNode pnode)
{
	DJGameImageItem *item = GetChipOfNode(pnode);
	if(item != NULL)
	{
		delete item;
	}
}

PMapNode	LandBattleDesktop::GetNodeOfChip(Q3CanvasItem *item)
{//
	if(item->rtti() != djgame_imageRTTI)
		return NULL;
	
	DJGameImageItem *imageitem = (DJGameImageItem*)item;
	if(imageitem->type() != DESKTOP_IMAGE_TYPE_CHIP)
		return NULL;
	//unsigned short sid = imageitem->sid();
	NODEID id;
	id.district  = 0;
	id.id = imageitem->tid();
	
	return SearchNode(c_chessmap,&id);
}

DJGameImageItem * LandBattleDesktop::GetChipOfNode(PMapNode pnode)
{
	return FindImageItem(pnode->id.id,pnode->chip.owner,DESKTOP_IMAGE_TYPE_CHIP);
}

void LandBattleDesktop::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	djDebug() << "LandBattleDesktop::gameWait" << mask << status;
	DJGameDesktop::gameWait(mask,status,timeout);
//	if(status == DJGAME_TABLE_STATUS_WAITSTART)
	{
		c_pbSave->hide();
		c_pbLoad->hide();
		c_pbSend->hide();
		c_pbDuce->hide();
		c_pbSurrender->hide();
	}
	c_LastTraceType = 0;
	switch(status)
	{
		case DJGAME_JUNQI_TABLE_STATUS_WAIT_ARRANGE :
		{
			c_pbSave->show();
			c_pbLoad->show();
			c_pbSend->show();
			if(c_LandBattlePanel->isLookingOn() || !IsWaittingForMe())
			{
				c_pbSave->setEnabled(false);
				c_pbLoad->setEnabled(false);
				c_pbSend->setEnabled(false);
				c_bWaitMoveACL = true;
			}else
			{
				c_pbSave->setEnabled(true);
				c_pbLoad->setEnabled(true);
				c_pbSend->setEnabled(true);
				c_bWaitMoveACL = false;
			}
			break;
		}
		case DJGAME_JUNQI_TABLE_STATUS_WAIT_MOVE : 
		{
			c_bWaitMoveACL = false;
			if(IsWaittingForMe())
			{
				c_pbDuce->show();
				c_pbSurrender->show();
			}else
			{
				c_pbDuce->hide();
				c_pbSurrender->hide();
			}
			if(c_pMoveChipNode != NULL)
			{
				ClearNodeChip(c_pMoveChipNode);
				DrawNodeChip(c_pMoveChipNode,false);
				c_pMoveChipNode=NULL;
			}
			break;
		}
	}
}

bool LandBattleDesktop::handleItemClicked(Qt::MouseButton btn,const QPoint &pos,Q3CanvasItem * item)
{
	if(!IsWaittingForMe() || c_bWaitMoveACL || c_LandBattlePanel->isLookingOn())
		return false;
	PMapNode pnode = Point2Node(c_chessmap,pos.x()-c_X,pos.y()-c_Y,LANDBATTLE_CHIP_WIDTH/2,LANDBATTLE_CHIP_WIDTH/2);
	if(pnode == NULL)
		return false;
	char iBoard = GetPhysicalBoard(pnode);
	if(iBoard == 1)
	{
		pnode = Point2Node(c_chessmap,pos.x()-c_X,pos.y()-c_Y,LANDBATTLE_CHIP_WIDTH/2,LANDBATTLE_CHIP_HEIGHT/2);
	}else
	{
		pnode = Point2Node(c_chessmap,pos.x()-c_X,pos.y()-c_Y,LANDBATTLE_CHIP_HEIGHT/2,LANDBATTLE_CHIP_WIDTH/2);
	}
	if(GetTableStatus() == DJGAME_JUNQI_TABLE_STATUS_WAIT_ARRANGE)
	{
		if(pnode == NULL)
		{
			if(c_pArrangeNode1 != NULL && c_pArrangeNode2 == NULL)
			{
				ClearNodeChip(c_pArrangeNode1);
				DrawNodeChip(c_pArrangeNode1,false);
				c_pArrangeNode1 = NULL;
			}
			return false;
		}
		DJGameImageItem * item = GetChipOfNode(pnode);
		if(item == NULL)
			return false;
		if(c_pArrangeNode1 != NULL && c_pArrangeNode2 != NULL)
			HandleExchangeTimeout();
		if(item->sid() != GetMapSiteOfSeat(c_LandBattlePanel->selfSeatId()))
			return false;
		if(c_pArrangeNode1 == NULL)
		{
			ClearNodeChip(pnode);
			DrawNodeChip(pnode,true);
			c_pArrangeNode1 = pnode;
			c_pArrangeNode2 = NULL;
			return true;
		}
		ClearNodeChip(pnode);
		DrawNodeChip(pnode,true);
		c_pArrangeNode2 = pnode;
		c_timerExchange->start(300);
		return true;
	}
	if(GetTableStatus() == DJGAME_JUNQI_TABLE_STATUS_WAIT_MOVE)
	{
		if(pnode == NULL || pnode == c_pMoveChipNode)
		{
			if(c_pMoveChipNode != NULL)
			{
				ClearNodeChip(c_pMoveChipNode);
				DrawNodeChip(c_pMoveChipNode,false);
				c_pMoveChipNode = NULL;
			}
			return false;
		}
		DJGameImageItem * item = GetChipOfNode(pnode);
		if(c_pMoveChipNode == NULL)
		{
			if(item == NULL || item->sid() != GetMapSiteOfSeat(c_LandBattlePanel->selfSeatId()) )
				return false;
			c_pMoveChipNode = pnode;
			ClearNodeChip(pnode);
			DrawNodeChip(pnode,true);
			return true;
		}
		if(pnode->chip.id != 0 && pnode->chip.owner == GetMapSiteOfSeat(c_LandBattlePanel->selfSeatId()))
		{
			ClearNodeChip(c_pMoveChipNode);
			DrawNodeChip(c_pMoveChipNode,false);
			ClearNodeChip(pnode);
			DrawNodeChip(pnode,true);
			c_pMoveChipNode = pnode;
			return true;
		}
		if(item != NULL && IsFriendlyPlayer(c_chUnion,c_chUnions,pnode->chip.owner,c_pMoveChipNode->chip.owner))
		{
			return false;
		}
		unsigned char *p;
	//	printf("move %04X->%04X\n",c_pMoveChipNode->id.id,pnode->id.id);
		if(MoveChip(c_chessmap,GET_BoardOfNode( (&(c_pMoveChipNode->id))),c_pMoveChipNode->id.id & 0xFF,
						GET_BoardOfNode( (&(pnode->id))),pnode->id.id & 0xFF,&p) )
		{
	//		printf("Move OK\n");
			unsigned char buffer[100],chPathLen = 0;
			JunqiTraceMove			*pmove;
			pmove = (JunqiTraceMove *)buffer;
			pmove->chMapSite = GetMapSiteOfSeat(c_LandBattlePanel->selfSeatId());
			pmove->chStartMapSite = GET_BoardOfNode( (&(c_pMoveChipNode->id)));
			pmove->sxy = c_pMoveChipNode->id.id & 0xFF;
			while(*p != 0)
			{
				pmove->path[chPathLen] = *p;
				p++;
				chPathLen ++;
			}
			pmove->path[chPathLen] = 0;
			pmove->path[chPathLen+1] = 0;
			c_bWaitMoveACL = true;
			QVariant parameters	= QVariant::fromValue((void*)this);
			SendGameTrace(JUNQI_GAMETRACE_MOVE,(char *)pmove,sizeof(JunqiTraceMove)+chPathLen,HandleArrangeACL,parameters);
			return true;	
		}else
		{;
			//printf("Move failed\n");
		}
	}
	return false;
}
void LandBattleDesktop::gameTrace( const GeneralGameTrace2Head *gameTrace )
{
	DJGameDesktop::gameTrace(gameTrace);
	GeneralGameTrace2Head *ptrace = (PGeneralGameTrace2Head)gameTrace;
	StaticGameTrace(ptrace);
	c_LastTraceType = gameTrace->chType;
	switch(gameTrace->chType)
	{
		case JUNQI_GAMETRACE_ARRANGECHIP:
		case JUNQI_GAMETRACE_INITCHIP : 
		case JUNQI_GAMETRACE_SHOWARRANGE :
		{
			PJunqiTraceInitChip psetchips = (PJunqiTraceInitChip)gameTrace->chBuf;
			ClearMapSiteChips(psetchips->chMapSite);
			RepaintMapSiteChips(psetchips->chMapSite);
			break;
		}
		case JUNQI_GAMETRACE_MOVE :
		{
			PJunqiTraceMove pmove = (PJunqiTraceMove)gameTrace->chBuf;
			if( (pmove->chStartMapSite == 0 && pmove->sxy == 0) || (pmove->chStartMapSite == 0xFF && pmove->sxy == 0xFF))
			{
				DJGameUser *puser = c_LandBattlePanel->userAtSeat(gameTrace->chSite);
				if( puser ) {
					QString text = puser->userName();
					if( pmove->chStartMapSite == 0xFF && pmove->sxy == 0xFF ) {
						djDebug()<<"chMapSite"<<pmove->chMapSite<<"chSite"<<gameTrace->chSite
								<<"self seat"<<c_LandBattlePanel->selfSeatId();
						QString text	= puser->userName();
						if ( m_hasRequestDraw ) {
							text	+= tr(" accept draw");
						}else {
							text	+= tr(" request draw");
						}
						c_LandBattlePanel->insertText2Browser(text);
						ShowAutoDisappearText(text);
						
						if ( gameTrace->chSite != c_LandBattlePanel->selfSeatId()
							&& !m_hasRequestDraw ) {
							text	= DJSCHEME_AHREF
											.arg( DJSCHEME )
											.arg( DJSCHEME_EVENT_ACCEPT_DRAW_CHESS )
											.arg( puser->userId() )
											.arg( DJSCHEME_HOST )
											.arg( tr("Accept") );
							c_LandBattlePanel->insertText2Browser( text );
						}
						m_hasRequestDraw	= true;
					}//else
						//text += tr(" throw up!");
					//c_LandBattlePanel->insertText2Browser(text);
					//ShowAutoDisappearText(text);
					
				}
				return;
			}

			NODEID id;
			id.district  = 0;
			id.id = GET_NODEID(pmove->chStartMapSite,0,pmove->sxy);
			PMapNode pstart = SearchNode(c_chessmap,&id);
			if(pstart != NULL)
			{
				ClearNodeChip(pstart);
				DrawNodeChip(pstart,true);
				DrawPath(pstart,pmove->path);
			}
			break;
		}
		case JUNQI_GAMETRACE_SETCHIP :
		{
			PJunqiSetChip psetchip = (PJunqiSetChip)gameTrace->chBuf;
			c_LandBattlePanel->playWave("move.wav");
			PMapNode pnode;
			NODEID nodeid;
			nodeid.district = 0;
			nodeid.id = GET_NODEID(psetchip->chMapSite,0,psetchip->chip.xy);
			pnode =SearchNode(c_chessmap,&nodeid);
			if(pnode != NULL)
			{
				ClearNodeChip(pnode);
				DrawNodeChip(pnode,false);
				if(pnode->chip.id == DJGAME_JUNQI_RANK_JUNQI)
				{
					quint32 uid;
					DJGameUser *puser;
					uid = c_LandBattlePanel->userIdOfSeat(gameTrace->chSite);
					if(uid != 0)
					{
						puser = c_LandBattlePanel->gameUser(uid);
						if(puser != NULL)
						{
							QString res = puser->userName();
							res += tr("'s commander is killed");
							c_LandBattlePanel->insertText2Browser(res);
							ShowAutoDisappearText(res);
						}
					}
				}
			}
			break;
		}
		case JUNQI_GAMETRACE_OVER :
		{
			if ( c_LandBattlePanel->selfSeatId() == gameTrace->chSite )
				m_isSelfOver	= true;
			ClearMapSiteChips(GetMapSiteOfSeat(gameTrace->chSite));
			quint32 uid;
			DJGameUser *puser;
			uid = c_LandBattlePanel->userIdOfSeat(gameTrace->chSite);
			if(uid != 0)
			{
				puser = c_LandBattlePanel->gameUser(uid);
				if(puser != NULL)
				{
					QString res = puser->userName();
					res += tr(" lost");
					c_LandBattlePanel->insertText2Browser(res);
				}
			}
			break;
		}
	}
}
bool LandBattleDesktop::isCloseable()
{
	bool closable	= DJGameDesktop::isCloseable();
	if ( !closable ) {
		if ( m_isSelfOver )
			closable	= true;
	}
	return closable;
}
void LandBattleDesktop::MoveItem(Q3CanvasItem* item,QPoint &ps,QPoint &pe)
{
}
	
void LandBattleDesktop::StaticInitDesktop()
{
	c_pArrangeNode1 = NULL;
	c_pArrangeNode2 = NULL;
	c_pMoveChipNode = NULL;
	c_PlayerMoveNodeStart = NULL;
	c_PlayerMoveNodeEnd = NULL;
	c_bWaitMoveACL = false;
	c_chUnions	= 0;
	m_isSelfOver	= false;
	m_hasRequestDraw	= false;
}

void LandBattleDesktop::InitMapSiteNodes(quint8 chMapSite,PJunqiTraceInitChip psetchips)
{
	PMapNode pnodes[40];
	unsigned char chChips = 0;
	int i;
	chChips = SearchOwnerNodes(c_chessmap,chMapSite,pnodes,28,-1);
	if(chChips > 0)
	{
		for(i=0;i<chChips;i++)
		{
			memset(&(pnodes[i]->chip),0,sizeof(CHIP));
		}
	}
	if(psetchips != NULL && psetchips->chChips > 0)
	{
		for(i=0;i<psetchips->chChips;i++)
		{
			SetChip(c_chessmap,psetchips->chMapSite,psetchips->chip[i].xy,&(psetchips->chip[i].chip));
		}
	}
}

void LandBattleDesktop::StaticGameTrace(GeneralGameTrace2Head *gameTrace)
{
	switch(gameTrace->chType)
	{
		case JUNQI_GAMETRACE_ARRANGECHIP:
		case JUNQI_GAMETRACE_INITCHIP :
		case JUNQI_GAMETRACE_SHOWARRANGE : 
		{
			PJunqiTraceInitChip psetchips = (PJunqiTraceInitChip)gameTrace->chBuf;
			InitMapSiteNodes(psetchips->chMapSite,psetchips);
			break;
		}
		case JUNQI_GAMETRACE_MOVE :
		{
			PlayerMoveChip(gameTrace);
			break;
		}
		case JUNQI_GAMETRACE_UNION :
		{
			if(gameTrace->chBuf[0] == 0)
				return;
			if(c_chUnions>0)
			{
				int i;
				for(i=0;i<c_chUnions;i++)
				{
					if(c_chUnion[i] == gameTrace->chBuf[0])
						return;
				}
			}
			c_chUnion[c_chUnions] = gameTrace->chBuf[0];
			c_chUnions++;
			break;
		}
		case JUNQI_GAMETRACE_UNIONOVER :
		{
			if(c_chUnions>0)
			{
				int i;
				for(i=0;i<c_chUnions;i++)
				{
					if(c_chUnion[i] == gameTrace->chBuf[0])
					{
						c_chUnion[i] = c_chUnion[c_chUnions-1];
						c_chUnions --;
						c_chUnion[c_chUnions] = 0;
						break;
					}
				}
			}
			break;
		}
		case JUNQI_GAMETRACE_SETCHIP :
		{
			PJunqiSetChip psetchip = (PJunqiSetChip)gameTrace->chBuf;
			SetChip(c_chessmap,psetchip->chMapSite,psetchip->chip.xy,&(psetchip->chip.chip));
			break;
		}
		case JUNQI_GAMETRACE_OVER :
		{
			InitMapSiteNodes(GetMapSiteOfSeat(gameTrace->chSite),NULL);
			break;
		}
	}
}

void LandBattleDesktop::PlayerMoveChip( const GeneralGameTrace2Head *gameTrace )
{
	PJunqiTraceMove pmove = (PJunqiTraceMove)gameTrace->chBuf;
	PMapNode 	pstart,pend;
	NODEID id;
	if(c_PlayerMoveNodeStart != NULL && c_PlayerMoveNodeEnd != NULL)
		HandleMoveTimeout();
	
	if(pmove->chStartMapSite == 0 && pmove->sxy == 0)
		return;
	
	if(pmove->chStartMapSite == 0xFF && pmove->sxy == 0xFF)
	{//求和
		return;
	}
	id.district  = 0;
	id.id = GET_NODEID(pmove->chStartMapSite,0,pmove->sxy);
	pstart = SearchNode(c_chessmap,&id);
	if(pstart != NULL)
	{
		CHIPPOWER power;
		power.power = JUNQI_MAX_POWER;
		DEBUG_OUTPUT(("Move : "));
		DEBUG_BUFFER(((char *)pmove,gameTrace->chBufLen));
		pend = CheckPath(c_chessmap,&power,pstart,pmove->path);
		if(pstart->chip.id == 0 || pend == NULL)
		{
			DEBUG_OUTPUT(("Move Error ! "));
		//	QMessageBox::information(c_Panel,tr("Error!"),tr("Local data error. Please quit game and login again."),QMessageBox::Ok);
			return;
		}
		
		if(pend != NULL)
		{
			c_PlayerMoveNodeStart = pstart;
			c_PlayerMoveNodeEnd   = pend;
			c_MoveResult = pmove->chResult;
			c_MoveTimer->start(1000);
		}
		
	}
	
}


void LandBattleDesktop::RepaintCurrentStatus()
{
	/*
	if(c_current.chPlayers <= 1)
	{
		RepaintChessmap(numberOfPlayers());
		return;
	}
	*/
	quint8 players	= numberOfPlayers();
	RepaintChessmap(players);
	for(int i=1;i<=players;i++)
	{
		ClearMapSiteChips(i);
		RepaintMapSiteChips(i);
	}
	
}
void LandBattleDesktop::RepaintChessmap(int iContext)
{
	if(c_itemBoard != NULL)
	{
		delete c_itemBoard;
		c_itemBoard = NULL;
	}
	QPixmap pix;
	
	switch(iContext)
	{
		case 2 :
			pix = QPixmap(QString(":/LandBattleRes/image/jq2board.png"));
			Init2CountryChessmap(c_chessmap,LANDBATTLEPANEL_CHESSMAP_MAXSIZE,1);
			break;
		case 3 :
			pix = QPixmap(QString(":/LandBattleRes/image/jq3board.png"));
			Init3CountryChessmap(c_chessmap,LANDBATTLEPANEL_CHESSMAP_MAXSIZE,1);
			break;
		case 4 :
			pix = QPixmap(QString(":/LandBattleRes/image/jq4board.png"));
			Init4CountryChessmap(c_chessmap,LANDBATTLEPANEL_CHESSMAP_MAXSIZE,1);
			break;
		default :
			return;
	}
	c_X = (GetRealWidth()-pix.width())>>1;
	c_Y = (GetRealHeight()-pix.height())>>1;
	QImage img = pix.convertToImage();
	QMatrix m = GetCurrentMatrix();
	c_itemBoard = new DJGameImageItem(img,canvas(),m,0,0,DESKTOP_IMAGE_TYPE_BOARD,false);
	c_itemBoard->setMatrix(m);
	c_itemBoard->move(c_X,c_Y);
	c_itemBoard->show();
	c_itemBoard->setZ(20);
}
void LandBattleDesktop::RepaintMapSiteChips(unsigned char chMapSite)
{
	PMapNode pnodes[40];
	unsigned char chChips = 0;
	int i;
	chChips = SearchOwnerNodes(c_chessmap,chMapSite,pnodes,28,-1);
	if(chChips > 0)
	{
		for(i=0;i<chChips;i++)
		{
			DrawNodeChip(pnodes[i],false);
		}
	}
}
void LandBattleDesktop::SetWaitMoveACLStatus(bool b)
{
	c_bWaitMoveACL = b;
}
		
void LandBattleDesktop::ClickSave()
{
	unsigned char buffer[255];
	PGeneralGameTrace2Head ptrace = (PGeneralGameTrace2Head)buffer;
	
	 QDir dir = QDir::home();
    if (!dir.exists(QString("DJGameSave")))
	{
		 dir.mkdir("DJGameSave");
	}
	dir.cd("DJGameSave");
	if(GetTableStatus() == DJGAME_JUNQI_TABLE_STATUS_WAIT_ARRANGE)
	{
		memset(buffer,0,255);
		GetCurrentLayout(ptrace,GetMapSiteOfSeat(c_LandBattlePanel->selfSeatId()));
		QString filename = QFileDialog::getSaveFileName(
                    this,
                    tr("Save the Layout"),
                    dir.path(),
                    tr("DJGame Save Files(*.sav)"));
		if(filename.isNull())
			return;
		if(!filename.endsWith(QString(".sav")) )
			filename += QString(".sav");
		InitSaveFileHead(c_LandBattlePanel,filename,DJGAME_JUNQI,JUNQI_GAMETRACE_INITCHIP);
		AppendData2SaveFile(filename,(char *)buffer,sizeof(GeneralGameTrace2Head)+ptrace->chBufLen);
	}
}
void LandBattleDesktop::GetCurrentLayout(PGeneralGameTrace2Head ptrace,unsigned char chMapSite)
{
	NODEID id;
	int i,j,k = 0;
	PJunqiTraceInitChip	psetchips = (PJunqiTraceInitChip)ptrace->chBuf;
	
	PMapNode pnode;
	id.district = 0;
	for(i=1;i<=6;i++)
	{
		for(j=1;j<=5;j++)
		{
			id.id = GET_NODEID(chMapSite,i,j);
			pnode = SearchNode(c_chessmap,&id);
			if(pnode != NULL && pnode->chip.id != 0)
			{
				psetchips->chip[k].xy = (unsigned char)(id.id & 0xFF);
				psetchips->chip[k].chip.id = (unsigned char)(pnode->chip.id);
				psetchips->chip[k].chip.owner = chMapSite;
				psetchips->chip[k].chip.power = (unsigned char )(pnode->chip.power.power);
				k++;
			}
		}
	}
	ptrace->chTable = c_LandBattlePanel->tableId();
	ptrace->chSite  = GetSeatOfMapSite(chMapSite);
	ptrace->chType  = JUNQI_GAMETRACE_INITCHIP;
	ptrace->chBufLen = k*sizeof(JunqiTraceChip) + sizeof(JunqiTraceInitChip);
	psetchips->chMapSite = chMapSite;
	psetchips->chChips = k;
	
}

unsigned char LandBattleDesktop::GetSeatOfMapSite(unsigned char chBoardSeat)
{
	return c_current.chMapSites[chBoardSeat];
}
unsigned char LandBattleDesktop::GetMapSiteOfSeat(unsigned char seat)
{
	int i = 1;
	while(i<=DJGAME_JUNQI_MAX_PLAYERS)
	{
		if(c_current.chMapSites[i] == seat)
			return i;
		i++;
	}
	return 0;
}
	
void LandBattleDesktop::ClickLoad()
{
	unsigned char buffer[255];
	unsigned char *savbuf;
	PGeneralGameTrace2Head ptrace = (PGeneralGameTrace2Head)buffer;
	PGeneralGameTrace2Head psavet;
	PJunqiTraceInitChip    pcurchips = (PJunqiTraceInitChip)ptrace->chBuf;
	PJunqiTraceInitChip	   psavchips;
	
	 QDir dir = QDir::home();
    if (!dir.exists(QString("DJGameSave")))
	{
		 dir.mkdir("DJGameSave");
	}
	dir.cd("DJGameSave");
	if(GetTableStatus() == DJGAME_JUNQI_TABLE_STATUS_WAIT_ARRANGE && IsWaittingForMe() &&
			!c_bWaitMoveACL )
	{
		memset(buffer,0,255);
		GetCurrentLayout(ptrace,GetMapSiteOfSeat(c_LandBattlePanel->selfSeatId()));
		QString filename = QFileDialog::getOpenFileName(
                    this,
                    tr("Load Layout"),
                    dir.path(),
                    tr("DJGame Save Files(*.sav)"));
		if(filename.isNull())
			return;
		unsigned int shLen;
		savbuf = (unsigned char *)GetSaveData(filename,DJGAME_JUNQI,JUNQI_GAMETRACE_INITCHIP,&shLen);
		if(savbuf == NULL)
		{
			QMessageBox::information( this,tr("Layout File Error"),tr("It isn't a DJGame save-file."),QMessageBox::Ok);
			return;
		}
		psavet = (PGeneralGameTrace2Head)savbuf;
		psavchips = (PJunqiTraceInitChip)psavet->chBuf;
		if(psavchips->chChips == pcurchips->chChips)
		{
			psavchips->chMapSite = GetMapSiteOfSeat(c_LandBattlePanel->selfSeatId());
			int i;
			for(i=0;i<=psavchips->chChips;i++)
			{
				psavchips->chip[i].chip.owner = psavchips->chMapSite;
			}
		}
			
		if(psavchips->chChips != pcurchips->chChips || !JunqiCheckArrange( pcurchips,psavchips ) )
		{
			QMessageBox::information( this,tr("Layout File Error"),tr("It is a layout file but cant match here."),QMessageBox::Ok);
			return;
		}
		InitMapSiteNodes(GetMapSiteOfSeat(c_LandBattlePanel->selfSeatId()),psavchips);
		ClearMapSiteChips(GetMapSiteOfSeat(c_LandBattlePanel->selfSeatId()));
		RepaintMapSiteChips(GetMapSiteOfSeat(c_LandBattlePanel->selfSeatId()));
	}
}
void LandBattleDesktop::ClickSend()
{
	unsigned char buffer[255];
	PGeneralGameTrace2Head ptrace = (PGeneralGameTrace2Head)buffer;
	PJunqiTraceInitChip    pcurchips = (PJunqiTraceInitChip)ptrace->chBuf;
	
	if(GetTableStatus() == DJGAME_JUNQI_TABLE_STATUS_WAIT_ARRANGE && !c_bWaitMoveACL
		&& IsWaittingForMe() )
	{
		GetCurrentLayout(ptrace,GetMapSiteOfSeat(c_LandBattlePanel->selfSeatId()));
		if(JunqiCheckArrange(pcurchips,pcurchips))
		{
			c_bWaitMoveACL = true;
			QVariant parameters	= QVariant::fromValue((void*)this);
			SendGameTrace(JUNQI_GAMETRACE_INITCHIP,(char *)ptrace->chBuf,ptrace->chBufLen,HandleArrangeACL,parameters);
		}else
		{
			QMessageBox::information( this,tr("Layout Error"),tr("This is a invalid layout."),QMessageBox::Ok);
		}
	}
}
void  HandleArrangeACL( const QByteArray& buf, const QVariant& parameters )
{
	djDebug()<< "HandleArrangeACL" << parameters;
	
    PDJGameProtocol pacl = (PDJGameProtocol)buf.data();
    LandBattleDesktop* desktop = static_cast<LandBattleDesktop *>(parameters.value<void*>());
    djDebug()<<"desktop"<<desktop;
	if(buf.size() == 0)
    {
        desktop->SetWaitMoveACLStatus(false);
    }
    if( DJGAME_SUCCESS(pacl->chBuf[0]) )
	{
		desktop->SetWaitMoveACLStatus(true);
	}else
	{
		printf("Arrange failed\n");
		desktop->SetWaitMoveACLStatus(false);
	}
}

void  HandleMoveACL( const QByteArray& buf, const QVariant& parameters )
{
    PDJGameProtocol pacl = (PDJGameProtocol)buf.data();
    LandBattleDesktop* desktop = static_cast<LandBattleDesktop *>(parameters.value<void*>());
    
	if(buf.size() == 0)
    {
        desktop->SetWaitMoveACLStatus(false);
    }
    if( DJGAME_SUCCESS(pacl->chBuf[0]) )
	{
		desktop->SetWaitMoveACLStatus(true);
	}else
	{
		printf("move failed\n");
		desktop->SetWaitMoveACLStatus(false);
	}
}

void LandBattleDesktop::ClickSurrender()
{
	if(c_LandBattlePanel->isLookingOn())
		return;

	JunqiTraceMove			move;
	move.chMapSite = GetMapSiteOfSeat(c_LandBattlePanel->selfSeatId());
	move.chStartMapSite = 0;
	move.sxy = 0x0;
	SendGameTrace(JUNQI_GAMETRACE_MOVE,(char *)&move,sizeof(JunqiTraceMove));
	c_pbSurrender->setEnabled(false);
}

void LandBattleDesktop::ClickDuce()
{
	if(c_LandBattlePanel->isLookingOn())
		return;
	djDebug()<<"ClickDuce";
	JunqiTraceMove			move;
	move.chMapSite = GetMapSiteOfSeat(c_LandBattlePanel->selfSeatId());
	move.chStartMapSite = 0xFF;
	move.sxy = 0xFF;
	QVariant parameters	= QVariant::fromValue((void*)c_pbDuce);
	SendGameTrace(JUNQI_GAMETRACE_MOVE,(char *)&move,sizeof(JunqiTraceMove),aclRequestDrawProc,parameters);
}


void LandBattleDesktop::HandleExchangeTimeout()
{
	c_timerExchange->stop();
	
	if(c_pArrangeNode1 == NULL || c_pArrangeNode2 == NULL)
		return;
	CHIP tmp;
	
	ClearNodeChip(c_pArrangeNode1);
	ClearNodeChip(c_pArrangeNode2);
	
	memcpy(&tmp,&(c_pArrangeNode1->chip),sizeof(CHIP));
	memcpy(&(c_pArrangeNode1->chip),&(c_pArrangeNode2->chip),sizeof(CHIP));
	memcpy(&(c_pArrangeNode2->chip),&tmp,sizeof(CHIP));

	DrawNodeChip(c_pArrangeNode1,false);
	DrawNodeChip(c_pArrangeNode2,false);
	
	c_pArrangeNode1 = NULL;
	c_pArrangeNode2 = NULL;
}
void LandBattleDesktop::HandleMoveTimeout()
{
	c_MoveTimer->stop();
	
	ClearPathLine();
	ClearNodeChip(c_PlayerMoveNodeEnd);
	switch(c_MoveResult)
	{
		case JUNQI_MOVERESULT_SET :
			if(c_LastTraceType == 0)
			{
				if(c_PlayerMoveNodeEnd->chip.id != 0)
					c_LandBattlePanel->playWave("kill.wav");
				else
					c_LandBattlePanel->playWave("move.wav");
			}
			memcpy(&c_PlayerMoveNodeEnd->chip,&c_PlayerMoveNodeStart->chip,sizeof(CHIP));
			break;
		case JUNQI_MOVERESULT_REMOVE:
			if(c_LastTraceType == 0)
			{
				c_LandBattlePanel->playWave("kill.wav");
			}
			break;
		case JUNQI_MOVERESULT_CLEAR :
			if(c_LastTraceType == 0)
				c_LandBattlePanel->playWave("dkill.wav");
			memset(&c_PlayerMoveNodeEnd->chip,0,sizeof(CHIP));
			break;
	}

	DrawNodeChip(c_PlayerMoveNodeEnd,false);
	
	ClearNodeChip(c_PlayerMoveNodeStart);
	memset(&c_PlayerMoveNodeStart->chip,0,sizeof(CHIP));
	
	c_PlayerMoveNodeStart = NULL;
	c_PlayerMoveNodeEnd = NULL;
}

void LandBattleDesktop::DrawPath(PMapNode pcurnode,unsigned char *path)
{
	bool bHigh =false;
	unsigned char pin;
	PMapNode plastnode = pcurnode,pnextnode = NULL;
	DJGameLineItem *item;
	QMatrix m = GetCurrentMatrix();
	while(pcurnode != NULL)
	{
		if(bHigh)
		{
			pin = (*path)>>4;
		}else
			pin =(*path) & 0x0F;
		if(pin == 0 || pin > DJGAME_CHESSMAP_MAX_PINS)
			break;
		pnextnode = pcurnode->pin[pin].pnode;
		if(pnextnode != NULL)
		{
			item = new DJGameLineItem(canvas(),m,1,1,DESKTOP_IMAGE_TYPE_PATH,false);
			item->setPoints(pcurnode->x+c_X,pcurnode->y+c_Y,pnextnode->x+c_X,pnextnode->y+c_Y);
		//	printf("pathline %04X(%d,%d)->%04X(%d,%d)\n",pcurnode->id.id,pcurnode->x,pcurnode->y,pnextnode->id.id,pnextnode->x,pnextnode->y);
			item->setPen(QPen(QColor(255,0,0),3));
			item->setZ(2000);
			item->show();
			item->setMatrix(m);
		}
		plastnode = pcurnode;
		pcurnode = pnextnode;
		if(!bHigh)
			path ++;
		bHigh =!bHigh;
	}
			
}

			
void LandBattleDesktop::ClearPathLine()
{
	Q3CanvasItemList list = canvas()->allItems();
    Q3CanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it)
    {
        if ( *it && (*(it))->rtti() == djgame_lineRTTI )
            delete *it;
    }
}
quint8 LandBattleDesktop::numberOfPlayers() const
{
	quint8 players	= c_current.chPlayers;
	if ( 0 != players )
		return players;
	else
		return DJGameDesktop::numberOfPlayers();
}
QList<quint8> LandBattleDesktop::seatsOfPlayers() const
{
	quint8 players	= c_current.chPlayers;
	if ( 0 != players ) {
		QList<quint8>	seatIds;
		for ( int i = 1; i <= players; ++i ) {
			seatIds << c_current.chMapSites[i];
		}
		return seatIds;
	}else {
		return DJGameDesktop::seatsOfPlayers();
	}	
}
void LandBattleDesktop::locateShowPositions( QList<QPoint>& points, QList<Qt::Alignment>& aligns, QList<int> &modes )
{
	djDebug() << "LandBattleDesktop::locateShowPositions";
	
	points.clear();
	aligns.clear();
	modes.clear();
	
	quint8 players	= numberOfPlayers();
	djDebug()<<"players="<<players;
	QList<quint8> seats	= seatsOfPlayers();
	djDebug()<<"seatsOfPlayers"<<seats;
	
	for ( quint8 seat = 0; seat <= c_LandBattlePanel->numberOfSeats(); ++seat ) {
		points 	<< QPoint();
		aligns	<< 0;
		modes	<< DJ_MATRIX_CANVAS_ITEM_HIDE ;
	}
	int centerX	= GetRealWidth() >> 1;
	int centerY	= GetRealHeight() >> 1;
	int leftX	= 0;
	int rightX	= GetRealWidth();
	int topY	= 0;
	int bottomY	= GetRealHeight(); 
	int space	= 136;
	
	foreach( quint8 seat, seats ) {
 		quint8 view = MapSeat2View(seat);
 		if ( 4 == players  ) {
    		switch( view ) {
        	case 1: {
        		points[seat].setX( centerX - space );
        		points[seat].setY( bottomY - 1 );
        		aligns[seat]	= Qt::AlignRight | Qt::AlignBottom;
        		modes[seat]	= DJ_MATRIX_CANVAS_ITEM_MATRIMOVE;
        		break;
        	} 
        	case 2: {
        		points[seat].setX( leftX );
        		points[seat].setY( centerY - space );
        		aligns[seat]	= Qt::AlignBottom;
        		modes[seat]	= DJ_MATRIX_CANVAS_ITEM_MATRIMOVE;
        		break;
        	}
        	case 3:  {
        		points[seat].setX( centerX + space );
        		points[seat].setY( topY );
        		//aligns[seat]	= Qt::AlignRight;
        		modes[seat]	= DJ_MATRIX_CANVAS_ITEM_MATRIMOVE;
        		break;
        	}
        	case 4:  {
        		points[seat].setX( rightX );
        		points[seat].setY( centerY - space );
        		aligns[seat]	= Qt::AlignRight | Qt::AlignBottom;
        		modes[seat]	= DJ_MATRIX_CANVAS_ITEM_MATRIMOVE;
        		break;
        	}
        	default:
        		break;
          	}
        }else if ( 3 == players ) {
    		switch( view ) {
        	case 1: {
        		//points[seat].setX( leftX );
        		//points[seat].setY( bottomY - 1 );
        		//aligns[seat]	= Qt::AlignBottom;
        		//modes[seat]	= DJ_MATRIX_CANVAS_ITEM_MATRIMOVE;
        		break;
        	} 
        	case 2: {
        		points[seat].setX( leftX );
        		points[seat].setY( topY );
        		modes[seat]	= DJ_MATRIX_CANVAS_ITEM_MATRIMOVE;
        		break;
        	}
        	case 4:  {
        		points[seat].setX( rightX );
        		points[seat].setY( topY );
        		aligns[seat]	= Qt::AlignRight;
        		modes[seat]	= DJ_MATRIX_CANVAS_ITEM_MATRIMOVE;
        		break;
        	}
        	default:
        		break;
          	}
    	}else if ( 2 == players ) {
    		if ( 1 == view ) {
    			points[seat].setX( leftX );
        		points[seat].setY( bottomY );
        		aligns[seat]	= Qt::AlignBottom;
        		modes[seat]	= DJ_MATRIX_CANVAS_ITEM_MATRIMOVE;
        	}else {
        		points[seat].setX( leftX );
        		points[seat].setY( topY );
        		modes[seat]	= DJ_MATRIX_CANVAS_ITEM_MATRIMOVE;
        	}
    	}
 	}
}
void LandBattleDesktop::locateNamePositions( QList<QPoint>& points, QList<Qt::Alignment>& aligns, QList<int> &modes )
{
	djDebug() << "LandBattleDesktop::locateNamePositions";
	
	points.clear();
	aligns.clear();
	modes.clear();
	
	quint8 players	= numberOfPlayers();
	djDebug()<<"players="<<players;
	QList<quint8> seats	= seatsOfPlayers();
	djDebug()<<"seatsOfPlayers"<<seats;
		
	for ( quint8 seat = 0; seat <= c_LandBattlePanel->numberOfSeats(); ++seat ) {
		points 	<< QPoint();
		aligns 	<< 0;
		modes	<< DJ_MATRIX_CANVAS_ITEM_HIDE; 
	}
	
	foreach( quint8 seat, seats ) {
     	QRect showRect	= idealRectOfShow( seat );
    	quint8 view = MapSeat2View(seat);
    	if ( 4 == players  ) {
    		switch( view ) {
        	case 1: {
        		points[seat].setX( showRect.left() - 1 );
    			points[seat].setY( showRect.bottom() - 1 );
    			aligns[seat]	= Qt::AlignRight | Qt::AlignBottom;
    			modes[seat]	= DJ_MATRIX_CANVAS_ITEM_MATRIMOVE;
        		break;
        	} 
        	case 2:  {
        		points[seat].setX( showRect.right() + 1 );
        		points[seat].setY( showRect.bottom() );
        		aligns[seat]	= Qt::AlignBottom;
        		modes[seat]	= DJ_MATRIX_CANVAS_ITEM_MATRIMOVE;
        		break;
        	}
        	case 3:  {
        		points[seat].setX( showRect.right() + 1 );
        		points[seat].setY( showRect.top() + 1 );
        		//aligns[seat]	= Qt::AlignRight;
        		modes[seat]	= DJ_MATRIX_CANVAS_ITEM_MATRIMOVE;
        		break;
        	}
        	case 4:  {
        		points[seat].setX( showRect.left() - 1 );
        		points[seat].setY( showRect.bottom() );
        		aligns[seat]	= Qt::AlignRight | Qt::AlignBottom;
        		modes[seat]	= DJ_MATRIX_CANVAS_ITEM_MATRIMOVE;
        		break;
        	}
        	default:
        		break;
          	}
    	}else if ( 3 == players ) {
    		switch( view ) {
        	case 1: {
        		points[seat].setX( GetRealWidth()>>1 );
    			points[seat].setY( GetRealHeight() - 1 );
    			aligns[seat]	= Qt::AlignHCenter | Qt::AlignBottom;
    			modes[seat]	= DJ_MATRIX_CANVAS_ITEM_MATRIMOVE;
        		break;
        	} 
        	case 2:  {
        		points[seat].setX( showRect.right() + 1 );
        		points[seat].setY( showRect.top() + 1 );
        		modes[seat]	= DJ_MATRIX_CANVAS_ITEM_MATRIMOVE;
        		break;
        	}
        	case 4:  {
        		points[seat].setX( showRect.left() - 1 );
        		points[seat].setY( showRect.top() + 1 );
        		aligns[seat]	= Qt::AlignRight;
        		modes[seat]	= DJ_MATRIX_CANVAS_ITEM_MATRIMOVE;
        		break;
        	}
        	default:
        		break;
          	}
    	}else if ( 2 == players ) {
    		if ( 1 == view ) {
        		points[seat].setX( showRect.right() + 1 );
        		points[seat].setY( showRect.bottom() - 1 );
        		aligns[seat]	= Qt::AlignBottom;
        		modes[seat]	= DJ_MATRIX_CANVAS_ITEM_MATRIMOVE;
        	}else {
        		points[seat].setX( showRect.right() + 1 );
        		points[seat].setY( showRect.top() + 1 );
        		modes[seat]	= DJ_MATRIX_CANVAS_ITEM_MATRIMOVE;
        	}
        	
    	}
    }	
}
void LandBattleDesktop::locateClockPosition( QPoint& point, Qt::Alignment& align, int &mode )
{
	djDebug() << "LandBattleDesktop::locateClockPositions";
	int centerX, centerY;
    GetDesktopCenterPoint( &centerX, &centerY, 0 );
	quint8 players	= numberOfPlayers();
	if ( players > 2 ) {
		point.setX( centerX );
    	point.setY( centerY - 60 );
    	align	= Qt::AlignCenter;
    	mode	= DJ_MATRIX_CANVAS_ITEM_MATRIMOVE;
	}else {
		point.setX( 0 );
    	point.setY( centerY );
    	align	= Qt::AlignVCenter;
    	mode	= DJ_MATRIX_CANVAS_ITEM_MATRIMOVE;
	}
}	

/////////////////////////////////////////////////////////////////////////////////////////
LandBattlePanel::LandBattlePanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f)
			: DJGamePanel( gameController, room, userIds, tableId, seatId, parent, f )
{	
	QSplitter* mainSplitter = new QSplitter(Qt::Horizontal , this);
	setCentralWidget( mainSplitter );
	QSplitter	*accessorySplitter, *gameSplitter;
	 	
	if ( 0 == layoutMode() ) {
		gameSplitter	= new QSplitter( Qt::Vertical , mainSplitter );
		accessorySplitter 	= new QSplitter( Qt::Vertical , mainSplitter );
	}else {
		accessorySplitter	= new QSplitter( Qt::Vertical , mainSplitter );
		gameSplitter 	= new QSplitter( Qt::Vertical , mainSplitter );
	}
	createPanelBar( gameSplitter );
	LandBattleDesktop* gameDesktop = new LandBattleDesktop(gameSplitter,this,950,950);
	
	setAccessorySplitter( accessorySplitter );
	setGameDesktop( gameDesktop );	
}

LandBattlePanel::~LandBattlePanel()
{
}
void LandBattlePanel::createAccessoryWidgets()
{
	DJGamePanel::createAccessoryWidgets();
	
	DJGroupChat *chat	= chatWidget();
	LandBattleDesktop *desktop	= static_cast<LandBattleDesktop*>(gameDesktop());
	if ( chat && desktop ) {
		connect( chat->textBrowser(), SIGNAL(acceptDrawChess()), desktop, SLOT(ClickDuce()) );
	}
}
////////////////////////////////////////////////////////////////////////////////////////
//以下函数用于保存布局文件，以后应由 Desktop 中的 DJGameTraceManager 取代
QString GetTempSaveFileName(unsigned short shGameID,unsigned char index)
{
	QDir dir = QDir::home();
    if (!dir.exists(QString("DJGameSave")))
	{
		 dir.mkdir("DJGameSave");
	}
	dir.cd("DJGameSave");
	QString name =dir.path();
	char buffer[32];
	sprintf(buffer,"%04x%02x_%d.sav",shGameID,index,rand());
	
	name += QString(buffer);
	return name;
}

typedef struct __tagDJGameSavePlayer
{
	unsigned char chGender;
	unsigned char chSeat;
	unsigned char szName[40];
}DJGameSavePlayer,*PDJGameSavePlayer;

typedef struct __tagDJGameSaveHead
{
	unsigned short shHeadLen;
	unsigned short shGameID;
	unsigned char  chType;
	unsigned char  chSeats;
	unsigned char  chMySeat;
	DJGameSavePlayer players[1];
}DJGameSaveHead,*PDJGameSaveHead;

void InitSaveFileHead(DJGamePanel *panel,QString &name,unsigned short shGameID,unsigned char chType)
{
	char *p;
	unsigned short shSize;
	PDJGameSaveHead	phead;
	
	if(panel == NULL)
		return ;
	DJGameRoom* nroom = panel->gameRoom();
	if(nroom->numberOfSeats() < 2)
			return ;
		
	QFile file(name);

	if (file.open(QIODevice::WriteOnly))
	{
		
		shSize = sizeof(DJGameSaveHead)+ sizeof(DJGameSavePlayer)* (nroom->numberOfSeats()-1);
		
		p = (char *)malloc(shSize+0xFF);
		memset(p,0,shSize+0xFF);
		phead = (PDJGameSaveHead)p;
		phead->shHeadLen = shSize;
		phead->shGameID = shGameID;
		phead->chType = chType;
		phead->chMySeat = panel->selfSeatId();
		phead->chSeats = nroom->numberOfSeats();
		quint32 uid,nLen,i;
		DJGameUser *puser;
		QString strUserName;
		for(i=0;i<nroom->numberOfSeats();i++)
		{
			uid = panel->userIdOfSeat(i+1);
			puser = panel->gameUser(uid);
			phead->players[i].chSeat = i+1;
			if(puser!=NULL)
			{
				phead->players[i].chGender = puser->gender();
				strUserName = puser->userName();
				QByteArray ba=strUserName.toUtf8();
				nLen = strlen(ba.data());
				if(nLen > 36)
					nLen = 36;
				memcpy(phead->players[i].szName,ba.data(),nLen);
			}
		}
		file.write((const char *)p,shSize);
		file.flush();
	}
}

char * GetSaveData(QString &name,unsigned short shGameID,unsigned char chType,unsigned int *iLen)
{
	QFile file(name);
	char *p,*ph;
	DJGameSaveHead *phead;
	
	if (file.open(QIODevice::ReadOnly))
	{
		if(file.size()<=3)
		{
			return NULL;
		}
		unsigned short shHeadLen=0,shSize;
		file.read((char *)&shHeadLen,sizeof(short));
		if(shHeadLen < sizeof(DJGameSaveHead) || file.size() < shHeadLen+3)
			return NULL;
		file.seek(0);
		ph = (char *)malloc(shHeadLen);
		file.read(ph,shHeadLen);
		phead =(PDJGameSaveHead)ph;
		
		if(shGameID != phead->shGameID || chType != phead->chType || 
				shHeadLen != sizeof(DJGameSaveHead)+ sizeof(DJGameSavePlayer)* (phead->chSeats-1) )
		{
			return NULL;
		}
		
		shSize = file.size()-shHeadLen;
		p = (char *)malloc(shSize + 0xFF);
		file.read(p,file.size());
		*iLen = shSize;
		return p;
	}
	return NULL;
}

void AppendData2SaveFile(QString &name,char *data,unsigned int shLen)
{
	QFile file(name);

	if (file.open(QIODevice::Append))
	{
//    	QDataStream out(&file);
//		out<<(unsigned short)shLen; 
		file.write((const char *)data,shLen);
		file.flush();
	}
}
