
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
#include "DJTreeItemDelegate.h"
#include "DJGameImage.h"
#include "DJGameDesktop.h"
#include "DJGameMJDesktop.h"
#include "GBMahjong.h"
#include "mjrule.h"
#include "mj2profile.h"
#include "MJSelect.h"
#include "MJTing.h"
#include "mjstrings.h"
#include "mjfans.h"
#include "DJGameUserContext.h"
#include "DJGamePlayerItem.h"
#include "DJToolButton.h"
#include "DJGlobal.h" 

/////////////////////////////////////////////////

#ifdef DJ_OEM
static QSize djToolButtonSize(28,25);
#else
static QSize djToolButtonSize(24,24);
#endif

GBMahjongDesktop::GBMahjongDesktop(QWidget* parent,DJGamePanel *panel)
			: DJGameMJDesktop(parent,panel)
{
	c_MahjongPanel = (MahjongPanel *)panel;
	
	connect(this,SIGNAL(gameStarted()),this,SLOT(PlayerStarted()));
	PMahjongRoom pmjroom = (PMahjongRoom)((c_MahjongPanel->gameRoom())->privateRoom());
	SetRule(&(pmjroom->rule));
	
    m_toolbar	= new QWidget(this);
    QHBoxLayout *layout	= new QHBoxLayout( m_toolbar );
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
	
#ifdef DJ_OEM	
	QIcon background;
	background.addPixmap( QPixmap(":/BaseRes/image/desktop/toolbutton/bg.png"), QIcon::Normal, QIcon::Off );
	background.addPixmap( QPixmap(":/BaseRes/image/desktop/toolbutton/bgpressed.png"), QIcon::Normal, QIcon::On );
#endif

#ifdef DJ_OEM
	m_style = new DJToolButton;
	m_style->setBackgroundIcon( background );
#else	
	m_style = new QToolButton;
#endif
    m_style->setEnabled(true);
	m_style->setToolTip(tr("change style of pool cards"));
	m_style->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/arrange.png") );
	m_style->setIconSize( djToolButtonSize );
    connect(m_style,SIGNAL(clicked()),this,SLOT(ClickStyle()));
    layout->addWidget( m_style );
    
#ifdef DJ_OEM
	m_qi = new DJToolButton;
	m_qi->setBackgroundIcon( background );
#else	
	m_qi = new QToolButton;
#endif
    m_qi->setEnabled(false);
	m_qi->setToolTip(tr("pass"));
	m_qi->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/qi.png") );
	m_qi->setIconSize( djToolButtonSize );
    connect(m_qi,SIGNAL(clicked()),this,SLOT(ClickQi()));
    layout->addWidget( m_qi );
    
#ifdef DJ_OEM
	m_chu = new DJToolButton;
	m_chu->setBackgroundIcon( background );
#else	
	m_chu = new QToolButton;
#endif
    m_chu->setEnabled(false);
	m_chu->setToolTip(tr("throw a card"));
	m_chu->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/chu.png") );
	m_chu->setIconSize( djToolButtonSize );
    connect(m_chu,SIGNAL(clicked()),this,SLOT(ClickChu()));
    layout->addWidget( m_chu );
    
#ifdef DJ_OEM
	m_chi = new DJToolButton;
	m_chi->setBackgroundIcon( background );
#else	
	m_chi = new QToolButton;
#endif
    m_chi->setEnabled(false);
	m_chi->setToolTip(tr("chi a card"));
	m_chi->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/chi.png") );
	m_chi->setIconSize( djToolButtonSize );
    connect(m_chi,SIGNAL(clicked()),this,SLOT(ClickChi()));
    layout->addWidget( m_chi );
	
#ifdef DJ_OEM
	m_peng = new DJToolButton;
	m_peng->setBackgroundIcon( background );
#else	
	m_peng = new QToolButton;
#endif
    m_peng->setEnabled(false);
	m_peng->setToolTip(tr("peng a card"));
	m_peng->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/peng.png") );
	m_peng->setIconSize( djToolButtonSize );
    connect(m_peng,SIGNAL(clicked()),this,SLOT(ClickPeng()));
    layout->addWidget( m_peng );
	
#ifdef DJ_OEM
	m_gang = new DJToolButton;
	m_gang->setBackgroundIcon( background );
#else	
	m_gang = new QToolButton;
#endif
    m_gang->setEnabled(false);
	m_gang->setToolTip(tr("gang a card"));
	m_gang->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/gang.png") );
	m_gang->setIconSize( djToolButtonSize );
    connect(m_gang,SIGNAL(clicked()),this,SLOT(ClickGang()));
    layout->addWidget( m_gang );
#ifdef DJ_OEM
	m_ting = new DJToolButton;
	m_ting->setBackgroundIcon( background );
	m_ting->hide();
#else	
	m_ting = new QToolButton;
#endif
    m_ting->setEnabled(false);
	m_ting->setToolTip(tr("ready to hu"));
	m_ting->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/ting.png") );
	m_ting->setIconSize( djToolButtonSize );
    connect(m_ting,SIGNAL(clicked()),this,SLOT(ClickTing()));
    layout->addWidget( m_ting );
#ifdef DJ_OEM
	m_hu = new DJToolButton;
	m_hu->setBackgroundIcon( background );
#else	
	m_hu = new QToolButton;
#endif
    m_hu->setEnabled(false);
	m_hu->setToolTip(tr("hu"));
	m_hu->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/hu.png") );
	m_hu->setIconSize( djToolButtonSize );
    connect(m_hu,SIGNAL(clicked()),this,SLOT(ClickHu()));
    layout->addWidget( m_hu );
        								
	c_bReqTing = false;
	
	StaticInitDesktop();
	
}
void GBMahjongDesktop::ClickStyle()
{
	int style	= styleOfPoolCards();
	style++;
	style %= 2;
	setStyleOfPoolCards( style );
	
	for ( int seatId = 1; seatId <= GetGamePanel()->numberOfSeats(); seatId++ ) {
		RepaintPoolCards( seatId );
	}
}
void GBMahjongDesktop::ClickQi()
{
	SendGameTrace(MAHJONG_GAMETRACE_EATCOLLISION);
	m_hu->setEnabled(false);
				m_gang->setEnabled(false);
				m_qi->setEnabled(false);
				m_chi->setEnabled(false);
				m_peng->setEnabled(false);
}
void GBMahjongDesktop::ClickChu()
{
	DJGameMJDesktop::handleItemClicked(Qt::RightButton,QPoint(),0);
}
void GBMahjongDesktop::ClickChi()
{
	quint8 mask = playerChiMask(c_MahjongPanel->selfSeatId());
	if(mask == 0)
		return ClickQi();
	MJSelect dlg(c_MahjongPanel);
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
void GBMahjongDesktop::ClickPeng()
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
void GBMahjongDesktop::ClickGang()
{
	if(GetTableStatus() == DJGAME_MAHJONG_TABLE_STATUS_WAIT_EATCOLLISION)
	{
		char buf[2];
		buf[0] = GetLastThrowCard();
		SendGameTrace(MAHJONG_GAMETRACE_GANGFLOWER,buf,1);
		m_hu->setEnabled(false);
				m_gang->setEnabled(false);
				m_qi->setEnabled(false);
				m_chi->setEnabled(false);
				m_peng->setEnabled(false);
	}else if(GetTableStatus() == DJGAME_MAHJONG_TABLE_STATUS_WAIT_OUT)
	{
		unsigned char buf[15];
		quint8 chPages = playerGangStatus(c_MahjongPanel->selfSeatId(),0,buf,15);
		if(chPages == 1)
		{
			SendGameTrace(MAHJONG_GAMETRACE_GANGFLOWER,(char *)buf,1);
		}else if(chPages > 1)
		{
			MJSelect dlg(c_MahjongPanel);
			for(int i=0;i<chPages;i++)
				dlg.addRow(&buf[i],1);
			dlg.exec();
			chPages = dlg.indexOfSelected();
			SendGameTrace(MAHJONG_GAMETRACE_GANGFLOWER,(char*)&(buf[chPages]),1);
		}
	}
}
void GBMahjongDesktop::ClickTing()
{
	if ( !hasPlayerTing(c_MahjongPanel->selfSeatId()) ) {
		MJTing tingDlg;
		if ( QDialog::Accepted == tingDlg.exec() ) {
			m_bTingAutoGang	= tingDlg.isAutoGang();
			m_bTingZimo	= tingDlg.isOnlyZimoHu();
			c_bReqTing = true;
			DJGameMJDesktop::handleItemClicked(Qt::RightButton,QPoint(),0);
			c_bReqTing = false;
		}
	}
}

void GBMahjongDesktop::ClickHu()
{
	djDebug() << "clickhu";
	m_hu->setEnabled(false);
	SendGameTrace(MAHJONG_GAMETRACE_HU);
}

GBMahjongDesktop::~GBMahjongDesktop()
{
	djDebug() << "GBMahjongDesktop destructor";
}

void GBMahjongDesktop::Hu(unsigned char chSite,unsigned char *data,unsigned char chDataLen)
{
    unsigned char ch;
    int iFans=0,iBase,iScore;
    PMahjongRoom pmjroom = (PMahjongRoom)((c_MahjongPanel->gameRoom())->privateRoom());
	
    ch = data[2]+3;
	while(data[ch] != 0 && ch<chDataLen)
	{
		iFans+=data[ch+1];
	    ch+=2;
	}
	iBase = pmjroom->rule.chHUMinTermFAN;
	
	QString result;
	for( int i=1; i<=4; i++ )
	{
		DJGameUser *puser = c_MahjongPanel->userAtSeat(i);
		if ( puser ) {
    		//result += puser->userName();
    		//result += QString(" : ");
    		if(i == chSite)
    		{
    	    	if(chSite == data[0] || data[0]== 0 || data[0]>4)
    				iScore = (iBase+iFans)*3;
    	    	else
    				iScore = iBase*3+iFans;
    		}else
    		{
    	    	if(i == data[0] || data[0] == chSite || data[0]==0)
    				iScore = -(iBase+iFans);
    	    	else
    				iScore = -iBase; 
    		}
    		//result += QString(" %1 \n").arg(iScore);
     		result	+= QString("<tr><td align=center>%1<td align=center>%2")
								.arg( puser->userName() )
								.arg( iScore );	
     		c_MahjongPanel->deltaUserData( puser, iScore );
		}
	}
	c_MahjongPanel->playWave("hu.wav");
	
	c_MahjongPanel->insertGameResult2Browser( result );
}

void GBMahjongDesktop::DisplayHu(unsigned char chSite,unsigned char *data,unsigned char chLen)
{
	djDebug()<<"GBMahjongDesktop::DisplayHu"<<"seat"<<chSite<<"len"<<chLen;
	DebugBuffer( (const char*)data, chLen );
    unsigned char ch;
    QString strName;
    DJGameImageItem *item;
    DJGameTextItem				*itemText;
    QImage					img;
    QFont					font;
    int x,y,i;
    
   	ClearPoolCards(0);
	ClearHandCards(0);
	ClearImage(0,0,DJGAME_DESKTOP_TYPE_POOL);
	
	DJGameUser *user	= c_MahjongPanel->userAtSeat( chSite );
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
		DJGameUser *dianpao	= c_MahjongPanel->userAtSeat( data[0] );
		if ( dianpao )
			strName	+= dianpao->userName();
		else
			strName	+= tr("no name");
		strName += tr("dian pao");
    }
	QMatrix m = GetCurrentMatrix();
    itemText = new DJGameTextItem(strName,canvas(),m,0x01,0x01,DJGAME_DESKTOP_TYPE_HU_TEXT,false);
    font = itemText->font();
    font.setPointSize(28);
    font.setBold(true);
    itemText->setFont(font);
	int h;
	img = GetView1Card(1,false,false,&h);
	GetDesktopCenterPoint(&x,&y,NULL);
    x -= (7*img.width()); 
    y = DJGAME_MAHJONG_DESKTOP_AVATAR_MAXHEIGHT+20;
    itemText->move( x,y);
    itemText->setZ(3000);
    itemText->show();
	itemText->setMatrix(m);
    y += 50;
    
	DJGameImageItem *huit = NULL;
    if(data[2]>0)
    {
    	djDebug()<<"data[2]="<<hex<<data[2];
		for(i=0;i<data[2];i++)
		{
			djDebug()<<"card="<<hex<<(data[3+i]&0x3F);
			img = GetView1Card((data[3+i]&0x3F),false,false,&h);
			item = new DJGameImageItem(img,canvas(),m,0x01,0x01,DJGAME_DESKTOP_TYPE_HU_CARD,false);
			item->move(x,y);
			if((data[3+i]&0x3F) == (data[1] & 0x3F))
				huit = item;
				
			item->setZ(3000);
			item->show();
			item->setMatrix(m);
			x += img.width();
		}
		if(huit != NULL)
			huit->setMark(true);
		ch = data[2]+3;
		GetDesktopCenterPoint(&x,NULL,NULL);
		y += img.height()+20;
		i=0;
		QString str;
		font.setPointSize(24);
		while(data[ch] != 0 && ch<chLen)
		{
			str = tr(fanNames[data[ch]]);
			itemText = new DJGameTextItem(str,canvas(),m,0x01,0x01,DJGAME_DESKTOP_TYPE_HU_TEXT,false);
			itemText->setFont(font);
			itemText->setHAlignment(Qt::AlignRight);
			itemText->move( x,y);
			itemText->setZ(3000);
			itemText->show();
			itemText->setMatrix(m);
			
			str = QString(" : %1 ").arg(data[ch+1])+ tr("fan");
			itemText = new DJGameTextItem(str,canvas(),m,0x01,0x01,DJGAME_DESKTOP_TYPE_HU_TEXT,false);
			itemText->setFont(font);
			i+=data[ch+1];
			itemText->move(x,y);
			itemText->setZ(3000);
			itemText->show();
			itemText->setMatrix(m);
			y+=30;
			
			ch+=2;
		}
		str = tr("total");
		itemText = new DJGameTextItem(str,canvas(),m,0x01,0x01,DJGAME_DESKTOP_TYPE_HU_TEXT,false);
		itemText->setFont(font);
		itemText->setHAlignment(Qt::AlignRight);
		itemText->move( x,y);
		itemText->setZ(3000);
		itemText->show();
		itemText->setMatrix(m);
		
		str = QString(" : %1 ").arg(i)+tr("fan");
		itemText = new DJGameTextItem(str,canvas(),m,0x01,0x01,DJGAME_DESKTOP_TYPE_HU_TEXT,false);
		itemText->setFont(font);
		itemText->move(x,y);
		itemText->setZ(3000);
		itemText->show();
		itemText->setMatrix(m);
    }
}
const MahjongCurrent& GBMahjongDesktop::mahjongCurrent() const
{
	return c_current;
}
void GBMahjongDesktop::handleMatrixChanged(const QMatrix &m)
{
	djDebug()<<"GBMahjongDesktop::handleMatrixChanged";
	DJGameMJDesktop::handleMatrixChanged(m);
		
	int x,y,is;
	GetDesktopCenterPoint(&x,&y,&is);
	x = GetRealWidth() >> 1;
	y = GetRealHeight()-is;
	int tx,ty;
	m.map(x,y,&tx,&ty);
	m_toolbar->move(tx-(m_toolbar->width()>>1),ty-djToolButtonSize.height());
	//m_toolbar->move(visibleWidth()-m_toolbar->width(),ty-10);
}
void GBMahjongDesktop::PlayerStarted()
{
	//StaticInitDesktop();
	//RepaintWallCards();
	//ClearPoolCards(0);
	//ClearHandCards(0);
	//ClearImage(0,0,DJGAME_DESKTOP_TYPE_HU_CARD);
	//ClearText(0,0,DJGAME_DESKTOP_TYPE_HU_TEXT);
}

void GBMahjongDesktop::StaticInitDesktop()
{
	DJGameMJDesktop::StaticInitDesktop();
	memset(&c_current,0,sizeof(MahjongCurrent));
	m_qi->setEnabled(false);
	m_chu->setEnabled(false);
	m_chi->setEnabled(false);
	m_peng->setEnabled(false);
	m_gang->setEnabled(false);
	m_ting->setEnabled(false);
	m_hu->setEnabled(false);
}

void GBMahjongDesktop::StaticGameTrace(GeneralGameTrace2Head *gameTrace)
{
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
			ResetPlayerCards(gameTrace->chSite,gameTrace->chBuf,gameTrace->chBufLen);
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

void GBMahjongDesktop::RepaintCurrentStatus()
{
}
QString	GBMahjongDesktop::playerItemNameSuffix( DJGameUser* user )
{
	djDebug()<<"GBMahjongDesktop::playerItemNameSuffix"<<c_current.chMaster;
	if ( c_current.chMaster && c_current.chMaster == user->seatId() )
		return tr("(zhuang)");
	else
		return QString();
}
void GBMahjongDesktop::gameInfo( const unsigned char *buf )
{
	m_lastMGangCard	= 0;
	StaticInitDesktop();
	memcpy(&c_current,buf,sizeof(MahjongCurrent));
	RepaintWallCards();
	ClearPoolCards(0);
	ClearHandCards(0);
	ClearFlowerCards(0);
	ClearImage(0,0,DJGAME_DESKTOP_TYPE_HU_CARD);
	ClearText(0,0,DJGAME_DESKTOP_TYPE_HU_TEXT);
}

void GBMahjongDesktop::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	djDebug() << "GBMahjongDesktop::gameWait" << mask << status << timeout;
	djDebug() << "m_lastMGangCard = " << hex << m_lastMGangCard;
	bool bDIY	= false;
	bool containsFlower	= false;
	
	DJGameMJDesktop::gameWait(mask,status,timeout);
	switch(status)
	{
		case DJGAME_MAHJONG_TABLE_STATUS_WAIT_OUT :
		{
			djDebug() << "DJGAME_MAHJONG_TABLE_STATUS_WAIT_OUT";
			if(IsWaittingForMe())
			{
				if(!c_MahjongPanel->isLookingOn())
				{
					quint8 buffer[20],pages;
					pages = GetPlayerCurrentCards(c_MahjongPanel->selfSeatId(),buffer,20);
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
				if(playerHuStatus(c_MahjongPanel->selfSeatId(),true)) {
					bDIY	= true;
					m_hu->setEnabled(true);
				}else
					m_hu->setEnabled(false);
				if(playerTingStatus(c_MahjongPanel->selfSeatId()))
					m_ting->setEnabled(true);
				else
					m_ting->setEnabled(false);
				if(playerGangStatus(c_MahjongPanel->selfSeatId(),0,NULL,0))
					m_gang->setEnabled(true);
				else
					m_gang->setEnabled(false);
				m_chu->setEnabled(true);
			}else
			{
				djDebug() << "not waiting for me";
				SetThrowFlag(false);
				m_ting->setEnabled(false);
				if ( 0 != m_lastMGangCard && playerHuStatus(c_MahjongPanel->selfSeatId(),false,m_lastMGangCard)) {
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
			if(IsWaittingForMe())
			{
				bool b=false;
				if(playerHuStatus(c_MahjongPanel->selfSeatId(),false))
				{
					b = true;
					m_hu->setEnabled(true);
				}else
					m_hu->setEnabled(false);
				if(playerGangStatus(c_MahjongPanel->selfSeatId(),GetLastThrowCard(),NULL,0))
				{
					b = true;
					m_gang->setEnabled(true);
				}else
					m_gang->setEnabled(false);
				if(playerChiMask(c_MahjongPanel->selfSeatId()) != 0)
				{
					b = true;
					m_chi->setEnabled(true);
				}else
					m_chi->setEnabled(false);
				if(playerPengStatus(c_MahjongPanel->selfSeatId()))
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
	if (hasPlayerTing(c_MahjongPanel->selfSeatId())) {
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
void GBMahjongDesktop::gameTrace( const GeneralGameTrace2Head *gameTrace )
{
	djDebug() << "GBMahjongDesktop::gameTrace" <<hex<<"type"<<gameTrace->chType
												<<"seat"<<gameTrace->chSite;

	DJGameDesktop::gameTrace(gameTrace);
	GeneralGameTrace2Head *ptrace = (PGeneralGameTrace2Head)gameTrace;
	
	quint8 lastThrowSeat	= GetLastThrowSeat();
	StaticGameTrace(ptrace);
		
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
			RepaintPoolCards(lastThrowSeat);
			RepaintPoolCards(gameTrace->chSite);
			quint8 seat	= gameTrace->chSite;
			quint8 card	= gameTrace->chBuf[0];
			djDebug() << "seat =" << seat << "card =" << card;
			QString wavName	= QString("%1.wav").arg(card,2,16,QChar::fromLatin1('0'));
    		c_MahjongPanel->playWave(wavName);
    		
    		if ( gameTrace->chSite == GetGamePanel()->selfSeatId() )
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
			}*///改在gameWait中自动补花
			break;
		}
		case MAHJONG_GAMETRACE_WALLSTART :
		{
			RepaintWallCards();
			break;
		}
		case MAHJONG_GAMETRACE_EATCOLLISION : {
			djDebug() << "MAHJONG_GAMETRACE_EATCOLLISION" << gameTrace->chBuf[0];
			if(gameTrace->chBuf[0] == gameTrace->chBuf[1]){
				c_MahjongPanel->playWave("peng.wav");
			}else {
				c_MahjongPanel->playWave("chi.wav");
			}
			RepaintPlayerCards(gameTrace->chSite);
			RepaintPoolCards(lastThrowSeat);
			break;
		}
		case MAHJONG_GAMETRACE_GANGFLOWER :
		{
			djDebug() << "MAHJONG_GAMETRACE_GANGFLOWER";
			quint8 mjcard	= gameTrace->chBuf[0];
			djDebug() << "mjcard = " << hex << mjcard;
			if ( MAHJONG_ISFLOWERCARD( mjcard ) ) {
				c_MahjongPanel->playWave("bu.wav");
				RepaintPlayerFlowers(gameTrace->chSite);
			}else {
				c_MahjongPanel->playWave("gang.wav");
			
				RepaintPlayerCards(gameTrace->chSite);
				RepaintPoolCards(lastThrowSeat);
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
			c_MahjongPanel->playWave("hu.wav");
			DisplayHu(gameTrace->chSite,ptrace->chBuf,gameTrace->chBufLen);
			break;
		}
	}
}
QString GBMahjongDesktop::bonusTarget( const DJGameBonus* bonus )
{
	QString text;
	switch( bonus->chTarget ) {
	case MAHJONG_GAMEBONUS_TARGET_FAN: {
		quint8 fan	= bonus->chParam1;
		djDebug()<<"bonusTarget fan="<<fan;
		QString name	= tr(fanNames[fan]);
		djDebug()<<name;
		text	= tr("The hu contains %1").arg(name);
		djDebug()<<text;
		break;
	}
	default:
		break;
	}
	return text;
}
quint8 GBMahjongDesktop::GetMahjongThrowTraceID(quint8 card)
{
	if(c_MahjongPanel->isLookingOn())
		return 0;
	if(GetTableStatus() == DJGAME_MAHJONG_TABLE_STATUS_WAIT_OUT && IsWaittingForMe())
	{
		m_hu->setEnabled(false);
		m_chu->setEnabled(false);
		m_ting->setEnabled(false);
		m_gang->setEnabled(false);
		if(c_bReqTing)
			return MAHJONG_GAMETRACE_TING;
		return MAHJONG_GAMETRACE_OUT;
	}
	if(GetTableStatus() == DJGAME_MAHJONG_TABLE_STATUS_WAIT_EATCOLLISION && IsWaittingForMe())
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
void GBMahjongDesktop::MoveItem(Q3CanvasItem* item,QPoint &ps,QPoint &pe)
{
	DJGameMJDesktop::MoveItem(item,ps,pe);
}

bool GBMahjongDesktop::handleItemClicked(Qt::MouseButton btn,const QPoint& pos,Q3CanvasItem * item)
{
	bool hasHandled	= DJGameMJDesktop::handleItemClicked(btn,pos,item);
	if ( !hasHandled ) {
		if ( Qt::RightButton == btn && m_qi->isEnabled() ) {
			ClickQi();
			hasHandled	= true;
		}else
			hasHandled	= DJGameDesktop::handleItemClicked(btn,pos,item);
	}
	return hasHandled;
}

/////////////////////////////////////////////////
MahjongPanel::MahjongPanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f)
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
	GBMahjongDesktop	*gameDesktop = new GBMahjongDesktop(gameSplitter,this);
	
	setAccessorySplitter( accessorySplitter );
	setGameDesktop( gameDesktop );
}
void MahjongPanel::playerItemHeaderSections( QList<int> &sectionIds, QStringList &sectionNames )
{
	sectionIds.clear();
	sectionNames.clear();
	
	sectionIds << UserName << NickName;
	if ( gameRoom()->options() & DJGAME_ROOMOPTION_COUNTER )
		sectionIds << PanelChips << Chips;
	else
		sectionIds << PanelScore << Score;
		
	sectionIds << DoorWind << RingWind << Seat;
	
	for ( int i = 0; i < sectionIds.size(); ++i ) {
		sectionNames << baseSectionName( sectionIds.at(i) );
	}
	sectionNames << "-";
}
DJGamePlayerItem* MahjongPanel::addPlayerItem( quint32 userId, bool isPlayer )
{
	DJGamePlayerItem *item	= DJGamePanel::addPlayerItem( userId, isPlayer );
	if ( 0 == item )
		return 0;
	if ( isPlayer ) {
		GBMahjongDesktop *desktop	= static_cast<GBMahjongDesktop*>(gameDesktop());
		const MahjongCurrent& current	= desktop->mahjongCurrent();
		
		DJGameUser *user	= gameUser( userId );
		quint8 wind	= 0;
		if ( user ) {
			wind	= current.chMenOrientation[user->seatId()];
		}
		item->setDoorWind( wind );
		item->setRingWind( current.chRingOrientation );
	}else {
		item->setDoorWind( 0 );
		item->setRingWind( 0 );
	}
	return item;
}
MahjongPanel::~MahjongPanel()
{
}
