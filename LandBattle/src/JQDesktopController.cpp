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
#include "JQDesktopController.h"
#include "DJPanelController.h"
#include "DJGraphicsPixmapItem.h"
#include "DJGraphicsChessTimerItem.h"
#include "DJGraphicsLineItem.h"
#include "DJDesktop.h"
#include "DJDesktopScene.h"
#include "DJGameRoom.h"
#include "DJGameUser.h"
#include "DJClientRequest.h"
#include "DJMessageBox.h"
#include "DJPanel.h"
#include "Utility.h"
#include "chessmap.h"
#include "DJGroupChat.h"
#include "DJTextBrowser.h"
#include "DJSchemeEvent.h"

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
void InitSaveFileHead(DJPanelController *pc,QString &name,unsigned short shGameID,unsigned char chType)
{
	char *p;
	unsigned short shSize;
	PDJGameSaveHead	phead;
	
	if(pc == 0)
		return ;
	DJGameRoom* nroom = pc->gameRoom();
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
		phead->chMySeat = pc->seatId();
		phead->chSeats = nroom->numberOfSeats();
		quint32 nLen,i;
		//DJGameUser *puser;
		//QString strUserName;
		for(i=0;i<nroom->numberOfSeats();i++)
		{
			DJGameUser *puser	= pc->player(i);
			//uid = panel->userIdOfSeat(i+1);
			//puser = panel->gameUser(uid);
			phead->players[i].chSeat = i+1;
			if(puser)
			{
				phead->players[i].chGender = puser->gender();
				QString name = puser->userName();
				QByteArray ba=name.toUtf8();
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
			return 0;
		}
		unsigned short shHeadLen=0,shSize;
		file.read((char *)&shHeadLen,sizeof(short));
		if(shHeadLen < sizeof(DJGameSaveHead) || file.size() < shHeadLen+3)
			return 0;
		file.seek(0);
		ph = (char *)malloc(shHeadLen);
		file.read(ph,shHeadLen);
		phead =(PDJGameSaveHead)ph;
		
		if(shGameID != phead->shGameID || chType != phead->chType || 
				shHeadLen != sizeof(DJGameSaveHead)+ sizeof(DJGameSavePlayer)* (phead->chSeats-1) )
		{
			return 0;
		}
		
		shSize = file.size()-shHeadLen;
		p = (char *)malloc(shSize + 0xFF);
		file.read(p,file.size());
		*iLen = shSize;
		return p;
	}
	return 0;
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
void HandleArrangeACL( const QByteArray& buf, const QVariant& parameters )
{
	djDebug()<< "HandleArrangeACL" << parameters;
	
    PDJGameProtocol pacl = (PDJGameProtocol)buf.data();
    JQDesktopController* dc = static_cast<JQDesktopController *>(parameters.value<void*>());
	
	dc->setWaitingMoveACL(false);
}
JQDesktopController::JQDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent )
	: DJDesktopController(panelController,size,parent)
{
	djDebug()<<"JQDesktopController constructor"<<size;
	
	memset(&m_current,0,sizeof(JunqiCurrent));
	memset(m_chUnion,0,5); 
	
	m_exchangeTimer = new QTimer(this);
	m_exchangeTimer->setSingleShot(true);
    connect(m_exchangeTimer,SIGNAL(timeout()),this,SLOT(handleExchangeTimeout()));
	m_moveTimer = new QTimer(this);
	m_moveTimer->setSingleShot(true);
    connect(m_moveTimer,SIGNAL(timeout()),this,SLOT(handleMoveTimeout()));
	
	m_line 	= new DJGraphicsLineItem(0, desktop()->scene()); 
	m_line->setPen(QPen(Qt::red,3));
	m_line->setZValue(2000);
	
	m_board 	= new DJGraphicsPixmapItem(0, desktop()->scene());
	m_board->setZValue( 1 );
	m_board->show();
	
	m_btnSave = new QPushButton(desktop());
	m_btnSave->setText( tr("save") );
	m_btnSave->adjustSize();
	connect(m_btnSave,SIGNAL(clicked()),this,SLOT(clickSave()));
	m_btnSave->hide();
	
	m_btnLoad= new QPushButton(desktop());
	m_btnLoad->setText( tr("load") );
	m_btnLoad->adjustSize();
	connect(m_btnLoad,SIGNAL(clicked()),this,SLOT(clickLoad()));
	m_btnLoad->hide();  
	
	m_btnChuZheng = new QPushButton(desktop());
	m_btnChuZheng->setText( tr("go") );
	m_btnChuZheng->adjustSize();
	connect(m_btnChuZheng,SIGNAL(clicked()),this,SLOT(clickChuZheng()));
	m_btnChuZheng->hide();
	
	m_btnQiuHe= new QPushButton(desktop());
	m_btnQiuHe->setText( tr("request draw") );
	m_btnQiuHe->adjustSize();
	connect(m_btnQiuHe,SIGNAL(clicked()),this,SLOT(clickQiuHe()));
	m_btnQiuHe->hide(); 
	
	m_btnRenShu= new QPushButton(desktop());	
	m_btnRenShu->setText( tr("surrender") );
	m_btnRenShu->adjustSize();
	connect(m_btnRenShu,SIGNAL(clicked()),this,SLOT(clickRenShu()));
	m_btnRenShu->hide();
	
	connect( panelController->chatWidget()->textBrowser(), SIGNAL(acceptDrawChess()), SLOT(clickQiuHe()) );
	
	QPixmap pix	= QPixmap(":/LandBattleRes/image/1_0.png");
	if ( !pix.isNull() ) {
		m_chipSize	= pix.size();
	}
	init();
}
JQDesktopController::~JQDesktopController()
{
	djDebug()<<"JQDesktopController destructor";
}
void JQDesktopController::init()
{ 
	m_nodeOfArrange1 = 0;
	m_nodeOfArrange2 = 0;
	m_nodeOfMoveChip = 0;
	m_nodeOfStartMove = 0;
	m_nodeOfEndMove = 0;
	setWaitingMoveACL( false );
	m_chUnions	= 0;
	m_lastTraceType 	= 0;
	m_line->hide();
	m_isSelfOver	= false;
	m_hasRequestDraw	= false;
	switch(m_current.chMapID)
	{
	case JUNQI_MAP_NORMAL2 :
		djDebug()<<"JUNQI_MAP_NORMAL2";
		Init2CountryChessmap(m_chessmap,LANDBATTLEPANEL_CHESSMAP_MAXSIZE,seat2MappedSeat(panelController()->seatId()));
		break;
	case JUNQI_MAP_NORMAL3 :
		djDebug()<<"JUNQI_MAP_NORMAL3";
		Init3CountryChessmap(m_chessmap,LANDBATTLEPANEL_CHESSMAP_MAXSIZE,seat2MappedSeat(panelController()->seatId()));
		break;
	case JUNQI_MAP_NORMAL4 : 
		djDebug()<<"JUNQI_MAP_NORMAL4";
		Init4CountryChessmap(m_chessmap,LANDBATTLEPANEL_CHESSMAP_MAXSIZE,seat2MappedSeat(panelController()->seatId()));
		break;
	default:
		break;	
	}
}
void JQDesktopController::initMappedSeatNodes(quint8 mappedSeat,PJunqiTraceInitChip psetchips)
{
	PMapNode pnodes[40];
	unsigned char chChips = 0;
	int i;
	chChips = SearchOwnerNodes(m_chessmap,mappedSeat,pnodes,28,-1);
	if(chChips > 0)
	{
		for(i=0;i<chChips;i++)
		{
			memset(&(pnodes[i]->chip),0,sizeof(CHIP));
		}
	}
	if(psetchips != 0 && psetchips->chChips > 0)
	{
		for(i=0;i<psetchips->chChips;i++)
		{
			SetChip(m_chessmap,psetchips->chMapSite,psetchips->chip[i].xy,&(psetchips->chip[i].chip));
		}
	}
}
quint8 JQDesktopController::viewOfNode( PMapNode pnode )
{
	if ( 0 == pnode )
		return 0;
	quint8 iBoard = GET_BoardOfNode(&(pnode->id));
	quint8 mappedSeat	= 0;
	if(iBoard == DJGAME_JUNQI_CONNECTER_AREA) {
		mappedSeat	= pnode->chip.owner;
	}else if ( iBoard <= DJGAME_JUNQI_MAX_PLAYERS ) {
		mappedSeat	= iBoard;
	}
	quint8 seat	= mappedSeat2Seat(mappedSeat);
	return seat2View(seat);
}

quint8 JQDesktopController::seat2MappedSeat( quint8 seat ) const
{
	quint8 i = 1;
	while(i<=DJGAME_JUNQI_MAX_PLAYERS)
	{
		if(m_current.chMapSites[i] == seat)
			return i;
		i++;
	}
	return 0;
}
quint8 JQDesktopController::mappedSeat2Seat( quint8 mappedSeat ) const
{
	if ( mappedSeat <= DJGAME_JUNQI_MAX_PLAYERS )
		return m_current.chMapSites[mappedSeat];
	return 0;
}
void JQDesktopController::clearAllChips()
{
	QList<QGraphicsItem*> items	= desktop()->desktopScene()->items();
	foreach ( QGraphicsItem *item, items ) {
		QVariant var0	= item->data(0);
		QVariant var1	= item->data(1);
		if ( var0.isValid() && var1.isValid() ) {
			int type	= var0.toInt();
			if ( JQ_TYPE_CHIP == type ) {
				delete item;
			}
		}
	}
}
void JQDesktopController::clearMappedSeatChips( quint8 mapSeat )
{ 
	djDebug()<<"JQDesktopController::clearMappedSeatChips"<<mapSeat;
	QList<QGraphicsItem*> items	= desktop()->desktopScene()->items();
	foreach ( QGraphicsItem *item, items ) {
		QVariant var0	= item->data(0);
		QVariant var1	= item->data(1);
		if ( var0.isValid() && var1.isValid() ) {
			int type	= var0.toInt();
			PMapNode pnode	= static_cast<PMapNode>(var1.value<void*>());
			djDebug()<<"pnode->chip.owner"<<pnode->chip.owner;
 			if ( JQ_TYPE_CHIP == type && pnode
				&& pnode->chip.owner == mapSeat ) {
				djDebug()<<"delete item";
				delete item;
			}
		}
	}
}
void JQDesktopController::repaintMappedSeatChips( quint8 mapSeat )
{
	djDebug()<<"repaintMappedSeatChips"<<mapSeat;
	
	PMapNode pnodes[40];
	unsigned char chChips = 0;
	int i;
	chChips = SearchOwnerNodes(m_chessmap,mapSeat,pnodes,28,-1);
	djDebug()<<"chChips"<<chChips;
	if(chChips > 0)
	{
		for(i=0;i<chChips;i++)
		{
			repaintNodeChip(pnodes[i],false);
		}
	}
}
void JQDesktopController::clearNodeChip(PMapNode pnode)
{
	DJGraphicsPixmapItem *item	= (DJGraphicsPixmapItem*)findChip(pnode);
	delete item;
}
QGraphicsItem* JQDesktopController::findChip(PMapNode pnode)
{
	if ( 0 == pnode )
		return 0;
	QList<QGraphicsItem*> items	= desktop()->desktopScene()->items();
	foreach ( QGraphicsItem *item, items ) {
		QVariant var0	= item->data(0);
		QVariant var1	= item->data(1);
		if ( var0.isValid() && var1.isValid() ) {
			int type	= var0.toInt();
			PMapNode theNode	= static_cast<PMapNode>(var1.value<void*>());
			
			if ( JQ_TYPE_CHIP == type && pnode == theNode ) {
				return item;
			}
		}
	}
	return 0;
}
void JQDesktopController::repaintNodeChip(PMapNode pnode,bool isSelected)
{
	djDebug()<<"repaintNode";
	
	if(pnode == 0)
		return;
	quint16 chipId	= pnode->chip.id;
	quint16 chipOwner	= pnode->chip.owner;
		
	if( 0 == chipId || 0 == chipOwner )
		return;
	if( DJGAME_JUNQI_RANK_WEIZHI == chipId )
		chipId = 0;
	
	quint8 view = viewOfNode(pnode);
	if ( 0 == view )
		return;
	
	QString file	= QString(":/LandBattleRes/image/%1_%2.png").arg(chipOwner).arg(chipId,1,16);
	djDebug()<<"file"<<file;
	QPixmap pix = QPixmap( file );	
	DJGraphicsPixmapItem *item = new DJGraphicsPixmapItem(pix,0, desktop()->scene());
	item->setData( 0, JQ_TYPE_CHIP);
	item->setData( 1, QVariant::fromValue((void*)pnode) );
	switch( view ) {
	case 2:
		item->setAngleOfRotation(90);
		item->setVirtualPos( QPoint(pnode->x+m_origin.x()+m_chipSize.height()/2,pnode->y+m_origin.y()-m_chipSize.width()/2) );
		break;
	//case 3:
		//item->setAngleOfRotation(180);
		//item->setVirtualPos( QPoint(pnode->x+m_origin.x()+m_chipSize.width()/2,pnode->y+m_origin.y()+m_chipSize.height()/2) );
		//break;
	case 4:
		item->setAngleOfRotation(270);
		item->setVirtualPos( QPoint(pnode->x-m_origin.x()-m_chipSize.height()/2,pnode->y+m_origin.y()+m_chipSize.width()/2) );
		break;
	default:
		item->setVirtualPos( QPoint(pnode->x+m_origin.x()-m_chipSize.width()/2,pnode->y+m_origin.y()-m_chipSize.height()/2) );
		break;
	}
	
	item->setExternalScale( desktop()->graphicsScale() );
	item->adjustPos( desktop()->graphicsMatrix() );
	item->setZValue(200);
	
	if ( isSelected ) {
		QPen pen( Qt::magenta );
		pen.setWidth( 2 );
		item->setDJSelected( true );
		item->setPen( pen );
	}
	item->show();
} 
void JQDesktopController::drawPathLine(PMapNode pcurnode,unsigned char *path)
{
	bool bHigh =false;
	unsigned char pin;
	PMapNode plastnode = pcurnode,pnextnode = 0;
	QList<QPointF> points;
	if ( 0 != pcurnode ) {
		points <<  QPointF(pcurnode->x+m_origin.x(), pcurnode->y+m_origin.y());
	}
  	while(pcurnode != 0)
	{
		if(bHigh)
		{
			pin = (*path)>>4;
		}else
			pin =(*path) & 0x0F;
		if(pin == 0 || pin > DJGAME_CHESSMAP_MAX_PINS)
			break;
		QPointF point(pcurnode->x+m_origin.x(), pcurnode->y+m_origin.y());
		points << point;
		pnextnode = pcurnode->pin[pin].pnode;
		if(pnextnode != 0)
		{
			 points << QPointF(pnextnode->x+m_origin.x(), pnextnode->y+m_origin.y());
		}
		plastnode = pcurnode;
		pcurnode = pnextnode;
		if(!bHigh)
			path ++;
		bHigh =!bHigh;
	}
	m_line->setPoints( points );
	m_line->adjustPos( desktop()->graphicsMatrix() );
	m_line->show();	
}
void JQDesktopController::playerMoveChip( const GeneralGameTrace2Head *gameTrace )
{
	PJunqiTraceMove pmove = (PJunqiTraceMove)gameTrace->chBuf;
	PMapNode 	pstart,pend;
	NODEID id;
	if(m_nodeOfStartMove != 0 && m_nodeOfEndMove != 0)
		handleMoveTimeout();
	
	if(pmove->chStartMapSite == 0 && pmove->sxy == 0)
		return;
	
	if(pmove->chStartMapSite == 0xFF && pmove->sxy == 0xFF)
	{//求和
		return;
	}
	id.district  = 0;
	id.id = GET_NODEID(pmove->chStartMapSite,0,pmove->sxy);
	pstart = SearchNode(m_chessmap,&id);
	if(pstart != 0)
	{
		CHIPPOWER power;
		power.power = JUNQI_MAX_POWER;
		pend = CheckPath(m_chessmap,&power,pstart,pmove->path);
		if(pstart->chip.id == 0 || pend == 0)
		{
		//	QMessageBox::information(c_Panel,tr("Error!"),tr("Local data error. Please quit game and login again."),QMessageBox::Ok);
			return;
		}
		
		if(pend != 0)
		{
			m_nodeOfStartMove = pstart;
			m_nodeOfEndMove   = pend;
			m_resultOfMove = pmove->chResult;
			m_moveTimer->start(1000);
		}
	}
}
void JQDesktopController::handleExchangeTimeout()
{
	m_exchangeTimer->stop();
	
	if(m_nodeOfArrange1 == 0 || m_nodeOfArrange2 == 0)
		return;
	CHIP tmp;
	
	clearNodeChip(m_nodeOfArrange1);
	clearNodeChip(m_nodeOfArrange2);
	
	memcpy(&tmp,&(m_nodeOfArrange1->chip),sizeof(CHIP));
	memcpy(&(m_nodeOfArrange1->chip),&(m_nodeOfArrange2->chip),sizeof(CHIP));
	memcpy(&(m_nodeOfArrange2->chip),&tmp,sizeof(CHIP));

	repaintNodeChip(m_nodeOfArrange1,false);
	repaintNodeChip(m_nodeOfArrange2,false);
	
	m_nodeOfArrange1 = 0;
	m_nodeOfArrange2 = 0;
}
void JQDesktopController::handleMoveTimeout()
{
	m_moveTimer->stop();
	
	m_line->hide();
	clearNodeChip(m_nodeOfEndMove);
	switch(m_resultOfMove)
	{
		case JUNQI_MOVERESULT_SET :
			if( 0 == m_lastTraceType )
			{
				if(m_nodeOfEndMove->chip.id != 0)
					playWave("kill.wav");
				else
					playWave("move.wav");
			}
			memcpy(&m_nodeOfEndMove->chip,&m_nodeOfStartMove->chip,sizeof(CHIP));
			break;
		case JUNQI_MOVERESULT_REMOVE:
			if( 0 == m_lastTraceType )
			{
				playWave("kill.wav");
			}
			break;
		case JUNQI_MOVERESULT_CLEAR :
			if( 0 == m_lastTraceType )
				playWave("dkill.wav");
			memset(&m_nodeOfEndMove->chip,0,sizeof(CHIP));
			break;
	}

	repaintNodeChip(m_nodeOfEndMove,false);
	
	clearNodeChip(m_nodeOfStartMove);
	memset(&m_nodeOfStartMove->chip,0,sizeof(CHIP));
	
	m_nodeOfStartMove = 0;
	m_nodeOfEndMove = 0;
}
void JQDesktopController::clickSave()
{
	unsigned char buffer[255];
	PGeneralGameTrace2Head ptrace = (PGeneralGameTrace2Head)buffer;
	
	 QDir dir = QDir::home();
    if (!dir.exists(QString("DJGameSave")))
	{
		 dir.mkdir("DJGameSave");
	}
	dir.cd("DJGameSave");
	if(gameWaitStatus() == DJGAME_JUNQI_TABLE_STATUS_WAIT_ARRANGE)
	{
		memset(buffer,0,255);
		GetCurrentLayout(ptrace,seat2MappedSeat(panelController()->seatId()));
		QString filename = QFileDialog::getSaveFileName(
                    panelController()->panel(),
                    tr("Save the Layout"),
                    dir.path(),
                    tr("DJGame Save Files(*.sav)"));
		if(filename.isNull())
			return;
		if(!filename.endsWith(QString(".sav")) )
			filename += QString(".sav");
		InitSaveFileHead(panelController(),filename,DJGAME_JUNQI,JUNQI_GAMETRACE_INITCHIP);
		AppendData2SaveFile(filename,(char *)buffer,sizeof(GeneralGameTrace2Head)+ptrace->chBufLen);
	}
}
void JQDesktopController::clickLoad()
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
	if( gameWaitStatus() == DJGAME_JUNQI_TABLE_STATUS_WAIT_ARRANGE
		&& isWaitingForMe() 
		&& !m_isWaitingMoveACL )
	{
		quint8 mappedSeat	= seat2MappedSeat(panelController()->seatId());
		
		memset(buffer,0,255);
		GetCurrentLayout(ptrace,mappedSeat);
		QString filename = QFileDialog::getOpenFileName(
                    panelController()->panel(),
                    tr("Load Layout"),
                    dir.path(),
                    tr("DJGame Save Files(*.sav)"));
		if(filename.isNull())
			return;
		unsigned int shLen;
		savbuf = (unsigned char *)GetSaveData(filename,DJGAME_JUNQI,JUNQI_GAMETRACE_INITCHIP,&shLen);
		if(savbuf == 0)
		{
			DJMessageBox::information( 15, panelController()->panel(),tr("Layout File Error"),tr("It isn't a DJGame save-file."),QMessageBox::Ok);
			return;
		}
		psavet = (PGeneralGameTrace2Head)savbuf;
		psavchips = (PJunqiTraceInitChip)psavet->chBuf;

		if(psavchips->chChips == pcurchips->chChips)
		{
			psavchips->chMapSite = mappedSeat;
			int i;
			for(i=0;i<=psavchips->chChips;i++)
			{
				psavchips->chip[i].chip.owner = psavchips->chMapSite;
			}
		}
			
		if(psavchips->chChips != pcurchips->chChips || !JunqiCheckArrange( pcurchips,psavchips ) )
		{
			DJMessageBox::information( 15, panelController()->panel(),tr("Layout File Error"),tr("It is a layout file but cant match here."),QMessageBox::Ok);
			return;
		}
		clearMappedSeatChips(mappedSeat);
		initMappedSeatNodes(mappedSeat,psavchips);
		repaintMappedSeatChips(mappedSeat);
	}
}
void JQDesktopController::clickChuZheng()
{
	unsigned char buffer[255];
	PGeneralGameTrace2Head ptrace = (PGeneralGameTrace2Head)buffer;
	PJunqiTraceInitChip    pcurchips = (PJunqiTraceInitChip)ptrace->chBuf;
	
	if( gameWaitStatus() == DJGAME_JUNQI_TABLE_STATUS_WAIT_ARRANGE 
		&& !m_isWaitingMoveACL
		&& isWaitingForMe() ) {
		GetCurrentLayout(ptrace,seat2MappedSeat(panelController()->seatId()));
		if(JunqiCheckArrange(pcurchips,pcurchips))
		{
			m_isWaitingMoveACL = true;
			QVariant parameters	= QVariant::fromValue((void*)this);
			sendGameTrace(JUNQI_GAMETRACE_INITCHIP,QByteArray((char *)ptrace->chBuf,ptrace->chBufLen),HandleArrangeACL,parameters);
		}else
		{
			DJMessageBox::information( 15, panelController()->panel(),tr("Layout Error"),tr("This is a invalid layout."),QMessageBox::Ok);
		}
	}
}
void JQDesktopController::clickQiuHe()
{
	if(panelController()->isLookingOn())
		return;
	JunqiTraceMove			move;
	move.chMapSite = seat2MappedSeat(panelController()->seatId());
	move.chStartMapSite = 0xFF;
	move.sxy = 0xFF;
	sendGameTrace(JUNQI_GAMETRACE_MOVE,QByteArray(( char *)&move,sizeof(JunqiTraceMove))); 
}
void JQDesktopController::clickRenShu()
{
	if(panelController()->isLookingOn())
		return;

	JunqiTraceMove			move;
	move.chMapSite = seat2MappedSeat(panelController()->seatId());
	move.chStartMapSite = 0;
	move.sxy = 0x0;
	sendGameTrace(JUNQI_GAMETRACE_MOVE,QByteArray((char *)&move,sizeof(JunqiTraceMove)));
}
void JQDesktopController::initUnderGameInfo( const QByteArray& buf )
{	
	DJDesktopController::initUnderGameInfo(buf);
	
	memcpy(&m_current,buf.data(),sizeof(JunqiCurrent));
	memset(m_chessmap,0,LANDBATTLEPANEL_CHESSMAP_MAXSIZE);
	
	clearAllChips();
	
	djDebug()<<"players"<<m_current.chPlayers<<"mapID"<<m_current.chMapID;
	for( int i = 0; i < DJGAME_JUNQI_MAX_PLAYERS; i++ ) {
		djDebug()<<"chMapSites"<<i<<m_current.chMapSites[i];
  	}
  	
  	QList<quint8> seats	= seatIdsOfPlayingUsers();
	int players	= seats.size();
	
  	if ( 2 == players )
		desktop()->setRealSize(750,750);
	//else if ( 3 == players )
		//desktop()->setRealSize(850,850);
	else
		desktop()->setRealSize(950,950);
		
	init();
}
void JQDesktopController::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	DJDesktopController::gameWait(mask,status,timeout);
	
	m_lastTraceType = 0;
	
	m_btnSave->hide();
	m_btnLoad->hide();
	m_btnChuZheng->hide();
	m_btnQiuHe->hide();
	m_btnRenShu->hide();
	switch(status)
	{
		case DJGAME_JUNQI_TABLE_STATUS_WAIT_ARRANGE :
		{
			m_btnSave->show();
			m_btnLoad->show();
			m_btnChuZheng->show();
			if(panelController()->isLookingOn() || !isWaitingForMe())
			{
				m_isWaitingMoveACL = true;
				
				m_btnSave->setEnabled(false);
				m_btnLoad->setEnabled(false);
				m_btnChuZheng->setEnabled(false);
			}else
			{	
				m_isWaitingMoveACL = false;
				
				m_btnSave->setEnabled(true);
				m_btnLoad->setEnabled(true);
				m_btnChuZheng->setEnabled(true);
			}
			break;
		}
		case DJGAME_JUNQI_TABLE_STATUS_WAIT_MOVE : 
		{
			m_isWaitingMoveACL = false;
			if(isWaitingForMe())
			{
				m_btnQiuHe->show();
				m_btnRenShu->show();
			}else
			{
				m_btnQiuHe->hide();
				m_btnRenShu->hide();
			}
			if( 0 != m_nodeOfMoveChip )
			{
				clearNodeChip(m_nodeOfMoveChip);
				repaintNodeChip(m_nodeOfMoveChip,false);
				m_nodeOfMoveChip	= 0;
			}
			break;
		}
	}
}
void JQDesktopController::gameTraceModel( const GeneralGameTrace2Head *gameTrace)
{
	switch(gameTrace->chType) {
	case JUNQI_GAMETRACE_ARRANGECHIP:
	case JUNQI_GAMETRACE_INITCHIP :
	case JUNQI_GAMETRACE_SHOWARRANGE : {
		PJunqiTraceInitChip psetchips = (PJunqiTraceInitChip)gameTrace->chBuf;
		initMappedSeatNodes(psetchips->chMapSite,psetchips);
		break;
	}
	case JUNQI_GAMETRACE_MOVE : {
		playerMoveChip(gameTrace);
		break;
	}
	case JUNQI_GAMETRACE_SETCHIP : {
		PJunqiSetChip psetchip = (PJunqiSetChip)gameTrace->chBuf;
		SetChip(m_chessmap,psetchip->chMapSite,psetchip->chip.xy,&(psetchip->chip.chip));
		break;
	}
	case JUNQI_GAMETRACE_UNION :{
		if(gameTrace->chBuf[0] == 0)
			return;
		if(m_chUnions>0)
		{
			int i;
			for(i=0;i<m_chUnions;i++)
			{
				if(m_chUnion[i] == gameTrace->chBuf[0])
					return;
			}
		}
		m_chUnion[m_chUnions] = gameTrace->chBuf[0];
		m_chUnions++;
		break;
	} 
	case JUNQI_GAMETRACE_UNIONOVER : {
		if(m_chUnions>0)
		{
			int i;
			for(i=0;i<m_chUnions;i++)
			{
				if(m_chUnion[i] == gameTrace->chBuf[0])
				{
					m_chUnion[i] = m_chUnion[m_chUnions-1];
					m_chUnions --;
					m_chUnion[m_chUnions] = 0;
					break;
				}
			}
		}
		break;
	}
	case JUNQI_GAMETRACE_OVER : {
		djDebug()<<"JUNQI_GAMETRACE_OVER"<<gameTrace->chSite<<seat2MappedSeat(gameTrace->chSite);
		if ( panelController()->seatId() == gameTrace->chSite )
			m_isSelfOver	= true;
		clearMappedSeatChips(seat2MappedSeat(gameTrace->chSite));
		initMappedSeatNodes(seat2MappedSeat(gameTrace->chSite),0);
		break;
	}
	default:
		break;
	}
}
void JQDesktopController::gameTraceView( const GeneralGameTrace2Head *gameTrace )
{
	djDebug()<<"JQDesktopController::gameTraceView";
	
	switch(gameTrace->chType) {
	case JUNQI_GAMETRACE_ARRANGECHIP:
	case JUNQI_GAMETRACE_INITCHIP :
	case JUNQI_GAMETRACE_SHOWARRANGE : {
		PJunqiTraceInitChip psetchips = (PJunqiTraceInitChip)gameTrace->chBuf;
		clearMappedSeatChips(psetchips->chMapSite);
		repaintMappedSeatChips(psetchips->chMapSite);
		break;
	}
	case JUNQI_GAMETRACE_MOVE : {
		PJunqiTraceMove pmove = (PJunqiTraceMove)gameTrace->chBuf;
		if( (pmove->chStartMapSite == 0 && pmove->sxy == 0) || (pmove->chStartMapSite == 0xFF && pmove->sxy == 0xFF)) {
			DJGameUser *puser = panelController()->player(gameTrace->chSite);
			if( 0 == puser )
				return;
			QString text = puser->userName();
			if( pmove->chStartMapSite == 0xFF && pmove->sxy == 0xFF ) {
				QString text	= puser->userName();
				if ( m_hasRequestDraw ) {
					text	+= tr(" accept draw");
				}else {
					text	+= tr(" request draw");
				}
				panelController()->insertText2Browser(text);
				//ShowAutoDisappearText(text);
						
				if ( gameTrace->chSite != panelController()->seatId()
					&& !m_hasRequestDraw ) {
					text	= DJSCHEME_AHREF
									.arg( DJSCHEME )
									.arg( DJSCHEME_EVENT_ACCEPT_DRAW_CHESS )
									.arg( puser->userId() )
									.arg( DJSCHEME_HOST )
									.arg( tr("Accept") );
					panelController()->insertText2Browser( text );
				}
				m_hasRequestDraw	= true;
			}
			return;
		}

		NODEID id;
		id.district  = 0;
		id.id = GET_NODEID(pmove->chStartMapSite,0,pmove->sxy);
		PMapNode pstart = SearchNode(m_chessmap,&id);
		if(pstart != 0) {
			clearNodeChip(pstart);
			repaintNodeChip(pstart,true);
			drawPathLine(pstart,pmove->path);
		}
		break;
	}
	case JUNQI_GAMETRACE_SETCHIP : {
		PJunqiSetChip psetchip = (PJunqiSetChip)gameTrace->chBuf;
		playWave("move.wav");
		PMapNode pnode;
		NODEID nodeid;
		nodeid.district = 0;
		nodeid.id = GET_NODEID(psetchip->chMapSite,0,psetchip->chip.xy);
		pnode =SearchNode(m_chessmap,&nodeid);
		if(pnode != 0)
		{
			clearNodeChip(pnode);
			repaintNodeChip(pnode,false);
			if(pnode->chip.id == DJGAME_JUNQI_RANK_JUNQI)
			{
				DJGameUser *user	= panelController()->player(gameTrace->chSite);
				if ( user ) {
					QString text = user->userName();
					text += tr("'s commander is killed");
					panelController()->insertText2Browser(text);
					//ShowAutoDisappearText(res);
				}
			}
		}
		break;
	}
	case JUNQI_GAMETRACE_OVER : {
		DJGameUser *user	= panelController()->player(gameTrace->chSite);
		if ( user ) {
			QString text = user->userName();
			text += " ";
 			text += tr("lose");
			panelController()->insertText2Browser(text);
		}
		break;
	}
	default:
		break;
	}
}
/*
int JQDesktopController::GetPhysicalBoard(PMapNode pnode)
{
	PChessMapHead phead  = (PChessMapHead)GetChessmapHead(m_chessmap);
	if(phead->iContext == 2)
		return 1;
	
	char iBoard = GET_BoardOfNode(&(pnode->id));
	if(iBoard == DJGAME_JUNQI_CONNECTER_AREA)
		return 1;
	if(phead->iContext == 4)
	{
		if(iBoard%2 == seat2MappedSeat(panelController()->seatId())%2)
			return 1;
		return 2;
	}
	if(phead->iContext == 3)
	{
		if(iBoard == seat2MappedSeat(panelController()->seatId()))
			return 1;
		return 2;
	}
	return 1;
}
*/
void JQDesktopController::handleMousePressed( QGraphicsSceneMouseEvent *mouseEvent )
{
	djDebug()<<"JQDesktopController::handleMousePressed";
	
	//if( panelController()->isLookingOn() )
		//return;
	
	QPointF scenePos	= mouseEvent->scenePos();
	QPointF pos	= desktop()->graphicsMatrix().inverted().map(scenePos);
	QGraphicsItem *item	= desktop()->desktopScene()->itemAt( scenePos );
	
	if( !isWaitingForMe() || m_isWaitingMoveACL )
		return;
		
	PMapNode pnode = Point2Node(m_chessmap,pos.x()-m_origin.x(),pos.y()-m_origin.y(),m_chipSize.width()/2,m_chipSize.width()/2);
	if( 0 == pnode )
		return;
	quint8 view	= viewOfNode(pnode);
	if( 1 == view || 3 == view )
		pnode = Point2Node(m_chessmap,pos.x()-m_origin.x(),pos.y()-m_origin.y(),m_chipSize.width()/2,m_chipSize.height()/2);
	else if ( 2 == view || 4 == view )
		pnode = Point2Node(m_chessmap,pos.x()-m_origin.x(),pos.y()-m_origin.y(),m_chipSize.height()/2,m_chipSize.width()/2);
	
	if(gameWaitStatus() == DJGAME_JUNQI_TABLE_STATUS_WAIT_ARRANGE)
	{
		if(pnode == 0)
		{
			if( 0 != m_nodeOfArrange1 && 0 == m_nodeOfArrange2 )
			{
				clearNodeChip(m_nodeOfArrange1);
				repaintNodeChip(m_nodeOfArrange1,false);
				m_nodeOfArrange1 = 0;
			}
			return;
		}
		DJGraphicsPixmapItem * item = (DJGraphicsPixmapItem*)findChip(pnode);
		if( 0 == item ) {
			return; 
		}
		if( 0 != m_nodeOfArrange1 && 0 != m_nodeOfArrange2 )
			handleExchangeTimeout();
		QVariant var0	= item->data(0);
		QVariant var1	= item->data(1);
		if ( !var0.isValid() || !var1.isValid() )
			return;
		//int type	= var0.toInt();
		PMapNode theNode	= static_cast<PMapNode>(var1.value<void*>());
			
		
		if(theNode->chip.owner != seat2MappedSeat(panelController()->seatId()))
			return;
		if(0 == m_nodeOfArrange1 )
		{
			clearNodeChip(pnode);
			repaintNodeChip(pnode,true);
			m_nodeOfArrange1 = pnode;
			m_nodeOfArrange2 = 0;
			return;
		}
		clearNodeChip(pnode);
		repaintNodeChip(pnode,true);
		m_nodeOfArrange2 = pnode;
		m_exchangeTimer->start(300);
		return;
	}
	if( gameWaitStatus() == DJGAME_JUNQI_TABLE_STATUS_WAIT_MOVE )
	{
		djDebug()<<"wait move";
		if( 0 == pnode || pnode == m_nodeOfMoveChip )
		{
			if( 0 != m_nodeOfMoveChip )
			{
				clearNodeChip(m_nodeOfMoveChip);
				repaintNodeChip(m_nodeOfMoveChip,false);
				m_nodeOfMoveChip = 0;
			}
			return;
		}
		if( 0 == m_nodeOfMoveChip )
		{
			DJGraphicsPixmapItem* item = (DJGraphicsPixmapItem*)findChip(pnode);
			
			if( 0 == item ) {
				return;
			}
			QVariant var0	= item->data(0);
			QVariant var1	= item->data(1);
			if ( !var0.isValid() || !var1.isValid() ) {
				return;
			}
			//int type	= var0.toInt();
			PMapNode theNode	= static_cast<PMapNode>(var1.value<void*>());
			
			if ( 0 == theNode ) {
				return;
			}
			if( theNode->chip.owner != seat2MappedSeat(panelController()->seatId()) ) {
				return;
			}
			m_nodeOfMoveChip = pnode;
			clearNodeChip(pnode);
			repaintNodeChip(pnode,true);
			return;
		}
		if(pnode->chip.id != 0 && pnode->chip.owner == seat2MappedSeat(panelController()->seatId()))
		{
			clearNodeChip(m_nodeOfMoveChip);
			repaintNodeChip(m_nodeOfMoveChip,false);
			clearNodeChip(pnode);
			repaintNodeChip(pnode,true);
			m_nodeOfMoveChip = pnode;
			return;
		}
		if(item != 0 && IsFriendlyPlayer(m_chUnion,m_chUnions,pnode->chip.owner,m_nodeOfMoveChip->chip.owner))
		{
			return;
		}
		unsigned char *p;
	//	printf("move %04X->%04X\n",c_pMoveChipNode->id.id,pnode->id.id);
		if(MoveChip(m_chessmap,GET_BoardOfNode( (&(m_nodeOfMoveChip->id))),m_nodeOfMoveChip->id.id & 0xFF,
						GET_BoardOfNode( (&(pnode->id))),pnode->id.id & 0xFF,&p) )
		{
	//		printf("Move OK\n");
			char buffer[100],chPathLen = 0;
			JunqiTraceMove			*pmove;
			pmove = (JunqiTraceMove *)buffer;
			pmove->chMapSite = seat2MappedSeat(panelController()->seatId());
			pmove->chStartMapSite = GET_BoardOfNode( (&(m_nodeOfMoveChip->id)));
			pmove->sxy = m_nodeOfMoveChip->id.id & 0xFF;
			while(*p != 0)
			{
				pmove->path[chPathLen] = *p;
				p++;
				chPathLen ++;
			}
			pmove->path[chPathLen] = 0;
			pmove->path[chPathLen+1] = 0;
			setWaitingMoveACL(true);
			QVariant parameters	= QVariant::fromValue((void*)this);
			sendGameTrace(JUNQI_GAMETRACE_MOVE,QByteArray(buffer,sizeof(JunqiTraceMove)+chPathLen),HandleArrangeACL,parameters);
			return;	
		}
	}
}
void JQDesktopController::locateAllNonMatrimoveItems()
{
	DJDesktopController::locateAllNonMatrimoveItems();
	
	QPoint pos( desktop()->realWidth(), (desktop()->realHeight() >> 1) + 60 );
	QMatrix matrix	= desktop()->graphicsMatrix();
	pos	= matrix.map( pos );
	//pos.rx()	-= m_btnStart->width()>>1;
	m_btnSave->move( pos.x() - m_btnSave->width(), pos.y() );
	m_btnLoad->move( pos.x() - m_btnLoad->width(), m_btnSave->pos().y() + m_btnSave->height() );
	m_btnChuZheng->move( pos.x() - m_btnChuZheng->width(), m_btnLoad->pos().y() + m_btnLoad->height() );
	m_btnQiuHe->move( pos.x() - m_btnQiuHe->width(), m_btnChuZheng->pos().y() + m_btnChuZheng->height() );
	m_btnRenShu->move( pos.x() - m_btnRenShu->width(), m_btnQiuHe->pos().y() + m_btnQiuHe->height() );
}
void JQDesktopController::repaintAll()
{
	DJDesktopController::repaintAll();
	
	repaintBoard();
	
	for(int i=1;i<=4;i++)
	{
		clearMappedSeatChips(i);
		repaintMappedSeatChips(i);
	}
}
void JQDesktopController::repaintCurrentStatus()
{
	djDebug()<<"JQDesktopController::repaintCurrentStatus";
	
	repaintAll();
}
bool JQDesktopController::isCloseable() const
{
	bool closable	= DJDesktopController::isCloseable();
	if ( !closable ) {
		if ( m_isSelfOver )
			closable	= true;
		else {
			QList<quint8> seats	= seatIdsOfPlayingUsers();
			if ( !seats.contains(panelController()->seatId()) )
				closable	= true;
		}
	}
	return closable;
}
bool JQDesktopController::isUserPlaying( quint8 seatId ) const
{
	for( quint8 seat = 0; seat <= DJGAME_JUNQI_MAX_PLAYERS; ++seat ) {
		if ( seatId == m_current.chMapSites[seat] )
			return true;
	}
	return false;
}
void JQDesktopController::GetCurrentLayout(PGeneralGameTrace2Head ptrace,unsigned char chMapSite)
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
			pnode = SearchNode(m_chessmap,&id);
			if(pnode != 0 && pnode->chip.id != 0)
			{
				psetchips->chip[k].xy = (unsigned char)(id.id & 0xFF);
				psetchips->chip[k].chip.id = (unsigned char)(pnode->chip.id);
				psetchips->chip[k].chip.owner = chMapSite;
				psetchips->chip[k].chip.power = (unsigned char )(pnode->chip.power.power);
				k++;
			}
		}
	}
	ptrace->chTable = panelController()->tableId();
	ptrace->chSite  = mappedSeat2Seat(chMapSite);
	ptrace->chType  = JUNQI_GAMETRACE_INITCHIP;
	ptrace->chBufLen = k*sizeof(JunqiTraceChip) + sizeof(JunqiTraceInitChip);
	psetchips->chMapSite = chMapSite;
	psetchips->chChips = k;
}
void JQDesktopController::repaintBoard()
{ 
	djDebug()<<"repaintBoard";
	
	QList<quint8> seats	= seatIdsOfPlayingUsers();
	int players	= seats.size();
	djDebug()<<"players"<<players;
 	QPixmap pix;
	switch(players) {
	case 2:
		//pix = QPixmap(":/LandBattleRes/image/board2.png");
		pix = QPixmap(":/LandBattleRes/image/jq2board.png");
		break;
	case 3: 
		//pix = QPixmap(":/LandBattleRes/image/board3.png");
		pix = QPixmap(":/LandBattleRes/image/jq3board.png");
		break;
	case 4: 
		//pix = QPixmap(":/LandBattleRes/image/board4.png");
		pix = QPixmap(":/LandBattleRes/image/jq4board.png");
		break;
	default:
		break;
	} 
	if ( !pix.isNull() ) {
		m_origin.setX( (desktop()->realWidth()-pix.width())>>1 );
		m_origin.setY( (desktop()->realHeight()-pix.height())>>1 );
		m_board->setPixmap( pix );
		m_board->setVirtualPos( m_origin );
		m_board->adjustPos( desktop()->graphicsMatrix() );
	}
}
void JQDesktopController::locateShowPositions( QList<QPoint>& points, QList<Qt::Alignment>& aligns )
{
	djDebug() << "JQDesktopController::locateShowPositions";
	
	int centerX	= desktop()->realWidth() >> 1;
	int centerY	= desktop()->realHeight() >> 1;
	int leftX	= 1;
	int rightX	= desktop()->realWidth();
	int topY	= 1;
	int bottomY	= desktop()->realHeight(); 
	int space	= 136;
	
	points.clear();
	aligns.clear();
	
	for ( quint8 seat = 0; seat <= panelController()->numberOfSeats(); ++seat ) {
		points 	<< QPoint();
		aligns	<< 0;
	}
	QList<quint8> seats	= seatIdsOfPlayingUsers();
	int players	= seats.size();
	
	djDebug()<<"players"<<players;
	
	foreach( quint8 seat, seats ) {
 		quint8 view = seat2View(seat);
 		djDebug()<<"seat"<<seat<<"map to view"<<view;
 		if ( 4 == players  ) {
    		switch( view ) {
        	case 1: {
        		points[seat].setX( centerX - space );
        		points[seat].setY( bottomY - 1 );
        		aligns[seat]	= Qt::AlignRight | Qt::AlignBottom;
        		break;
        	} 
        	case 2: {
        		points[seat].setX( leftX );
        		points[seat].setY( centerY - space );
        		aligns[seat]	= Qt::AlignBottom;
        		break;
        	}
        	case 3:  {
        		points[seat].setX( centerX + space );
        		points[seat].setY( topY );
        		//aligns[seat]	= Qt::AlignRight;
        		break;
        	}
        	case 4:  {
        		points[seat].setX( rightX );
        		points[seat].setY( centerY - space );
        		aligns[seat]	= Qt::AlignRight | Qt::AlignBottom;
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
        		break;
        	}
        	case 4:  {
        		points[seat].setX( rightX );
        		points[seat].setY( topY );
        		aligns[seat]	= Qt::AlignRight;
        		break;
        	}
        	default:
        		break;
          	}
    	}else if ( 2 == players ) {
    		/*
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
        	*/
        	if ( 1 == view ) {
    			points[seat].setX( rightX );
        		points[seat].setY( bottomY );
        		aligns[seat]	= Qt::AlignRight | Qt::AlignBottom;
        	}else {
        		points[seat].setX( rightX );
        		points[seat].setY( topY );
        		aligns[seat]	= Qt::AlignRight;
        	}
    	}
 	}
}
void JQDesktopController::locateNamePositions( QList<QPoint>& points, QList<Qt::Alignment>& aligns )
{
	djDebug() << "JQDesktopController::locateNamePositions";
	
	points.clear();
	aligns.clear();
	
	QList<quint8> seats	= seatIdsOfPlayingUsers();
	int players	= seats.size();
		
	for ( int i = 0; i <= panelController()->numberOfSeats(); ++i ) {
		points 	<< QPoint();
		aligns	<< 0;
	}
	
	foreach( quint8 seat, seats ) {
     	QRect showRect	= rectOfSeatShow( seat );
     	djDebug()<<"showRect"<<"seat"<<seat<<showRect;
    	quint8 view = seat2View(seat);
    	if ( 4 == players  ) {
    		switch( view ) {
        	case 1: {
        		points[seat].setX( showRect.left() - 1 );
    			points[seat].setY( showRect.bottom() - 1 );
    			aligns[seat]	= Qt::AlignRight | Qt::AlignBottom;
        		break;
        	} 
        	case 2:  {
        		points[seat].setX( showRect.right() + 1 );
        		points[seat].setY( showRect.bottom() );
        		aligns[seat]	= Qt::AlignBottom;
        		break;
        	}
        	case 3:  {
        		points[seat].setX( showRect.right() + 1 );
        		points[seat].setY( showRect.top() + 1 );
        		//aligns[seat]	= Qt::AlignRight;
        		break;
        	}
        	case 4:  {
        		points[seat].setX( showRect.left() - 1 );
        		points[seat].setY( showRect.bottom() );
        		aligns[seat]	= Qt::AlignRight | Qt::AlignBottom;
        		break;
        	}
        	default:
        		break;
          	}
    	}else if ( 3 == players ) {
    		switch( view ) {
        	case 1: {
        		points[seat].setX( desktop()->realWidth()>>1 );
    			points[seat].setY( desktop()->realHeight() - 1 );
    			aligns[seat]	= Qt::AlignHCenter | Qt::AlignBottom;
        		break;
        	} 
        	case 2:  {
        		points[seat].setX( showRect.right() + 1 );
        		points[seat].setY( showRect.top() + 1 );
        		break;
        	}
        	case 4:  {
        		points[seat].setX( showRect.left() - 1 );
        		points[seat].setY( showRect.top() + 1 );
        		aligns[seat]	= Qt::AlignRight;
        		break;
        	}
        	default:
        		break;
          	}
    	}else if ( 2 == players ) {
    		/*
    		if ( 1 == view ) {
        		points[seat].setX( showRect.right() + 1 );
        		points[seat].setY( showRect.bottom() - 1 );
        		aligns[seat]	= Qt::AlignBottom;
        	}else {
        		points[seat].setX( showRect.right() + 1 );
        		points[seat].setY( showRect.top() + 1 );
        	}
        	*/
        	if ( 1 == view ) {
        		points[seat].setX( showRect.left() - 1 );
        		points[seat].setY( showRect.bottom() - 1 );
        		aligns[seat]	= Qt::AlignRight | Qt::AlignBottom;
        	}else {
        		points[seat].setX( showRect.left() - 1 );
        		points[seat].setY( showRect.top() + 1 );
        		aligns[seat]	= Qt::AlignRight;
        	}
    	}
    }	
}
void JQDesktopController::locateClockPosition( QPoint& point, Qt::Alignment& align )
{
	djDebug() << "JQDesktopController::locateClockPositions";
	
	QList<quint8> seats	= seatIdsOfPlayingUsers();
	int players	= seats.size();
	
	if ( players > 2 ) {
		point.setX( centerPoint().x() );
    	point.setY( centerPoint().y() - 60 );
    	align	= Qt::AlignCenter;
	}else {
		point.setX( desktop()->realWidth() );
    	point.setY( centerPoint().y() );
    	align	= Qt::AlignRight | Qt::AlignVCenter;
	}
}
