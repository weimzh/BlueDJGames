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
#include "chudd.h"
#include "chuddrule.h"
#include "chuddprof.h"
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

unsigned char GetTotalPages(unsigned char *chBuf,unsigned char chLen);

ChuddDesktop::ChuddDesktop(QWidget* parent,DJGamePanel *panel)
			: DJGamePokerDesktop(parent,panel)
{
	c_ChuddPanel = (ChuddPanel *)panel;
	canvas()->setAdvancePeriod(300);
	
	SetCardPictureSize(70,popupSize(),pageLayoutSizeX(),pageLayoutSizeY());
	
	connect(this,SIGNAL(gameStarted()),this,SLOT(PlayerStarted()));
	
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
    
    QSize iconSize( 19 * 2, 19 );
    
	m_tip = new QPushButton(this);
	m_tip->setToolTip(tr("tip"));
	m_tip->setIcon( QIcon(":/BaseRes/image/desktop/pushbutton/TiShi.png") );
	m_tip->setIconSize( iconSize );
	m_tip->adjustSize();
    connect(m_tip,SIGNAL(clicked()),this,SLOT(ClickTip()));
    m_tip->hide();
    
	m_throw = new QPushButton(this);
	m_throw->setToolTip(tr("thrown card"));
	m_throw->setIcon( QIcon(":/BaseRes/image/desktop/pushbutton/ChuPai.png") );
	m_throw->setIconSize( iconSize );
	m_throw->adjustSize();
    connect(m_throw,SIGNAL(clicked()),this,SLOT(ClickThrow()));
    m_throw->hide();
	
	m_pass = new QPushButton(this);
	m_pass->setToolTip(tr("pass"));
	m_pass->setIcon( QIcon(":/BaseRes/image/desktop/pushbutton/GuoPai.png") );
	m_pass->setIconSize( iconSize );
	m_pass->adjustSize();
    connect(m_pass,SIGNAL(clicked()),this,SLOT(ClickPass()));
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
	m_toolbar->show();
#endif	
	SetOrder(false,false,false);

	setRemainderEnabled( true );
	
	setBridgeCard( false );
	setPokerSortMode( PointSuit );
	setRankOfPoints( Num2PointRank );
	
	StaticInitDesktop();
}

void ChuddDesktop::ClickThrow()
{
	DJGamePokerDesktop::handleItemClicked(Qt::RightButton,QPoint(),0);
}
void ChuddDesktop::ClickPass()
{
	if(GetTableStatus() == DJGAME_CHUDD_TABLE_STATUS_WAIT_THROW && IsWaittingForMe())
		SendGameTrace(CHUDD_GAMETRACE_THROW);
#ifdef DJ_OEM
	m_pass->hide();
#else		
	m_pass->setEnabled(false);
#endif	
}
/*
bool ChuddDesktop::groupGreaterThan( const QList<DJPoker>& group1,
									const QList<DJPoker>& group2 )
{
	QList<DJPoker> pokers1	= group1;
	QList<DJPoker> pokers2	= group2;
	
	saveSortInfo();
	setPokerSortMode( PointSuit );
	sortPokers( pokers1 );
	sortPokers( pokers2 );
	restoreSortInfo();
	
	DJPoker poker1	= pokers1.first();
	DJPoker poker2	= pokers2.first();
	
	int value1	= rankOfPoint(poker1.point());
	int value2	= rankOfPoint(poker2.point());
	if ( value1 == value2 ) {
		//point value is equal ,then compare suit value
		return rankOfSuit(poker1.suit()) > rankOfSuit(poker2.suit());
	}else {
		return value1 > value2;
	}
}
bool ChuddDesktop::shunGreaterThan( const QList<DJPoker>& shun1,
										const QList<DJPoker>& shun2 )
{
	DJPoker poker1	= shun1.last();
	DJPoker poker2	= shun2.last();
	
	int value1	= rankOfPoint(poker1.point());
	int value2	= rankOfPoint(poker2.point());
	if ( value1 == value2 ) {
		//point value is equal ,then compare suit value
		return rankOfSuit(poker1.suit()) > rankOfSuit(poker2.suit());
	}else {
		return value1 > value2;
	}
}
DJPoker ChuddDesktop::higherSerialPoker( const DJPoker& keyPoker, bool isStrict ) const
{
	if ( DJPoker::Ace == keyPoker.point() ) {
		if ( isStrict )
			return DJPoker(keyPoker.suit(), DJPoker::Num2);
		else
			return DJPoker(DJPoker::AnySuit, DJPoker::Num2);
	}
	return DJGamePokerDesktop::higherSerialPoker(keyPoker,isStrict);
}
DJPoker ChuddDesktop::lowerSerialPoker( const DJPoker& keyPoker, bool isStrict ) const
{
	if ( DJPoker::Num2 == keyPoker.point() ) {
		if ( isStrict )
			return DJPoker(keyPoker.suit(), DJPoker::Ace);
		else
			return DJPoker(DJPoker::AnySuit, DJPoker::Ace);
	}
	return DJGamePokerDesktop::higherSerialPoker(keyPoker,isStrict);
}
DJPoker ChuddDesktop::lowestSerialPoker() const
{
	return DJPoker(DJPoker::AnySuit,DJPoker::Ace);
}
*/
void ChuddDesktop::ClickTip()
{
	djDebug()<<"ChuddDesktop::ClickTip";

	if ( m_tipHandCards.isEmpty() ) {
		//first tip
		djDebug()<<"it is the first tip";
		quint8 *prevBuf	= c_chThrow[c_chLastThrowSeat];
		quint8 prevLen	= c_chThrowPages[c_chLastThrowSeat];
		bool result	= ChuddRule_GetCardType( prevBuf, prevLen, &m_tipOtherType );
		if ( !result ) {
			return;
		}
		m_tipHandCards	= cardsOfSeat( GetGamePanel()->selfSeatId() );
		m_tipHandIndexes.clear();
	}
	do {
    	iterateCombination( m_tipHandCards.size(), m_tipOtherType.chCardPages, m_tipHandIndexes );
    	if ( !m_tipHandIndexes.isEmpty() ) {
    		int count	= 0;
        	for (int i = 0; i < m_tipHandIndexes.size(); ++i ) {
        		if ( m_tipHandIndexes.at(i) ) {
        			m_tipSelectedCards[count++]	= m_tipHandCards.at(i);
        		}
        	}
        	memcpy(&m_tipSelfType,&m_tipOtherType,sizeof(ChuddRuleCardType));
        	bool result	= ChuddRule_CheckThrow( GetPlayerCardImage(GetGamePanel()->selfSeatId()),
        									&m_tipSelfType,
        									m_tipSelectedCards,
        									count );
        	if ( result ) {
        		QList<DJPoker>	pokers;
    			for( int i = 0; i < count; ++i ) {
    				pokers << DJPoker( m_tipSelectedCards[i], isBridgeCard() );
    			}
    			selectPokerItems( pokers );
    			return;
        	}
    	}
	}while( !m_tipHandIndexes.isEmpty() );
	
	m_tipHandCards.clear();
	selectPokerItems();
}
void ChuddDesktop::ClickArrange()
{
	//SetOrder(false,false,!orderByClass());
	//RepaintSeatCard(c_ChuddPanel->selfSeatId());
	c_chArrangeStyle++;
	c_chArrangeStyle	%= 3;
	switch( c_chArrangeStyle ) {
	case 0:
		setPokerSortMode( PointSuit );
		break;
	case 1:
		setPokerSortMode( ASCountPointSuit );
		break;
	case 2:
		setPokerSortMode( SuitPoint );
		break;
	}
	repaintCards(c_ChuddPanel->selfSeatId());
}
bool ChuddDesktop::handleItemClicked( Qt::MouseButton btn,const QPoint& pos, Q3CanvasItem *item )
{
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
ChuddDesktop::~ChuddDesktop()
{
}
void ChuddDesktop::handleMatrixChanged(const QMatrix &m)
{
	DJGamePokerDesktop::handleMatrixChanged(m);
		
	int x,y,iSelfHeight;
	GetDesktopCenterPoint(&x,&y,&iSelfHeight);
#ifdef DJ_OEM
	y = GetRealHeight()-iSelfHeight;
	int tx,ty;
	m.map(x,y,&tx,&ty);
	m_throw->move(tx-(m_throw->width()>>1),ty);
	m_pass->move(tx-(m_throw->width()>>1)-m_pass->width()-20,ty);
	m_tip->move(tx+(m_throw->width()>>1)+20,ty);
	
	m_arrange->move(width()-m_arrange->width()-20,ty+40);
#else	
	y = GetRealHeight()-iSelfHeight;
	x = GetRealWidth()-100;
	int tx,ty;
	m.map(x,y,&tx,&ty);
	m_toolbar->move(tx - m_toolbar->width(),ty + 10 );
#endif	
}
void ChuddDesktop::PlayerStarted()
{
	StaticInitDesktop();
}
void ChuddDesktop::GetPageInterval(quint8 chView,quint8 *pcards,quint8 chPages,int *dx,int *dy)
{
	*dx = 1;
	*dy = 0;
}



void ChuddDesktop::StaticInitDesktop()
{
	memset(&c_current,0,sizeof(ChuddCurrent));
	
	c_chLastThrowSeat = 0;
	for(int i=0;i<5;i++)
		c_chThrowPages[i] = 0;
}

void ChuddDesktop::StaticGameTrace(GeneralGameTrace2Head *gameTrace)
{
	djDebug() << "StaticGameTrace";
	switch(gameTrace->chType)
	{
		case CHUDD_GAMETRACE_PICKUP : 
		case CHUDD_GAMETRACE_PICKUP2 :
		{
			ResetPlayerCards(gameTrace->chSite,gameTrace->chBuf,gameTrace->chBufLen);
			break;
		}

		case CHUDD_GAMETRACE_THROW :
		{
			if(gameTrace->chBufLen > 0)
			{
				RemovePlayerCards(gameTrace->chSite,gameTrace->chBuf,gameTrace->chBufLen);
				c_chLastThrowSeat = gameTrace->chSite;
				c_chThrowPages[gameTrace->chSite] = gameTrace->chBufLen;
				memcpy(&(c_chThrow[gameTrace->chSite][0]),gameTrace->chBuf,gameTrace->chBufLen);
				c_ChuddPanel->playWave("out.wav");
			}else
			{
				c_chThrowPages[gameTrace->chSite] = 1;
				unsigned char chNext = ((gameTrace->chSite-1)==0)?(c_ChuddPanel->gameRoom())->numberOfSeats():gameTrace->chSite-1;
				if(chNext == c_chLastThrowSeat)
				{
					c_chThrow[gameTrace->chSite][0] = DJGAME_POKERDESKTOP_EMOTION_GOD;
				}else
				{
					c_chThrow[gameTrace->chSite][0] = DJGAME_POKERDESKTOP_EMOTION_SOS;
				}
			}
			break;
		}
	}
}

void ChuddDesktop::RepaintCurrentStatus()
{
	DJGameRoom *room= c_ChuddPanel->gameRoom();
	for(int i=1;i<=room->numberOfSeats();i++)
	{
		//RepaintSeatCard(i);
		repaintCards(i);
	}
}

bool ChuddDesktop::IsThrowStep(PGeneralGameTrace2Head ptrace)
{
	if(ptrace->chType == CHUDD_GAMETRACE_THROW)
		return true;
	return false;
}

void ChuddDesktop::gameInfo( const unsigned char *buf )
{
	StaticInitDesktop();
	memcpy(&c_current,buf,sizeof(ChuddCurrent));
	ClearSeatHandCard(0);
	ClearSeatAttachCard(0);
	ClearSeatThrowCard(0);
	
	hideAllRemainders();
}

void ChuddDesktop::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	DJGameDesktop::gameWait(mask,status,timeout);
	if(status == DJGAME_TABLE_STATUS_WAITSTART)
	{
		setWaitingThrow(false);
	}
	if ( DJGAME_CHUDD_TABLE_STATUS_WAIT_THROW == status
		&& IsWaittingForMe() && c_chLastThrowSeat
		&& c_chLastThrowSeat != c_ChuddPanel->selfSeatId() ){
		m_tipHandCards.clear();
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
		case DJGAME_CHUDD_TABLE_STATUS_WAIT_THROW :
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
				quint8 selfSeat	= c_ChuddPanel->selfSeatId();
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
	}
}
void ChuddDesktop::gameTrace( const GeneralGameTrace2Head *gameTrace )
{
	djDebug() << "ChuddDesktop::gameTrace" << gameTrace->chSite << hex << gameTrace->chType ;
	DebugBuffer( (char*)gameTrace->chBuf, gameTrace->chBufLen );
	DJGameDesktop::gameTrace(gameTrace);
	GeneralGameTrace2Head *ptrace = (PGeneralGameTrace2Head)gameTrace;
	if( gameTrace->chType == CHUDD_GAMETRACE_THROW )
	{
		if(c_chLastThrowSeat == 0)
			c_chLastThrowSeat = gameTrace->chSite;
		quint8 seat = c_chLastThrowSeat;
		do
		{
			seat ++;
			if(seat > (c_ChuddPanel->gameRoom())->numberOfSeats())
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
		case CHUDD_GAMETRACE_PICKUP : 
		case CHUDD_GAMETRACE_PICKUP2 :
		{
			if ( gameTrace->chType == CHUDD_GAMETRACE_PICKUP ) {
				djDebug() << "CHUDD_GAMETRACE_DEAL" << gameTrace->chSite << gameTrace->chBufLen;
			}else if ( gameTrace->chType == CHUDD_GAMETRACE_PICKUP2 ) {
				djDebug() << "CHUDD_GAMETRACE_PICKUP" << gameTrace->chSite << gameTrace->chBufLen;
			}
			//RepaintSeatCard(gameTrace->chSite);
			repaintCards(gameTrace->chSite);
			break;
		}

		case CHUDD_GAMETRACE_THROW :
		{
			//RepaintSeatCard(gameTrace->chSite);
			repaintCards(gameTrace->chSite);
			RepaintThrowCard(gameTrace->chSite,c_chThrow[gameTrace->chSite],c_chThrowPages[gameTrace->chSite]);
			repaintRemainder(gameTrace->chSite);
			break;
		}
	}
}
QString ChuddDesktop::bonusTarget( const DJGameBonus* bonus )
{
	QString text;
	switch( bonus->chTarget ) {
	case CHUDD_GAMEBONUS_TARGET_MAX: {
		text	= tr("All enemies were blocked");
		break;
	}
	case CHUDD_GAMEBONUS_TARGET_13: {
		text	= tr("One enemy was blocked");
		break;
	}
	case CHUDD_GAMEBONUS_TARGET_LASTCARD: {
		quint8 value	= bonus->chParam1;
		text	= tr("The last thrown cards contains %1").arg(value);
		break;
	}
	default:
		break;
	}
	return text;
}
QImage ChuddDesktop::GetCardPicture(quint8 chCard,quint8 chView,qreal rate)
{
	/*
	if(chCard >= DJGAME_POKERDESKTOP_EMOTION_GAMEDEFINE)
	{
		QPixmap pix;
		QImage  img;
		int x,y;
		quint8 chPoints = chCard-DJGAME_POKERDESKTOP_EMOTION_GAMEDEFINE;
		quint32 uid;
		uid = c_LandlordPanel->userIdOfSeat(MapView2Seat(chView));
		DJGameUser *puser = c_LandlordPanel->gameUser(uid,true);
		quint8 chGender = puser->gender();
		if(chPoints == 0)
		{
			pix = QPixmap(QString(":/BaseRes/image/emotion/6.png"));
		}else
		{
			if(chGender != DJGAME_GENDER_FEMALE)
			{
				pix = QPixmap(QString(":/LandLordRes/image/call1.png"));
			}else
			{
				pix = QPixmap(QString(":/LandLordRes/image/call2.png"));
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
	}*/
	if(chCard <= 0x3F && !DJGAME_POKER_ERRORCARD(chCard))
	{
		if(DJGAME_POKER_CLASS(chCard) == DJGAME_POKER_CLASS_D)
			return DJGamePokerDesktop::GetCardPicture(DJGAME_POKER_MAKE_CODE(0,DJGAME_POKER_CLASS_C,DJGAME_POKER_VALUE(chCard)),chView,rate);
		if(DJGAME_POKER_CLASS(chCard) == DJGAME_POKER_CLASS_C)
			return DJGamePokerDesktop::GetCardPicture(DJGAME_POKER_MAKE_CODE(0,DJGAME_POKER_CLASS_D,DJGAME_POKER_VALUE(chCard)),chView,rate);
	}

	return DJGamePokerDesktop::GetCardPicture(chCard,chView,rate);
}

void ChuddDesktop::RecvThrowACL(bool bFailed)
{
	djDebug() << "ChuddDesktop::RecvThrowACL" << bFailed;
	DJGamePokerDesktop::RecvThrowACL(bFailed);
}

unsigned char GetTotalPages(unsigned char *chBuf,unsigned char chLen)
{
	int i;
	unsigned char page = 0;
	for(i=0;i<chLen;i++)
	{
		if(chBuf[i] != 0)
			page++;
	}
	return page;
}

int ChuddDesktop::Compare2Card(quint8 chCard1,quint8 chCard2,PDJGamePokerImage pPokerImage)
{
	if(chCard1 == chCard2)
		return DJGAME_POKERDESKTOP_COMPARE_RESUST_E;
	if(ChuddRule_Compare2Card(chCard1,chCard2))
		return DJGAME_POKERDESKTOP_COMPARE_RESUST_1;
	return DJGAME_POKERDESKTOP_COMPARE_RESUST_2;
}

bool Chudd_CompareLessthan(unsigned char chCard1,unsigned char chCard2);
bool Chudd_CompareLessthan(unsigned char chCard1,unsigned char chCard2)
{
	return !(ChuddRule_Compare2Card(chCard1,chCard2));
}

void ChuddDesktop::ArrangeCards(quint8 *cards,quint8 chPages, int mode )
{

}
void ChuddDesktop::CreateThrowTrace(PGeneralGameTrace2Head ptrace)
{
//	printf("throw %02x\n",ptrace->chBuf[0]);
	//quint8 seat = c_ChuddPanel->selfSeatId();
	//PDJGamePokerImage pokerimg = GetPlayerCardImage(seat);
	if(GetTableStatus() == DJGAME_CHUDD_TABLE_STATUS_WAIT_THROW )
	{
		ptrace->chType = CHUDD_GAMETRACE_THROW;
		saveSortInfo();
		setPokerSortMode( ASCountPointSuit );
		sortCards( ptrace->chBuf, ptrace->chBufLen );
		restoreSortInfo();
	}else 
	{
		ptrace->chBufLen = 0;
	}
}
void ChuddDesktop::MoveItem(Q3CanvasItem* item,QPoint &ps,QPoint &pe)
{
	DJGameDesktop::MoveItem(item,ps,pe);
}

/////////////////////////////////////////////////
ChuddPanel::ChuddPanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f)
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
	ChuddDesktop	*gameDesktop= new ChuddDesktop(gameSplitter,this);
	
	setAccessorySplitter( accessorySplitter );
	setGameDesktop( gameDesktop );	
}
ChuddPanel::~ChuddPanel()
{
}
