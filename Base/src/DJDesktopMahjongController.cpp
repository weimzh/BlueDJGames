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

#include "DJDesktopMahjongController.h"
#include "DJPanelController.h"
#include "DJPanel.h"
#include "DJGameRoom.h"
#include "DJDesktop.h"
#include "DJDesktopItem.h"
#include "DJGraphicsTextItem.h"
#include "DJGraphicsPixmapItem.h"
#include "DJGraphicsSvgItem.h"
#include "DJGameUser.h"
#include "DJMessageBox.h"
#include "Utility.h"
#include "DJDesktopScene.h"
#include "DJClientRequest.h"
#include "mjrule.h"
#include "MJUtility.h"
#include "UIMJSelect.h"
//#include "mjdebug.h"

static	int	MAHJONG_GROUP_SPACE			=	30;
static	int	MAHJONG_LYING_DISTANCE		=	28;
static	int	MAHJONG_STANDING_DISTANCE	=	58;
static	int	MAHJONG_WALL_DISTANCE		=	34;
static	int MAHJONG_WALL_LYING_HEIGHT	=	42;

static	int	MAHJONG_SELECT_DELTAHEIGHT	=	22;

static	int	MAHJONG_TOOLBUTTON_SIZE		=	24;

static const char *gFanNames[]={
	QT_TRANSLATE_NOOP("DJDesktopMahjongController","Unknown"),
	QT_TRANSLATE_NOOP("DJDesktopMahjongController","Unknown"),
	QT_TRANSLATE_NOOP("DJDesktopMahjongController","Unknown"),
	QT_TRANSLATE_NOOP("DJDesktopMahjongController","Unknown"),
	QT_TRANSLATE_NOOP("DJDesktopMahjongController","Unknown"),
	QT_TRANSLATE_NOOP("DJDesktopMahjongController","Unknown"),
	QT_TRANSLATE_NOOP("DJDesktopMahjongController","Unknown"),
	QT_TRANSLATE_NOOP("DJDesktopMahjongController","Tian Hu"),
	QT_TRANSLATE_NOOP("DJDesktopMahjongController","Di Hu"),
	QT_TRANSLATE_NOOP("DJDesktopMahjongController","Ren Hu"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Da 4 Xi"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Da 3 Yuan"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Lv 1 Se"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Hong Kong Que"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","9 Lian Bao Deng"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","4 Gang"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Lian 7 Dui"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","13 1"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Qing 1 9"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Xiao 4 Xi"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Xiao 3 Yuan"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Zi 1 Se"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","4 An Ke"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","1 Se 2 Long Hui"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","1 Se 4 Tong Shun"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","1 Se 4 Jie Gao"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","1 Se 4 Bu Gao"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","3 Gang"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Hun 1 9"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","7 Dui"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","7 Xing Bu Kao"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Quan 2 Ke"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Qing 1 Se"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","1 Se 3 Tong Shun"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","1 Se 3 Jie Gao"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Quan Da"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Quan Zhong"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Quan Xiao"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Qing Long"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","3 Se 2 Long Hui"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","1 Se 3 Bu Gao"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Quan Dai 5"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","3 Tong Ke"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","3 An Ke"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Quan Bu Kao"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Zu He Long"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController"," > 5"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController"," < 5"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","3 Feng Ke"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Hua Long"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Tui Bu Dao"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","3 Se 3 Tong Shun"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","3 Se 3 Jie Gao"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Wu Fan Hu"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Miao Shou Hui Chun"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Hai Di Lao Yue"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Gang Shang Kai Hua"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Qiang Gang Hu"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Peng Peng Hu"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Hun 1 Se"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","3 Se 3 Bu Gao"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","5 Men Qi"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Quan Qiu Ren"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","2 An Gang"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","2 Jian Ke"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Quan Dai 1"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Bu Qiu Ren"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","2 Ming Gang"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Hu Jue Zhang"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Jian Ke"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Quan Feng Ke"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Men Feng Ke"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Men Qian Qing"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Ping Hu"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","4 Gui 1"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","2 Tong Ke"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","2 An Ke"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","An Gang"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Duan 1 9"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","YI Ban Gao"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Xi Xiang Feng"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Lian 6"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Lao Shao Fu"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","1 9 Ke"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Ming Gang"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Que 1 Men"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Wu Zi"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Bian Zhang"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Kan Zhang"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Dan Diao"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Zi Mo"),
    QT_TRANSLATE_NOOP("DJDesktopMahjongController","Hua Pai")
};

static bool MJCardLessThan( MJCard card1, MJCard card2 )
{ 
	card1	= MAHJONG_CARD(card1);
	card2	= MAHJONG_CARD(card2);
	
	if ( card1  < 0x10 && card2 >= 0x10 ) {
		return false;
	}else if ( card2  < 0x10 && card1 >= 0x10 ) {
		return true;
	}else {
		return card1  < card2;
	}
}
static bool DJMahjongItemLessThan( DJDesktopItem* item1, DJDesktopItem* item2 )
{
	return MJCardLessThan( item1->value(), item2->value() );
}
DJDesktopMahjongController::DJDesktopMahjongController( DJPanelController* panelController, const QSize& size, QWidget *parent )
	:DJDesktopController( panelController, size, parent)
{	
	djDebug() << "DJDesktopMahjongController constructor";
	
	for ( int view = 0; view <= 4; ++view ) {
		m_scaleOfViewStandingKnownHand << 0.5;
		m_scaleOfViewStandingUnknownHand << 0.5;
		m_scaleOfViewLyingHand << 0.5;
		m_scaleOfViewFlower << 0.4;
		m_scaleOfViewWall << 0.4;
		m_scaleOfViewPool << 0.5;
		m_scaleOfViewChu << 0.75;
		
		m_sizeOfViewStandingKnownHand << QSize();
		m_sizeOfViewStandingUnknownHand << QSize();
		m_sizeOfViewLyingHand << QSize();
		m_sizeOfViewWall << QSize();
		
		m_distanceOfViewStandingKnownHand << 0;
		m_distanceOfViewStandingUnknownHand << 0;
		m_distanceOfViewLyingHand << 0;
		m_distanceOfViewWall << 0;
		
		m_deltaOfViewPoolNewLine << 0;
		m_deltaOfViewHandGang << QPointF();
	}
	m_scaleOfViewStandingKnownHand[1]	= 0.75;
	m_scaleOfViewStandingUnknownHand[1]	= 0.75;
	m_scaleOfViewLyingHand[1]	= 0.64;
	
	QStringList dirs;
	dirs << ":/BaseRes/svg/mj/back";
	dirs << ":/BaseRes/svg/mj/lying1";
	dirs << ":/BaseRes/svg/mj/lying2";
	dirs << ":/BaseRes/svg/mj/lying3";
	dirs << ":/BaseRes/svg/mj/lying4";
	dirs << ":/BaseRes/svg/mj/standing1";
	foreach( QString dir, dirs ) {
		QDir	d(dir);
		QStringList files	= d.entryList(QDir::Files);
		foreach( QString file, files ) {
			QString filepath	= d.filePath(file);
			djDebug()<<"resfile"<<filepath;
			QSvgRenderer *renderer	= new QSvgRenderer( filepath, this );
			m_mahjongSvgCache.insert( filepath, renderer );
		}
	}
	
	m_chuItem	= new DJGraphicsSvgItem;
	desktop()->scene()->addItem( m_chuItem );
	QPen pen( Qt::red );
	pen.setWidth( 2 );
	m_chuItem->setDJSelected( true );
	m_chuItem->setPen( pen );
	m_huTitle	= 0;
	
	QString resPath;
	QSize resSize;
	resPath	= ":/BaseRes/svg/mj/standing1/01.svg";
	resSize	= cardRenderer( resPath )->defaultSize();
	m_sizeOfViewStandingKnownHand[1]	= resSize;
	
	resPath	= ":/BaseRes/svg/mj/lying2/01.svg";
	resSize	= cardRenderer( resPath )->defaultSize();
	resSize.transpose();
	m_sizeOfViewStandingKnownHand[2]	= resSize;
	
	resPath	= ":/BaseRes/svg/mj/lying3/01.svg";
	resSize	= cardRenderer( resPath )->defaultSize();
	m_sizeOfViewStandingKnownHand[3]	= resSize;
	
	resPath	= ":/BaseRes/svg/mj/lying4/01.svg";
	resSize	= cardRenderer( resPath )->defaultSize();
	resSize.transpose();
	m_sizeOfViewStandingKnownHand[4]	= resSize;
	
	resPath	= ":/BaseRes/svg/mj/standing1/40.svg";
	resSize	= cardRenderer( resPath )->defaultSize();
	m_sizeOfViewStandingUnknownHand[1]	= resSize;
	
	resPath	= ":/BaseRes/svg/mj/back/standing2.svg";
	resSize	= cardRenderer( resPath )->defaultSize();
	resSize.transpose();
	m_sizeOfViewStandingUnknownHand[2]	= resSize;
	
	resPath	= ":/BaseRes/svg/mj/back/standing3.svg";
	resSize	= cardRenderer( resPath )->defaultSize();
	m_sizeOfViewStandingUnknownHand[3]	= resSize;
	
	resPath	= ":/BaseRes/svg/mj/back/standing4.svg";
	resSize	= cardRenderer( resPath )->defaultSize();
	resSize.transpose();
	m_sizeOfViewStandingUnknownHand[4]	= resSize;
	
	resPath	= ":/BaseRes/svg/mj/lying1/01.svg";
	resSize	= cardRenderer( resPath )->defaultSize();
	m_sizeOfViewLyingHand[1]	= resSize;
	
	resPath	= ":/BaseRes/svg/mj/lying2/01.svg";
	resSize	= cardRenderer( resPath )->defaultSize();
	resSize.transpose();
	m_sizeOfViewLyingHand[2]	= resSize;
	
	resPath	= ":/BaseRes/svg/mj/lying3/01.svg";
	resSize	= cardRenderer( resPath )->defaultSize();
	m_sizeOfViewLyingHand[3]	= resSize;
	
	resPath	= ":/BaseRes/svg/mj/lying4/01.svg";
	resSize	= cardRenderer( resPath )->defaultSize();
	resSize.transpose();
	m_sizeOfViewLyingHand[4]	= resSize;
	
	resPath	= ":/BaseRes/svg/mj/back/lying13.svg";
	resSize	= cardRenderer( resPath )->defaultSize();
	m_sizeOfViewWall[1]	= resSize;
	m_sizeOfViewWall[3]	= resSize;
	
	resPath	= ":/BaseRes/svg/mj/back/lying24.svg";
	resSize	= cardRenderer( resPath )->defaultSize();
	resSize.transpose();
	m_sizeOfViewWall[2]	= resSize;
	m_sizeOfViewWall[4]	= resSize;
	
	m_distanceOfViewStandingKnownHand[1]	= 0;
	m_distanceOfViewStandingKnownHand[2]	= -MAHJONG_LYING_DISTANCE;
	m_distanceOfViewStandingKnownHand[3]	= 0;
	m_distanceOfViewStandingKnownHand[4]	= -MAHJONG_LYING_DISTANCE;
	
	m_distanceOfViewStandingUnknownHand[1]	= 0;
	m_distanceOfViewStandingUnknownHand[2]	= -MAHJONG_STANDING_DISTANCE;
	m_distanceOfViewStandingUnknownHand[3]	= 0;
	m_distanceOfViewStandingUnknownHand[4]	= -MAHJONG_STANDING_DISTANCE;
	
	m_distanceOfViewLyingHand[1]	= 0;
	m_distanceOfViewLyingHand[2]	= -MAHJONG_LYING_DISTANCE;
	m_distanceOfViewLyingHand[3]	= 0;
	m_distanceOfViewLyingHand[4]	= -MAHJONG_LYING_DISTANCE;
	
	m_distanceOfViewWall[1]	= 0;
	m_distanceOfViewWall[2]	= -MAHJONG_WALL_DISTANCE;
	m_distanceOfViewWall[3]	= 0;
	m_distanceOfViewWall[4]	= -MAHJONG_WALL_DISTANCE;
	
	m_deltaOfViewPoolNewLine[1]	= -104;
	m_deltaOfViewPoolNewLine[2]	= 108;
	m_deltaOfViewPoolNewLine[3]	= 114;
	m_deltaOfViewPoolNewLine[4]	= -104;
	
	m_deltaOfViewHandGang[1]	= QPointF(0,-26);
	m_deltaOfViewHandGang[2]	= QPointF(0,-32);
	m_deltaOfViewHandGang[3]	= QPointF(0,-26);
	m_deltaOfViewHandGang[4]	= QPointF(0,-32);
	
	m_selfEdge	= desktop()->realHeight();
	
	m_supportedHuStyle	= 0xFFFF;
	
	m_actionContainer	= new QWidget( desktop() );
    QHBoxLayout *layout	= new QHBoxLayout( m_actionContainer );
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
    
    QSize toolButtonSize(MAHJONG_TOOLBUTTON_SIZE,MAHJONG_TOOLBUTTON_SIZE);
    QIcon icon;
    m_qi = new QToolButton;
    m_qi->setEnabled(false);
	m_qi->setToolTip(tr("pass"));
	icon	= QIcon(":/BaseRes/image/desktop/toolbutton/qi.png");
	icon.addPixmap( QPixmap(":/BaseRes/image/desktop/toolbutton/qi_disabled.png"), QIcon::Disabled );
	m_qi->setIcon( icon );
	m_qi->setIconSize( toolButtonSize );
    connect(m_qi,SIGNAL(clicked()),this,SLOT(clickQi()));
    layout->addWidget( m_qi );
    
    m_chu = new QToolButton;
    m_chu->setEnabled(false);
	m_chu->setToolTip(tr("throw a card"));
	icon	= QIcon(":/BaseRes/image/desktop/toolbutton/chu.png");
	icon.addPixmap( QPixmap(":/BaseRes/image/desktop/toolbutton/chu_disabled.png"), QIcon::Disabled );
	m_chu->setIcon( icon );
	//m_chu->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/chu.png") );
	m_chu->setIconSize( toolButtonSize );
    connect(m_chu,SIGNAL(clicked()),this,SLOT(clickChu()));
    layout->addWidget( m_chu );
    
    m_chi = new QToolButton;
    m_chi->setEnabled(false);
	m_chi->setToolTip(tr("chi a card"));
	icon	= QIcon(":/BaseRes/image/desktop/toolbutton/chi.png");
	icon.addPixmap( QPixmap(":/BaseRes/image/desktop/toolbutton/chi_disabled.png"), QIcon::Disabled );
	m_chi->setIcon( icon );
	//m_chi->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/chi.png") );
	m_chi->setIconSize( toolButtonSize );
    connect(m_chi,SIGNAL(clicked()),this,SLOT(clickChi()));
    layout->addWidget( m_chi );
	
	m_peng = new QToolButton;
    m_peng->setEnabled(false);
	m_peng->setToolTip(tr("peng a card"));
	icon	= QIcon(":/BaseRes/image/desktop/toolbutton/peng.png");
	icon.addPixmap( QPixmap(":/BaseRes/image/desktop/toolbutton/peng_disabled.png"), QIcon::Disabled );
	m_peng->setIcon( icon );
	//m_peng->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/peng.png") );
	m_peng->setIconSize( toolButtonSize );
    connect(m_peng,SIGNAL(clicked()),this,SLOT(clickPeng()));
    layout->addWidget( m_peng );
    
    m_gang = new QToolButton;
    m_gang->setEnabled(false);
	m_gang->setToolTip(tr("gang a card"));
	icon	= QIcon(":/BaseRes/image/desktop/toolbutton/gang.png");
	icon.addPixmap( QPixmap(":/BaseRes/image/desktop/toolbutton/gang_disabled.png"), QIcon::Disabled );
	m_gang->setIcon( icon );
	//m_gang->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/gang.png") );
	m_gang->setIconSize( toolButtonSize );
    connect(m_gang,SIGNAL(clicked()),this,SLOT(clickGang()));
    layout->addWidget( m_gang );
    
    m_ting = new QToolButton;
    m_ting->setEnabled(false);
	m_ting->setToolTip(tr("ting"));
	icon	= QIcon(":/BaseRes/image/desktop/toolbutton/ting.png");
	icon.addPixmap( QPixmap(":/BaseRes/image/desktop/toolbutton/ting_disabled.png"), QIcon::Disabled );
	m_ting->setIcon( icon );
	//m_ting->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/ting.png") );
	m_ting->setIconSize( toolButtonSize );
    connect(m_ting,SIGNAL(clicked()),this,SLOT(clickTing()));
    layout->addWidget( m_ting );
    
    m_hu = new QToolButton;
    m_hu->setEnabled(false);
	m_hu->setToolTip(tr("hu"));
	icon	= QIcon(":/BaseRes/image/desktop/toolbutton/hu.png");
	icon.addPixmap( QPixmap(":/BaseRes/image/desktop/toolbutton/hu_disabled.png"), QIcon::Disabled );
	m_hu->setIcon( icon );
	//m_hu->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/hu.png") );
	m_hu->setIconSize( toolButtonSize );
    connect(m_hu,SIGNAL(clicked()),this,SLOT(clickHu()));
    layout->addWidget( m_hu );
    
    /*
    unsigned char buf[20];
	memset( buf, 0, 20 );
	buf[0]	= 0x11;
	buf[1]	= 0x11;
	buf[2]	= 0x12;
	buf[3]	= 0x12;
	buf[4]	= 0x21;
	buf[5]	= 0x21;
	buf[6]	= 0x23;
	buf[7]	= 0x23;
	buf[8]	= 0x31;
	buf[9]	= 0x31;
	buf[10]	= 0x35;
	buf[11]	= 0x35;
	buf[12]	= 0x0a;
	buf[13]	= 0x0a;
	MahjongHUTypeWrap *presult	= MahjongRule_CheckHu( 0, buf );
	if ( 0 == presult ) {
		djDebug()<<"no hu";
	}else {
		djDebug()<<"hu le";
		FreeHUTypeWrap( presult );
	}
	*/
}
DJDesktopMahjongController::~DJDesktopMahjongController()
{
	djDebug() << "DJDesktopMahjongController destructor";
}
int DJDesktopMahjongController::bricksPerWall() const
{
	return 18;
}
void DJDesktopMahjongController::initUnderGameInfo( const QByteArray& buf )
{
	djDebug()<<"DJDesktopMahjongController::initUnderGameInfo";
	
	memset( &m_selfLyingCards, 0, sizeof(MahjongHUTypeWrap) );
	InitializeNormalWrap( &m_selfLyingCards );
	m_selfStandingCards.clear();

/*
#if !defined(QT_NO_DEBUG) || defined(DJ_DEBUG_OUTPUT)	
	PrintCardWrap((PMahjongCardWrap)&m_selfLyingCards);
	PrintMJCards(m_selfStandingCards);
#endif
*/
	m_bricksRemovedFromHead	= 0;
	m_bricksRemovedFromTail	= 0;
	setWallHeadTail( 1, 2 );
	
	setLastTraceGang( false );
	setLastChuSeat( 0 );
	setLastChuCard( 0 );
	
	clearHuText();
}
void DJDesktopMahjongController::locateAll()
{
	djDebug()<<"DJDesktopMahjongController::locateAll";
	DJDesktopController::locateAll();
	
	locateWallSpace( m_wallSpace );
	locatePoolSpace( m_poolSpace );
	locateCenterSpace( m_centerSpace );
	
	djDebug()<<"centerPoint"<<centerPoint();
	djDebug()<<"availableSpace"<<availableSpace();
	djDebug()<<"wallSpace"<<wallSpace();
	djDebug()<<"poolSpace"<<poolSpace();
}
void DJDesktopMahjongController::locateWallSpace( QRect& rect )
{
	int space	= 10;
	int height1	= sizeOfViewStandingKnownHand(1).height() + MAHJONG_SELECT_DELTAHEIGHT + space;
	int height2	= sizeOfViewLyingHand(2).height() + space;
	int height3	= sizeOfViewStandingKnownHand(3).height() + space;
	int height4 = sizeOfViewLyingHand(4).height() + space;
	
	m_selfEdge	= desktop()->realHeight() - height1;
	int flowerHeight1	= sizeOfViewLyingHand(1).height() + space;
	
	rect.setBottom( availableSpace().bottom() - static_cast<int>(scaleOfViewStandingUnknownHand(1) * height1) - static_cast<int>(scaleOfViewFlower(1) * flowerHeight1) );
	rect.setLeft( availableSpace().left() + static_cast<int>(scaleOfViewStandingUnknownHand(2) * height2) );
	rect.setTop( availableSpace().top() + static_cast<int>(scaleOfViewStandingUnknownHand(3) * height3) );
	rect.setRight( availableSpace().right() - static_cast<int>(scaleOfViewStandingUnknownHand(4) * height4) );
}
void DJDesktopMahjongController::locatePoolSpace( QRect& rect )
{
	int space	= 10;
	int height1	= sizeOfViewWall(1).height() + MAHJONG_WALL_LYING_HEIGHT + space;
	int height2	= sizeOfViewWall(2).height() + space;
	int height3	= sizeOfViewWall(3).height() + MAHJONG_WALL_LYING_HEIGHT + space;
	int height4 = sizeOfViewWall(4).height() + space;
	
	rect.setBottom( wallSpace().bottom() - static_cast<int>(scaleOfViewWall(1) * height1) );
	rect.setLeft( wallSpace().left() + static_cast<int>(scaleOfViewWall(2) * height2) );
	rect.setTop( wallSpace().top() + static_cast<int>(scaleOfViewWall(3) * height3) );
	rect.setRight( wallSpace().right() - static_cast<int>(scaleOfViewWall(4) * height4) );
}
void DJDesktopMahjongController::locateCenterSpace( QRect& rect )
{
	rect.setBottom( centerPoint().y() + 20 );
	rect.setLeft( centerPoint().x() - 20 );
	rect.setTop( centerPoint().y() - 20 );
	rect.setRight( centerPoint().x() + 20 );
}
void DJDesktopMahjongController::locateAllNonMatrimoveItems()
{
	DJDesktopController::locateAllNonMatrimoveItems();
	
	QMatrix m	= desktop()->graphicsMatrix();
	QPoint pos( wallSpace().right(), selfEdge() );
	pos	= m.map( pos );
	m_actionContainer->move( pos.x() - m_actionContainer->width(), pos.y() - m_actionContainer->height() );
}
DJDesktopItem* DJDesktopMahjongController::takeDesktopItem( int seat, int type, int value )
{
	DesktopItems& items	= desktopItems( seat, type );
	DesktopItemsIterator errorItemIt	= items.end();
	DesktopItemsIterator it	= items.begin();
	while( it != items.end() ) {
		DJDesktopItem *item	= *it;
		if ( MAHJONG_ISERRORCARD(item->value()) ) {
			errorItemIt	= it;
		}
		if ( value == item->value() ) {
			items.erase(it);
			return item;
		}
		it++;
	}
	if ( errorItemIt != items.end() ) {
		DJDesktopItem *item	= *errorItemIt;
		items.erase( errorItemIt );
		return item;
	}
	return 0;
}
void DJDesktopMahjongController::syncSelfStandingCards()
{
	m_selfStandingCards.clear();
	
	DesktopItems& items	= desktopItems( panelController()->seatId(), MAHJONG_TYPE_STANDING_HAND );
	foreach( DJDesktopItem *item, items ) {
		m_selfStandingCards.push_back( item->value() );
	}
}
void DJDesktopMahjongController::syncSelfLyingCards()
{
	memset( &m_selfLyingCards, 0, sizeof(MahjongHUTypeWrap) );
	InitializeNormalWrap( &m_selfLyingCards );
	
	int	indexOfChi	= 0;
	int indexOfPeng	= 4;
	
	for( int i = 0; i < MAHJONG_MAX_LYING_HAND; i++ ) {
		int type	= MAHJONG_TYPE_LYING_HAND + i;
		DesktopItems& items	= desktopItems( panelController()->seatId(), type );
		if ( items.isEmpty() )
			break;
		if ( 4 == items.size() ) {
			quint8 card	= items.last()->value();
			m_selfLyingCards.cardwrap[indexOfPeng].chExist	= 1;
			for( int i = 0; i < 4; i++ ) {
				m_selfLyingCards.cardwrap[indexOfPeng].chWrap[i] = card;
			}
			indexOfPeng++;
		}else {
			if ( MAHJONG_CARD(items.first()->value()) == MAHJONG_CARD(items.last()->value()) ) {
				//peng
				m_selfLyingCards.cardwrap[indexOfPeng].chExist	= 1;
				for( int i = 0; i < 3; i++ ) {
					m_selfLyingCards.cardwrap[indexOfPeng].chWrap[i] = items.at(i)->value();
				}
				indexOfPeng++;
			}else {
				//chi
				m_selfLyingCards.cardwrap[indexOfChi].chExist	= 1;
				for( int i = 0; i < 3; i++ ) {
					m_selfLyingCards.cardwrap[indexOfChi].chWrap[i] = items.at(i)->value();
				}
				indexOfChi++;
			}
		}
	}
}
MJCardsGroups DJDesktopMahjongController::testChi() const
{
	MJCardsGroups 	result;
	do {
		if ( lastChuSeat() == panelController()->seatId() ) 
			break;
		if ( lastChuSeat() != prevPlayerSeatId(panelController()->seatId()) ) 
			break;
		if(MAHJONG_ISERRORCARD(MAHJONG_CARD(lastChuCard())))
			break;
		if(MAHJONG_ISZI(MAHJONG_CARD(lastChuCard())))
			break;
		unsigned char theClass	= GET_MAHJONG_CARDCLASS( lastChuCard() );
		unsigned char theValue	= GET_MAHJONG_CARDVALUE( lastChuCard() );
		
		unsigned char chP1 = 0,chP2=0,chN1=0,chN2=0;
		for( size_t i = 0; i < m_selfStandingCards.size(); i++ ) {
			if( !MAHJONG_ISERRORCARD(m_selfStandingCards[i]) 
				&& GET_MAHJONG_CARDCLASS(m_selfStandingCards[i]) == theClass ) {
				if(GET_MAHJONG_CARDVALUE(m_selfStandingCards[i]) == theValue-1 && theValue>=2)
					chP1 ++;
				if(GET_MAHJONG_CARDVALUE(m_selfStandingCards[i]) == theValue-2 && theValue >=3)
					chP2 ++;
				if(GET_MAHJONG_CARDVALUE(m_selfStandingCards[i]) == theValue+1 && theValue <=8)
					chN1 ++;
				if(GET_MAHJONG_CARDVALUE(m_selfStandingCards[i]) == theValue+2 && theValue <=7 )
					chN2 ++;
			}
		}
		if(chP1 > 0 && chP2 > 0) {
			//12X 
			MJCards	mjcards;
			mjcards.push_back( MAHJONG_MAKE_CARD(theClass,theValue-2) );
			mjcards.push_back( MAHJONG_MAKE_CARD(theClass,theValue-1) );
			mjcards.push_back( MAHJONG_MAKE_CARD(theClass,theValue) );
			result.push_back( mjcards );
		}
		if(chP1 > 0 && chN1 > 0) {
			//1X3
			MJCards	mjcards;
			mjcards.push_back( MAHJONG_MAKE_CARD(theClass,theValue-1) );
			mjcards.push_back( MAHJONG_MAKE_CARD(theClass,theValue+1) );
			mjcards.push_back( MAHJONG_MAKE_CARD(theClass,theValue) );
			result.push_back( mjcards );
		}
		if(chN1 > 0 && chN2 > 0) {
			//X23
			MJCards	mjcards;
			mjcards.push_back( MAHJONG_MAKE_CARD(theClass,theValue+1) );
			mjcards.push_back( MAHJONG_MAKE_CARD(theClass,theValue+2) );
			mjcards.push_back( MAHJONG_MAKE_CARD(theClass,theValue) );
			result.push_back( mjcards );
		}
	}while(false);
	return result;
}
MJCards DJDesktopMahjongController::testPeng() const
{
	MJCards	result; 
	do {
		if ( lastChuSeat() == panelController()->seatId() ) 
			break;
		if(MAHJONG_ISERRORCARD(MAHJONG_CARD(lastChuCard())))
			break;
		MJCard pengCard	= MAHJONG_CARD(lastChuCard());
		int kes	= count(m_selfStandingCards.begin(),m_selfStandingCards.end(),pengCard);
		if ( kes >= 2 ) {
			for ( int i = 0; i < 3; ++i ) {
				result.push_back( pengCard );
			}
		}
	}while(false);
	return result;
}
MJCardsGroups DJDesktopMahjongController::testZimoGang() const
{
	djDebug()<<"DJDesktopMahjongController::testZimoGang";
/*
#if !defined(QT_NO_DEBUG) || defined(DJ_DEBUG_OUTPUT)	
	PrintCardWrap((PMahjongCardWrap)&m_selfLyingCards);
	PrintMJCards(m_selfStandingCards);
#endif	
*/
	MJCardsGroups  result;
	MJCardsMultiSet  multiset	= MakeMultiSet( m_selfStandingCards ); 
	
	for(int i = 4; i < 8; i++ ) {
		if( 0 != m_selfLyingCards.cardwrap[i].chExist
			&& MAHJONG_CARDWRAP_NAME_KE == m_selfLyingCards.cardwrap[i].chCardWrapName ) {
			unsigned char mjcard	= MAHJONG_CARD(m_selfLyingCards.cardwrap[i].chWrap[0]);
			if( 0 == m_selfLyingCards.cardwrap[i].chWrap[3]
				&& 1 == multiset.count(mjcard) ) {
				MJCards mjcards;
				for ( int i = 0; i < 4; ++i ) {
					mjcards.push_back( mjcard );
				}
				result.push_back( mjcards );
			}
		} 
	}
	MJCardsSet	set	= MakeSet( m_selfStandingCards );
	MJCardsSetIterator  it;
	for ( it = set.begin(); it != set.end(); ++it ) {
		unsigned char mjcard	= (*it);
		if ( !MAHJONG_ISERRORCARD(mjcard) && 4 == multiset.count(mjcard) ) {
			MJCards mjcards;
			for ( int i = 0; i < 4; ++i ) {
				mjcards.push_back( mjcard );
			}
			result.push_back( mjcards );
		}
	}
/*
#if !defined(QT_NO_DEBUG) || defined(DJ_DEBUG_OUTPUT)	
	PrintMJCardsGroups( result );
#endif
*/	
	return result;
}
MJCards DJDesktopMahjongController::testDianGang() const
{
	MJCards	result; 
	do {
		if ( lastChuSeat() == panelController()->seatId() ) 
			break;
		if(MAHJONG_ISERRORCARD(MAHJONG_CARD(lastChuCard())))
			break;
		unsigned char gangCard	= MAHJONG_CARD(lastChuCard());
		int kes	= count(m_selfStandingCards.begin(),m_selfStandingCards.end(),gangCard);
		if ( kes == 3 ) {
			for ( int i = 0; i < 4; ++i ) {
				result.push_back( gangCard );
			}
		}
	}while(false);
	return result;
}
int DJDesktopMahjongController::testHu( MJCard qiangGangCard ) const
{
	djDebug()<<"DJDesktopMahjongController::testHu"<<hex<<qiangGangCard;
	
	if ( lastChuSeat() == panelController()->seatId() )
		return 0;
	MJCards cards	= m_selfStandingCards;
	int mode	= cards.size()  % 3;
	if ( 1 == mode ) {
		//check dian hu
		if ( 0 == qiangGangCard ) {
			//check normal dain pao hu
			djDebug()<<"check normal hu";
			cards.push_back( lastChuCard() );
		}else {
			//check qiang gang hu
			djDebug()<<"check qiang gang hu";
			cards.push_back( qiangGangCard );
		}
	}
	sort( cards.begin(), cards.end(), MJCardLessThan );
	unsigned char buf[20];
	memset( buf, 0, 20 );
	for( uint i = 0; i < cards.size(); i++) {
		buf[i]	= cards[i];
	}
	MahjongHUTypeWrap *presult,*pSelfLyingCards = 0;
	for( int i = 0; i < 8; i++ ) {
		if( m_selfLyingCards.cardwrap[i].chExist != 0 ) {
			pSelfLyingCards = (MahjongHUTypeWrap*)&m_selfLyingCards;
			break;
		}
	}
/*	
#if !defined(QT_NO_DEBUG) || defined(DJ_DEBUG_OUTPUT)
	PrintCardWrap((PMahjongCardWrap)&m_selfLyingCards);
#endif	
*/
	SortHuTypeWrap ( pSelfLyingCards );	
/*
#if !defined(QT_NO_DEBUG) || defined(DJ_DEBUG_OUTPUT)	
	PrintCardWrap((PMahjongCardWrap)&m_selfLyingCards);
	PrintMJCards(m_selfStandingCards);
#endif	
*/	
	djDebug()<<"pSelfLyingCards"<<pSelfLyingCards;
	presult	= MahjongRule_CheckHu( pSelfLyingCards, buf, m_supportedHuStyle );
	if ( 0 == presult ) {
		djDebug()<<"no hu";
		return 0;
	}else {
		djDebug()<<"hu le";
		FreeHUTypeWrap( presult );
		return 1;
	}
}
MJCard DJDesktopMahjongController::testHua() const
{
	for( size_t i = 0; i < m_selfStandingCards.size(); i++ ) {
		if (  MAHJONG_ISFLOWERCARD(m_selfStandingCards[i]) ) {
			return m_selfStandingCards[i];
		}
	}
	return 0;
}
QString DJDesktopMahjongController::fanName( int fan ) const
{
	return tr( gFanNames[fan] );
}
QString DJDesktopMahjongController::cardResPath( quint8 card, Pose pose, quint8 view ) const
{
	QString resPath	= ":/BaseRes/svg/mj/";
	card	= MAHJONG_CARD(card);

	if ( Wall == pose ) {
		switch(view) {
		case 1:
		case 3:
			resPath	+= "back/lying13.svg";
			break;
		case 2:
		case 4:
			resPath	+= "back/lying24.svg";
			break;
		default:
			resPath.clear();
			break;
		}
		return resPath;
	}
		
	if ( MAHJONG_ISERRORCARD(card) ) {
		switch(view) {
		case 1:
			if ( Standing == pose )
				resPath	+= "standing1/40.svg";
			else if ( Lying == pose )
				resPath	+= "back/lying13.svg";
			else
				resPath.clear();
			break;
		case 2:
			if ( Standing == pose )
				resPath	+= "back/standing2.svg";
			else if ( Lying == pose )
				resPath	+= "back/lying24.svg";
			else
				resPath.clear();
			break;
		case 3:
			if ( Standing == pose )
				resPath	+= "back/standing3.svg";
			else if ( Lying == pose )
				resPath	+= "back/lying13.svg";
			else
				resPath.clear();
			break;
		case 4:
			if ( Standing == pose )
				resPath	+= "back/standing4.svg";
			else if ( Lying == pose )
				resPath	+= "back/lying24.svg";
			else
				resPath.clear();
			break;
		}
	}else {
		switch(view) {
		case 1:
			if ( Standing == pose )
				resPath	+= QString("standing1/%1.svg").arg(card,2,16,QLatin1Char('0'));
			else if ( Lying == pose )
				resPath	+= QString("lying1/%1.svg").arg(card,2,16,QLatin1Char('0'));
			else
				resPath.clear();
			break;
		case 2:
			resPath	+= QString("lying2/%1.svg").arg(card,2,16,QLatin1Char('0'));
			break;
		case 3:
			resPath	+= QString("lying3/%1.svg").arg(card,2,16,QLatin1Char('0'));
			break;
		case 4:
			resPath	+= QString("lying4/%1.svg").arg(card,2,16,QLatin1Char('0'));
			break;
		}
	}
	return resPath;
}
void DJDesktopMahjongController::removeFromStandingHand( int seat, MJCard card )
{
	djDebug()<<"DJDesktopMahjongController::removeFromStandingHand"<<"seat"<<seat<<"card"<<hex<<card;
	DJDesktopItem *item	= takeDesktopItem( seat, MAHJONG_TYPE_STANDING_HAND, MAHJONG_CARD(card) );
	
	delete item;
}
void DJDesktopMahjongController::chipeng( int seat, const MJCards& cards )
{
	djDebug()<<"DJDesktopMahjongController::chipeng";
	djDebug()<<"cards size"<<cards.size();
	
	setLastChuSeat( 0 );
	setLastChuCard( 0 );
	
	for( int i = 0; i < 2; i++ ) {
		removeFromStandingHand( seat, cards.at(i) );
	}
	for( int i = 0; i < MAHJONG_MAX_LYING_HAND; i++ ) {
		int type	= MAHJONG_TYPE_LYING_HAND + i;
		DesktopItems& items	= desktopItems( seat, type );
		if ( items.isEmpty() ) {
			//found a free space
			foreach( MJCard card, cards ) {
				DJDesktopItem *item	= createDesktopItem( seat, type, card | MAHJONG_CARD_STYLE_CHI );
				items.append( item );
			}
			break;
		}
	}
}
//return if can be qiang gang hu
bool DJDesktopMahjongController::gangflower( int seat, MJCard card )
{
	setLastChuSeat( 0 );
	setLastChuCard( 0 );
	setLastTraceGang( true );
	
	if ( MAHJONG_ISFLOWERCARD(card) ) {
		removeFromStandingHand( seat, card );
		appendDesktopItem( seat, MAHJONG_TYPE_FLOWER, card );
		return false;
	}
	
	//is gang, check peng gang
	for( int i = 0; i < MAHJONG_MAX_LYING_HAND; i++ ) {
		int type	= MAHJONG_TYPE_LYING_HAND + i;
		DesktopItems& items	= desktopItems( seat, type );
		if ( !items.isEmpty() &&
			MAHJONG_CARD(items.first()->value()) == MAHJONG_CARD(card) ) {
			removeFromStandingHand( seat, card );
			DJDesktopItem *item	= createDesktopItem( seat, type, card | MAHJONG_CARD_STYLE_CHI );
			items.append( item );
			return true;
		}
	}
	//no peng gang
	for( int i = 0; i < MAHJONG_MAX_LYING_HAND; i++ ) {
		int type	= MAHJONG_TYPE_LYING_HAND + i;
		DesktopItems& items	= desktopItems( seat, type );
		if ( items.isEmpty() ) {
			//found a free space
			if ( MAHJONG_ISPLUTUS(card) ) {
				//hun gang
				for( int i = 0; i < 4; i++ ) {
					removeFromStandingHand( seat, card );
					DJDesktopItem *item	= createDesktopItem( seat, type, card );
					items.append( item );
				}
			}else if ( MAHJONG_ISDIY( card ) ) {
				//an gang
				for( int i = 0; i < 4; i++ ) {
					removeFromStandingHand( seat, card );
				}
				for( int i = 0; i < 3; i++ ) {
					DJDesktopItem *item	= createDesktopItem( seat, type, MAHJONG_CARD_BACKGROUND );
					items.append( item );
				}
				DJDesktopItem *item	= createDesktopItem( seat, type, card );
				items.append( item );
			}else {
				//ming gang
				for( int i = 0; i < 3; i++ ) {
					removeFromStandingHand( seat, card );
				}
				for( int i = 0; i < 4; i++ ) {
					DJDesktopItem *item	= createDesktopItem( seat, type, card | MAHJONG_CARD_STYLE_CHI);
					items.append( item );
				}
			}
			return false;
		}
	}
	return false;
}
void DJDesktopMahjongController::chu( int seat, MJCard card )
{
	DJDesktopItem *item	= takeDesktopItem( seat, MAHJONG_TYPE_STANDING_HAND, MAHJONG_CARD(card) );
	delete item;
	setLastChuSeat( seat );
	setLastChuCard( MAHJONG_CARD(card) );
}
void DJDesktopMahjongController::zhua( int seat, MJCard card )
{
	DesktopItems &items	= desktopItems( seat, MAHJONG_TYPE_STANDING_HAND );
	if ( MAHJONG_ISERRORCARD(card)
		&& !items.isEmpty()
		&& !MAHJONG_ISERRORCARD(items.first()->value()) )
		return;
	
	appendDesktopItem( seat, MAHJONG_TYPE_STANDING_HAND, MAHJONG_CARD(card) );
	if ( isLastTraceGang() )
		removeFromWall( false );
	else
		removeFromWall( true );
	setLastTraceGang( false );
	if ( lastChuSeat() != 0 && lastChuCard() != 0 ) {
		appendDesktopItem( lastChuSeat(), MAHJONG_TYPE_POOL, lastChuCard() );
	}
}
int DJDesktopMahjongController::lengthOfHand( int seat )
{
	djDebug()<<"DJDesktopMahjongController::lengthOfHand";
	
	int totalLength	= 0;
	quint8 view = seat2View(seat);
	qreal scale	= scaleOfViewLyingHand(view);
	for( int i = 0; i < MAHJONG_MAX_LYING_HAND; i++ ) {
		int type	= MAHJONG_TYPE_LYING_HAND + i;
		DesktopItems& lyingItems	= desktopItems( seat, type );
		if ( !lyingItems.isEmpty() ) {
			int eachItemWidth	= sizeOfViewLyingHand(view).width() + distanceOfViewLyingHand(view);
			int groupLength	= eachItemWidth * 3;
			totalLength	+= static_cast<int>( groupLength * scale );
			totalLength	+= static_cast<int>(MAHJONG_GROUP_SPACE * scale);
		}
	}
	
	DesktopItems&	standingItems	= desktopItems( seat, MAHJONG_TYPE_STANDING_HAND );
	if ( standingItems.isEmpty() )
		return totalLength;
	DJDesktopItem *item	= standingItems.at(0);
	int eachItemWidth	= 0;
	if ( MAHJONG_ISERRORCARD(item->value()) ) {
		scale	= scaleOfViewStandingUnknownHand(view);
		eachItemWidth	= sizeOfViewStandingUnknownHand(view).width() + distanceOfViewStandingUnknownHand(view);
	}else {
		scale	= scaleOfViewStandingKnownHand(view);
		eachItemWidth	= sizeOfViewStandingKnownHand(view).width() + distanceOfViewStandingKnownHand(view);
	}
	int numberOfStanding	= standingItems.size();
	if ( (numberOfStanding % 3) != 2 )
		numberOfStanding	+= 1;
	int groupLength	= eachItemWidth * numberOfStanding;
	totalLength	+= static_cast<int>( groupLength * scale );
	
	return totalLength;
}
int DJDesktopMahjongController::lengthOfWall( int view )
{
	int totalLength	= 0;
	
	DesktopItems&	bottomWallItems	= desktopItems( view, MAHJONG_TYPE_BOTTOM_WALL );
	if ( bottomWallItems.isEmpty() )
		return totalLength;
	qreal scale	= scaleOfViewWall(view);
	int eachItemWidth	= sizeOfViewWall(view).width() + distanceOfViewWall(view);
	
	int groupLength	= eachItemWidth * bricksPerWall();
	totalLength	+= static_cast<int>( groupLength * scale );
	
	return totalLength;
}
void DJDesktopMahjongController::validateView( int& view )
{
	if ( view > 4 )
		view	= 1;
	else if ( view < 1 )
		view	= 4;
}
void DJDesktopMahjongController::getWallTailViewIndex( int& view, int& index )
{
	index++;
	if ( bricksPerWall() == index ) {
		//anticlockwise
		index	= 0;
		view--;
	}
	validateView( view );
}
void DJDesktopMahjongController::setWallHeadTail( int seat, int skips )
{
	djDebug()<<"setWallHeadTail"<<seat<<skips;
	int view	= seat2View(seat);
	djDebug()<<"view"<<view;
	validateView( view );
	djDebug()<<"view"<<view;
	while( skips >= bricksPerWall() ) {
		skips	-= bricksPerWall();
		view++;
		validateView( view );
	}
	djDebug()<<"view"<<view;
	int index	= bricksPerWall() - 1 - skips;
	m_wallHead.first	= view;
	m_wallHead.second	= index;
	djDebug()<<"head"<<"view"<<view<<"index"<<index;
	getWallTailViewIndex( view, index );
	
	m_wallTail.first	= view;
	m_wallTail.second	= index;
	djDebug()<<"tail"<<"view"<<view<<"index"<<index;
}
void DJDesktopMahjongController::removeFromWall( bool fromHead )
{
	djDebug()<<"DJDesktopMahjongController::removeFromWall"<<fromHead;
	djDebug()<<"head"<<m_wallHead.first<<m_wallHead.second;
	djDebug()<<"tail"<<m_wallTail.first<<m_wallTail.second;
	if ( fromHead ) {
		m_bricksRemovedFromHead++;
		
		int view	= m_wallHead.first;
		int index	= m_wallHead.second;
		
		DesktopItems&	bottomWallItems	= desktopItems( view, MAHJONG_TYPE_BOTTOM_WALL );
		DesktopItems&	topWallItems	= desktopItems( view, MAHJONG_TYPE_TOP_WALL );
	
		DJDesktopItem *item	= topWallItems.at(index);
		if ( item->graphicsItem()->isVisible() ) {
			item->graphicsItem()->hide();
		}else {
			item	= bottomWallItems.at(index);
			item->graphicsItem()->hide();
			
			index--;
			if ( -1 == index ) {
				index	= bricksPerWall() - 1;
				view++;
				validateView( view );
			}
			m_wallHead.first	= view;
			m_wallHead.second	= index;
		}
	}else {
		m_bricksRemovedFromTail++;
		
		int view	= m_wallTail.first;
		int index	= m_wallTail.second;
		
		DesktopItems&	bottomWallItems	= desktopItems( view, MAHJONG_TYPE_BOTTOM_WALL );
		DesktopItems&	topWallItems	= desktopItems( view, MAHJONG_TYPE_TOP_WALL );
	
		DJDesktopItem *item	= topWallItems.at(index);
		if ( item->graphicsItem()->isVisible() ) {
			item->graphicsItem()->hide();
		}else {
			item	= bottomWallItems.at(index);
			item->graphicsItem()->hide();
			index++;
			if ( bricksPerWall() == index ) {
				index	= 0;
				view--;
				validateView( view );
			}
			m_wallTail.first	= view;
			m_wallTail.second	= index;
		}
	}
	djDebug()<<"head"<<m_wallHead.first<<m_wallHead.second;
	djDebug()<<"tail"<<m_wallTail.first<<m_wallTail.second;
}
void DJDesktopMahjongController::repaintWall( int wallStartSeat, int wallStartSkips )
{
	setWallHeadTail( wallStartSeat, wallStartSkips );
	for( int view = 1; view <= 4; view++ ) {
		repaintViewWall( view );
	}
	djDebug()<<"remove wall";
	int brfh	= bricksRemovedFromHead();
	int brft	= bricksRemovedFromTail();
	m_bricksRemovedFromHead = 0;
	m_bricksRemovedFromTail = 0;
	for( int i = 0; i < brfh; i++) {
		removeFromWall( true );
	}
	for( int i = 0; i < brft; i++) {
		removeFromWall( false );
	}
}
void DJDesktopMahjongController::repaintViewWall( int view )
{
	djDebug()<<"DJDesktopMahjongController::repaintViewWall"<<view;
	int length	= lengthOfWall(view);
	DesktopItems&	bottomWallItems	= desktopItems( view, MAHJONG_TYPE_BOTTOM_WALL );
	DesktopItems&	topWallItems	= desktopItems( view, MAHJONG_TYPE_TOP_WALL );
	qreal scale	= scaleOfViewWall(view);
	
	int x;
	int y;
	qreal z	= 200;
	Qt::Alignment align	= 0;
	int eachItemWidth	= sizeOfViewWall(view).width();
	int distance	= distanceOfViewWall(view);
	switch( view ) {
		case 1 : {
			x	= centerPoint().x() - (length >> 1);
			y	= wallSpace().bottom();
			align	= Qt::AlignBottom;
			repaintViewWall( view, bottomWallItems, scale,
							x, y, z, 1, 0, 1,
							eachItemWidth, distance, align );
			y	-=	static_cast<int>(MAHJONG_WALL_LYING_HEIGHT*scaleOfViewWall(view));
			z	+=	1;
			repaintViewWall( view, topWallItems, scale,
							x, y, z, 1, 0, 1,
							eachItemWidth, distance, align );
			break;
		}
		case 2 : {
			x	= wallSpace().left();
			y	= centerPoint().y() - (length >> 1);
			repaintViewWall( view, bottomWallItems, scale,
							x, y, z, 0, 1, 1,
							eachItemWidth, distance, align );
			y	-=	static_cast<int>(MAHJONG_WALL_LYING_HEIGHT*scaleOfViewWall(view));;
			z	+=	1;
			repaintViewWall( view, topWallItems, scale,
							x, y, z, 0, 1, 1,
							eachItemWidth, distance, align );
			break;
		}
		case 3 : {
			x	= centerPoint().x() + (length >> 1);
			y	= wallSpace().top() + static_cast<int>(MAHJONG_WALL_LYING_HEIGHT*scaleOfViewWall(view));
			align	= Qt::AlignRight;
			repaintViewWall( view, bottomWallItems, scale,
							x, y, z, -1, 0, 1,
							eachItemWidth, distance, align );
			y	-=	static_cast<int>(MAHJONG_WALL_LYING_HEIGHT*scaleOfViewWall(view));
			z	+=	1;
			repaintViewWall( view, topWallItems, scale,
							x, y, z, -1, 0, 1,
							eachItemWidth, distance, align );
			break;
		}
		case 4 : {
			x	= wallSpace().right();
			y	= centerPoint().y() + (length >> 1);
			align	= Qt::AlignRight | Qt::AlignBottom;
			repaintViewWall( view, bottomWallItems, scale,
							x, y, z, 0, -1, -1,
							eachItemWidth, distance, align );
			y	-=	static_cast<int>(MAHJONG_WALL_LYING_HEIGHT*scaleOfViewWall(view));
			z	+=	1;
			repaintViewWall( view, topWallItems, scale,
							x, y, z, 0, -1, -1,
							eachItemWidth, distance, align );
			break;
		}
	}
}
void DJDesktopMahjongController::repaintViewWall(
				int view, DesktopItems &items, qreal scale, 
				int x, int y, qreal z, 
				int dx, int dy, int dz,
				int eachItemWidth, int distance, Qt::Alignment align )
{
	foreach( DJDesktopItem *item, items ) {
		QGraphicsItem *graphicsItem	= item->graphicsItem();
		DJGraphicsSvgItem *imageItem	= 0;	
		if ( 0 == graphicsItem ) {
			QString resPath	= cardResPath( MAHJONG_CARD(item->value()), Wall, view );
			QSvgRenderer *svgRenderer	= cardRenderer( resPath );
			imageItem	= new DJGraphicsSvgItem;
			imageItem->setSharedRenderer( svgRenderer );
			desktop()->scene()->addItem( imageItem );			
			item->setGraphicsItem( imageItem );
			imageItem->setVirtualPos( QPointF(x,y) );
			imageItem->setAlignment( align );
			imageItem->setExternalScale( desktop()->graphicsScale() );
			imageItem->setInternalScale( scale );
		}else {
			imageItem	= (DJGraphicsSvgItem*)graphicsItem;
		}
		imageItem->setZValue(z);
		imageItem->setDJSelected( false );
		imageItem->adjustPos( desktop()->graphicsMatrix() );
		imageItem->show();
		
		int deltaPosOfNextItem	= static_cast<int>( (eachItemWidth + distance) * scale );
		
		if ( dx > 0 ) {
			x	+= deltaPosOfNextItem;
		}else if ( dx < 0 ) {
			x	-= deltaPosOfNextItem;
		}
		if ( dy > 0 ) {
			y	+= deltaPosOfNextItem;
		}else if ( dy < 0 ) {
			y	-= deltaPosOfNextItem;
		}
		if ( dz > 0 ) {
			z	+= 2;
		}else if ( dz < 0 ) {
			z	-= 2;
		}
	}
}
void DJDesktopMahjongController::repaintSeatHand( int seat, bool repaintSize, bool repaintPos, bool resort, bool isLastPickup )
{
	djDebug()<<"DJDesktopMahjongController::repaintHandMahjong"<<seat<<repaintSize<<repaintPos<<resort;
	
	DesktopItems&	standingItems	= desktopItems( seat, MAHJONG_TYPE_STANDING_HAND );
	int length	= lengthOfHand(seat);
	
	djDebug()<<"length="<<length;
	if ( standingItems.isEmpty() )
		return;
	
	if ( resort	)
		qSort( standingItems.begin(), standingItems.end(), DJMahjongItemLessThan );
	quint8 view = seat2View(seat);
	djDebug()<<"view"<<view;
		
	int x;
	int y;
	qreal z	= 200;
	Qt::Alignment align	= 0;
	
	switch( view ) {
		case 1 : {
			x	= centerPoint().x() - (length >> 1);
			y	= availableSpace().bottom();
			align	= Qt::AlignBottom;
			repaintSeatViewHand( seat, view, 
								repaintSize, repaintPos, isLastPickup,
								x, y, z, 1, 0, 1, align );
			break;
		}
		case 2: {
			x	= availableSpace().left();
			y	= centerPoint().y() - (length >> 1);
			repaintSeatViewHand( seat, view, 
								repaintSize, repaintPos, isLastPickup,
								x, y, z, 0, 1, 1, align );
			break;
		}
		case 3: {
			x	= centerPoint().x() + (length >> 1);
			y	= availableSpace().top();
			align	= Qt::AlignRight;
			
			repaintSeatViewHand( seat, view, 
								repaintSize, repaintPos, isLastPickup,
								x, y, z, -1, 0, 1, align );
			break;
		}
		case 4: {
			x	= availableSpace().right();
			y	= centerPoint().y() + (length >> 1);
			align	= Qt::AlignRight | Qt::AlignBottom;
			
			repaintSeatViewHand( seat, view, 
								repaintSize, repaintPos, isLastPickup,
								x, y, z, 0, -1, -1, align );
			break;
		}
		default:
			break;
	}
}
void DJDesktopMahjongController::repaintSeatViewHand(
				int seat, int view,
				bool repaintSize, bool repaintPos, bool isLastPickup,
				int& x, int& y, qreal& z,
				int dx, int dy, int dz,
				Qt::Alignment align )
{
	//draw lying hand cards
	qreal scale	= scaleOfViewLyingHand(view);
	int eachItemWidth	= sizeOfViewLyingHand(view).width();
	int distance	= distanceOfViewLyingHand(view);
	
	for( int i = 0; i < MAHJONG_MAX_LYING_HAND; i++ ) {
		int type	= MAHJONG_TYPE_LYING_HAND + i;
		DesktopItems& lyingItems	= desktopItems( seat, type );
		if ( !lyingItems.isEmpty() ) {
			repaintViewHand( view, lyingItems,
							repaintSize, repaintPos, 
							Lying, scale,
							x, y, z, dx, dy, dz,
							eachItemWidth, distance, align );
			int space	= static_cast<int>(MAHJONG_GROUP_SPACE * scale);
			if ( dx > 0 )
				x	+= space;
			else if ( dx < 0 )
				x	-= space;
			if ( dy > 0 )
				y	+= space;
			else if ( dy < 0 )
				y	-= space;
		}
	}
	//draw standing hand cards
	DesktopItems&	standingItems	= desktopItems( seat, MAHJONG_TYPE_STANDING_HAND );
	if ( standingItems.isEmpty() )
		return;
	DJDesktopItem *item	= standingItems.at(0);
	if ( MAHJONG_ISERRORCARD(item->value()) ) {
		scale	= scaleOfViewStandingUnknownHand(view);
		eachItemWidth	= sizeOfViewStandingUnknownHand(view).width();
		distance	= distanceOfViewStandingUnknownHand(view);
	}else {
		scale	= scaleOfViewStandingKnownHand(view);
		eachItemWidth	= sizeOfViewStandingKnownHand(view).width();
		distance	= distanceOfViewStandingKnownHand(view);
	}
	if ( isLastPickup ) {
		//split to 2 parts
		//the pick up card is isolated
		DesktopItems items	= standingItems;
		DesktopItems lastItems;
			
		DJDesktopItem *lastItem	= items.takeLast();
		lastItems.append( lastItem );
			
		repaintViewHand( view, items,
						repaintSize, repaintPos,
						Standing, scale,
						x, y, z, dx, dy, dz,
						eachItemWidth, distance, align );
		int space	= static_cast<int>(MAHJONG_GROUP_SPACE * scale);
		if ( dx > 0 )
			x	+= space;
		else if ( dx < 0 )
			x	-= space;
		if ( dy > 0 )
			y	+= space;
		else if ( dy < 0 )
			y	-= space;
		repaintViewHand( view, lastItems, 
						repaintSize, repaintPos,
						Standing, scale, 
						x, y, z, dx, dy, dz,
						eachItemWidth, distance, align );
	}else {
		repaintViewHand( view, standingItems, 
						repaintSize, repaintPos, 
						Standing, scale, 
						x, y, z, dx, dy, dz,
						eachItemWidth, distance, align );
	}
}
void DJDesktopMahjongController::repaintViewHand(
		int view, DesktopItems &items, 
		bool repaintSize, bool repaintPos, 
		Pose pose, qreal scale, 
		int& x, int& y, qreal& z, 
		int dx, int dy, int dz,
		int eachItemWidth, int distance, Qt::Alignment align )
{
	djDebug()<<"repaintViewHand"<<"view"<<view<<"itemsize"<<items.size()<<"distance"<<distance;
	
	int deltaPosOfNextItem	= static_cast<int>( (eachItemWidth + distance) * scale );
	int size	= items.size();
	
	QPointF deltaOfGang	= deltaOfViewHandGang(view);
	deltaOfGang	*= scale;
	
	QPen pen( Qt::red );
	pen.setWidth( 2 );
	
	//foreach( DJDesktopItem *item, items ) {
	for ( int i = 0; i < size; i++ ) {
		DJDesktopItem *item	= items.at(i);
		QGraphicsItem *graphicsItem	= item->graphicsItem();
		DJGraphicsSvgItem *imageItem	= 0;
		QPointF pos	= QPointF(x,y);
		qreal	zvalue	= z;
		
		if ( Lying == pose && 4 == size && i == size -1 ) {
			if ( dx > 0 ) {
    			pos.rx()	-= 2 * deltaPosOfNextItem;
    			pos	+= deltaOfGang;
    		}else if ( dx < 0 ) {
    			pos.rx()	+= 2 * deltaPosOfNextItem;
    			pos	+= deltaOfGang;
    		}
    		if ( dy > 0 ) {
    			pos.ry()	-= 2 * deltaPosOfNextItem;
    			pos	+= deltaOfGang;
    		}else if ( dy < 0 ) {
    			pos.ry()	+= 2 * deltaPosOfNextItem;
    			pos	+= deltaOfGang;
    		}
    		zvalue	+= 3;
		}
		if ( 0 == graphicsItem ) {
			QString resPath	= cardResPath( MAHJONG_CARD(item->value()), pose, view );
			//djDebug()<<"resPath"<<resPath;
			QSvgRenderer *svgRenderer	= cardRenderer( resPath );
			imageItem	= new DJGraphicsSvgItem;
			imageItem->setSharedRenderer( svgRenderer );
			desktop()->scene()->addItem( imageItem );		
			item->setGraphicsItem( imageItem );
			imageItem->setVirtualPos( pos );
			
			imageItem->setAlignment( align );
			imageItem->setExternalScale( desktop()->graphicsScale() );
			imageItem->setInternalScale( scale );
		}else {
			imageItem	= (DJGraphicsSvgItem*)graphicsItem;			
			if ( repaintPos ) {
				imageItem->setVirtualPos( pos );
				imageItem->setAlignment( align );
			}
			if ( repaintSize ) {
				imageItem->setExternalScale( desktop()->graphicsScale() );
				imageItem->setInternalScale( scale );
			}
		}
		imageItem->setZValue( zvalue );
		imageItem->setDJSelected( false );
		if ( 1 == view ) {
			imageItem->setDeltaPos( QPointF(0,-MAHJONG_SELECT_DELTAHEIGHT) );	
			imageItem->setPen( pen );
		}
		imageItem->adjustPos( desktop()->graphicsMatrix() );
		imageItem->show();
		
		if ( Lying == pose && 4 == size && i == size -1 )
			break;
		if ( dx > 0 ) {
    		x	+= deltaPosOfNextItem;
    	}else if ( dx < 0 ) {
    		x	-= deltaPosOfNextItem;
    	}
    	if ( dy > 0 ) {
    		y	+= deltaPosOfNextItem;
    	}else if ( dy < 0 ) {
    		y	-= deltaPosOfNextItem;
    	}
    	if ( dz > 0 ) {
    		z++;
    	}else if ( dz < 0 ) {
    		z--;
    	}
	}
}
void DJDesktopMahjongController::repaintSeatPool( int seat, bool repaintSize, bool repaintPos )
{
	djDebug()<<"DJDesktopMahjongController::repaintSeatPool";
	
	DesktopItems&	poolItems	= desktopItems( seat, MAHJONG_TYPE_POOL );
	
	quint8 view = seat2View(seat);
	qreal scale	= scaleOfViewPool( view );
		
	int x;
	int y;
	qreal z	= 800;
	Qt::Alignment align	= 0;
	int eachItemWidth	= sizeOfViewLyingHand(view).width();
	int distance	= distanceOfViewLyingHand(view);
	int deltaOfLine	= deltaOfViewPoolNewLine(view);
	switch( view ) {
		case 1 : {
			x	= poolSpace().left();
			y	= poolSpace().bottom();
			align	= Qt::AlignBottom;
			repaintViewPool( view, poolItems, scale,
							repaintSize, repaintPos,
							x, y, z, 1, 0, -1,
							eachItemWidth, distance, align,
							deltaOfLine, centerPoint().y() );
			break;
		}
		case 2: {
			x	= poolSpace().left();
			y	= poolSpace().top();
			repaintViewPool( view, poolItems, scale,
							repaintSize, repaintPos,
							x, y, z, 0, 1, 1,
							eachItemWidth, distance, align,
							deltaOfLine, centerPoint().x() );
			break;
		}
		case 3: {
			x	= poolSpace().right();
			y	= poolSpace().top();
			align	= Qt::AlignRight;
			repaintViewPool( view, poolItems, scale,
							repaintSize, repaintPos,
							x, y, z, -1, 0, 1,
							eachItemWidth, distance, align,
							deltaOfLine, centerPoint().x() );
			break;
		}
		case 4: {
			x	= poolSpace().right();
			y	= poolSpace().bottom();
			align	= Qt::AlignRight | Qt::AlignBottom;
			repaintViewPool( view, poolItems, scale,
							repaintSize, repaintPos,
							x, y, z, 0, -1, -1,
							eachItemWidth, distance, align,
							deltaOfLine, centerPoint().y() );
			break;
		}
		default:
			break;
	}
}
void DJDesktopMahjongController::repaintViewPool(
				int view, DesktopItems &items, 
				qreal scale, bool repaintSize, bool repaintPos,
				int startx, int starty, qreal z,
				int dx, int dy, int dz,
				int eachItemWidth, int distance, Qt::Alignment align,
				int deltaOfLine, int border )
{
	djDebug()<<"repaintViewPool"<<"view"<<view<<"items"<<items.size();
	
	int lengthOfNextItem	= static_cast<int>( eachItemWidth * scale );
	int deltaPosOfNextItem	= static_cast<int>( (eachItemWidth + distance) * scale );
	int deltaOfNewLine	= static_cast<int>( deltaOfLine * scale );
	
	int x	= startx;
	int y	= starty;
	
	foreach( DJDesktopItem *item, items ) {
		QGraphicsItem *graphicsItem	= item->graphicsItem();
		DJGraphicsSvgItem *imageItem	= 0;		
		if ( 0 == graphicsItem ) {
			QString resPath	= cardResPath( MAHJONG_CARD(item->value()), Lying, view );
			//djDebug()<<"resPath"<<resPath;
			QSvgRenderer *svgRenderer	= cardRenderer( resPath );
			imageItem	= new DJGraphicsSvgItem;
			imageItem->setSharedRenderer( svgRenderer );
			desktop()->scene()->addItem( imageItem );
			item->setGraphicsItem( imageItem );
			imageItem->setVirtualPos( QPointF(x,y) );
			imageItem->setAlignment( align );
			imageItem->setExternalScale( desktop()->graphicsScale() );
			imageItem->setInternalScale( scale );
		}else {
			imageItem	= (DJGraphicsSvgItem*)graphicsItem;			
			if ( repaintPos ) {
				imageItem->setVirtualPos( QPointF(x,y) );
				imageItem->setAlignment( align );
			}
			if ( repaintSize ) {
				imageItem->setExternalScale( desktop()->graphicsScale() );
				imageItem->setInternalScale( scale );
			}
		}
		imageItem->setZValue(z);
		imageItem->setDJSelected( false );
		imageItem->adjustPos( desktop()->graphicsMatrix() );
		imageItem->show();
		
		if ( dx > 0 ) {
			if ( x + deltaPosOfNextItem + lengthOfNextItem > border ) {
				x	= startx;
				y	+= deltaOfNewLine;
			}else {
				x	+= deltaPosOfNextItem;
			}
		}else if ( dx < 0 ) {
			if ( x - deltaPosOfNextItem - lengthOfNextItem < border ) {
				x	= startx;
				y	+= deltaOfNewLine;
			}else {
				x	-= deltaPosOfNextItem;
			}
		}
		if ( dy > 0 ) {
			if ( y + deltaPosOfNextItem + lengthOfNextItem > border ) {
				x	+= deltaOfNewLine;
				y	= starty;
			}else {
				y	+= deltaPosOfNextItem;
			}
		}else if ( dy < 0 ) {
			if ( y - deltaPosOfNextItem - lengthOfNextItem < border ) {
				x	+= deltaOfNewLine;
				y	= starty;
			}else {
				y	-= deltaPosOfNextItem;
			}
		}
		if ( dz > 0 ) {
			z++;
		}else if ( dz < 0 ) {
			z--;
		}
	}
}
void DJDesktopMahjongController::repaintSeatFlower( int seat, bool repaintSize, bool repaintPos )
{
	djDebug()<<"DJDesktopMahjongController::repaintSeatFlower";
	
	DesktopItems&	flowerItems	= desktopItems( seat, MAHJONG_TYPE_FLOWER );
	
	quint8 view = seat2View(seat);
	qreal scale	= scaleOfViewFlower( view );
		
	int x;
	int y;
	qreal z	= 800;
	Qt::Alignment align	= 0;
	int eachItemWidth	= sizeOfViewLyingHand(view).width();
	int distance	= distanceOfViewLyingHand(view);
	int deltaOfLine	= deltaOfViewPoolNewLine(view);
	switch( view ) {
		case 1 : {
			x	= 0;
			y	= m_selfEdge;
			align	= Qt::AlignBottom;
			repaintViewPool( view, flowerItems, scale,
							repaintSize, repaintPos,
							x, y, z, 1, 0, -1,
							eachItemWidth, distance, align,
							deltaOfLine, availableSpace().right() );
			break;
		}
		case 2: {
			x	= 0;
			y	= rectOfViewShow(2).top();
			align	= Qt::AlignBottom;
			repaintViewPool( view, flowerItems, scale,
							repaintSize, repaintPos,
							x, y, z, 0, -1, -1,
							eachItemWidth, distance, align,
							deltaOfLine, availableSpace().top() );
			break;
		}
		case 3: {
			x	= rectOfViewShow(3).right();
			y	= 0;
			repaintViewPool( view, flowerItems, scale,
							repaintSize, repaintPos,
							x, y, z, 1, 0, 1,
							eachItemWidth, distance, align,
							deltaOfLine, availableSpace().right() );
			break;
		}
		case 4: {
			x	= desktop()->realWidth();
			y	= rectOfViewShow(4).top();
			align	= Qt::AlignRight | Qt::AlignBottom;
			repaintViewPool( view, flowerItems, scale,
							repaintSize, repaintPos,
							x, y, z, 0, -1, -1,
							eachItemWidth, distance, align,
							deltaOfLine, availableSpace().top() );
			break;
		}
		default:
			break;
	}
}
void DJDesktopMahjongController::repaintChu( bool show )
{
	if ( !show || 0 == m_lastChuSeat || 0 == m_lastChuCard ) {
		m_chuItem->hide();
		return;
	}
	quint8 view = seat2View( m_lastChuSeat );
	qreal scale	= scaleOfViewChu( view );
	
	QString resPath	= cardResPath( m_lastChuCard, Lying, view );
	QSvgRenderer *svgRenderer	= cardRenderer( resPath );
	m_chuItem->setSharedRenderer( svgRenderer );
	
	int x	= 0;
	int	y	= 0;
	qreal	z	= 1000;
	
	Qt::Alignment align	= 0;
	switch( view ) {
	case 1:
		x	= centerPoint().x();
		y	= poolSpace().bottom() + MAHJONG_SELECT_DELTAHEIGHT;
		align	= Qt::AlignHCenter | Qt::AlignBottom;
		break;
	case 2:
		x	= poolSpace().left();
		y	= centerPoint().y();
		align	= Qt::AlignVCenter;
		break;
	case 3:
		x	= centerPoint().x();
		y	= poolSpace().top();
		align	= Qt::AlignHCenter;
		break;
	case 4:
		x	= poolSpace().right();
		y	= centerPoint().y();
		align	= Qt::AlignVCenter | Qt::AlignRight;
		break;
	default:
		return;
	}
	
	m_chuItem->setVirtualPos( QPointF(x,y) );
	m_chuItem->setAlignment( align );
	m_chuItem->setExternalScale( desktop()->graphicsScale() );
	m_chuItem->setInternalScale( scale );
	m_chuItem->setZValue(z);
	m_chuItem->adjustPos( desktop()->graphicsMatrix() );
	m_chuItem->show();
}
void DJDesktopMahjongController::repaintHu( int seatOfHu, const unsigned char *buf, quint8 len )
{
	djDebug()<<"DJDesktopMahjongController::repaintHu";
	
	QFont font;
	font.setBold( true );
	font.setPointSize( 20 );
		
	int x	= centerPoint().x();
	int	y	= availableSpace().top();
	djDebug()<<"topY"<<y;
	Qt::Alignment align	= Qt::AlignHCenter;
		
	DJGameUser *playerOfHu	= panelController()->player(seatOfHu);
	DJGameUser *playerOfDian	= panelController()->player(buf[0]);
	if ( playerOfHu && playerOfDian ) {	
		QString text;
		if ( playerOfHu == playerOfDian ) {
			text	+= playerOfHu->userName();
			text	+= " ";
			text	+= tr("zi mo");	
			text	+= " ";
			text	+= tr("hu pai");
		}else {
			text	+= playerOfHu->userName();
			text	+= " ";
			text	+= tr("hu pai");
			text	+= ", ";
			text	+= playerOfDian->userName();
			text	+= " ";
			text	+= tr("dian pao");
		}
		m_huTitle	= new DJGraphicsTextItem( text, 0, desktop()->scene() );
		m_huTitle->setFont( font );
		m_huTitle->setVirtualPos( QPointF(x,y) );
		m_huTitle->setAlignment( align );
		m_huTitle->setExternalScale( desktop()->graphicsScale() );
		m_huTitle->adjustPos( desktop()->graphicsMatrix() );
		m_huTitle->show();
		
		y	+= m_huTitle->boundingRect().size().toSize().height();
	}
	djDebug()<<"imageY"<<y;
	DesktopItems&	huItems	= desktopItems( seatOfHu, MAHJONG_TYPE_HU );
	qreal scale	= 0.75;
	int eachItemWidth	= sizeOfViewStandingKnownHand(1).width();
	int distance	= distanceOfViewStandingKnownHand(1);
	int deltaPosOfNextItem	= static_cast<int>( (eachItemWidth + distance) * scale );
	MJCard	lastCard	= MAHJONG_CARD( buf[1] );
	quint8 numberOfCards	= buf[2];
	
	QPen pen( Qt::red );
	pen.setWidth( 2 );
	
	int lengthOfHuCards	= static_cast<int>( ((eachItemWidth + distance)* numberOfCards - distance ) * scale );
	x	= centerPoint().x() - (lengthOfHuCards >> 1);
	qreal	z	= 100;
	align	= 0;
	bool foundLastCard	= false;
	
	foreach( DJDesktopItem *item, huItems ) {
		QGraphicsItem *graphicsItem	= item->graphicsItem();
		DJGraphicsSvgItem *imageItem	= 0;
		
		if ( 0 == graphicsItem ) {
			QString resPath	= cardResPath( MAHJONG_CARD(item->value()), Standing, 1 );
			//djDebug()<<"resPath"<<resPath;
			QSvgRenderer *svgRenderer	= cardRenderer( resPath );
			imageItem	= new DJGraphicsSvgItem;
			imageItem->setSharedRenderer( svgRenderer );
			desktop()->scene()->addItem( imageItem );		
			item->setGraphicsItem( imageItem );
			imageItem->setVirtualPos( QPointF(x,y) );
			
			imageItem->setAlignment( align );
			imageItem->setExternalScale( desktop()->graphicsScale() );
			imageItem->setInternalScale( scale );
		}else {
			imageItem	= (DJGraphicsSvgItem*)graphicsItem;			
		}
		imageItem->setDJSelected( false );
		if ( !foundLastCard && MAHJONG_CARD(item->value()) == lastCard ) {
			foundLastCard	= true;
			imageItem->setPen( pen );
			imageItem->setDJSelected( true );
		}
		imageItem->adjustPos( desktop()->graphicsMatrix() );
		imageItem->show();
		
		x	+= deltaPosOfNextItem;
    	z++;
	}
	
	//x	= centerPoint().x();
	y	+=	static_cast<int>( sizeOfViewStandingKnownHand(1).height() * scale );
	djDebug()<<"fanY"<<y;
	quint8 indexOfFan = buf[2]+3;
	int totalFan	= 0;
	while( buf[indexOfFan] != 0 && indexOfFan < len ) {
		QString fanName	= tr( gFanNames[ buf[indexOfFan] ] );
		DJGraphicsTextItem *fanNameItem	= new DJGraphicsTextItem( fanName, 0, desktop()->scene() );
		m_huFanNames << fanNameItem;
		
		x	= centerPoint().x() - 10;
		align	= Qt::AlignRight;
		fanNameItem->setFont( font );
		fanNameItem->setVirtualPos( QPointF(x,y) );
		fanNameItem->setAlignment( align );
		fanNameItem->setExternalScale( desktop()->graphicsScale() );
		fanNameItem->adjustPos( desktop()->graphicsMatrix() );
		fanNameItem->show();
		
		int	fan	= buf[indexOfFan+1];
		totalFan	+= fan;
		QString fanValue= QString("%1").arg(fan)+ tr("fan");
		DJGraphicsTextItem *fanValueItem	= new DJGraphicsTextItem( fanValue, 0, desktop()->scene() );
		m_huFanValues << fanValueItem;
		
		x	= centerPoint().x() + 10;
		align	= 0;
		fanValueItem->setFont( font );
		fanValueItem->setVirtualPos( QPointF(x,y) );
		fanValueItem->setAlignment( align );
		fanValueItem->setExternalScale( desktop()->graphicsScale() );
		fanValueItem->adjustPos( desktop()->graphicsMatrix() );
		fanValueItem->show();
		
		y	+= fanValueItem->boundingRect().size().toSize().height();
		indexOfFan	+= 2;
	}
	//total
	DJGraphicsTextItem *fanNameItem	= new DJGraphicsTextItem( tr("Total"), 0, desktop()->scene() );
	m_huFanNames << fanNameItem;
	x	= centerPoint().x() - 10;
	align	= Qt::AlignRight;
	fanNameItem->setFont( font );
	fanNameItem->setVirtualPos( QPointF(x,y) );
	fanNameItem->setAlignment( align );
	fanNameItem->setExternalScale( desktop()->graphicsScale() );
	fanNameItem->adjustPos( desktop()->graphicsMatrix() );
	fanNameItem->show();
	
	QString fanValue= QString("%1").arg(totalFan)+ tr("fan");
	DJGraphicsTextItem *fanValueItem	= new DJGraphicsTextItem( fanValue, 0, desktop()->scene() );
	m_huFanValues << fanValueItem;
		
	x	= centerPoint().x() + 10;
	align	= 0;
	fanValueItem->setFont( font );
	fanValueItem->setVirtualPos( QPointF(x,y) );
	fanValueItem->setAlignment( align );
	fanValueItem->setExternalScale( desktop()->graphicsScale() );
	fanValueItem->adjustPos( desktop()->graphicsMatrix() );
	fanValueItem->show();
}
void DJDesktopMahjongController::clearHuText()
{
	delete m_huTitle;
	m_huTitle	= 0;
	
	qDeleteAll( m_huFanNames );
	m_huFanNames.clear();
	
	qDeleteAll( m_huFanValues );
	m_huFanValues.clear();
}
void DJDesktopMahjongController::throwSelectedCard()
{
	djDebug()<<"DJDesktopMahjongController::throwSelectedCard";
	DesktopItems selectedItems	= selectedDesktopItems( panelController()->seatId(), MAHJONG_TYPE_STANDING_HAND );
	DJDesktopItem *item	= 0;
	if ( selectedItems.isEmpty() ) {
		DesktopItems items	= desktopItems( panelController()->seatId(), MAHJONG_TYPE_STANDING_HAND );
		item	= items.last();
	}else {
		item	= selectedItems.first();
	}
	quint8 chuID	= gameTraceIDOfChu();
	QByteArray data;
	data.append( item->value() );
	sendGameTrace( chuID, data );
}
void DJDesktopMahjongController::handleMousePressed( QGraphicsSceneMouseEvent *mouseEvent )
{
	djDebug()<<"GBMJDesktopController::handleMousePressed"<<mouseEvent->button()<<mouseEvent->buttons();
	
	//if( panelController()->isLookingOn() )
		//return;
	 
	if ( Qt::LeftButton == mouseEvent->button() ) {
		QPointF scenePos	= mouseEvent->scenePos();
		QGraphicsItem *clickedItem	= desktop()->desktopScene()->itemAt( scenePos );
		
		DesktopItems& items	= desktopItems( panelController()->seatId(), MAHJONG_TYPE_STANDING_HAND );
		foreach( DJDesktopItem *item, items ) {
			DJGraphicsSvgItem *svgItem	= (DJGraphicsSvgItem*)item->graphicsItem();
			if ( svgItem && clickedItem ) {
				if ( svgItem != (DJGraphicsSvgItem*)clickedItem ) {
					svgItem->setDJSelected( false );
				}else {
					svgItem->setDJSelected( !svgItem->isDJSelected() );
				}
			}
			svgItem->adjustPos( desktop()->graphicsMatrix() );
		}
	}
}
void DJDesktopMahjongController::handleMouseDoubleClicked( QGraphicsSceneMouseEvent *mouseEvent )
{
	djDebug()<<"DJDesktopMahjongController::handleMouseDoubleClicked"<<mouseEvent->button()<<mouseEvent->buttons();
	
	if ( Qt::LeftButton == mouseEvent->button() ) {
		QPointF scenePos	= mouseEvent->scenePos();
		QGraphicsItem *clickedItem	= desktop()->desktopScene()->itemAt( scenePos );
		
		DesktopItems& items	= desktopItems( panelController()->seatId(), MAHJONG_TYPE_STANDING_HAND );
		foreach( DJDesktopItem *item, items ) {
			DJGraphicsSvgItem *svgItem	= (DJGraphicsSvgItem*)item->graphicsItem();
			if ( svgItem && clickedItem ) {
				if ( svgItem != (DJGraphicsSvgItem*)clickedItem ) {
					svgItem->setDJSelected( false );
				}else {
					svgItem->setDJSelected( true );
				}
			}
			svgItem->adjustPos( desktop()->graphicsMatrix() );
		}
		try2Chu();
	}
}
void DJDesktopMahjongController::handleMouseReleased( QGraphicsSceneMouseEvent *mouseEvent )
{
	djDebug()<<"GBMJDesktopController::handleMouseReleased"<<mouseEvent->button()<<mouseEvent->buttons();
	
	if( Qt::RightButton == mouseEvent->button() ) {
		//right button released
		if ( !try2Chu() )
			try2Qi(); 
    }
}
QSvgRenderer* DJDesktopMahjongController::cardRenderer( const QString& resPath )
{
	return m_mahjongSvgCache.value( resPath );
}
int DJDesktopMahjongController::doMJSelect( const MJCardsGroups& groups )
{
	UIMJSelect mjSelect(this,groups);
	mjSelect.exec();
	mjSelect.adjustSize();
	return mjSelect.indexOfSelected();
}
quint8 DJDesktopMahjongController::gameTraceIDOfChiPeng() const
{
	return 0x05;
}
quint8 DJDesktopMahjongController::gameTraceIDOfChu() const
{
	return 0x04;
}
quint8 DJDesktopMahjongController::gameTraceIDOfGangFlower() const
{
	return 0x06;
}
quint8 DJDesktopMahjongController::gameTraceIDOfHu() const
{
	return 0x09;
}
quint8 DJDesktopMahjongController::gameTraceIDOfTing() const
{
	return 0x0a;
}
quint8 DJDesktopMahjongController::gameWaitIDOfChiPeng() const
{
	return 0x05;
}
quint8 DJDesktopMahjongController::gameWaitIDOfChu() const
{
	return 0x06;
}
/*
bool DJDesktopMahjongController::isChuEnabled() const
{
	return m_chu->isEnabled();
}
bool DJDesktopMahjongController::isQiEnabled() const
{
	return m_qi->isEnabled();
}
*/
bool DJDesktopMahjongController::try2Chu()
{
	if ( m_chu->isEnabled() ) {
		clickChu();
		return true;
	}
	return false;
}
bool DJDesktopMahjongController::try2Qi()
{
	if ( m_qi->isEnabled() ) {
		clickQi();
		return true;
	}
	return false;
}
void DJDesktopMahjongController::clickQi()
{
	m_qi->setEnabled( false );
	
	sendGameTrace( gameTraceIDOfChiPeng() );
}
void DJDesktopMahjongController::clickChu()
{
	m_chu->setEnabled( false );
	
	throwSelectedCard();
}
void DJDesktopMahjongController::clickChi()
{
	m_chi->setEnabled( false );
	
	MJCardsGroups chiGroups	= testChi();
	if ( chiGroups.empty() )
		return;
	QByteArray buf;
	if ( 1 == chiGroups.size() ) {
		const MJCards& cards	= chiGroups[0];
		buf.append(cards[0]);
		buf.append(cards[1]);
	}else {
		int index	= doMJSelect( chiGroups );
		const MJCards& cards = chiGroups[index];
		if ( cards.empty() )
			return;
		buf.append(cards[0]);
		buf.append(cards[1]);
	}
	sendGameTrace( gameTraceIDOfChiPeng(), buf );
}
void DJDesktopMahjongController::clickPeng()
{
	m_peng->setEnabled( false );
	
	MJCards pengCards	= testPeng();
	if ( pengCards.empty() ) 
		return;
	QByteArray buf;
	buf.append(pengCards[0]);
	buf.append(pengCards[1]);
	sendGameTrace( gameTraceIDOfChiPeng(), buf );
}
void DJDesktopMahjongController::clickGang()
{
	m_gang->setEnabled( false );
	
	if ( gameWaitStatus() == gameWaitIDOfChiPeng() ) {
		MJCards dianGangCards	= testDianGang();
		if ( dianGangCards.empty() )
			return;
		QByteArray buf;
		buf.append(dianGangCards[0]);
		sendGameTrace( gameTraceIDOfGangFlower(), buf );
	}else if ( gameWaitStatus() == gameWaitIDOfChu() ) {
		MJCardsGroups zimoGangGroups	= testZimoGang();
		if ( zimoGangGroups.empty() )
			return;
		QByteArray buf;
		if ( 1 == zimoGangGroups.size() ) {
			const MJCards& cards	= zimoGangGroups[0];
			buf.append(cards[0]);
		}else {
			int index	= doMJSelect( zimoGangGroups );
			const MJCards& cards = zimoGangGroups[index];
			if ( cards.empty() )
				return;
			buf.append(cards[0]);
		}
		sendGameTrace( gameTraceIDOfGangFlower(), buf );
	}
}
void DJDesktopMahjongController::clickTing()
{
	m_ting->setEnabled( false );
}
void DJDesktopMahjongController::clickHu()
{
	m_hu->setEnabled( false );
	
	sendGameTrace( gameTraceIDOfHu() );
}
