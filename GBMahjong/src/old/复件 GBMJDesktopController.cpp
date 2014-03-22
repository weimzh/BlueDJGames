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
#include "GBMJDesktopController.h"
#include "DJPanelController.h"
#include "DJPanel.h"
#include "DJGameRoom.h"
#include "DJDesktop.h"
#include "DJGraphicsTextItem.h"
#include "DJGraphicsPixmapItem.h"
#include "DJGameUser.h"
#include "DJMessageBox.h"
#include "Utility.h"
#include "DJDesktopScene.h"
#include "MJSelect.h"
#include "MJTing.h"
#include "mjstrings.h"

static QSize djToolButtonSize(24,24);

GBMJDesktopController::GBMJDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent )
	: DJDesktopMJController(panelController,size,parent)
{
	djDebug()<<"GBMJDesktopController constructor"<<size;
	
	PMahjongRoom pmjroom = (PMahjongRoom)((panelController->gameRoom())->privateRoom());
	SetRule(&(pmjroom->rule));
	
	m_toolbar	= new QWidget(desktop());
    QHBoxLayout *layout	= new QHBoxLayout( m_toolbar );
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    
    m_style = new QToolButton;
    m_style->setEnabled(true);
	m_style->setToolTip(tr("change style of pool cards"));
	m_style->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/arrange.png") );
	m_style->setIconSize( djToolButtonSize );
    connect(m_style,SIGNAL(clicked()),this,SLOT(ClickStyle()));
    layout->addWidget( m_style );
    
    m_qi = new QToolButton;
    m_qi->setEnabled(false);
	m_qi->setToolTip(tr("pass"));
	m_qi->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/qi.png") );
	m_qi->setIconSize( djToolButtonSize );
    connect(m_qi,SIGNAL(clicked()),this,SLOT(ClickQi()));
    layout->addWidget( m_qi );
    
    m_chu = new QToolButton;
    m_chu->setEnabled(false);
	m_chu->setToolTip(tr("throw a card"));
	m_chu->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/chu.png") );
	m_chu->setIconSize( djToolButtonSize );
    connect(m_chu,SIGNAL(clicked()),this,SLOT(ClickChu()));
    layout->addWidget( m_chu );
    
    m_chi = new QToolButton;
    m_chi->setEnabled(false);
	m_chi->setToolTip(tr("chi a card"));
	m_chi->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/chi.png") );
	m_chi->setIconSize( djToolButtonSize );
    connect(m_chi,SIGNAL(clicked()),this,SLOT(ClickChi()));
    layout->addWidget( m_chi );
    
    m_peng = new QToolButton;
    m_peng->setEnabled(false);
	m_peng->setToolTip(tr("peng a card"));
	m_peng->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/peng.png") );
	m_peng->setIconSize( djToolButtonSize );
    connect(m_peng,SIGNAL(clicked()),this,SLOT(ClickPeng()));
    layout->addWidget( m_peng );
    
    m_gang = new QToolButton;
    m_gang->setEnabled(false);
	m_gang->setToolTip(tr("gang a card"));
	m_gang->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/gang.png") );
	m_gang->setIconSize( djToolButtonSize );
    connect(m_gang,SIGNAL(clicked()),this,SLOT(ClickGang()));
    layout->addWidget( m_gang );
    
    m_ting = new QToolButton;
    m_ting->setEnabled(false);
	m_ting->setToolTip(tr("ready to hu"));
	m_ting->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/ting.png") );
	m_ting->setIconSize( djToolButtonSize );
    connect(m_ting,SIGNAL(clicked()),this,SLOT(ClickTing()));
    layout->addWidget( m_ting );
    
    m_hu = new QToolButton;
    m_hu->setEnabled(false);
	m_hu->setToolTip(tr("hu"));
	m_hu->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/hu.png") );
	m_hu->setIconSize( djToolButtonSize );
    connect(m_hu,SIGNAL(clicked()),this,SLOT(ClickHu()));
    layout->addWidget( m_hu );
        								
	c_bReqTing = false;
	
	StaticInitDesktop();
}
GBMJDesktopController::~GBMJDesktopController()
{
	djDebug()<<"GBMJDesktopController destructor";
}

void GBMJDesktopController::gameInfo( const QByteArray& buf )
{
	djDebug()<<"GBMJDesktopController::gameInfo";
	
	DJDesktopMJController::gameInfo( buf );
	
	m_lastMGangCard	= 0;
	memcpy(&c_current,buf.data(),sizeof(MahjongCurrent));
	RepaintWallCards();
	
	for ( quint8 seat = 1; seat <= panelController()->numberOfSeats(); ++seat) {
		clearSeatTypeItems(seat,DJGAME_DESKTOP_TYPE_HAND);
		clearSeatTypeItems(seat,DJGAME_DESKTOP_TYPE_STATIC);
		clearSeatTypeItems(seat,DJGAME_DESKTOP_TYPE_POOL);
		clearSeatTypeItems(seat,DJGAME_DESKTOP_TYPE_FLOWER);
		clearSeatTypeItems(seat,DJGAME_DESKTOP_TYPE_HU_CARD);
		clearSeatTypeItems(seat,DJGAME_DESKTOP_TYPE_HU_TEXT);
	}
	djDebug()<<"GBMJDesktopController::gameInfo end";
}
void GBMJDesktopController::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	djDebug() << "GBMJDesktopController::gameWait" << mask << status << timeout;
	djDebug() << "m_lastMGangCard = " << hex << m_lastMGangCard;
	bool bDIY	= false;
	bool containsFlower	= false;
	
	DJDesktopMJController::gameWait(mask,status,timeout);
		
	switch(status)
	{
		case DJGAME_MAHJONG_TABLE_STATUS_WAIT_OUT :
		{
			djDebug() << "DJGAME_MAHJONG_TABLE_STATUS_WAIT_OUT";
			if(isWaitingForMe())
			{
				if(!panelController()->isLookingOn())
				{
					quint8 buffer[20],pages;
					pages = GetPlayerCurrentCards(panelController()->seatId(),buffer,20);
					for(int i=0;i<pages;i++)
					{
						if(MAHJONG_ISFLOWERCARD(buffer[i]))
						{
							containsFlower	= true;
							SendGameTrace(MAHJONG_GAMETRACE_GANGFLOWER,(char *)&(buffer[i]),1);
						}
					}
					if ( containsFlower ) {
						djDebug()<<"contains flower card";
						//only one chance to catch qiang gang hu,clear gang flag if not hu
						m_lastMGangCard	= 0;
						return;
					}
				}
				djDebug() << "waiting for me";
				SetThrowFlag(true);
				if(playerHuStatus(panelController()->seatId(),true)) {
					bDIY	= true;
					m_hu->setEnabled(true);
				}else
					m_hu->setEnabled(false);
				if(playerTingStatus(panelController()->seatId()))
					m_ting->setEnabled(true);
				else
					m_ting->setEnabled(false);
				if(playerGangStatus(panelController()->seatId(),0,NULL,0))
					m_gang->setEnabled(true);
				else
					m_gang->setEnabled(false);
				m_chu->setEnabled(true);
			}else
			{
				djDebug() << "not waiting for me";
				SetThrowFlag(false);
				m_ting->setEnabled(false);
				if ( 0 != m_lastMGangCard && playerHuStatus(panelController()->seatId(),false,m_lastMGangCard)) {
					m_hu->setEnabled(true);
				}else {
					m_hu->setEnabled(false);
				}
				m_chu->setEnabled(false);
				m_gang->setEnabled(false);
			}
			m_qi->setEnabled(false);
			m_chi->setEnabled(false);
			m_peng->setEnabled(false);
			break;
		}
		case DJGAME_MAHJONG_TABLE_STATUS_WAIT_EATCOLLISION :
		{
			djDebug() << "DJGAME_MAHJONG_TABLE_STATUS_WAIT_EATCOLLISION";
			SetThrowFlag(false);
			if(isWaitingForMe())
			{
				bool b=false;
				if(playerHuStatus(panelController()->seatId(),false))
				{
					b = true;
					m_hu->setEnabled(true);
				}else
					m_hu->setEnabled(false);
				if(playerGangStatus(panelController()->seatId(),GetLastThrowCard(),NULL,0))
				{
					b = true;
					m_gang->setEnabled(true);
				}else
					m_gang->setEnabled(false);
				if(playerChiMask(panelController()->seatId()) != 0)
				{
					b = true;
					m_chi->setEnabled(true);
				}else
					m_chi->setEnabled(false);
				if(playerPengStatus(panelController()->seatId()))
				{
					b = true;
					m_peng->setEnabled(true);
				}else
					m_peng->setEnabled(false);
				m_qi->setEnabled(true);
				if(!b)
					ClickQi();
			}else
			{
				m_hu->setEnabled(false);
				m_gang->setEnabled(false);
				m_qi->setEnabled(false);
				m_chi->setEnabled(false);
				m_peng->setEnabled(false);
			}
			
			m_ting->setEnabled(false);
			m_chu->setEnabled(false);
			break;
		}
	}
	if (hasPlayerTing(panelController()->seatId())) {
		djDebug() << "self is ting";
		do {
			if ( m_hu->isEnabled() ) {
    			djDebug() << "hu is enabled";
    			if ( m_bTingZimo ) {
    				if ( bDIY ) {
    					ClickHu();
    					break;
    				}
    			}else {
    				ClickHu();
    				break;
    			}
    		}
    		if ( m_gang->isEnabled() ) {
    			djDebug() << "gang is enabled";
    			if ( m_bTingAutoGang ) {
    				ClickGang();
    				break;
    			}
    		}
    		if ( m_chu->isEnabled() ) {
    			djDebug() << "chu is enabled";
    			ClickChu();
    			break;
    		}
    		if ( m_qi->isEnabled() ) {
    			djDebug() << "qi is enabled";
    			ClickQi();
    			break;
    		}
		}while(false);	
	}
	//only one chance to catch qiang gang hu,clear gang flag if not hu
	m_lastMGangCard	= 0;
}
void GBMJDesktopController::gameTraceModel( const GeneralGameTrace2Head *gameTrace )
{
	m_lastThrowSeat	= GetLastThrowSeat();
	
	if(DJGAME_PRIVATE_TRACE(gameTrace->chType))
		SetSeatPrivate(gameTrace->chSite);
	switch(gameTrace->chType)
	{
		case MAHJONG_GAMETRACE_RESET :
		//case MAHJONG_GAMETRACE_DEAL :
		case MAHJONG_GAMETRACE_INIT :
		{
			if(gameTrace->chType == MAHJONG_GAMETRACE_RESET)
			{
				RemoveWallCard(gameTrace->chBufLen);
			}
			//if(gameTrace->chType ==  MAHJONG_GAMETRACE_DEAL)
				//SetSeatPrivate(gameTrace->chSite);
			ResetPlayerCards(gameTrace->chSite,(quint8*)gameTrace->chBuf,gameTrace->chBufLen);
			break;
		}
		case MAHJONG_GAMETRACE_OUT :
		{
			PlayerThrow(gameTrace->chSite,gameTrace->chBuf[0]);
			break;
		}
		case MAHJONG_GAMETRACE_PICKUP :
		case MAHJONG_GAMETRACE_DRAW :
		{
			if(gameTrace->chType ==  MAHJONG_GAMETRACE_PICKUP)
				SetSeatPrivate(gameTrace->chSite);
			if(GetSeatPrivate(gameTrace->chSite) && gameTrace->chType == MAHJONG_GAMETRACE_DRAW)
				break;
			PlayerPickup(gameTrace->chSite,gameTrace->chBuf[0]);
			break;
		}
		case MAHJONG_GAMETRACE_WALLSTART :
		{
			SetWallStartPoint(gameTrace->chBuf[0]);
			break;
		}
		case MAHJONG_GAMETRACE_EATCOLLISION :
		{
			if(gameTrace->chBuf[0] == gameTrace->chBuf[1])
			{
				PlayerPENG(gameTrace->chSite,gameTrace->chBuf[0]);
			}else {
				PlayerCHI(gameTrace->chSite,gameTrace->chBuf[0],gameTrace->chBuf[1],gameTrace->chBuf[2]);
			}
			break;
		}
		case MAHJONG_GAMETRACE_GANGFLOWER :
		{
			if(MAHJONG_ISFLOWERCARD(gameTrace->chBuf[0]))
			{
				AppendPlayerFlowerCard(gameTrace->chSite,gameTrace->chBuf[0]);
			}else
				PlayerGANG(gameTrace->chSite,gameTrace->chBuf[0]);
			break;
		}
		case MAHJONG_GAMETRACE_TING :
		{
			djDebug()<<"MAHJONG_GAMETRACE_TING";
			PlayerTing(gameTrace->chSite);
			break;
		}
	}
}
void GBMJDesktopController::gameTraceView( const GeneralGameTrace2Head *gameTrace )
{		
	switch(gameTrace->chType)
	{
		case MAHJONG_GAMETRACE_RESET :
		//case MAHJONG_GAMETRACE_DEAL :
		case MAHJONG_GAMETRACE_INIT :
		{
	//		printf("trace=%02x seat=%d buflen=%d\n",gameTrace->chType,gameTrace->chSite,gameTrace->chBufLen);
			RenewWall();
			RepaintPlayerCards(gameTrace->chSite);
			
			break;
		}
		case MAHJONG_GAMETRACE_OUT :
		{
			djDebug() << "MAHJONG_GAMETRACE_OUT";
			RepaintPlayerCards(gameTrace->chSite);
			RepaintPoolCards(m_lastThrowSeat);
			RepaintPoolCards(gameTrace->chSite);
			quint8 seat	= gameTrace->chSite;
			quint8 card	= gameTrace->chBuf[0];
			djDebug() << "seat =" << seat << "card =" << card;
			QString wavName	= QString("%1.wav").arg(card,2,16,QChar::fromLatin1('0'));
    		playWave(wavName);
    		
    		if ( gameTrace->chSite == panelController()->seatId() )
    			RepaintView1Flowers(gameTrace->chSite);
			break;
		}
		case MAHJONG_GAMETRACE_PICKUP :
		case MAHJONG_GAMETRACE_DRAW :
		{
			if(GetSeatPrivate(gameTrace->chSite) && gameTrace->chType == MAHJONG_GAMETRACE_DRAW)
				break;
			RepaintPlayerCards(gameTrace->chSite);
			RenewWall();
	/*		if ( MAHJONG_ISFLOWERCARD( gameTrace->chBuf[0] ) && !c_MahjongPanel->isLookingOn())
			{//自动补花
				char ch = gameTrace->chBuf[0];
				SendGameTrace(MAHJONG_GAMETRACE_GANGFLOWER,&ch,1,NULL);
			}*///改在gameWait中自动补�?			break;
		}
		case MAHJONG_GAMETRACE_WALLSTART :
		{
			RepaintWallCards();
			break;
		}
		case MAHJONG_GAMETRACE_EATCOLLISION : {
			djDebug() << "MAHJONG_GAMETRACE_EATCOLLISION" << gameTrace->chBuf[0];
			if(gameTrace->chBuf[0] == gameTrace->chBuf[1]){
				playWave("peng.wav");
			}else {
				playWave("chi.wav");
			}
			RepaintPlayerCards(gameTrace->chSite);
			RepaintPoolCards(m_lastThrowSeat);
			break;
		}
		case MAHJONG_GAMETRACE_GANGFLOWER :
		{
			djDebug() << "MAHJONG_GAMETRACE_GANGFLOWER";
			quint8 mjcard	= gameTrace->chBuf[0];
			djDebug() << "mjcard = " << hex << mjcard;
			if ( MAHJONG_ISFLOWERCARD( mjcard ) ) {
				playWave("bu.wav");
				RepaintPlayerFlowers(gameTrace->chSite);
			}else {
				playWave("gang.wav");
			
				RepaintPlayerCards(gameTrace->chSite);
				RepaintPoolCards(m_lastThrowSeat);
				if ( !MAHJONG_ISDIY(mjcard) ) {
					djDebug() << "minggang";
					//ming gang,check qiang gang hu
					m_lastMGangCard	= MAHJONG_CARD(mjcard);
				}
			}
			break;
		}
		case MAHJONG_GAMETRACE_TING :
		{
			RepaintPlayerCards(gameTrace->chSite);
			break;
		}
		case MAHJONG_GAMETRACE_HU :
		{
			//Hu(gameTrace->chSite,ptrace->chBuf,gameTrace->chBufLen);
			playWave("hu.wav");
			DisplayHu(gameTrace->chSite,gameTrace->chBuf,gameTrace->chBufLen);
			break;
		}
	}
}
QString	GBMJDesktopController::userItemNameSuffix( DJGameUser* user )
{
	if ( c_current.chMaster && c_current.chMaster == user->seatId() )
		return tr("(zhuang)");
	else
		return QString();
}
void GBMJDesktopController::StaticInitDesktop()
{
	djDebug()<<"GBMJDesktopController::StaticInitDesktop";
	
	DJDesktopMJController::StaticInitDesktop();
	
	memset(&c_current,0,sizeof(MahjongCurrent));
	m_qi->setEnabled(false);
	m_chu->setEnabled(false);
	m_chi->setEnabled(false);
	m_peng->setEnabled(false);
	m_gang->setEnabled(false);
	m_ting->setEnabled(false);
	m_hu->setEnabled(false);
}
quint8 GBMJDesktopController::GetMahjongThrowTraceID(quint8 card)
{
	if(panelController()->isLookingOn())
		return 0;
	if(gameWaitStatus() == DJGAME_MAHJONG_TABLE_STATUS_WAIT_OUT && isWaitingForMe())
	{
		m_hu->setEnabled(false);
		m_chu->setEnabled(false);
		m_ting->setEnabled(false);
		m_gang->setEnabled(false);
		if(c_bReqTing)
			return MAHJONG_GAMETRACE_TING;
		return MAHJONG_GAMETRACE_OUT;
	}
	if(gameWaitStatus() == DJGAME_MAHJONG_TABLE_STATUS_WAIT_EATCOLLISION && isWaitingForMe())
	{
		m_hu->setEnabled(false);
		m_qi->setEnabled(false);
		m_chi->setEnabled(false);
		m_peng->setEnabled(false);
		m_gang->setEnabled(false);
		return MAHJONG_GAMETRACE_EATCOLLISION;
	}
	return 0;
}
void GBMJDesktopController::ClickStyle()
{
	int style	= styleOfPoolCards();
	style++;
	style %= 2;
	setStyleOfPoolCards( style );
	
	for ( int seatId = 1; seatId <= panelController()->numberOfSeats(); seatId++ ) {
		RepaintPoolCards( seatId );
	}
}
void GBMJDesktopController::ClickQi()
{
	SendGameTrace(MAHJONG_GAMETRACE_EATCOLLISION);
	m_hu->setEnabled(false);
				m_gang->setEnabled(false);
				m_qi->setEnabled(false);
				m_chi->setEnabled(false);
				m_peng->setEnabled(false);
}
void GBMJDesktopController::ClickChu()
{
	//DJGameMJDesktop::handleItemClicked(Qt::RightButton,QPoint(),0);
}
void GBMJDesktopController::ClickChi()
{
	quint8 mask = playerChiMask(panelController()->seatId());
	if(mask == 0)
		return ClickQi();
	MJSelect dlg(panelController()->panel());
	quint8 ch[3][3],chSend[3][3];
	quint8 index = 0;
	quint8 chLastCard = MAHJONG_CARD(GetLastThrowCard());
	if(mask & DJGAME_MAHJONG_CHIMASK_12X)
	{
		ch[index][2] = chLastCard;
		ch[index][1] = chLastCard-1;
		ch[index][0] = chLastCard-2;
		chSend[index][0] = ch[index][0];
		chSend[index][1] = ch[index][1];
		dlg.addRow(ch[index],3);
		index ++;
	}
	if(mask & DJGAME_MAHJONG_CHIMASK_1X3)
	{
		ch[index][1] = chLastCard;
		ch[index][2] = chLastCard+1;
		ch[index][0] = chLastCard-1;
		chSend[index][0] = ch[index][0];
		chSend[index][1] = ch[index][2];
		dlg.addRow(ch[index],3);
		index ++;
	}
	if(mask & DJGAME_MAHJONG_CHIMASK_X23)
	{
		ch[index][0] = chLastCard;
		ch[index][1] = chLastCard+1;
		ch[index][2] = chLastCard+2;
		chSend[index][0] = ch[index][1];
		chSend[index][1] = ch[index][2];
		dlg.addRow(ch[index],3);
		index ++;
	}
	if(index > 1)
	{
		dlg.exec();
		index = dlg.indexOfSelected();
	}else if(index == 1)
		index = 0;
	else index = 5;
	if(index <3)
		SendGameTrace(MAHJONG_GAMETRACE_EATCOLLISION,(char *)&(chSend[index][0]),2);	
	m_hu->setEnabled(false);
	m_gang->setEnabled(false);
	m_qi->setEnabled(false);
	m_chi->setEnabled(false);
	m_peng->setEnabled(false);
	
}
void GBMJDesktopController::ClickPeng()
{
	char buf[3];
	buf[0] = GetLastThrowCard();
	buf[1] = buf[0];
	SendGameTrace(MAHJONG_GAMETRACE_EATCOLLISION,buf,2);
	m_hu->setEnabled(false);
	m_gang->setEnabled(false);
	m_qi->setEnabled(false);
	m_chi->setEnabled(false);
	m_peng->setEnabled(false);
}
void GBMJDesktopController::ClickGang()
{
	if( gameWaitStatus() == DJGAME_MAHJONG_TABLE_STATUS_WAIT_EATCOLLISION)
	{
		char buf[2];
		buf[0] = GetLastThrowCard();
		SendGameTrace(MAHJONG_GAMETRACE_GANGFLOWER,buf,1);
		m_hu->setEnabled(false);
		m_gang->setEnabled(false);
		m_qi->setEnabled(false);
		m_chi->setEnabled(false);
		m_peng->setEnabled(false);
	}else if(gameWaitStatus() == DJGAME_MAHJONG_TABLE_STATUS_WAIT_OUT)
	{
		unsigned char buf[15];
		quint8 chPages = playerGangStatus(panelController()->seatId(),0,buf,15);
		if(chPages == 1)
		{
			SendGameTrace(MAHJONG_GAMETRACE_GANGFLOWER,(char *)buf,1);
		}else if(chPages > 1)
		{
			MJSelect dlg(panelController()->panel());
			for(int i=0;i<chPages;i++)
				dlg.addRow(&buf[i],1);
			dlg.exec();
			chPages = dlg.indexOfSelected();
			SendGameTrace(MAHJONG_GAMETRACE_GANGFLOWER,(char*)&(buf[chPages]),1);
		}
	}
}
void GBMJDesktopController::ClickTing()
{
	if ( !hasPlayerTing(panelController()->seatId()) ) {
		MJTing tingDlg;
		if ( QDialog::Accepted == tingDlg.exec() ) {
			m_bTingAutoGang	= tingDlg.isAutoGang();
			m_bTingZimo	= tingDlg.isOnlyZimoHu();
			c_bReqTing = true;
			//DJGameMJDesktop::handleItemClicked(Qt::RightButton,QPoint(),0);
			c_bReqTing = false;
		}
	}
}

void GBMJDesktopController::ClickHu()
{
	djDebug() << "clickhu";
	m_hu->setEnabled(false);
	SendGameTrace(MAHJONG_GAMETRACE_HU);
}
void GBMJDesktopController::DisplayHu(unsigned char chSite,const unsigned char *data,unsigned char chLen)
{
	djDebug()<<"GBMJDesktopController::DisplayHu"<<"seat"<<chSite<<"len"<<chLen;
	DebugBuffer( (const char*)data, chLen );
    unsigned char ch;
    QString strName;
    DJGraphicsPixmapItem *item;
    DJGraphicsTextItem	*itemText;
    QPixmap			pix;
    QFont			font;
    int x,y,i;
    
    for ( int i = 0; i <= panelController()->numberOfSeats(); ++i ) {	
		clearSeatTypeItems(i,0);
	}
	
	DJGameUser *user	= panelController()->player( chSite );
	if ( user )
		strName	= user->userName();
	else
		strName	= tr("no name");
    if(data[0] == chSite || data[0] == 0 || data[0]>4)
    {
		strName+= tr("zi mo");
		strName += tr("hu");
    }else
    {
		strName += tr("hu");
		strName += QString(" , ");
		DJGameUser *dianpao	= panelController()->player( data[0] );
		if ( dianpao )
			strName	+= dianpao->userName();
		else
			strName	+= tr("no name");
		strName += tr("dian pao");
    }
    //itemText = new DJGameTextItem(strName,canvas(),m,0x01,0x01,DJGAME_DESKTOP_TYPE_HU_TEXT,false);
    itemText	= new DJGraphicsTextItem(strName, 0, desktop()->scene());
    appendSeatTypeItem(chSite,DJGAME_DESKTOP_TYPE_HU_TEXT,itemText);
    font = itemText->font();
    font.setPointSize(28);
    font.setBold(true);
    itemText->setFont(font);
	int h;
	pix = GetView1Card(1,false,false,&h);
	GetDesktopCenterPoint(&x,&y,NULL);
    x -= (7*pix.width()); 
    y = DJGAME_MAHJONG_DESKTOP_AVATAR_MAXHEIGHT+20;
    itemText->setZValue(3000);
    itemText->setVirtualPos( QPointF(x,y) );
	itemText->setMatrix( desktop()->scaleMatrix() );
	itemText->adjustPos( desktop()->totalMatrix() );
	itemText->show();
    y += 50;
    
	DJGraphicsPixmapItem *huit = NULL;
    if(data[2]>0)
    {
    	djDebug()<<"data[2]="<<hex<<data[2];
		for(i=0;i<data[2];i++)
		{
			djDebug()<<"card="<<hex<<(data[3+i]&0x3F);
			pix = GetView1Card((data[3+i]&0x3F),false,false,&h);
			item = new DJGraphicsPixmapItem(pix, 0, desktop()->scene());
			appendSeatTypeItem(chSite,DJGAME_DESKTOP_TYPE_HU_CARD,item);
			if((data[3+i]&0x3F) == (data[1] & 0x3F))
				huit = item;
			item->setZValue(3000);
    		item->setVirtualPos( QPointF(x,y) );
			item->setMatrix( desktop()->scaleMatrix() );
			item->adjustPos( desktop()->totalMatrix() );
			item->show();
			x += pix.width();
		}
		if(huit != NULL)
			huit->setDJSelected(true);
		ch = data[2]+3;
		GetDesktopCenterPoint(&x,NULL,NULL);
		y += pix.height()+20;
		i=0;
		QString str;
		font.setPointSize(24);
		while(data[ch] != 0 && ch<chLen)
		{
			str = tr(fanNames[data[ch]]);
			//itemText = new DJGameTextItem(str,canvas(),m,0x01,0x01,DJGAME_DESKTOP_TYPE_HU_TEXT,false);
			itemText	= new DJGraphicsTextItem(str, 0, desktop()->scene());
			appendSeatTypeItem(chSite,DJGAME_DESKTOP_TYPE_HU_TEXT,itemText);
			itemText->setFont(font);
			itemText->setAlignment(Qt::AlignRight);
			itemText->setZValue(3000);
			itemText->setVirtualPos( QPointF(x,y) );
			itemText->setMatrix( desktop()->scaleMatrix() );
			itemText->adjustPos( desktop()->totalMatrix() );
			itemText->show();
			
			str = QString(" : %1 ").arg(data[ch+1])+ tr("fan");
			//itemText = new DJGameTextItem(str,canvas(),m,0x01,0x01,DJGAME_DESKTOP_TYPE_HU_TEXT,false);
			itemText	= new DJGraphicsTextItem(str, 0, desktop()->scene());
			appendSeatTypeItem(chSite,DJGAME_DESKTOP_TYPE_HU_TEXT,itemText);
			itemText->setFont(font);
			i+=data[ch+1];
			itemText->setZValue(3000);
			itemText->setVirtualPos( QPointF(x,y) );
			itemText->setMatrix( desktop()->scaleMatrix() );
			itemText->adjustPos( desktop()->totalMatrix() );
			itemText->show();
			y+=30;
			
			ch+=2;
		}
		str = tr("total");
		//itemText = new DJGameTextItem(str,canvas(),m,0x01,0x01,DJGAME_DESKTOP_TYPE_HU_TEXT,false);
		itemText	= new DJGraphicsTextItem(str, 0, desktop()->scene());
		appendSeatTypeItem(chSite,DJGAME_DESKTOP_TYPE_HU_TEXT,itemText);
		itemText->setFont(font);
		itemText->setAlignment(Qt::AlignRight);
		itemText->setZValue(3000);
		itemText->setVirtualPos( QPointF(x,y) );
		itemText->setMatrix( desktop()->scaleMatrix() );
		itemText->adjustPos( desktop()->totalMatrix() );
		itemText->show();
		
		str = QString(" : %1 ").arg(i)+tr("fan");
		//itemText = new DJGameTextItem(str,canvas(),m,0x01,0x01,DJGAME_DESKTOP_TYPE_HU_TEXT,false);
		itemText	= new DJGraphicsTextItem(str, 0, desktop()->scene());
		appendSeatTypeItem(chSite,DJGAME_DESKTOP_TYPE_HU_TEXT,itemText);
		itemText->setFont(font);
		itemText->setZValue(3000);
		itemText->setVirtualPos( QPointF(x,y) );
		itemText->setMatrix( desktop()->scaleMatrix() );
		itemText->adjustPos( desktop()->totalMatrix() );
		itemText->show();
    }
}