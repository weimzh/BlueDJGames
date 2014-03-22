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
#include "landlord.h"
#include "ddzrule.h"
#include "ddzprofile.h"
#include "DJMatrixCanvasImage.h"
#include "DJMatrixCanvasText.h"
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

LandlordDesktop::LandlordDesktop(QWidget* parent,DJGamePanel *panel)
			: DJGamePokerDesktop(parent,panel)
{
	c_LandlordPanel = (LandlordPanel *)panel;
	
	SetCardPictureSize(70,popupSize(),pageLayoutSizeX(),pageLayoutSizeY());
	
	connect(this,SIGNAL(gameStarted()),this,SLOT(PlayerStarted()));
	PDoudzhuRoom pddzroom = (PDoudzhuRoom)((c_LandlordPanel->gameRoom())->privateRoom());
	
	c_chArrangeStyle = 0;
#ifdef DJ_OEM	
	QIcon background;
	background.addPixmap( QPixmap(":/BaseRes/image/desktop/toolbutton/bg.png"), QIcon::Normal, QIcon::Off );
	background.addPixmap( QPixmap(":/BaseRes/image/desktop/toolbutton/bgpressed.png"), QIcon::Normal, QIcon::On );
	
	m_arrange = new DJToolButton(this);
	m_arrange->setToolTip(tr("arrange cards"));
	m_arrange->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/arrange.png") );
	m_arrange->setIconSize( djToolButtonSize );
	m_arrange->setBackgroundIcon( background );
    connect(m_arrange,SIGNAL(clicked()),this,SLOT(ClickArrange()));
    
    m_previous = new DJToolButton(this);
	m_previous->setToolTip(tr("look last round"));
	m_previous->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/left.png") );
	m_previous->setIconSize( djToolButtonSize );
	m_previous->setBackgroundIcon( background );
    connect(m_previous,SIGNAL(pressed()),this,SLOT(PreviousPressed()));
    connect(m_previous,SIGNAL(released()),this,SLOT(PreviousRelesed()));  	
    
    QSize iconSize( 19 * 2, 19 );
    
	m_tip = new QPushButton(this);
	m_tip->setToolTip(tr("tip"));
	m_tip->setIcon( QIcon(":/BaseRes/image/desktop/pushbutton/TiShi.png") );
	m_tip->setIconSize( iconSize );
    connect(m_tip,SIGNAL(clicked()),this,SLOT(ClickTip()));
    m_tip->adjustSize();
    m_tip->hide();
    
	m_throw = new QPushButton(this);
	m_throw->setToolTip(tr("thrown card"));
	m_throw->setIcon( QIcon(":/BaseRes/image/desktop/pushbutton/ChuPai.png") );
	m_throw->setIconSize( iconSize );
    connect(m_throw,SIGNAL(clicked()),this,SLOT(ClickThrow()));
    m_throw->adjustSize();
	m_throw->hide();
	
	m_pass = new QPushButton(this);
	m_pass->setToolTip(tr("pass"));
	m_pass->setIcon( QIcon(":/BaseRes/image/desktop/pushbutton/GuoPai.png") );
	m_pass->setIconSize( iconSize );
    connect(m_pass,SIGNAL(clicked()),this,SLOT(ClickPass()));
    m_pass->adjustSize();
    m_pass->hide();
#else    
	m_toolbar	= new QWidget(this);
    QHBoxLayout *layout	= new QHBoxLayout( m_toolbar );
    layout->setMargin( 0 );
    layout->setSpacing( 0 );
	
	m_arrange = new QToolButton;
	m_arrange->setToolTip(tr("arrange cards"));
	m_arrange->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/arrange.png") );
	m_arrange->setIconSize( djToolButtonSize );
    connect(m_arrange,SIGNAL(clicked()),this,SLOT(ClickArrange()));
    layout->addWidget( m_arrange );
    
	m_tip = new QToolButton;
	m_tip->setToolTip(tr("tip"));
	m_tip->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/tip.png") );
	m_tip->setIconSize( djToolButtonSize );
    connect(m_tip,SIGNAL(clicked()),this,SLOT(ClickTip()));
    layout->addWidget( m_tip );
    
	m_throw = new QToolButton;
	m_throw->setToolTip(tr("thrown card"));
	m_throw->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/chu.png") );
	m_throw->setIconSize( djToolButtonSize );
    connect(m_throw,SIGNAL(clicked()),this,SLOT(ClickThrow()));
    layout->addWidget( m_throw );
	
	m_pass = new QToolButton;
	m_pass->setToolTip(tr("pass"));
	m_pass->setIcon( QIcon(":/BaseRes/image/desktop/toolbutton/qi.png") );
	m_pass->setIconSize( djToolButtonSize );
    connect(m_pass,SIGNAL(clicked()),this,SLOT(ClickPass()));
    layout->addWidget( m_pass );
#endif	
	
	memset(llcallb,0,(DOUDZHU_MAX_CALLLEVEL+1)*sizeof(LandlordCallButton *));
	int i=0;
	while(pddzroom->rule.chCall[i] != 0)
	{
		llcallb[i] = new LandlordCallButton(this,pddzroom->rule.chCall[i]);
		llcallb[i]->hide();
		i++;
	}
	llcallb[i] = new LandlordCallButton(this,0);
	llcallb[i]->hide();
	
	SetOrder(false,false,false);
	QMatrix m = GetCurrentMatrix();
	
	QString str;
	str	= tr("Current Bombs : ");
	c_CurrentBombsText = new DJGameTextItem(str,canvas(),m,DJGAME_LANDLORD_TID_DESCRIBE,0xFF,DJGAME_LANDLORD_TYPE_DESCRIBE,false);
	QFont f = c_CurrentBombsText->font();
	f.setPointSize(14);
	f.setBold(true);
	c_CurrentBombsText->setFont(f);
//	c_CurrentBombsText->setColor(QColor(0,0xFF,0));
	c_CurrentBombsText->setHAlignment(Qt::AlignRight);
	c_CurrentBombsText->move(120,55);
	c_CurrentBombsText->show();
	c_CurrentBombsText->setMatrix(m);
	
	str = tr("Current Score : ");
	c_CurrentScoreText = new DJGameTextItem(str,canvas(),m,DJGAME_LANDLORD_TID_DESCRIBE,0xFF,DJGAME_LANDLORD_TYPE_DESCRIBE,false);
	c_CurrentScoreText->setFont(f);
//	c_CurrentScoreText->setColor(QColor(0,0xFF,0));
	c_CurrentScoreText->setHAlignment(Qt::AlignRight);
	c_CurrentScoreText->move(120,85);
	c_CurrentScoreText->show();
	c_CurrentScoreText->setMatrix(m);
	
	str = QString("0");
	c_CurrentBombs = new DJGameTextItem(str,canvas(),m,DJGAME_LANDLORD_TID_DESCRIBE,0xFF,DJGAME_LANDLORD_TYPE_DESCRIBE,false);
	c_CurrentBombs->setFont(f);
//	c_CurrentBombsText->setColor(QColor(0,0xFF,0));
	c_CurrentBombs->move(120,55);
	c_CurrentBombs->show();
	c_CurrentBombs->setMatrix(m);
	
	c_CurrentScore = new DJGameTextItem(str,canvas(),m,DJGAME_LANDLORD_TID_DESCRIBE,0xFF,DJGAME_LANDLORD_TYPE_DESCRIBE,false);
	c_CurrentScore->setFont(f);
//	c_CurrentBombsText->setColor(QColor(0,0xFF,0));
	c_CurrentScore->move(120,85);
	c_CurrentScore->show();
	c_CurrentScore->setMatrix(m);
	
	setRemainderEnabled( true );
	setHostEnabled( true );
	DJMatrixCanvasImage* host	= hostItem();
	host->setOriginalPixmap( QPixmap(":/LandLordRes/image/base/game_32.png") );
	
	setPokerSortMode( ASCountPointSuit );
	setRankOfPoints( Num2PointRank );
	
	StaticInitDesktop();
}

void LandlordDesktop::DisplayBombs()
{
	QString str;
	str	= QString("%1").arg(c_Bombs);
	c_CurrentBombs->setText(str);
}

void LandlordDesktop::DisplayScore()
{
	QString str;
	str	= QString("%1").arg(c_Score);
	c_CurrentScore->setText(str);
}
void LandlordDesktop::ClickThrow()
{
	DJGamePokerDesktop::handleItemClicked(Qt::RightButton,QPoint(),0);
}
void LandlordDesktop::ClickPass()
{	
	if(GetTableStatus() == DJGAME_DOUDZHU_TABLE_STATUS_WAIT_THROW && IsWaittingForMe())
		SendGameTrace(DOUDZHU_GAMETRACE_THROW);
}
DJPoker LandlordDesktop::higherSerialPoker( const DJPoker& poker, bool isStrict ) const
{
	if ( DJPoker::Num2 == poker.point() )
		return DJPoker();
	else
		return DJGamePokerDesktop::higherSerialPoker(poker,isStrict);
		
}
DJPoker LandlordDesktop::lowerSerialPoker( const DJPoker& poker, bool isStrict ) const
{
	if ( DJPoker::Num3 == poker.point() )
		return DJPoker();
	else
		return DJGamePokerDesktop::lowerSerialPoker(poker,isStrict);
}
DJPoker LandlordDesktop::greaterPoker( const DJPoker& poker ) const
{
	if ( !poker.isValid() )
		return DJPoker( DJPoker::AnySuit, DJPoker::Num3 );
	if ( DJPoker::Num2 == poker.point() )
		return DJPoker( DJPoker::Joker, DJPoker::SmallJoker );
	else if ( DJPoker::SmallJoker == poker.point() ) {
		return DJPoker( DJPoker::Joker, DJPoker::BigJoker );
	}else if ( DJPoker::BigJoker == poker.point() ) {
		return DJPoker();
	}else if ( DJPoker::Ace == poker.point() ) {
		return DJPoker( DJPoker::AnySuit, DJPoker::Num2 );
	}else
		return higherSerialPoker( poker, false );
}
QList<DJPoker> LandlordDesktop::findMajor( const QHash<DJPoker,int>& countPokers,
									const DJPoker& otherKeyPoker, 
									int width, int length, 
									QSet<DJPoker>& excluder ) const
{
	djDebug()<<"getMajor"<<otherKeyPoker.description()<<width<<length;
	QList<DJPoker> pokers;
	DJPoker keyPoker	= otherKeyPoker;
	bool foundTip	= false;
	while( !foundTip ) {
		keyPoker	= greaterPoker( keyPoker );
		djDebug()<<"exact keyPoker"<<keyPoker.description();
		if ( !keyPoker.isValid() )
			break;
		if ( excluder.contains(keyPoker) )
			continue;
		QList<DJPoker> shun	= findShun( countPokers, keyPoker,
													width, length,
													true, false, true );
		if ( !shun.isEmpty() ) {
			djDebug()<<"found shun";
			excluder.insert( keyPoker );
			foundTip	= true;
			pokers << shun;
		}
	}
	if ( foundTip )
		return pokers;
	
	keyPoker	= otherKeyPoker;
	while( !foundTip ) {
		keyPoker	= greaterPoker( keyPoker );
		djDebug()<<"nonexact keyPoker"<<keyPoker.description();
		if ( !keyPoker.isValid() )
			break;
		if ( excluder.contains(keyPoker) )
			continue;
		QList<DJPoker> shun	= findShun( countPokers, keyPoker,
													width, length,
													true, false, false );
		if ( !shun.isEmpty() ) {
			djDebug()<<"found shun";
			excluder.insert( keyPoker );
			foundTip	= true;
			pokers << shun;
		}
	}
	if ( foundTip )
		return pokers;
	else
		return QList<DJPoker>();
}
QList<DJPoker> LandlordDesktop::findMinor( const QHash<DJPoker,int>& countPokers, int width, 
										int length, QSet<DJPoker>& excluder ) const
{
	QList<DJPoker> pokers;
	DJPoker keyPoker	= DJPoker();
	bool foundTip	= true;
	djDebug()<<"find exactly equal width"<<length;
	while( length ) {
		keyPoker	= greaterPoker( keyPoker );
		djDebug()<<"keyPoker"<<keyPoker.description();
		if ( !keyPoker.isValid() ) {
			break;
		}
		if ( excluder.contains(keyPoker) )
			continue;
		QList<DJPoker> group	= findGroup( countPokers, keyPoker,
										width, false, true );
		if ( group.isEmpty() )
			continue;
		excluder.insert( keyPoker );
		pokers << group;
		length--;
	}
	
	djDebug()<<"find greater equal width"<<length;
	keyPoker	= DJPoker();
	while( length ) {
		keyPoker	= greaterPoker( keyPoker );
		djDebug()<<"keyPoker"<<keyPoker.description();
		if ( !keyPoker.isValid() ) {
			foundTip	= false;
			break;
		}
		if ( excluder.contains(keyPoker) )
			continue;
		QList<DJPoker> group	= findGroup( countPokers, keyPoker,
										width, false, false );
		if ( group.isEmpty() )
			continue;
		pokers << group;
		length--;
	}
	if ( foundTip )
		return pokers;
	else
		return QList<DJPoker>();
}
void LandlordDesktop::ClickTip()
{
	djDebug()<<"LandlordDesktop::ClickTip";

	if ( m_tipHandPokers.isEmpty() ) {
		//first tip
		djDebug()<<"begin tip";
		m_tipHandPokers	= pokersOfSeat( c_LandlordPanel->selfSeatId() );
		m_tipCountPokers	= createCountPokers( m_tipHandPokers );
		m_tipMajorExcluder.clear();
		m_tipBombExcluder.clear();
		m_tipRocket	= false;
	}
	
	quint8 *prevBuf	= c_chThrow[c_chLastThrowSeat];
	quint8 prevLen	= c_chThrowPages[c_chLastThrowSeat];
	
	//djDebug()<<"last thrown pokers";
	QList<DJPoker> lastPokers;
	for( int i = 0; i < prevLen; ++i ) {
		DJPoker poker( prevBuf[i], isBridgeCard() );
		//djDebug()<<poker.description();
		lastPokers << poker;
	}
	djDebug()<<"last thrown pokers patterns";
	QList<DJPokerPattern> patterns	= DJPokerPattern::fromPokers( this, lastPokers, false );
	foreach( DJPokerPattern pattern, patterns ) {
		djDebug()<<pattern.description();
	}

	//there are two patterns in thrown pokers at most
	//major pattern
	DJPokerPattern pattern	= patterns.takeFirst();
	int majorWidth	= pattern.width();
	int majorLength	= pattern.length();
	DJPoker	otherMajorKeyPoker	= pattern.keyPoker();
	QList<DJPoker> otherMajorPokers	= pattern.pokers();
	
	int minorWidth	= 0;
	if ( !patterns.isEmpty() ) {
    	//minor pattern
    	pattern	= patterns.takeFirst();
		minorWidth	= pattern.width();
	}
	DJPoker selfMajorKeyPoker	= otherMajorKeyPoker;
	djDebug()<<"otherMajorKeyPoker"<<otherMajorKeyPoker.description();
	while( true ) {
		
		QList<DJPoker> selfMajorPokers	= findMajor( m_tipCountPokers, otherMajorKeyPoker,
													majorWidth, majorLength,
													m_tipMajorExcluder );
		if ( selfMajorPokers.isEmpty() )
			break;
		
		QList<DJPoker> selfMinorPokers;
		
		if ( 0 != minorWidth ) {
			QSet<DJPoker> tipMinorExcluder;
    		DJPoker selfMinorKeyPoker	= selfMajorKeyPoker;
    		//minor exlucder inherits from major exluder
    		for ( int i = 0; i < majorLength; ++i ) {
    			tipMinorExcluder.insert( selfMinorKeyPoker );
    			selfMinorKeyPoker	= higherSerialPoker( selfMinorKeyPoker, false );
    		}
    		selfMinorPokers	= findMinor( m_tipCountPokers, minorWidth,
    													majorLength, tipMinorExcluder );
    		if ( selfMinorPokers.isEmpty() ) {
    			//no minor match
				//don't break,there may be matching group above this
    			continue;
    		}
		}
		selectPokerItems( selfMajorPokers + selfMinorPokers );
		return;
	}
	
	//iterate bomb
	int bombWidth;
	PDoudzhuRoom pddzroom = (PDoudzhuRoom)((c_LandlordPanel->gameRoom())->privateRoom());
	if( !ISBOMB(&(pddzroom->rule),m_lastThrowType) )
		bombWidth	= pddzroom->rule.chBombPages;
	else {
		//bombs that width == majorWidth have been iterated by last pattern
		bombWidth	= majorWidth + 1;
	}
	DJPoker bombKeyPoker	= DJPoker();
	while( true ) {
		bombKeyPoker	= greaterPoker( bombKeyPoker );
		if ( !bombKeyPoker.isValid() )
			break;
		if ( m_tipBombExcluder.contains(bombKeyPoker) )
			continue;
		m_tipBombExcluder.insert( bombKeyPoker );
		QList<DJPoker> bombPokers	= findGroup( m_tipCountPokers, bombKeyPoker,
												bombWidth, false, false );		
		if ( bombPokers.isEmpty() )
			continue;
		selectPokerItems( bombPokers );
		return;
	}
	//iterate rocket
	int deck	= pddzroom->rule.chCardPages;
	if ( !m_tipRocket
		&& deck == m_tipCountPokers.value(DJPoker(DJPoker::Joker,DJPoker::SmallJoker))
		&& deck == m_tipCountPokers.value(DJPoker(DJPoker::Joker,DJPoker::BigJoker)) ) {
		QList<DJPoker> rocketPokers;
		while( deck-- ) {
			rocketPokers << DJPoker(DJPoker::Joker,DJPoker::SmallJoker)
						<< DJPoker(DJPoker::Joker,DJPoker::BigJoker);
		}
		m_tipRocket	= true;
		selectPokerItems( rocketPokers );
		return;		
	}
	//iteration end
	//reset to start iteration again
	m_tipHandPokers.clear();
	selectPokerItems();
}
void LandlordDesktop::ClickArrange()
{
	djDebug()<<"LandlordDesktop::ClickArrange";
	
	c_chArrangeStyle ++;
	if(c_chArrangeStyle > DOUDZHU_ARRANGE_STYLE_MAX)
		c_chArrangeStyle = 0;
		
	RepaintSeatCard(c_LandlordPanel->selfSeatId());
	/*
	if ( 0 == c_chArrangeStyle )
		setPokerSortMode( PointASCountSuit );
	else
		setPokerSortMode( ASCountPointSuit );
	repaintCards( c_LandlordPanel->selfSeatId() );
	*/
}
#ifdef DJ_OEM
void LandlordDesktop::PreviousPressed()
{
	RepaintThrowCard(1,&(m_previousThrow[1][0]),m_previousThrowPages[1]);
	RepaintThrowCard(2,&(m_previousThrow[2][0]),m_previousThrowPages[2]);
	RepaintThrowCard(3,&(m_previousThrow[3][0]),m_previousThrowPages[3]);
	RepaintThrowCard(4,&(m_previousThrow[4][0]),m_previousThrowPages[4]);
}
void LandlordDesktop::PreviousRelesed()
{
	RepaintThrowCard(1,&(c_chThrow[1][0]),c_chThrowPages[1]);
	RepaintThrowCard(2,&(c_chThrow[2][0]),c_chThrowPages[2]);
	RepaintThrowCard(3,&(c_chThrow[3][0]),c_chThrowPages[3]);
	RepaintThrowCard(4,&(c_chThrow[4][0]),c_chThrowPages[4]);
}
#endif
bool LandlordDesktop::handleItemClicked( Qt::MouseButton btn, const QPoint &pos, Q3CanvasItem *item )
{
	djDebug()<<"LandlordDesktop::handleItemClicked";
	bool hasHandled	= DJGamePokerDesktop::handleItemClicked(btn,pos,item);
	if ( !hasHandled ) {
		if ( Qt::RightButton == btn && 0 == item ) {
			ClickPass();
			hasHandled	= true;
		}else
			hasHandled	= DJGameDesktop::handleItemClicked(btn,pos,item);
	}
	return hasHandled;
}
LandlordDesktop::~LandlordDesktop()
{
	djDebug()<<"LandlordDesktop destructor";
}
void LandlordDesktop::handleMatrixChanged(const QMatrix &m)
{
	DJGamePokerDesktop::handleMatrixChanged(m);
	
	int x,y,iSelfHeight;
	int tx,ty;
	GetDesktopCenterPoint(&x,&y,&iSelfHeight);
	y = GetRealHeight()-iSelfHeight;
	m.map(x,y,&tx,&ty);
	
	int i=0,w = 0;
	while(llcallb[i] != NULL)
	{
		w += (llcallb[i]->width()+10);
		i ++;
	}
	x = tx- (w>>1);
	i = 0;
	while(llcallb[i] != NULL)
	{
		llcallb[i]->move(x,ty);
		x += (llcallb[i]->width()+10);
		i ++;
	}
#ifdef DJ_OEM
	m_throw->move(tx-(m_throw->width()>>1),ty);
	m_pass->move(tx-(m_throw->width()>>1)-m_pass->width()-20,ty);
	m_tip->move(tx+(m_throw->width()>>1)+20,ty);
#endif	
	y = GetRealHeight()-iSelfHeight;
	x = GetRealWidth()-100;
	m.map(x,y,&tx,&ty);
#ifdef DJ_OEM
	m_arrange->move(width()-m_arrange->width()-20,height()-80);
	m_previous->move(width()-m_previous->width()-20,height()-40);
#else	
	m_toolbar->move(tx-m_toolbar->width(),ty+10);
#endif	
}
void LandlordDesktop::PlayerStarted()
{
	//StaticInitDesktop();
	//ClearSeatHandCard(0);
	//ClearSeatThrowCard(0);
	//ClearSeatAttachCard(0);
	//DisplayBombs();
	//DisplayScore();
}

void LandlordDesktop::RequestCall(quint8 chScore)
{
	SendGameTrace(DOUDZHU_GAMETRACE_CALL,(char *)&chScore,1);
}
void LandlordDesktop::GetPageInterval(quint8 chView,quint8 *pcards,quint8 chPages,int *dx,int *dy)
{
	*dx = 1;
	*dy = 0;
}

int LandlordDesktop::Compare2Card(quint8 chCard1,quint8 chCard2,PDJGamePokerImage pimg)
{
	if(DJGAME_POKER_VALUE(chCard1) == DJGAME_POKER_VALUE(chCard2))
		return DJGAME_POKERDESKTOP_COMPARE_RESUST_E;
	if(c_chArrangeStyle == DOUDZHU_ARRANGE_STYLE_DOUBLE && !DJGAME_POKER_ERRORCARD(chCard1) && !DJGAME_POKER_ERRORCARD(chCard2))
	{
		quint8 ch1Pages,ch2Pages;
		PDoudzhuRoom pddzroom = (PDoudzhuRoom)((c_LandlordPanel->gameRoom())->privateRoom());
		if(DJGAME_POKER_ISBK(chCard1) || DJGAME_POKER_ISSK(chCard1))
		{
			if(pimg->chPage[DJGAME_POKER_BIGKING] + pimg->chPage[DJGAME_POKER_SMALLKING] == pddzroom->rule.chCardPages*2)
			{
				if(DJGAME_POKER_ISBK(chCard2) || DJGAME_POKER_ISSK(chCard2))
				{
					if(chCard1 > chCard2)
						return DJGAME_POKERDESKTOP_COMPARE_RESUST_1;
					else if(chCard2 > chCard1)
						return DJGAME_POKERDESKTOP_COMPARE_RESUST_2;
					else
						return DJGAME_POKERDESKTOP_COMPARE_RESUST_E;
				}
				return DJGAME_POKERDESKTOP_COMPARE_RESUST_1;
			}
			ch1Pages = pimg->chPage[chCard1];
		}else 
			ch1Pages = DJGamePoker_GetTotalPages(pimg,0,DJGAME_POKER_VALUE(chCard1));
		
		if(DJGAME_POKER_ISBK(chCard2) || DJGAME_POKER_ISSK(chCard2))
		{
			if(pimg->chPage[DJGAME_POKER_BIGKING] + pimg->chPage[DJGAME_POKER_SMALLKING] == pddzroom->rule.chCardPages*2)
			{
				return DJGAME_POKERDESKTOP_COMPARE_RESUST_1;
			}
			ch2Pages = pimg->chPage[chCard2];
		}else 
			ch2Pages = DJGamePoker_GetTotalPages(pimg,0,DJGAME_POKER_VALUE(chCard2));
		
		if(ch1Pages > ch2Pages)
			return DJGAME_POKERDESKTOP_COMPARE_RESUST_1;
		if(ch1Pages < ch2Pages)
			return DJGAME_POKERDESKTOP_COMPARE_RESUST_2;
	}
	if(DJGAME_POKER_VALUE(chCard1) == 2)
	{
		if(DJGAME_POKER_ISBK(chCard2) || DJGAME_POKER_ISSK(chCard2))
			return DJGAME_POKERDESKTOP_COMPARE_RESUST_2;
		return DJGAME_POKERDESKTOP_COMPARE_RESUST_1;
	}
	if(DJGAME_POKER_VALUE(chCard2) == 2)
	{
		if(DJGAME_POKER_ISBK(chCard1) || DJGAME_POKER_ISSK(chCard1))
			return DJGAME_POKERDESKTOP_COMPARE_RESUST_1;
		return DJGAME_POKERDESKTOP_COMPARE_RESUST_2;
	}
	
	return DJGamePokerDesktop::Compare2Card(chCard1,chCard2,pimg);
}

void LandlordDesktop::StaticInitDesktop()
{
	memset(&c_current,0,sizeof(DoudzhuCurrent));
	c_Bombs = 0;
	c_chLastCall = 0;
	c_chLastCallSeat = 0;
	c_chLastThrowSeat = 0;
	for(int i=0;i<5;i++)
		c_chThrowPages[i] = 0;
#ifdef DJ_OEM
	for(int i=0;i<5;i++)
		m_previousThrowPages[i] = 0;
#endif			
	int i=0;
	while(llcallb[i] != NULL)
	{
		llcallb[i]->hide();
		i ++;
	}
	c_Score =0;
}

void LandlordDesktop::StaticGameTrace(GeneralGameTrace2Head *gameTrace)
{
	djDebug() << "StaticGameTrace";
	switch(gameTrace->chType)
	{
		//case DOUDZHU_GAMETRACE_RESET :
		case DOUDZHU_GAMETRACE_DEAL : 
		case DOUDZHU_GAMETRACE_PICKUP :
		{
			ResetPlayerCards(gameTrace->chSite,gameTrace->chBuf,gameTrace->chBufLen);
			break;
		}
		case DOUDZHU_GAMETRACE_CALL : 
		{
			if(gameTrace->chBuf[0] != 0)
			{
				c_chLastCall = gameTrace->chBuf[0];
				if ( c_chLastCall )
					c_chLastCallSeat = gameTrace->chSite;
					
				c_current.chMaster =  c_chLastCallSeat;
				c_chThrowPages[gameTrace->chSite] = 1;
				c_chThrow[gameTrace->chSite][0] = DJGAME_POKERDESKTOP_EMOTION_GAMEDEFINE+gameTrace->chBuf[0];
				c_Score = c_chLastCall;
		//		printf("c_chLastCall =%d\n",c_Score);
			}else
			{
				c_chThrowPages[gameTrace->chSite] = 1;
				c_chThrow[gameTrace->chSite][0] = DJGAME_POKERDESKTOP_EMOTION_GAMEDEFINE;
			}
			break;
		}
		case DOUDZHU_GAMETRACE_HAND : 
		{
			djDebug() << "DOUDZHU_GAMETRACE_HAND";
			if ( isCardVisible(gameTrace->chSite) ) {
				djDebug() << "visible";
				AppendPlayerCards(gameTrace->chSite,gameTrace->chBuf,gameTrace->chBufLen);
			}else {
				djDebug() << "NOT visible";
				static quint8 temp[256];
				memset(temp, 0x0F, gameTrace->chBufLen);
				AppendPlayerCards(gameTrace->chSite,temp,gameTrace->chBufLen);
			}
			break;
		}
		case DOUDZHU_GAMETRACE_THROW :
		{
			if(gameTrace->chBufLen > 0)
			{
				RemovePlayerCards(gameTrace->chSite,gameTrace->chBuf,gameTrace->chBufLen);
				c_chLastThrowSeat = gameTrace->chSite;
#ifdef DJ_OEM				
				m_previousThrowPages[gameTrace->chSite]	= c_chThrowPages[gameTrace->chSite];
				memcpy(&(m_previousThrow[gameTrace->chSite][0]),c_chThrow[gameTrace->chSite],c_chThrowPages[gameTrace->chSite]);
#endif				
				c_chThrowPages[gameTrace->chSite] = gameTrace->chBufLen;
				memcpy(&(c_chThrow[gameTrace->chSite][0]),gameTrace->chBuf,gameTrace->chBufLen);
				PDoudzhuRoom pddzroom = (PDoudzhuRoom)((c_LandlordPanel->gameRoom())->privateRoom());
				//unsigned char chValue,chSer;//,chType;
				m_lastThrowType = CheckCardTypeCode(&(pddzroom->rule),gameTrace->chBuf,gameTrace->chBufLen,
													&m_lastThrowValue,&m_lastThrowSerial);
				
				if(ISROCKET(m_lastThrowType) )
				{
					c_Bombs ++;
					c_Score *= 2;
				}else if(ISBOMB(&(pddzroom->rule),m_lastThrowType) )
				{
					c_Bombs ++;
					c_Score *= 2;
				}
				c_LandlordPanel->playWave("out.wav");
			}else
			{
#ifdef DJ_OEM				
				m_previousThrowPages[gameTrace->chSite]	= 0;
#endif				
				c_chThrowPages[gameTrace->chSite] = 1;
				if(IsFriend(gameTrace->chSite,c_chLastThrowSeat))
				{
					c_chThrow[gameTrace->chSite][0] = DJGAME_POKERDESKTOP_EMOTION_YEH;
				}else
				{
					quint8 chNextSeat = gameTrace->chSite-1;
					if(chNextSeat == 0)
						chNextSeat = (c_LandlordPanel->gameRoom())->numberOfSeats();
					if(IsFriend(gameTrace->chSite,chNextSeat))
						c_chThrow[gameTrace->chSite][0] = DJGAME_POKERDESKTOP_EMOTION_SOS;
					else
						c_chThrow[gameTrace->chSite][0] = DJGAME_POKERDESKTOP_EMOTION_GOD;
				}
			}
			break;
		}
	}
}

bool LandlordDesktop::IsFriend(quint8 seat1,quint8 seat2)
{
	if(seat1 == seat2)
		return true;
	if(seat1 == c_chLastCallSeat || seat2 == c_chLastCallSeat)
		return false;
	return true;
}

void LandlordDesktop::RepaintCurrentStatus()
{
	/*
	DJGameRoom *room= c_LandlordPanel->gameRoom();
	for(int i=1;i<=room->numberOfSeats();i++)
	{
		RepaintSeatCard(i);
	}
	RepaintLandLord(c_chLastCallSeat);
	*/
}

bool LandlordDesktop::IsThrowStep(PGeneralGameTrace2Head ptrace)
{
	if(ptrace->chType == DOUDZHU_GAMETRACE_THROW)
		return true;
	return false;
}
QString	LandlordDesktop::playerItemNameSuffix( DJGameUser* user )
{
	if ( c_current.chMaster && c_current.chMaster == user->seatId() )
		return tr("(landlord)");
	else
		return QString();
}
void LandlordDesktop::gameInfo( const unsigned char *buf )
{
	StaticInitDesktop();
	memcpy(&c_current,buf,sizeof(DoudzhuCurrent));
	ClearSeatHandCard(0);
	ClearSeatAttachCard(0);
	ClearSeatThrowCard(0);
	
	hideAllRemainders();
	hideHost();
}

void LandlordDesktop::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	if(GetTableStatus() == DJGAME_DOUDZHU_TABLE_STATUS_WAIT_CALL && 
		status == DJGAME_DOUDZHU_TABLE_STATUS_WAIT_THROW)
	{
#ifdef DJ_OEM
		m_throw->hide();
		m_pass->hide();
#else
		m_throw->setEnabled(false);
		m_pass->setEnabled(false);
#endif		
		for(int i=0;i<DOUDZHU_MAX_CALLLEVEL;i++)
		{
			if(llcallb[i] != NULL)
			{
				llcallb[i]->hide();
			}
		}
	}
	DJGameDesktop::gameWait(mask,status,timeout);
	if(status == DJGAME_TABLE_STATUS_WAITSTART)
	{
		setWaitingThrow(false);
	}
	if ( DJGAME_DOUDZHU_TABLE_STATUS_WAIT_THROW == status
		&& IsWaittingForMe() && c_chLastThrowSeat 
		&& c_chLastThrowSeat != GetGamePanel()->selfSeatId() ){
		m_tipHandPokers.clear();
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
		case DJGAME_DOUDZHU_TABLE_STATUS_WAIT_THROW :
		{
			if(IsWaittingForMe())
			{
				setWaitingThrow(true);
				setWaitingThrowAcl(false);
#ifdef DJ_OEM
				m_throw->show();
				m_pass->show();
#else				
				m_throw->setEnabled(true);
				m_pass->setEnabled(true);
#endif				
				quint8 selfSeat	= c_LandlordPanel->selfSeatId();
				if ( c_chLastThrowSeat != selfSeat )
					ClearSeatThrowCard(selfSeat);
			}else
			{
				setWaitingThrow(false);
#ifdef DJ_OEM
				m_throw->hide();
				m_pass->hide();
#else			
				m_throw->setEnabled(false);
				m_pass->setEnabled(false);
#endif				
			}
			break;
		}
		case DJGAME_DOUDZHU_TABLE_STATUS_WAIT_CALL :
		{
#ifdef DJ_OEM
			m_throw->hide();
			m_pass->hide();
#else			
			m_throw->setEnabled(false);
			m_pass->setEnabled(false);
#endif		
			for(int i=0;i<DOUDZHU_MAX_CALLLEVEL;i++)
			{
				if(llcallb[i] != 0)
				{
					if(IsWaittingForMe())
					{
						llcallb[i]->show();
						if ( 0 == llcallb[i]->score() )
							llcallb[i]->setEnabled(true);
						else {
							if(llcallb[i]->score() > c_chLastCall)
								llcallb[i]->setEnabled(true);
							else
								llcallb[i]->setEnabled(false);
						}
					}else llcallb[i]->hide();
				}
			}
			break;
		}
	}
}
void LandlordDesktop::gameTrace( const GeneralGameTrace2Head *gameTrace )
{
	djDebug() << "LandlordDesktop::gameTrace" << gameTrace->chSite << hex << gameTrace->chType ;
	DebugBuffer( (char*)gameTrace->chBuf, gameTrace->chBufLen );
	DJGameDesktop::gameTrace(gameTrace);
	GeneralGameTrace2Head *ptrace = (PGeneralGameTrace2Head)gameTrace;
	if( gameTrace->chType == DOUDZHU_GAMETRACE_THROW )
	{
		if(c_chLastThrowSeat == 0)
			c_chLastThrowSeat = gameTrace->chSite;
		quint8 seat = c_chLastThrowSeat;
		do
		{
			seat ++;
			if(seat > (c_LandlordPanel->gameRoom())->numberOfSeats())
				seat = 1;
			if(seat !=  gameTrace->chSite)
			{
				ClearSeatThrowCard(seat);
			}
		}while(seat != gameTrace->chSite);
	}
	StaticGameTrace(ptrace);
	switch(gameTrace->chType)
	{
		//case DOUDZHU_GAMETRACE_RESET :
		case DOUDZHU_GAMETRACE_DEAL : 
		case DOUDZHU_GAMETRACE_PICKUP :
		{
			if ( gameTrace->chType == DOUDZHU_GAMETRACE_DEAL ) {
				djDebug() << "DOUDZHU_GAMETRACE_DEAL" << gameTrace->chSite << gameTrace->chBufLen;
			}else if ( gameTrace->chType == DOUDZHU_GAMETRACE_PICKUP ) {
				djDebug() << "DOUDZHU_GAMETRACE_PICKUP" << gameTrace->chSite << gameTrace->chBufLen;
			}
			RepaintSeatCard(gameTrace->chSite);
			break;
		}
		case DOUDZHU_GAMETRACE_CALL : 
		{
			RepaintThrowCard(gameTrace->chSite,c_chThrow[gameTrace->chSite],c_chThrowPages[gameTrace->chSite]);
			if ( gameTrace->chBuf[0] ) {
				repaintHostItem(gameTrace->chSite);
				c_LandlordPanel->repaintAllPlayerItems();
			}
			DisplayBombs();
			DisplayScore();
			break;
		}
		case DOUDZHU_GAMETRACE_HAND : 
		{
			RepaintSeatCard(gameTrace->chSite);
			RepaintAttachCards(gameTrace->chSite,(quint8 *)(gameTrace->chBuf),gameTrace->chBufLen);
			break;
		}
		case DOUDZHU_GAMETRACE_THROW :
		{
			RepaintSeatCard(gameTrace->chSite);
			RepaintThrowCard(gameTrace->chSite,c_chThrow[gameTrace->chSite],c_chThrowPages[gameTrace->chSite]);
			DisplayBombs();
			DisplayScore();
			repaintRemainder(gameTrace->chSite);
			break;
		}
	}
}
QString LandlordDesktop::bonusTarget( const DJGameBonus* bonus )
{
	QString text;
	switch( bonus->chTarget ) {
	case DOUDZHU_GAMEBONUS_TARGET_LASTCARD: {
		quint8 value	= bonus->chParam1;
		text	= tr("The last thrown cards contains %1").arg(value);
		break;
	}
	default:
		break;
	}
	return text;
}
void LandlordDesktop::repaintHost()
{
	repaintHostItem( c_chLastCallSeat );
	c_LandlordPanel->repaintAllPlayerItems();
}
QImage LandlordDesktop::GetCardPicture(quint8 chCard,quint8 chView,qreal rate)
{
	if(chCard >= DJGAME_POKERDESKTOP_EMOTION_GAMEDEFINE)
	{
		QPixmap pix;
		QImage  img;
		int x,y;
		quint8 chPoints = chCard-DJGAME_POKERDESKTOP_EMOTION_GAMEDEFINE;
	
		if(chPoints == 0)
		{
			pix = QPixmap(":/BaseRes/image/emotion/6.png");
		}else
		{
			DJGameUser *puser = c_LandlordPanel->userAtSeat(MapView2Seat(chView));
			quint8 chGender	= DJGAME_GENDER_MALE;
			if ( puser )
				chGender	= puser->gender();
			if(chGender != DJGAME_GENDER_FEMALE)
			{
				pix = QPixmap(":/LandLordRes/image/call1.png");
			}else
			{
				pix = QPixmap(":/LandLordRes/image/call2.png");
			}
			QPixmap pixnum1,pixnum2;
			if(chPoints > 9)
			{
				pixnum1 = QPixmap(QString(":/LandLordRes/image/nums/%1.png").arg((int)(chPoints/10)));
				pixnum2 = QPixmap(QString(":/LandLordRes/image/nums/%1.png").arg((int)(chPoints%10)));
				unsigned char chTotalWidth = pixnum1.width()+pixnum2.width()+2;
				QPainter p(&pix);
				x = ((31-chTotalWidth)>>1)+1;
				y = ((23-pixnum1.height())>>1)+5;
				p.drawPixmap(x,y,pixnum1);
				x += pixnum1.width()+2;
				p.drawPixmap(x,y,pixnum2);
				p.end();
			}else
			{
				pixnum1 = QPixmap(QString(":/LandLordRes/image/nums/%1.png").arg(chPoints));
				QPainter p(&pix);
				x = ((31-pixnum1.width())>>1)+1;
				y = ((23-pixnum1.height())>>1)+5;
				p.drawPixmap(x,y,pixnum1);
				p.end();
			}
		}
		img = pix.convertToImage();
   		if(rate != 1)
    	{
        	img = img.smoothScale((int)(img.width()*rate),(int)(img.height()*rate));
    	}
		return img;
	}
	return DJGamePokerDesktop::GetCardPicture(chCard,chView,rate);
}

void LandlordDesktop::RecvThrowACL(bool bFailed)
{
	djDebug() << "LandlordDesktop::RecvThrowACL" << bFailed;
	DJGamePokerDesktop::RecvThrowACL(bFailed);
}

void LandlordDesktop::CreateThrowTrace(PGeneralGameTrace2Head ptrace)
{
	djDebug()<<"LandlordDesktop::CreateThrowTrace";
	DebugBuffer( (char*)ptrace->chBuf, ptrace->chBufLen);
	if(GetTableStatus() == DJGAME_DOUDZHU_TABLE_STATUS_WAIT_THROW)
	{
		ptrace->chType = DOUDZHU_GAMETRACE_THROW;
		saveSortInfo();
		setPokerSortMode( ASCountPointSuit );
		sortCards( ptrace->chBuf, ptrace->chBufLen );
		restoreSortInfo();
	}else 
	{
//		printf("throw failed\n");
		ptrace->chBufLen = 0;
	}
}
void LandlordDesktop::MoveItem(Q3CanvasItem* item,QPoint &ps,QPoint &pe)
{
	DJGameDesktop::MoveItem(item,ps,pe);
}
void LandlordDesktop::repaintViewAttach(quint8 seat,quint8 *cards,quint8 chPages)
{
	if(chPages == 0 || cards == NULL)
		return;
	QImage img = GetCardPicture(cards[0],1,getAttachFx());
	int w	= img.width() + 10;
	int x	= GetRealWidth() / 2 - (chPages/2 * w );
	QMatrix m = GetCurrentMatrix();
	DJGamePokerItem *item;
	for(int i=0;i<chPages;i++)
	{
		img = GetCardPicture(cards[i],1,getAttachFx());
		item = new DJGamePokerItem(img,canvas(),m,cards[i],seat,DJGAME_POKERDESKTOP_TYPE_ATTACHCARD,false);
		item->setZ(i);
		item->move(x,0);
		item->setHAlignment( Qt::AlignHCenter );
		item->show();
		item->setMatrix(m);
		x += w;
	}
}
void LandlordDesktop::RepaintView1Attach(quint8 seat,quint8 *cards,quint8 chPages)
{
	if ( 3 == c_LandlordPanel->numberOfSeats() )
		repaintViewAttach(seat,cards,chPages);
	else
		DJGamePokerDesktop::RepaintView1Attach(seat,cards,chPages);
}
void LandlordDesktop::RepaintView2Attach(quint8 seat,quint8 *cards,quint8 chPages)
{
	if ( 3 == c_LandlordPanel->numberOfSeats() )
		repaintViewAttach(seat,cards,chPages);
	else
		DJGamePokerDesktop::RepaintView2Attach(seat,cards,chPages);
}
void LandlordDesktop::RepaintView3Attach(quint8 seat,quint8 *cards,quint8 chPages)
{
	if ( 3 == c_LandlordPanel->numberOfSeats() )
		repaintViewAttach(seat,cards,chPages);
	else
		DJGamePokerDesktop::RepaintView3Attach(seat,cards,chPages);
}
void LandlordDesktop::RepaintView4Attach(quint8 seat,quint8 *cards,quint8 chPages)
{
	if ( 3 == c_LandlordPanel->numberOfSeats() )
		repaintViewAttach(seat,cards,chPages);
	else
		DJGamePokerDesktop::RepaintView4Attach(seat,cards,chPages);
}
/////////////////////////////////////////////////
LandlordPanel::LandlordPanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f)
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
	LandlordDesktop* gameDesktop = new LandlordDesktop(gameSplitter,this);
	
	setAccessorySplitter( accessorySplitter );
	setGameDesktop( gameDesktop );		
}
LandlordPanel::~LandlordPanel()
{
}
/////////////////////////////
LandlordCallButton::LandlordCallButton(QWidget *parent,quint8 chScore)
	: QPushButton(parent)
{
	connect(this,SIGNAL(clicked()),this,SLOT(HandleRelesed()));
	c_Parent= (LandlordDesktop *)parent;
	c_chScore = chScore;
	QString file;
	if ( 0 == chScore )
		file	= QString(":/BaseRes/image/desktop/pushbutton/BuJiao.png");
	else
		file	= QString(":/BaseRes/image/desktop/pushbutton/%1Fen.png").arg(c_chScore);
	if ( QFile::exists( file ) ) {
		QPixmap	pix = QPixmap( file  );
		setIcon( pix );
		setIconSize( pix.size() );
	}else
		setText(QString("%1").arg(chScore)+QString(tr(" Points")));
	adjustSize();
}

LandlordCallButton::~LandlordCallButton()
{
}
quint8 LandlordCallButton::score()
{
	return c_chScore;
}
void LandlordCallButton::HandleRelesed()
{
	c_Parent->RequestCall(c_chScore);
}

///////////////////////////////////////////////////////////
