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
#include "GameUtility.h"
#include "DJTreeItemDelegate.h"
#include "DJGameImage.h"
#include "DJGameDesktop.h"
#include "DJGamePokerDesktop.h"
#include "Chaodip.h"
#include "ug2profile.h"
#include "DJMessageBox.h"
#include "DJPoker.h"
#include "DJPokerPattern.h"
#include "DJToolButton.h"
#include "DJGlobal.h"
/////////////////////////////////////////////////
#ifdef DJ_OEM
static QSize djToolButtonSize(28,25);
#else
static QSize djToolButtonSize(24,24);
#endif

static int intervaly	= 22;
static int offsety	= 5;
static int offsetx	= 100;
static int trumpw	= 20;
static int trumph	= 20;
static int DealInterval	= 500;

static QList<DJPoker::Point> series	= QList<DJPoker::Point>()	<< DJPoker::Num2
																<< DJPoker::Num3
																<< DJPoker::Num4
																<< DJPoker::Num5
																<< DJPoker::Num6
																<< DJPoker::Num7
																<< DJPoker::Num8
																<< DJPoker::Num9
																<< DJPoker::Num10
																<< DJPoker::Jack
																<< DJPoker::Queen
																<< DJPoker::King
																<< DJPoker::Ace;
QPixmap CreateConjoinedPixmap( const QList<QPixmap>& pixmaps, int width, int height)
{
	QPixmap conjoinedPixmap( width * pixmaps.size(), height );
	conjoinedPixmap.fill( Qt::black );
	QBitmap	mask	= conjoinedPixmap.createMaskFromColor( Qt::black );
	conjoinedPixmap.setMask( mask );
	
	QPainter painter( &conjoinedPixmap );
	//painter.setBackgroundMode( Qt::TransparentMode );
		
	int offsetX	= 0;
	foreach( QPixmap pix, pixmaps ) {
		painter.drawPixmap( offsetX, 0, width, height, pix );
		offsetX	+= width;
	}
	return conjoinedPixmap;
}
ChaodipDesktop::ChaodipDesktop(QWidget* parent,DJGamePanel *panel)
			: DJGamePokerDesktop(parent,panel)
{
	c_ChaodipPanel = (ChaodipPanel *)panel;
		
	connect(this,SIGNAL(gameStarted()),this,SLOT(PlayerStarted()));
	PUpgradeRoom pugroom = (PUpgradeRoom)((c_ChaodipPanel->gameRoom())->privateRoom());
	
#if QT_VERSION >= 0x040200		
	m_timeline = new QTimeLine(1000, this);
	m_timeline->setFrameRange(0,0);
 	m_timeline->setCurveShape(QTimeLine::LinearCurve);
 	connect(m_timeline, SIGNAL(frameChanged(int)), SLOT(dealCard(int)));
 	connect(m_timeline, SIGNAL(finished()), SLOT(dealCardFinished()));
#else
	c_drawTimer = new QTimer(this);
	connect(c_drawTimer,SIGNAL(timeout()),this,SLOT(HandleDrawTimeout()));
#endif
	
#ifdef DJ_OEM	
	QIcon background;
	background.addPixmap( QPixmap(":/BaseRes/image/desktop/toolbutton/bg.png"), QIcon::Normal, QIcon::Off );
	background.addPixmap( QPixmap(":/BaseRes/image/desktop/toolbutton/bgpressed.png"), QIcon::Normal, QIcon::On );
#endif
	c_lShow	= new QWidget(this);
    QHBoxLayout* layout	= new QHBoxLayout( c_lShow );
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
#ifdef DJ_OEM	
	c_bShows[0] = new DJToolButton;
	c_bShows[0]->setBackgroundIcon( background );
#else
	c_bShows[0] = new QToolButton;
#endif
	c_bShows[0]->setToolTip(tr("show diamond"));
	c_bShows[0]->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/d.png") );
	c_bShows[0]->setIconSize( djToolButtonSize );
    connect(c_bShows[0],SIGNAL(clicked()),this,SLOT(ShowClover()));
    layout->addWidget( c_bShows[0] );
    
#ifdef DJ_OEM	
	c_bShows[1] = new DJToolButton;
	c_bShows[1]->setBackgroundIcon( background );
#else
	c_bShows[1] = new QToolButton;
#endif
	c_bShows[1]->setToolTip(tr("show club"));
	c_bShows[1]->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/c.png") );
	c_bShows[1]->setIconSize( djToolButtonSize );
    connect(c_bShows[1],SIGNAL(clicked()),this,SLOT(ShowDiamond()));
    layout->addWidget( c_bShows[1] );
    
#ifdef DJ_OEM	
	c_bShows[2] = new DJToolButton;
	c_bShows[2]->setBackgroundIcon( background );
#else
	c_bShows[2] = new QToolButton;
#endif
	c_bShows[2]->setToolTip(tr("show heart"));
	c_bShows[2]->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/h.png") );
	c_bShows[2]->setIconSize( djToolButtonSize );
    connect(c_bShows[2],SIGNAL(clicked()),this,SLOT(ShowHeart()));
    layout->addWidget( c_bShows[2] );
    
#ifdef DJ_OEM	
	c_bShows[3] = new DJToolButton;
	c_bShows[3]->setBackgroundIcon( background );
#else
	c_bShows[3] = new QToolButton;
#endif
	c_bShows[3]->setToolTip(tr("show spade"));
	c_bShows[3]->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/s.png") );
	c_bShows[3]->setIconSize( djToolButtonSize );
    connect(c_bShows[3],SIGNAL(clicked()),this,SLOT(ShowSpade()));
    layout->addWidget( c_bShows[3] );
    
#ifdef DJ_OEM	
	c_bShows[4] = new DJToolButton;
	c_bShows[4]->setBackgroundIcon( background );
#else
	c_bShows[4] = new QToolButton;
#endif
	c_bShows[4]->setToolTip(tr("show nt"));
	c_bShows[4]->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/nt.png") );
	c_bShows[4]->setIconSize( djToolButtonSize );
    connect(c_bShows[4],SIGNAL(clicked()),this,SLOT(ShowNT()));
    layout->addWidget( c_bShows[4] );
    
#ifdef DJ_OEM	
	c_bShows[5] = new DJToolButton;
	c_bShows[5]->setBackgroundIcon( background );
#else
	c_bShows[5] = new QToolButton;
#endif
	c_bShows[5]->setToolTip(tr("give up to show"));
	c_bShows[5]->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/qi.png") );
	c_bShows[5]->setIconSize( djToolButtonSize );
    connect(c_bShows[5],SIGNAL(clicked()),this,SLOT(NoShow()));
    layout->addWidget( c_bShows[5] );
    c_lShow->hide();
     
#ifdef DJ_OEM
	QSize iconSize( 19 * 2, 19 );
	
	m_tip = new QPushButton(this);
	m_tip->setToolTip(tr("tip"));
	m_tip->setIcon( QIcon(":/BaseRes/image/desktop/pushbutton/TiShi.png") );
	m_tip->setIconSize( iconSize );
	m_tip->adjustSize();
	connect(m_tip,SIGNAL(clicked()),this,SLOT(ClickTip()));
    m_tip->hide();
    
    m_throw = new QPushButton(this);
	m_throw->setToolTip(tr("throw cards"));
	m_throw->setIcon( QIcon(":/BaseRes/image/desktop/pushbutton/ChuPai.png") );
	m_throw->setIconSize( iconSize );
	m_throw->adjustSize();
    connect(m_throw,SIGNAL(clicked()),this,SLOT(ClickThrow()));
    m_throw->hide();
    
    m_previous = new DJToolButton(this);
    m_previous->setEnabled(false);
	m_previous->setToolTip(tr("look last round"));
	m_previous->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/left.png") );
	m_previous->setIconSize( djToolButtonSize );
	m_previous->setBackgroundIcon( background );
    connect(m_previous,SIGNAL(pressed()),this,SLOT(PreviousPressed()));
    connect(m_previous,SIGNAL(released()),this,SLOT(PreviousRelesed()));
    
    m_subtract = new DJToolButton(this);
	m_subtract->setEnabled(false);
	m_subtract->setToolTip(tr("look discarded cards"));
	m_subtract->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/down.png") );
	m_subtract->setIconSize( djToolButtonSize );
	m_subtract->setBackgroundIcon( background );
	connect(m_subtract,SIGNAL(pressed()),this,SLOT(SubtractPressed()));
    connect(m_subtract,SIGNAL(released()),this,SLOT(SubtractReleased()));
#else      	
   	m_toolbar	= new QWidget(this);
    layout	= new QHBoxLayout( m_toolbar );
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
	
	m_tip = new QToolButton;
    m_tip->setEnabled(false);
	m_tip->setToolTip(tr("tip"));
	m_tip->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/tip.png") );
	m_tip->setIconSize( djToolButtonSize );
    connect(m_tip,SIGNAL(clicked()),this,SLOT(ClickTip()));
    layout->addWidget( m_tip );
    
    m_throw = new QToolButton;
    m_throw->setEnabled(false);
	m_throw->setToolTip(tr("throw cards"));
	m_throw->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/chu.png") );
	m_throw->setIconSize( djToolButtonSize );
    connect(m_throw,SIGNAL(clicked()),this,SLOT(ClickThrow()));
    layout->addWidget( m_throw );
    	
	m_previous = new QToolButton;
    m_previous->setEnabled(false);
	m_previous->setToolTip(tr("look last round"));
	m_previous->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/left.png") );
	m_previous->setIconSize( djToolButtonSize );
    connect(m_previous,SIGNAL(pressed()),this,SLOT(PreviousPressed()));
    connect(m_previous,SIGNAL(released()),this,SLOT(PreviousRelesed()));
    layout->addWidget( m_previous );
    
    m_subtract = new QToolButton;
	m_subtract->setEnabled(false);
	m_subtract->setToolTip(tr("look discarded cards"));
	m_subtract->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/down.png") );
	m_subtract->setIconSize( djToolButtonSize );
	connect(m_subtract,SIGNAL(pressed()),this,SLOT(SubtractPressed()));
    connect(m_subtract,SIGNAL(released()),this,SLOT(SubtractReleased()));
    layout->addWidget( m_subtract );
#endif
    
	if(pugroom->chCards > 2)
		SetOrder(false,true);
	else
		SetOrder(false);
	//SetFx(getSelfFx(),getOtherFx(),getThrowFx(),1.0);
	SetCardPictureSize(35,40,pageLayoutSizeX(),pageLayoutSizeY());
	
	QMatrix m = GetCurrentMatrix();
	QString str = QString("0");
	c_Result = new DJGameTextItem(str,canvas(),m,DJGAME_UPGRADE_TID_RESULT,0xFF,DJGAME_UPGRADE_TYPE_RESULT,false);
	int x,y;
	GetDesktopCenterPoint(&x,&y,NULL);
	c_Result->setVAlignment(Qt::AlignVCenter);
	c_Result->setHAlignment(Qt::AlignHCenter);
	c_Result->move(x,y);
	QFont f = c_Result->font();
	f.setPointSize(40);
	f.setBold(true);
	c_Result->setFont(f);
	
	c_Result->hide();
	c_Result->setZ(3000);
	c_Result->setMatrix(m);
	
	x	= offsetx;
	y	= offsety;
	
	c_MyTeam = new DJGameTextItem(str,canvas(),m,DJGAME_UPGRADE_TID_DESCRIBE,0xFF,DJGAME_UPGRADE_TYPE_DESCRIBE,false);
	f = c_MyTeam->font();
	f.setPointSize(TEXT_ITEM_FONT_SIZE);
	f.setBold(true);
	c_MyTeam->setFont(f);
	c_MyTeam->setColor(QColor(0,0xFF,0));
	c_MyTeam->setHAlignment(Qt::AlignRight);
	c_MyTeam->move(x,y);
	c_MyTeam->show();
	c_MyTeamLevel = new DJGameTextItem(str,canvas(),m,DJGAME_UPGRADE_TID_DESCRIBE,0xFF,DJGAME_UPGRADE_TYPE_DESCRIBE,false);
	c_MyTeamLevel->setFont(f);
	c_MyTeamLevel->move(x,y);
	c_MyTeamLevel->setColor(QColor(0,0xFF,0));
	c_MyTeamLevel->show();
	
	y	+=  intervaly;
	c_OtherTeam = new DJGameTextItem(str,canvas(),m,DJGAME_UPGRADE_TID_DESCRIBE,0xFF,DJGAME_UPGRADE_TYPE_DESCRIBE,false);
	c_OtherTeam->setFont(f);
	c_OtherTeam->setHAlignment(Qt::AlignRight);
	c_OtherTeam->move(x,y);
	c_OtherTeam->show();
	c_OtherLevel = new DJGameTextItem(str,canvas(),m,DJGAME_UPGRADE_TID_DESCRIBE,0xFF,DJGAME_UPGRADE_TYPE_DESCRIBE,false);
	c_OtherLevel->setFont(f);
	c_OtherLevel->move(x,y);
	c_OtherLevel->show();
	
	y	+=  intervaly;
	str = tr("Master : ");
	c_Master = new DJGameTextItem(str,canvas(),m,DJGAME_UPGRADE_TID_DESCRIBE,0xFF,DJGAME_UPGRADE_TYPE_DESCRIBE,false);
	c_Master->setFont(f);
	c_Master->setHAlignment(Qt::AlignRight);
	c_Master->move(x,y);
	c_Master->show();
	c_MasterName = new DJGameTextItem(str,canvas(),m,DJGAME_UPGRADE_TID_DESCRIBE,0xFF,DJGAME_UPGRADE_TYPE_DESCRIBE,false);
	c_MasterName->setFont(f);
	c_MasterName->setColor(QColor(0xFF,0,0));
	c_MasterName->move(x,y);
	c_MasterName->show();
	
	y	+=  intervaly;
	str = tr("Show : ");
	c_Shower = new DJGameTextItem(str,canvas(),m,DJGAME_UPGRADE_TID_DESCRIBE,0xFF,DJGAME_UPGRADE_TYPE_DESCRIBE,false);
	c_Shower->setFont(f);
	c_Shower->setHAlignment(Qt::AlignRight);
	c_Shower->move(x,y);
	c_Shower->show();
	c_ShowerName = new DJGameTextItem(str,canvas(),m,DJGAME_UPGRADE_TID_DESCRIBE,0xFF,DJGAME_UPGRADE_TYPE_DESCRIBE,false);
	c_ShowerName->setFont(f);
	c_ShowerName->move(x,y);
	c_ShowerName->show();
	
	y	+=  intervaly;
	str = tr("Trump : ");
	c_Trump = new DJGameTextItem(str,canvas(),m,DJGAME_UPGRADE_TID_DESCRIBE,0xFF,DJGAME_UPGRADE_TYPE_DESCRIBE,false);
	c_Trump->setFont(f);
	c_Trump->setHAlignment(Qt::AlignRight);
	c_Trump->move(x,y);
	c_Trump->show();
	m_trumpImage	= new DJGameImageItem(QImage(),canvas(),m,DJGAME_UPGRADE_TID_DESCRIBE,0xFF,DJGAME_UPGRADE_TYPE_DESCRIBE,false);
	m_trumpImage->move(x,y);
	m_trumpImage->show();
	
	y	+=  intervaly;
	str = tr("Score : ");
	c_ScoreText = new DJGameTextItem(str,canvas(),m,DJGAME_UPGRADE_TID_DESCRIBE,0xFF,DJGAME_UPGRADE_TYPE_DESCRIBE,false);
	c_ScoreText->setFont(f);
	c_ScoreText->setHAlignment(Qt::AlignRight);
	c_ScoreText->move(x,y);
	c_ScoreText->show();
	c_ScoreNumber = new DJGameTextItem(str,canvas(),m,DJGAME_UPGRADE_TID_DESCRIBE,0xFF,DJGAME_UPGRADE_TYPE_DESCRIBE,false);
	c_ScoreNumber->setFont(f);
	c_ScoreNumber->move(x,y);
	c_ScoreNumber->show();
	
	StaticInitDesktop();
	ModifyPlayerName();
	ModifyScoreNumber();
	ModifyMaster();
	ModifyShow(0,0);
	
	setBridgeCard( false );
}

void ChaodipDesktop::ModifyPlayerName()
{
	QString str[2];
	DJGameUser *puser;
	quint32 uid;
	str[0] = QString("");
	str[1] = QString("");
	for(int i=1;i<=4;i++)
	{
		uid = c_ChaodipPanel->userIdOfSeat(i);
		puser = c_ChaodipPanel->gameUser(uid);
		if ( !puser )
			continue;
		if(i<3)
		{
			str[i%2] += puser->userName();
			str[i%2] += QString("-");
		}else
		{
			str[i%2] += puser->userName();
			str[i%2] += QString(" : ");
		}
	}
	if(c_ChaodipPanel->selfSeatId()%2 == 0)
	{
		c_MyTeam->setText(str[0]);
		c_OtherTeam->setText(str[1]);
	}else
	{
		c_MyTeam->setText(str[1]);
		c_OtherTeam->setText(str[0]);
	}
	c_MyTeam->setHAlignment(Qt::AlignRight);
	c_MyTeam->move(offsetx,offsety);
	c_OtherTeam->setHAlignment(Qt::AlignRight);
	c_OtherTeam->move(offsetx,offsety + intervaly);
}
void ChaodipDesktop::ModifyScoreNumber()
{
	c_ScoreNumber->setText(QString("%1").arg(c_shCurrentScore));
}
void ChaodipDesktop::ModifyShow(quint8 seat,quint8 chCard)
{
	djDebug() << "ModifyShow" << hex << seat << chCard;
	if(seat == 0 || chCard == 0)
	{
		c_ShowerName->setText(tr("Unknown"));
		QImage image;
		m_trumpImage->changeImage( image );
		return;
	}
	DJGameUser *puser;
	quint32 uid;
	uid = c_ChaodipPanel->userIdOfSeat(seat);
	puser = c_ChaodipPanel->gameUser(uid);
	if ( puser )
		c_ShowerName->setText( puser->userName());
	
	QPixmap	trumpPix;
	quint8 card	= chCard & 0x3F;
	quint8 numberOfSuit	= ((chCard & 0xC0) >> 6) + 1;
	if ( 0x3e == card )
		trumpPix	= QPixmap(":/BaseRes/image/desktop/toolbutton/ngray.png");
	else if ( 0x3f == card )
		trumpPix	= QPixmap(":/BaseRes/image/desktop/toolbutton/nred.png");
	else {
		quint8 suit = card >> 4;
		switch(suit) {
		case 0:
			trumpPix	= QPixmap(":/BaseRes/image/desktop/toolbutton/d.png");
			break;
		case 1:
			trumpPix	= QPixmap(":/BaseRes/image/desktop/toolbutton/c.png");
			break;
		case 2:
			trumpPix	= QPixmap(":/BaseRes/image/desktop/toolbutton/h.png");
			break;
		case 3:
			trumpPix	= QPixmap(":/BaseRes/image/desktop/toolbutton/s.png");
			break;
		default:
			break;
		}
	}
	QList<QPixmap> pixmaps;
	for ( int i = 0; i < numberOfSuit; i++ ) {
		pixmaps << trumpPix;
	}
	QPixmap conjoinedPix	= CreateConjoinedPixmap( pixmaps, trumpw,trumph );
	QImage image	= conjoinedPix.toImage();
	m_trumpImage->changeImage( image );
}

void ChaodipDesktop::ModifyMaster()
{
	if(c_current.chMaster == 0)
	{
		c_MasterName->setText(tr("Unknown"));
	}else
	{
		DJGameUser *puser;
		quint32 uid;
		uid = c_ChaodipPanel->userIdOfSeat(c_current.chMaster);
		puser = c_ChaodipPanel->gameUser(uid);
		if ( puser )
			c_MasterName->setText( puser->userName());
	}
	
	char chLevel[14][4]={"0","A","2","3","4","5","6","7","8","9","10","J","Q","K"};
	
	if(c_ChaodipPanel->selfSeatId()%2 == 0)
	{
		c_MyTeamLevel->setText(QString(chLevel[c_current.chLevels[0]]));
		c_OtherLevel->setText(QString(chLevel[c_current.chLevels[1]]));
	}else
	{
		c_MyTeamLevel->setText(QString(chLevel[c_current.chLevels[1]]));
		c_OtherLevel->setText(QString(chLevel[c_current.chLevels[0]]));
	}
}

void ChaodipDesktop::PreviousPressed()
{
	RepaintThrowCard(1,&(c_ThrowCards[1][1][1]),c_ThrowCards[1][1][0]);
	RepaintThrowCard(2,&(c_ThrowCards[2][1][1]),c_ThrowCards[2][1][0]);
	RepaintThrowCard(3,&(c_ThrowCards[3][1][1]),c_ThrowCards[3][1][0]);
	RepaintThrowCard(4,&(c_ThrowCards[4][1][1]),c_ThrowCards[4][1][0]);
}
void ChaodipDesktop::PreviousRelesed()
{
	RepaintThrowCard(1,&(c_ThrowCards[1][0][1]),c_ThrowCards[1][0][0]);
	RepaintThrowCard(2,&(c_ThrowCards[2][0][1]),c_ThrowCards[2][0][0]);
	RepaintThrowCard(3,&(c_ThrowCards[3][0][1]),c_ThrowCards[3][0][0]);
	RepaintThrowCard(4,&(c_ThrowCards[4][0][1]),c_ThrowCards[4][0][0]);
}
void ChaodipDesktop::SubtractPressed()
{
	RepaintAttachCards(c_current.chMaster,c_chSubstract,c_chSubstractPages);
}
void ChaodipDesktop::SubtractReleased()
{
	ClearSeatAttachCard(c_current.chMaster);
}

void ChaodipDesktop::GetPageInterval(quint8 chView,quint8 *pcards,quint8 chPages,int *dx,int *dy)
{
	(*dx) = 0;
	(*dy) = 0;
	if(chView != 1)
	{
		*dx = 1;
		return;
	}
	bool b = lineClass();
	bool bc = SameCardClass(pcards[chPages],pcards[chPages+1]);
	if(b )
	{
		if(!bc)
			*dy = 1;
		else
			*dx = 1;
	}else
	{
		if(bc)
			*dx = 1;
		else
			*dx = 2;
	}
}

bool ChaodipDesktop::SameCardClass(quint8 chCard1,quint8 chCard2)
{
	if(DJGAME_POKER_VALUE(chCard1) == c_current.chLevel || DJGAME_POKER_ISBK(chCard1) || DJGAME_POKER_ISSK(chCard1))
	{
		if(DJGAME_POKER_VALUE(chCard2) == c_current.chLevel || DJGAME_POKER_ISBK(chCard2) || DJGAME_POKER_ISSK(chCard2))
			return true;
		if(c_chLastShow != 0)
		{
			if(DJGAME_POKER_CLASS(chCard2) == DJGAME_POKER_CLASS(c_chLastShow) && !DJGAME_POKER_ISBK(c_chLastShow) && !DJGAME_POKER_ISSK(c_chLastShow))
				return true;
		}
		return false;
	}
	if(DJGAME_POKER_VALUE(chCard2) == c_current.chLevel || DJGAME_POKER_ISBK(chCard2) || DJGAME_POKER_ISSK(chCard2))
	{
		if(c_chLastShow != 0)
		{
			if(DJGAME_POKER_CLASS(chCard1) == DJGAME_POKER_CLASS(c_chLastShow) && !DJGAME_POKER_ISBK(c_chLastShow) && !DJGAME_POKER_ISSK(c_chLastShow) )
				return true;
		}
		return false;
	}
	if(DJGAME_POKER_CLASS(chCard1) != DJGAME_POKER_CLASS(chCard2))
		return false;
	return true;
}
void ChaodipDesktop::sortCards( quint8 *pCards, quint8 len )
{
  	quint8 iTemp;
  	for(int i=1;i<len;i++)
  	{
  		bool bComplete	= true;
    	for(int j=len-1;j>=i;j--)
    	{
      		if (Compare2Card( pCards[j], pCards[j-1], 0 ) > 0 )
      		{
      			bComplete	= false;
       		 	iTemp = pCards[j-1];
       	 		pCards[j-1] = pCards[j];
        		pCards[j] = iTemp;
      		}
    	}
    	if ( bComplete )
    		return;
  	}
}
int ChaodipDesktop::Compare2Card(quint8 chCard1,quint8 chCard2,PDJGamePokerImage pimage)
{
	if(!DJGAME_POKER_ISBK(chCard1) && !DJGAME_POKER_ISSK(chCard1) &&
			!DJGAME_POKER_ISBK(chCard2) && !DJGAME_POKER_ISSK(chCard2))
	{
		if(DJGAME_POKER_CARD(chCard1) == DJGAME_POKER_CARD(chCard2))
			return DJGAME_POKERDESKTOP_COMPARE_RESUST_E;
		if(DJGAME_POKER_VALUE(chCard1) == c_current.chLevel)
		{
			if(DJGAME_POKER_VALUE(chCard2) == c_current.chLevel)
			{
				if(c_chLastShow != 0)
				{
					if(DJGAME_POKER_CLASS(chCard2) == DJGAME_POKER_CLASS(c_chLastShow))
						return DJGAME_POKERDESKTOP_COMPARE_RESUST_2;
				}
			}
			return DJGAME_POKERDESKTOP_COMPARE_RESUST_1;
		}
		if(DJGAME_POKER_VALUE(chCard2) == c_current.chLevel)
		{
			return DJGAME_POKERDESKTOP_COMPARE_RESUST_2;
		}
		if(c_chLastShow != 0)
		{
			if(DJGAME_POKER_CLASS(chCard1) == DJGAME_POKER_CLASS(c_chLastShow)
					&& DJGAME_POKER_CLASS(chCard2) != DJGAME_POKER_CLASS(c_chLastShow) )
				return DJGAME_POKERDESKTOP_COMPARE_RESUST_1;
			if(DJGAME_POKER_CLASS(chCard2) == DJGAME_POKER_CLASS(c_chLastShow)
					&& DJGAME_POKER_CLASS(chCard1) != DJGAME_POKER_CLASS(c_chLastShow) )
				return DJGAME_POKERDESKTOP_COMPARE_RESUST_2;
		}
	}
	return DJGamePokerDesktop::Compare2Card(chCard1,chCard2,pimage);
}
#if QT_VERSION >= 0x040200
void ChaodipDesktop::dealCard(int currentCardFrame )
{
	quint8 totalFrame	= m_timeline->endFrame();
	djDebug()<<"totalFrame="<<totalFrame<<"currentCardFrame"<<currentCardFrame;
	if ( 0 == totalFrame ) {
		return;
	}
	
	int i;
	for(i=1;i<=4;i++)
	{
		quint8 numberOfUndealedCard	=  c_chPlayerCardPages[i];
		quint8 numberOfDealedCards	= totalFrame - numberOfUndealedCard;
		quint8 delta	= currentCardFrame - numberOfDealedCards;
	
		djDebug()<<"i="<<i
				<<"numberOfUndealedCard"<<numberOfUndealedCard
				<<"numberOfDealedCards"<<numberOfDealedCards
				<<"delta"<<delta;
			
		if( c_chPlayerCardPages[i] > 0 )
		{
			//DebugBuffer( (const char*)c_chPlayerCardsBuffer[i], c_chPlayerCardPages[i] );
			while( delta-- ) {
				AppendPlayerCards(i,&(c_chPlayerCardsBuffer[i][c_chPlayerCardPages[i]-1]),1);
				c_chPlayerCardPages[i] --;
			}
			//RepaintSeatCard(i);
			if(i==c_ChaodipPanel->selfSeatId()) {
				RepaintSeatCard(i);
				c_ChaodipPanel->playWave("out.wav");
				CheckShowStatus();
			}
		}
	}
}
void ChaodipDesktop::dealCardFinished()
{
	for(int i=1;i<=4;i++)
	{
		RepaintSeatCard(i);
	}
}	
#else
void ChaodipDesktop::HandleDrawTimeout()
{
	int i,j=0;
	for(i=1;i<=4;i++)
	{
		if(c_chPlayerCardPages[i] > 0)
		{
			AppendPlayerCards(i,&(c_chPlayerCardsBuffer[i][c_chPlayerCardPages[i]-1]),1);
			c_chPlayerCardPages[i] --;
			RepaintSeatCard(i);
			j ++;
			if(i==c_ChaodipPanel->selfSeatId()) {
				c_ChaodipPanel->playWave("out.wav");
				CheckShowStatus();
			}
		}
	}
	if(j == 0)
		c_drawTimer->stop();
	//if(j ==  1)
		//c_drawTimer->start(100);
	//else
		//c_drawTimer->start(500);
}
#endif
void ChaodipDesktop::ShowHeart()
{
	quint8 chPages = GetCardPages(c_ChaodipPanel->selfSeatId(),DJGAME_POKER_MAKE_CODE(0,DJGAME_POKER_CLASS_H,c_current.chLevel));
	if(chPages > 0 && (c_chLastShow == 0 || chPages >= DJGAME_POKER_PAGES(c_chLastShow)))
		ShowCard(((chPages-1)<<6)+DJGAME_POKER_MAKE_CODE(0,DJGAME_POKER_CLASS_H,c_current.chLevel));
}

void ChaodipDesktop::ShowSpade()
{
	quint8 chPages = GetCardPages(c_ChaodipPanel->selfSeatId(),DJGAME_POKER_MAKE_CODE(0,DJGAME_POKER_CLASS_S,c_current.chLevel));
	if(chPages > 0 && (c_chLastShow == 0 || chPages >= DJGAME_POKER_PAGES(c_chLastShow)))
		ShowCard(((chPages-1)<<6)+DJGAME_POKER_MAKE_CODE(0,DJGAME_POKER_CLASS_S,c_current.chLevel));
}
void ChaodipDesktop::ShowDiamond()
{
	quint8 chPages = GetCardPages(c_ChaodipPanel->selfSeatId(),DJGAME_POKER_MAKE_CODE(0,DJGAME_POKER_CLASS_D,c_current.chLevel));
	if(chPages > 0 && (c_chLastShow == 0 || chPages >= DJGAME_POKER_PAGES(c_chLastShow)))
		ShowCard(((chPages-1)<<6)+DJGAME_POKER_MAKE_CODE(0,DJGAME_POKER_CLASS_D,c_current.chLevel));
}
void ChaodipDesktop::ShowClover()
{
	quint8 chPages = GetCardPages(c_ChaodipPanel->selfSeatId(),DJGAME_POKER_MAKE_CODE(0,DJGAME_POKER_CLASS_C,c_current.chLevel));
	if(chPages > 0 && (c_chLastShow == 0 || chPages >= DJGAME_POKER_PAGES(c_chLastShow)))
		ShowCard(((chPages-1)<<6)+DJGAME_POKER_MAKE_CODE(0,DJGAME_POKER_CLASS_C,c_current.chLevel));
}
void ChaodipDesktop::ShowNT()
{
	quint8 chPages = GetCardPages(c_ChaodipPanel->selfSeatId(),DJGAME_POKER_SMALLKING);
	if(chPages > 1 && (c_chLastShow == 0 || chPages >= DJGAME_POKER_PAGES(c_chLastShow)))
		ShowCard(((chPages-1)<<6)+DJGAME_POKER_SMALLKING);
	else
	{
		chPages = GetCardPages(c_ChaodipPanel->selfSeatId(),DJGAME_POKER_BIGKING);
		if(chPages > 1 && (c_chLastShow == 0 || chPages >= DJGAME_POKER_PAGES(c_chLastShow)))
			ShowCard(((chPages-1)<<6)+DJGAME_POKER_BIGKING);
	}
}
void ChaodipDesktop::NoShow()
{
}

void ChaodipDesktop::ShowCard(quint8 chCard)
{
	SendGameTrace(UPGRADE_GAMETRACE_SHOW,(char *)&chCard,1);
}
void ChaodipDesktop::ClickThrow()
{
	DJGamePokerDesktop::handleItemClicked(Qt::RightButton,QPoint(),0);
}
bool ChaodipDesktop::isTrump( const DJPoker& poker ) const
{
	DJPoker trumpLevelPoker( c_chLastShow & 0x3f, isBridgeCard() );
	int suit	= poker.suit();
	DJPoker::Point point	= poker.point();
	if ( DJPoker::Joker == suit )
		return true;
	if ( point == c_current.chLevel )
		return true;
	if ( suit == trumpLevelPoker.suit() )
		return true;
	return false;
}
int ChaodipDesktop::suitOfPoker( const DJPoker& poker ) const
{
	if ( isTrump(poker) )
		return DJPoker::Trump;
	else
		return poker.suit();
}
QList<DJPoker> ChaodipDesktop::pokersOfSuit( quint8 seat, int suit, QList<DJPoker>& leftPokers  ) const
{
	QList<DJPoker> pokers;
	leftPokers.clear();
	DJPoker trumpLevelPoker( c_chLastShow & 0x3f, isBridgeCard() );
	for( quint8 i=1; i<=0x3F; ++i ) {
		DJPoker poker( i, isBridgeCard() );
		bool isSelected	= false;
		if ( DJPoker::Trump == suit && isTrump(poker) ) {
			isSelected	= true;
		}else if ( poker.suit() == suit && !isTrump(poker) ) {
			isSelected	= true;
		}
		quint8 pages	= GetPlayerCardImage(seat)->chPage[i];
		while(pages--) {
			if ( isSelected )
				pokers << poker;
			else
				leftPokers << poker;
		}
	}
	return pokers;
}
DJPoker ChaodipDesktop::higherSerialPoker( const DJPoker& poker, bool isStrict ) const
{
	DJPoker::Point point	= poker.point();
	int	suit	= poker.suit();
	DJPoker trumpLevelPoker( c_chLastShow & 0x3f, isBridgeCard() );
	int index	= m_series.indexOf( point );
	if ( -1 != index ) {
		//normal poker
		if ( index + 1 == m_series.size() ) {
			//the top poker in series,check if it is trump
			if ( suit == trumpLevelPoker.suit() ) {
				//is trump, next is non-trump level pokers
				//suit	= DJPoker::AnySuit;
				//suit	^=	trumpLevelPoker.suit();
				return DJPoker( DJPoker::AnySuit ^ trumpLevelPoker.suit(), trumpLevelPoker.point() );
			}
		}else {
			//not top,just return the higher poker in series
			index++;
			point	= m_series.at(index);
			return DJPoker ( suit, point );
		}
	}else {
		//special poker
		if ( point == trumpLevelPoker.point() ) {
			//level poker
			if ( suit == trumpLevelPoker.suit() ) {
				//trump level poker
				return DJPoker( DJPoker::Joker, DJPoker::SmallJoker );
			}else {
				//non-trump level poker
				return trumpLevelPoker;
			}
		}else if ( point == DJPoker::SmallJoker )
			return DJPoker( DJPoker::Joker, DJPoker::BigJoker );
	}
	return DJPoker();
}
DJPoker ChaodipDesktop::lowerSerialPoker( const DJPoker& poker, bool isStrict ) const
{
	DJPoker::Point point	= poker.point();
	int	suit	= poker.suit();
	DJPoker trumpLevelPoker( c_chLastShow & 0x3f, isBridgeCard() );
	int index	= m_series.indexOf( point );
	if ( -1 != index ) {
		//normal poker
		if ( 0 != index ) {
			//not bottom,just return the lower poker in series
			index--;
			point	= m_series.at(index);
			return DJPoker ( suit, point );
		}
	}else {
		//special poker
		if ( point == trumpLevelPoker.point() ) {
			//level poker
			if ( suit == trumpLevelPoker.suit() ) {
				//trump level poker
				//suit	= DJPoker::AnySuit;
				//suit	^=	trumpLevelPoker.suit();
				return DJPoker( DJPoker::AnySuit ^ trumpLevelPoker.suit(), trumpLevelPoker.point() );
			}else {
				//non-trump level poker
				return DJPoker( trumpLevelPoker.suit(), m_series.last() );
			}
		}else if ( point == DJPoker::SmallJoker ) {
			return trumpLevelPoker;
		}else if ( point == DJPoker::BigJoker ) {
			return DJPoker( DJPoker::Joker, DJPoker::SmallJoker );
		}
	}
	return DJPoker();
}

int ChaodipDesktop::findMatchingWidth( QList<DJPokerPattern>& patterns, int width )
{
	qSort( patterns.begin(), patterns.end(), qGreater<DJPokerPattern>() );
	
	for ( QList<DJPokerPattern>::iterator it = patterns.begin(); it != patterns.end(); ++it ) {
		DJPokerPattern& pattern	= *(it);
		if ( 0 == pattern.width() )
			return 0;
		if ( pattern.width() < width ) {
			return pattern.width();
		}
	}
	return 0;
}
QList<DJPokerPattern>::iterator ChaodipDesktop::findMatchingPattern( QList<DJPokerPattern>& patterns,
														 int width, int length, int mode )
{
	int minCha	= 100;
	
	QList<DJPokerPattern>::iterator matchingIt	= patterns.end();
	for ( QList<DJPokerPattern>::iterator it = patterns.begin(); it != patterns.end(); ++it ) {
		DJPokerPattern& pattern	= *(it);
		if ( pattern.width() == width ) {
			if ( pattern.length() == length && 0 == mode ) {
				return it;
			}else if ( pattern.length() > length && mode > 0 ) {
				int cha	= pattern.length() - length;
				if ( cha < minCha ) {
					minCha	= cha;
					matchingIt	= it;
				}
			}else if ( pattern.length() < length && mode < 0 ) {
				int cha	= length - pattern.length();
				if ( cha < minCha ) {
					minCha	= cha;
					matchingIt	= it;
				}
			}
		}
	}
	return matchingIt;
}
void ChaodipDesktop::ClickTip()
{
	djDebug() << "UpgradeDesktop::ClickTip"<<m_masterSeat;
	if ( 0 == m_masterSeat )
		return;
	quint8 *otherBuf	= &c_ThrowCards[m_masterSeat][0][1];
	quint8 otherTotalLen	= c_ThrowCards[m_masterSeat][0][0];
	
	//djDebug()<<"other thrown pokers";
	QList<DJPoker> otherPokers;
	for( int i = 0; i < otherTotalLen; ++i ) {
		DJPoker poker( otherBuf[i], isBridgeCard() );
		//djDebug()<<poker.description();
		otherPokers << poker;
	}
	
	djDebug()<<"other thrown pokers patterns";
	QList<DJPokerPattern> otherPatterns	= DJPokerPattern::fromPokers( this, otherPokers, true );
	foreach( DJPokerPattern otherPattern, otherPatterns ) {
		djDebug()<<otherPattern.description();
	}
	
	int suit	= suitOfPoker( otherPokers.first() );
	djDebug()<<"other thrown pokers suit"<<suit;
	
	djDebug()<<"self suit pokers";
	QList<DJPoker> leftPokers;
	QList<DJPoker> suitPokers	= pokersOfSuit( GetGamePanel()->selfSeatId(), suit, leftPokers );
	foreach( DJPoker poker, suitPokers ) {
		djDebug()<<poker.description();
	}
	djDebug()<<"other suit pokers"<<leftPokers.size();
	
	djDebug()<<"self hand pokers patterns";
	QList<DJPokerPattern> selfPatterns	= DJPokerPattern::fromPokers( this, suitPokers, true );
	foreach( DJPokerPattern selfPattern, selfPatterns ) {
		djDebug()<<selfPattern.description();
	}
	
	QList<DJPoker> tipPokers;
	while( !otherPatterns.isEmpty() ) {
		djDebug()<<"otherPatterns is not empty"<<otherPatterns.size();
		qSort( otherPatterns.begin(), otherPatterns.end(), qGreater<DJPokerPattern>() );
		
		QList<DJPokerPattern>::iterator otherIt	= otherPatterns.begin();
		while ( otherIt != otherPatterns.end() ) {
			DJPokerPattern& otherPattern	= *otherIt;
			djDebug()<<"take an other pattern"<<otherPattern.width()<<otherPattern.length();
			QList<DJPokerPattern>::iterator selfIt	= findMatchingPattern( selfPatterns, 
										otherPattern.width(), otherPattern.length(), 0 );
			if ( selfIt != selfPatterns.end() ) {
				//found match mode equal( self length equal to other length )
				DJPokerPattern& selfPattern	= *selfIt;
				djDebug()<<"found match mode equal";
				
				tipPokers << selfPattern.pokers();
				otherPatterns.erase( otherIt );
				selfPatterns.erase( selfIt );
				//search from other patterns begin again
				break;
			}
			
			selfIt	= findMatchingPattern( selfPatterns, otherPattern.width(),
									 otherPattern.length(), 1 );
			if ( selfIt != selfPatterns.end() ) {
				//found match mode greater( self length greater than other length )
				DJPokerPattern& selfPattern	= *selfIt;
				djDebug()<<"found match mode greater"<<selfPattern.length();
								
				tipPokers << selfPattern.takeLength( otherPattern.length() );
				otherPatterns.erase( otherIt );
				//search from other patterns begin again
				break;
			}
			
			selfIt	= findMatchingPattern( selfPatterns, otherPattern.width(),
									 otherPattern.length(), -1 );
			if ( selfIt != selfPatterns.end() ) {
				//found match mode less( self length less than other length )
				DJPokerPattern& selfPattern	= *selfIt;
				djDebug()<<"found match mode less"<<selfPattern.length();
				
				tipPokers << selfPattern.pokers();
				otherPattern.takeLength( selfPattern.length() );
				selfPatterns.erase( selfIt );
				//search from other patterns begin again
				break;
			}
			
			//here: there is no equal width pattern, split other pattern
			//find the biggest width in self patterns ( less than otherPattern.width() )
			djDebug()<<"no equal width";
			int width	= findMatchingWidth( selfPatterns, otherPattern.width() );
			if ( 0 != width ) {
				//split other pattern
				djDebug()<<"split other pattern"<<width<<otherPattern.width();
				DJPokerPattern split1( width, otherPattern.length() );
				DJPokerPattern split2( otherPattern.width() - width, otherPattern.length() );
				otherPatterns.erase( otherIt );
				otherPatterns.append( split1 );
				otherPatterns.append( split2 );
				//search from other patterns begin again
				break;
			}
			//here: no less width than other width
			//split greater width in self patterns
			djDebug()<<"split self greater width";
			QList<DJPoker> skipedPokers;
			foreach( DJPokerPattern pattern, selfPatterns ) {
				skipedPokers << pattern.pokers();
			}
			if ( tipPokers.size() < otherTotalLen ) {
				djDebug()<<"not enough less width poker"<<tipPokers.size();
				int cha	= otherTotalLen - tipPokers.size();
				cha	= qMin( skipedPokers.size(), cha );
				djDebug()<<"cha"<<cha;
				for ( int i = 0; i < cha; ++i ) {
					tipPokers << skipedPokers.value(i,DJPoker());
				}
			}
			otherPatterns.clear();
			break;
		}
	}
	if ( tipPokers.size() < otherTotalLen ) {
		int cha	= otherTotalLen - tipPokers.size();
		djDebug()<<"not enough same suit poker"<<cha<<otherTotalLen<<tipPokers.size();
		for ( int i = 0; i < cha; ++i ) {
			tipPokers << leftPokers.value(i,DJPoker());
		}
	}
	djDebug()<<"final tip"<<tipPokers.size();
	foreach( DJPoker poker, tipPokers ) {
		djDebug()<<poker.description();
	}
    selectPokerItems( tipPokers );    
}
void ChaodipDesktop::CheckShowStatus()
{
	djDebug() << "ChaodipDesktop::CheckShowStatus";
	int i;
	if(GetTableStatus() != DJGAME_UPGRADE_TABLE_STATUS_WAIT_SHOW)
		c_lShow->hide();
	if(c_ChaodipPanel->isLookingOn() || !IsWaittingForMe())
	{
		for(i=0;i<=5;i++)
            c_bShows[i]->setEnabled(false);
		return;
	}
	quint8 seat = c_ChaodipPanel->selfSeatId();
	PDJGamePokerImage pokerimg = GetPlayerCardImage(seat);
	if(c_chLastShow == 0)
	{
		for(i=DJGAME_POKER_CLASS_C;i<=DJGAME_POKER_CLASS_S;i++)
		{
			if(pokerimg->chPage[DJGAME_POKER_MAKE_CODE(0,i,c_current.chLevel)]>0)
				c_bShows[i-1]->setEnabled(true);
			else
				c_bShows[i-1]->setEnabled(false);
		}
		if(pokerimg->chPage[DJGAME_POKER_BIGKING] >= 2 || pokerimg->chPage[DJGAME_POKER_SMALLKING] >= 2)
			c_bShows[DJGAME_POKER_CLASS_S]->setEnabled(true);
		else
			c_bShows[DJGAME_POKER_CLASS_S]->setEnabled(false);
	}else
	{
		djDebug() << "lastshow=" << hex << c_chLastShow;
		unsigned char chCard = DJGAME_POKER_CARD(c_chLastShow);
		unsigned char chPages = DJGAME_POKER_PAGES(c_chLastShow);
		if(c_chLastShowSeat == seat)
		{
			for(i=0;i<=4;i++)
        	{
				c_bShows[i]->setEnabled(false);
			}
			if(pokerimg->chPage[chCard] > chPages )
			{
				if(DJGAME_POKER_ISSK(chCard) || DJGAME_POKER_ISBK(chCard))
				{
					c_bShows[4]->setEnabled(true);
				}else
					c_bShows[(chCard>>4)]->setEnabled(true);
			}
			return;
		}//if(c_chLastShowSeat == seat)
		for(i=0;i<4;i++)
        {
        	if ( canShowHigher(pokerimg, (i<<4)+c_current.chLevel,c_chLastShow) )
				c_bShows[i]->setEnabled(true);
			else 
				c_bShows[i]->setEnabled(false);
		}
		if ( canShowHigher(pokerimg,DJGAME_POKER_SMALLKING,c_chLastShow) ||
			canShowHigher(pokerimg,DJGAME_POKER_BIGKING,c_chLastShow) )
			c_bShows[4]->setEnabled(true);
		else 
			c_bShows[4]->setEnabled(false);
	}
}
ChaodipDesktop::~ChaodipDesktop()
{

}

void ChaodipDesktop::handleMatrixChanged(const QMatrix &m)
{
	DJGamePokerDesktop::handleMatrixChanged(m);
		
	int x,y,tx,ty,iSelfHeight;
	GetDesktopCenterPoint(&x,&y,&iSelfHeight);

#ifdef DJ_OEM
	y = GetRealHeight()-iSelfHeight;
	m.map(x,y,&tx,&ty);
	
	c_lShow->move(tx,ty);
	
	m_throw->move(tx-(m_throw->width()>>1),ty);
	m_tip->move(tx+(m_throw->width()>>1)+20,ty);
	
	m_previous->move(width()-m_previous->width()-20,height()-80);
	m_subtract->move(width()-m_subtract->width()-20,height()-40);
#else	
	x = GetRealWidth()/2;
	y = GetRealHeight()-iSelfHeight-c_lShow->height()-m_toolbar->height();
	m.map(x,y,&tx,&ty);
	
	c_lShow->move(tx+50,ty);
	m_toolbar->move(tx+50,ty+c_lShow->height());
#endif	
}
void ChaodipDesktop::PlayerStarted()
{
	//StaticInitDesktop();
	//ClearSeatAttachCard(0);
	//ClearSeatHandCard(0);
	//ClearSeatThrowCard(0);
}

void ChaodipDesktop::StaticInitDesktop()
{
	memset(&c_current,0,sizeof(UpgradeCurrent));
	c_chLastShow = 0;
	c_chLastShowSeat = 0;
	
	memset(c_chPlayerCardPages,0,UPGRADE_MAX_SITES+1);
	
	c_bFirstPlayer = true;
	c_chFirstPages = 0;
	c_ScorePages = 0;
	memset(c_chSubstract,0,20);
	c_chSubstractPages = 0;
	
	m_previous->setEnabled(false);
	m_subtract->setEnabled(false);
	
	c_shCurrentScore = 0;
	c_chLastMulti = 0;
	c_chLastScore = 0;
	
	c_Result->hide();
	ClearImage(0,0,DJGAME_UPGRADE_TYPE_RESULT);
	c_bSelfThrow = true;
	ModifyMaster();
	ModifyShow(0,0);
	ModifyScoreNumber();
}

void ChaodipDesktop::StaticGameTrace(GeneralGameTrace2Head *gameTrace)
{
	switch(gameTrace->chType)
	{
		case UPGRADE2_GAMETRACE_RESET :
		//case UPGRADE_GAMETRACE_DIAL :
		case UPGRADE2_GAMETRACE_PICKUP :
		{
			ResetPlayerCards(gameTrace->chSite,NULL,0);
			memcpy(c_chPlayerCardsBuffer[gameTrace->chSite],gameTrace->chBuf,gameTrace->chBufLen);
			c_chPlayerCardPages[gameTrace->chSite] = gameTrace->chBufLen;
#if QT_VERSION >= 0x040200
			quint8 numberOfCards	= c_chPlayerCardPages[gameTrace->chSite];
			m_timeline->setDuration( DealInterval * numberOfCards );
			m_timeline->setFrameRange(0, numberOfCards );
#endif				
		//	if(c_ChaodipPanel->isLookingOn())
		//		DrawOver(false);
			break;
		}
		case UPGRADE_GAMETRACE_THROW : 
		{
			if ( 0 == m_masterSeat )
				m_masterSeat	= gameTrace->chSite;
			DrawOver(false);
			if(c_bFirstPlayer == true)
			{
				if(c_current.chMaster == 0)
				{
					c_current.chMaster = gameTrace->chSite;
					ModifyMaster();
				}
				if(gameTrace->chSite != c_ChaodipPanel->selfSeatId())
					c_bSelfThrow = false;
				c_bFirstPlayer = false;
				c_chFirstPages = gameTrace->chBufLen;
				c_chFirstClass = DJGAME_POKER_CLASS(gameTrace->chBuf[0]);
			}
			if(gameTrace->chSite == c_ChaodipPanel->selfSeatId())
				c_bSelfThrow = true;
			RemovePlayerCards(gameTrace->chSite,gameTrace->chBuf,gameTrace->chBufLen);
			memcpy(&(c_ThrowCards[gameTrace->chSite][0][1]),gameTrace->chBuf,gameTrace->chBufLen);
			c_ThrowCards[gameTrace->chSite][0][0] = gameTrace->chBufLen;
			break;
		}
		case UPGRADE2_GAMETRACE_CLEARDESKTOP : 
		{
			DJGameRoom *room= c_ChaodipPanel->gameRoom();
			for(int i= UPGRADE_MAX_SAVE_ROUND;i>= 0;i--)
			{
				for(int j=1;j<=room->numberOfSeats();j++)
				{
					if(i>0)
						memcpy(c_ThrowCards[j][i],c_ThrowCards[j][i-1],60);
					else
						memset(c_ThrowCards[j][0],0,60);
				}
			}
			m_previous->setEnabled(true);
			c_bFirstPlayer = true;
			c_chFirstPages = 0;
			c_bSelfThrow = false;
			break;
		}
		case UPGRADE_GAMETRACE_MASTER :
		{
			m_masterSeat	= gameTrace->chSite;
			if(gameTrace->chBufLen > 0)
			{
				memcpy(&(c_ScoreCards[c_ScorePages]),gameTrace->chBuf,gameTrace->chBufLen);
				c_ScorePages += gameTrace->chBufLen;
				for(int i=0;i<gameTrace->chBufLen;i++)
				{
					if(DJGAME_POKER_VALUE(gameTrace->chBuf[i]) == 5)
						c_shCurrentScore += 5;
					else
						c_shCurrentScore += 10;
				}
			}
			break;
		}
		case UPGRADE_GAMETRACE_SHOW :
		{
			if(gameTrace->chBuf[0] > 0)
			{
				quint8 buffer[6];
				quint8 chPages = (gameTrace->chBuf[0]>>6)+1;
				for(int i=0;i<chPages;i++)
					buffer[i] = gameTrace->chBuf[0]&0x3F;
				
				memcpy(&(c_ThrowCards[gameTrace->chSite][0][1]),buffer,chPages);
				c_ThrowCards[gameTrace->chSite][0][0] = chPages;
				c_chLastShow = gameTrace->chBuf[0];
				c_chLastShowSeat = gameTrace->chSite;
			}
			
			break;
		}
		//case UPGRADE_GAMETRACE_HANDS :
		case UPGRADE2_GAMETRACE_HANDS :
		{
			DrawOver(false);
			AppendPlayerCards(gameTrace->chSite,gameTrace->chBuf,gameTrace->chBufLen);
			break;
		}
		//case UPGRADE2_GAMETRACE_SUBSTRACT :
		case UPGRADE3_GAMETRACE_SUBSTRACT :
		{
			djDebug() << "substract" << gameTrace->chSite;
			DrawOver(false);
			if(c_current.chMaster == 0)
			{
				djDebug() << "master==0";
				c_current.chMaster = gameTrace->chSite;
				ModifyMaster();
			}
			RemovePlayerCards(gameTrace->chSite,gameTrace->chBuf,gameTrace->chBufLen);
			memcpy(c_chSubstract,gameTrace->chBuf,gameTrace->chBufLen);
			c_chSubstractPages = gameTrace->chBufLen;
			m_subtract->setEnabled(true);
			break;
		}
		case UPGRADE_GAMETRACE_GAMEOVER :
		{
			c_chLastMulti = gameTrace->chBuf[0];
			c_chLastScore = 0;
			for(int i=1;i<gameTrace->chBufLen;i++)
			{
				if(DJGAME_POKER_VALUE(gameTrace->chBuf[i]) == 5)
					c_chLastScore += 5;
				else if(DJGAME_POKER_VALUE(gameTrace->chBuf[i]) == 10 || DJGAME_POKER_VALUE(gameTrace->chBuf[i])==DJGAME_POKER_K)
					c_chLastScore += 10;
			}
			break;
		}
	}
}

void ChaodipDesktop::RepaintCurrentStatus()
{
	DJGameRoom *room= c_ChaodipPanel->gameRoom();
	for(int i=1;i<=room->numberOfSeats();i++)
	{
		RepaintSeatCard(i);
		RepaintThrowCard(i,&(c_ThrowCards[i][0][1]),c_ThrowCards[i][0][0]);
	}
	quint8 seat;
	if(c_current.chMaster%2 == c_ChaodipPanel->selfSeatId()%2)
	{
		seat = (c_ChaodipPanel->selfSeatId()==1)?4:c_ChaodipPanel->selfSeatId()-1;
	}else
		seat = c_ChaodipPanel->selfSeatId();
	RepaintAttachCards(seat,c_ScoreCards,c_ScorePages);
	ModifyScoreNumber();
	ModifyMaster();
				
}

bool ChaodipDesktop::IsThrowStep(PGeneralGameTrace2Head ptrace)
{
	if(ptrace->chType == UPGRADE_GAMETRACE_THROW)
		return true;
	return false;
}

void ChaodipDesktop::userArrived( quint32 userId, quint8 seatId )
{
	DJGameDesktop::userArrived(userId,seatId);
		
	ModifyPlayerName();
	ModifyMaster();
//	ModifyShow();
}
void ChaodipDesktop::gameInfo( const unsigned char *buf )
{
	//if(GetTableStatus() == DJGAME_TABLE_STATUS_WAITSTART)
	//{
		StaticInitDesktop();
		ClearSeatAttachCard(0);
		ClearSeatHandCard(0);
		ClearSeatThrowCard(0);
	//}
	memcpy(&c_current,buf,sizeof(UpgradeCurrent));
	ModifyMaster();
	m_masterSeat	= 0;
	
	m_series	= series;
	m_series.removeAll( (DJPoker::Point)c_current.chLevel );
}

void ChaodipDesktop::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	DJGameDesktop::gameWait(mask,status,timeout);
	if(status == DJGAME_TABLE_STATUS_WAITSTART)
	{
		setWaitingThrow(false);
#ifdef DJ_OEM
		m_throw->hide();
#else				
		m_throw->setEnabled( false );
#endif		
	}
	if ( DJGAME_UPGRADE_TABLE_STATUS_WAIT_OUT == status
		&& IsWaittingForMe() && m_masterSeat 
		&& m_masterSeat != GetGamePanel()->selfSeatId() ){
#ifdef DJ_OEM
		m_tip->show();
#else		
		m_tip->setEnabled(true);
#endif
	}else {
#ifdef DJ_OEM
		m_tip->hide();
#else				
		m_tip->setEnabled(false);
#endif		
	}
	switch(status)
	{
		case DJGAME_UPGRADE_TABLE_STATUS_WAIT_HANDS :
		{
			if(c_current.chMaster == 0)
			{
				QList<quint8> seats	= mask2Seat( mask );
				if ( 1 == seats.size() ) {
					c_current.chMaster = seats.first();
					ModifyMaster();
				}
			}
			c_lShow->hide();
			DrawOver(true);
			if(IsWaittingForMe()) {
				setWaitingThrow(true);
				setWaitingThrowAcl(false);
#ifdef DJ_OEM
				m_throw->show();
#else								
				m_throw->setEnabled( true );
#endif				
			}else {
				setWaitingThrow(false);
#ifdef DJ_OEM
				m_throw->hide();
#else								
				m_throw->setEnabled( false );
#endif				
			}
			break;
		}
		case DJGAME_UPGRADE_TABLE_STATUS_WAIT_SHOW : 
		{
#if QT_VERSION >= 0x040200
			m_timeline->start();
#else	
			c_drawTimer->start(500);
#endif				
			if(IsWaittingForMe()) {
				c_lShow->show();
				CheckShowStatus();
			}else
				c_lShow->hide();
			break;
		}
		case DJGAME_UPGRADE_TABLE_STATUS_WAIT_OUT :
		{
			c_lShow->hide();
			DrawOver(true);
			if(IsWaittingForMe())
			{
				djDebug() << "WaittingForMeOUT";
				setWaitingThrow(true);
				setWaitingThrowAcl(false);
#ifdef DJ_OEM
				m_throw->show();
#else				
				m_throw->setEnabled( true );
#endif				
				quint8 selfSeat	= c_ChaodipPanel->selfSeatId();
				quint8 prevSeat	= prevSeatId( selfSeat );
				quint8 prevSeatPages	= c_ThrowCards[prevSeat][0][0];
				
				djDebug() << selfSeat << numberOfCards(selfSeat) << prevSeat << prevSeatPages ;
				if ( m_masterSeat != selfSeat && prevSeatPages == numberOfCards(selfSeat) ) {
					djDebug() << "the final out";
					throwAllCards();
				}
			}else {
				setWaitingThrow(false);
#ifdef DJ_OEM
				m_throw->hide();
#else				
				m_throw->setEnabled( false );
#endif				
			}
			break;
		}
	}
}

void ChaodipDesktop::DrawOver(bool bRepaint)
{
#if QT_VERSION < 0x040200	
	c_drawTimer->stop();
#endif
	for(int i=1;i<=4;i++)
	{
		if(c_chPlayerCardPages[i] > 0)
		{
			AppendPlayerCards(i,&(c_chPlayerCardsBuffer[i][0]),c_chPlayerCardPages[i]);
			c_chPlayerCardPages[i] = 0;
			if(bRepaint)
				RepaintSeatCard(i);
		}
	}
}
void ChaodipDesktop::gameTrace( const GeneralGameTrace2Head *gameTrace )
{
	DJGameDesktop::gameTrace(gameTrace);
	GeneralGameTrace2Head *ptrace = (PGeneralGameTrace2Head)gameTrace;
	StaticGameTrace(ptrace);
	switch(gameTrace->chType)
	{
		case UPGRADE2_GAMETRACE_RESET :
		//case UPGRADE_GAMETRACE_DIAL :
		case UPGRADE2_GAMETRACE_PICKUP :
		{
			if(c_ChaodipPanel->isLookingOn())
				DrawOver(true);
			break;
		}
		case UPGRADE_GAMETRACE_THROW : 
		{
		//	DrawOver(true);
			RepaintSeatCard(gameTrace->chSite);
			RepaintThrowCard(gameTrace->chSite,&(c_ThrowCards[gameTrace->chSite][0][1]),c_ThrowCards[gameTrace->chSite][0][0]);
			c_ChaodipPanel->playWave("out.wav");
			break;
		}
		case UPGRADE2_GAMETRACE_CLEARDESKTOP : 
		{
			ClearSeatThrowCard(0);
			break;
		}
		case UPGRADE_GAMETRACE_MASTER :
		{
			if(gameTrace->chBufLen > 0)
			{
				quint8 seat;
				if(c_current.chMaster%2 == c_ChaodipPanel->selfSeatId()%2)
				{
					seat = (c_ChaodipPanel->selfSeatId()==1)?4:c_ChaodipPanel->selfSeatId()-1;
				}else
					seat = c_ChaodipPanel->selfSeatId();
				RepaintAttachCards(seat,c_ScoreCards,c_ScorePages);
				ModifyScoreNumber();
			}
			break;
		}
		case UPGRADE_GAMETRACE_SHOW :
		{
			RepaintThrowCard(gameTrace->chSite,&(c_ThrowCards[gameTrace->chSite][0][1]),c_ThrowCards[gameTrace->chSite][0][0]);
			RepaintSeatCard(1);
			RepaintSeatCard(2);
			RepaintSeatCard(3);
			RepaintSeatCard(4);
			CheckShowStatus();
			ModifyShow(gameTrace->chSite,gameTrace->chBuf[0]);
			break;
		}
		//case UPGRADE_GAMETRACE_HANDS :
		case UPGRADE2_GAMETRACE_HANDS :
		{
		//	DrawOver(true);
			RepaintSeatCard(gameTrace->chSite);
			break;
		}
		//case UPGRADE2_GAMETRACE_SUBSTRACT :
		case UPGRADE3_GAMETRACE_SUBSTRACT :
		{
			RepaintSeatCard(gameTrace->chSite);
			break;
		}
		case UPGRADE_GAMETRACE_GAMEOVER :	
		{
			int x,y;
			GetDesktopCenterPoint(&x,&y,NULL);
			c_Result->setText(QString("%1 + %2 x %3 = %4").arg(c_shCurrentScore).arg(c_chLastScore).arg(c_chLastMulti).arg(c_shCurrentScore+c_chLastScore*c_chLastMulti));
			c_Result->move(x,y);
			c_Result->show();
			
			DrawHCards(x,y,1.0,0xFF,DJGAME_UPGRADE_TYPE_RESULT,(quint8 *)&(gameTrace->chBuf[1]),gameTrace->chBufLen-1,Qt::AlignHCenter,1900);
			break;
		}
	}
}
void ChaodipDesktop::RecvThrowACL(bool bFailed)
{
	DJGamePokerDesktop::RecvThrowACL(bFailed);
	
	if(GetTableStatus() == DJGAME_UPGRADE_TABLE_STATUS_WAIT_SHOW)
	{
	//	quint8 seat = c_ChaodipPanel->selfSeatId();
	//	PDJGamePokerImage ppokerimg = GetPlayerCardImage(seat); 
		;
		
	}
}

void ChaodipDesktop::CreateThrowTrace(PGeneralGameTrace2Head ptrace)
{
	djDebug() << "UpgradeDesktop::CreateThrowTrace" << c_bFirstPlayer << c_bSelfThrow << c_chFirstPages ;
	DebugBuffer( (char*)ptrace->chBuf, ptrace->chBufLen);
	if(GetTableStatus() == DJGAME_UPGRADE_TABLE_STATUS_WAIT_OUT)
	{
		if(c_bSelfThrow)
		{//本轮第一个出牌
			if(ptrace->chBufLen > 1)
			{
				for(int i=1;i<ptrace->chBufLen;i++)
				{
					if(!SameCardClass(ptrace->chBuf[i],ptrace->chBuf[0]))
					{//多于一种花色
						printf("Multi class error\n");
						DJMessageBox::information(10,GetGamePanel(),tr("throw card failed"),tr("You must throw cards with the same suit"),QMessageBox::Ok);
						ptrace->chBufLen = 0;
						return;
					}
				}
			}
		}else
		{
			if(ptrace->chBufLen != c_chFirstPages)
			{//跟牌张数不对
				printf("pages error\n");
				ptrace->chBufLen = 0;
				return;
			}
		}
		ptrace->chType = UPGRADE_GAMETRACE_THROW;
		
		sortCards( ptrace->chBuf, ptrace->chBufLen );
	}else if(GetTableStatus() == DJGAME_UPGRADE_TABLE_STATUS_WAIT_HANDS)
	{
		PUpgradeRoom pugroom = (PUpgradeRoom)((c_ChaodipPanel->gameRoom())->privateRoom());
		if(ptrace->chBufLen != pugroom->chHands)
		{//扣牌张数不对
			printf("hand pages error\n");
			ptrace->chBufLen = 0;
			return;
		}
		ptrace->chType = UPGRADE2_GAMETRACE_HANDS;
	}
}
void ChaodipDesktop::MoveItem(Q3CanvasItem* item,QPoint &ps,QPoint &pe)
{
	DJGameDesktop::MoveItem(item,ps,pe);
}
QImage ChaodipDesktop::GetCardPicture(quint8 chCard,quint8 chView,qreal rate)
{
	do {
		if ( DJGAME_POKER_ERRORCARD(chCard))
			break;
		if ( DJGAME_POKER_BACKGROUND == chCard )
			break;
		if ( DJGAME_POKER_ISBK(chCard) || DJGAME_POKER_ISSK(chCard) )
			break;
		if ( DJGAME_POKER_CLASS_D == DJGAME_POKER_CLASS(chCard) ) {
			chCard	-= 0x10;
		}else if (  DJGAME_POKER_CLASS_C == DJGAME_POKER_CLASS(chCard) ) {
			chCard	+= 0x10;
		}
	}while(false);
	
	return DJGamePokerDesktop::GetCardPicture(chCard,chView,rate);
}
void ChaodipDesktop::RepaintView1Cards(quint8 seat,quint8 *cards,quint8 chPages)
{
	bool visible	= isCardVisible (seat);
	int iSelfHeight;
	GetDesktopCenterPoint(NULL,NULL,&iSelfHeight);
	QImage img = GetCardPicture(DJGAME_POKER_BACKGROUND,1,getSelfFx());
	int x,y,dx;
	if(lineClass())
		y = (GetRealHeight()-iSelfHeight)+30;
	else
		y = GetRealHeight()-((iSelfHeight-img.height())>>1)-img.height();
	
	PUpgradeRoom pugroom = (PUpgradeRoom)((c_ChaodipPanel->gameRoom())->privateRoom());
	if ( pugroom->chCards >= 3 )
		y	+= 40;
	
	quint8 iMaxWidth = GetCardsMaxWidth(1,cards,chPages);
	dx = 0;
	if(iMaxWidth >=1 )
		dx = ((iMaxWidth-1)*((int)(pageLayoutSizeX()*getSelfFx()))+img.width());
	if(dx >= (int)GetRealWidth())
		x = 3;
	else 
		x = (((int)GetRealWidth() -dx)>>1);
	
	DJGamePokerImage pokerimg;
	memset(&pokerimg,0,sizeof(DJGamePokerImage));
	DJGamePokerItem *item;
	int cx =x,cy = y,dy,i;
	QMatrix m = GetCurrentMatrix();
	for(i=0;i<chPages;i++)
	{
		img = GetCardPicture(cards[i],1,getSelfFx());
		item = new DJGamePokerItem(img,canvas(),m,(pokerimg.chPage[cards[i]]<<8)+cards[i],seat,DJGAME_POKERDESKTOP_TYPE_HANDCARD,false);
		pokerimg.chPage[cards[i]] ++;
		item->setdx((int)(popupSize()*getSelfFx()));
		item->setdy((int)((popupSize()*getSelfFx())/2));
		item->setZ(100+i);
		item->move(cx,cy);
		item->show();
		item->setMatrix(m);
		GetPageInterval(1,cards,i,&dx,&dy);
		if(dy > 0)
		{
			if ( visible ) {
				cy += dy*((int)(pageLayoutSizeY()*getSelfFx()));
				cx = x;
			}else
				break;
		}else
		{
			cx += dx*((int)(pageLayoutSizeX()*getSelfFx()));
			if(cx > (int)GetRealWidth()-((int)(pageLayoutSizeX()*getSelfFx()))*2)
			{
				if ( visible ) {
					cy += ((int)(pageLayoutSizeY()*getSelfFx()));
					cx = x;
				}else
					break;
			}
		}
	}
	canvas()->update();
}
bool ChaodipDesktop::handleItemClicked(Qt::MouseButton btn, const QPoint& pos,Q3CanvasItem * item)
{
	bool hasHandled	= DJGamePokerDesktop::handleItemClicked(btn,pos,item);
	if ( !hasHandled ) {
		hasHandled	= DJGameDesktop::handleItemClicked(btn,pos,item);
	}
	return hasHandled;
}
/////////////////////////////////////////////////
ChaodipPanel::ChaodipPanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f)
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
	createPanelBar(gameSplitter);
	ChaodipDesktop	*gameDesktop = new ChaodipDesktop(gameSplitter,this);
	
	setAccessorySplitter( accessorySplitter );
	setGameDesktop( gameDesktop );	
}
ChaodipPanel::~ChaodipPanel()
{
}
