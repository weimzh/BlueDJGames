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
#include "RenjuDesktopController.h"
#include "DJPanelController.h"
#include "DJGraphicsPixmapItem.h"
#include "DJGraphicsTextItem.h"
#include "DJGraphicsChessTimerItem.h"
#include "DJDesktop.h"
#include "DJDesktopScene.h"
#include "DJGameRoom.h"
#include "DJGameUser.h"
#include "DJClientRequest.h"
#include "Utility.h"
#include "DJGroupChat.h"
#include "DJTextBrowser.h"
#include "DJSchemeEvent.h"

RenjuChip::RenjuChip(RenjuDesktopController *dc,quint8 chOwner,int x,int y,quint16 chStep)
{
	m_dc	= dc;
	m_owner = chOwner;
	m_number  = chStep;
	m_isCurrent = false;
	m_isNumber = true;
	m_numberBase = 0;
	
	QPixmap pix;
	QColor color;
	
	if(DJGAME_RENJU_OWNER_BLACK == chOwner)
	{
		pix = QPixmap(QString(":/RenjuRes/image/black.png"));
		color = Qt::white;
	}else
	{
		pix = QPixmap(QString(":/RenjuRes/image/white.png"));
		color = Qt::black;
	}
	
	m_itemChip = new DJGraphicsPixmapItem(pix,0, m_dc->desktop()->scene());
	m_itemChip->setZValue(100);
	m_itemChip->setAlignment(Qt::AlignCenter);
	m_itemChip->setVirtualPos( QPoint(x,y) + m_dc->boardPos() );
	m_itemChip->setExternalScale( m_dc->desktop()->graphicsScale() );
	m_itemChip->adjustPos( m_dc->desktop()->graphicsMatrix() );
	
	QString str;
	m_itemNumber = new DJGraphicsTextItem(str,0, m_dc->desktop()->scene());
	m_itemNumber->setZValue(110);
	m_itemNumber->setDefaultTextColor(color);
	QFont f = m_itemNumber->font();
	f.setPointSize(14);
	m_itemNumber->setFont(f);
	m_itemNumber->setAlignment(Qt::AlignCenter);
	m_itemNumber->setVirtualPos( QPoint(x,y) + m_dc->boardPos() );
	m_itemNumber->setExternalScale( m_dc->desktop()->graphicsScale() );
	m_itemNumber->adjustPos( m_dc->desktop()->graphicsMatrix() );
	
	pix = QPixmap(":/RenjuRes/image/current.png");
	m_itemCurrent = new DJGraphicsPixmapItem(pix,0, m_dc->desktop()->scene());
	m_itemCurrent->setZValue(120);
	m_itemCurrent->setAlignment(Qt::AlignCenter);
	m_itemCurrent->setVirtualPos( QPoint(x,y) + m_dc->boardPos() );
	m_itemCurrent->setExternalScale( m_dc->desktop()->graphicsScale() );
	m_itemCurrent->adjustPos( m_dc->desktop()->graphicsMatrix() );
	
}
RenjuChip::~RenjuChip()
{
	delete m_itemChip;
	m_itemChip	= 0;
	delete m_itemNumber;
	m_itemNumber	= 0;
	delete m_itemCurrent;
	m_itemCurrent	= 0;
}
void RenjuChip::setCurrentImage(quint8 ch)
{
	QPixmap pix;
	if(ch == RENJU_STEP_DESCRIBE_FORBID)
	{
		pix = QPixmap(":/RenjuRes/image/loseicon.png");
	}else 
	{
		pix = QPixmap(":/RenjuRes/image/winicon.png");
	}
	m_itemCurrent->setPixmap(pix);
	m_itemCurrent->adjustPos( m_dc->desktop()->graphicsMatrix() );
}
void RenjuChip::show()
{
	m_itemChip->show();
	if(m_isNumber && m_number > m_numberBase)
	{
		m_itemNumber->setPlainText(QString("%1").arg(m_number-m_numberBase));
		m_itemNumber->adjustPos( m_dc->desktop()->graphicsMatrix() );
		m_itemNumber->show();
	}else
		m_itemNumber->hide();
	
	if(m_isCurrent) {
		m_itemCurrent->show();
	}else
		m_itemCurrent->hide();
}
void RenjuChip::hide()
{
	m_itemChip->hide();
	m_itemNumber->hide();
	m_itemCurrent->hide();
}
void RenjuChip::setNumberBase(quint16 base)
{
	m_numberBase = base;
	setDisplayNumber(m_isNumber);
}
quint16 RenjuChip::currentNumber() const
{
	if(m_number > m_numberBase)
		return m_number - m_numberBase;
	return 0;
}
void RenjuChip::setDisplayNumber(bool b)
{
	m_isNumber = b;
	if(b && m_number > m_numberBase)
	{
		m_itemNumber->setPlainText(QString("%1").arg(m_number-m_numberBase));
		m_itemNumber->adjustPos( m_dc->desktop()->graphicsMatrix() );
		m_itemNumber->show();
	}else
		m_itemNumber->hide();
}
void RenjuChip::setCurrentChip(bool b)
{
	m_isCurrent = b;
	if(b)
	{
		m_itemCurrent->adjustPos( m_dc->desktop()->graphicsMatrix() );
		m_itemCurrent->show();
	}else
	{
		m_itemCurrent->hide();
	}
}
bool RenjuChip::isMe(QGraphicsItem *item) const
{
	if(item == NULL)
		return false;
	if(item == m_itemCurrent || item == m_itemChip || item == m_itemNumber)
		return true;
	return false;
}
void HandleMoveACL( const QByteArray& buf, const QVariant& parameters )
{
    PDJGameProtocol pacl = (PDJGameProtocol)buf.data();
    RenjuDesktopController* dc = static_cast<RenjuDesktopController *>(parameters.value<void*>());    
	
    dc->setWaitingMoveACL(false);
}
RenjuDesktopController::RenjuDesktopController( DJPanelController* panelController, const QSize& size, QWidget *parent )
	: DJDesktopController(panelController,size,parent)
{
	djDebug()<<"RenjuDesktopController constructor"<<size;
	
	memset(&m_current,0,sizeof(RenjuCurrent));
	memset(&m_board,0,sizeof(RenjuBoard));
	memset(m_chips,0,sizeof(RenjuChip*)*(DJGAME_RENJU_WIDTH+1)*(DJGAME_RENJU_WIDTH+1));
	
	PRenjuRoom prenjugroom = (PRenjuRoom)(panelController->gameRoom()->privateRoom());
	
	m_current.chBlack = 1;
	m_current.chWhite = 2;
	m_blackTimer = new DJGraphicsChessTimerItem(0,0,desktop()->scene());
	connect(m_blackTimer, SIGNAL(countdown(unsigned char)), SLOT(handleCountdown(unsigned char)));
	m_whiteTimer = new DJGraphicsChessTimerItem(0,0,desktop()->scene());
	connect(m_whiteTimer, SIGNAL(countdown(unsigned char)), SLOT(handleCountdown(unsigned char)));
	SetPlayerTimer(DJGAME_RENJU_OWNER_BLACK,&(prenjugroom->rule.timer));
	SetPlayerTimer(DJGAME_RENJU_OWNER_WHITE,&(prenjugroom->rule.timer));
	
	QPixmap pix = QPixmap(":/RenjuRes/image/board.png");
	
	m_boardPos.setX(0);
	m_boardPos.setY((size.height()-pix.height())>>1);
	
	m_itemBoard = new DJGraphicsPixmapItem(pix,0, desktop()->scene());
	m_itemBoard->setVirtualPos( m_boardPos );
	m_itemBoard->setZValue(1);
	m_itemBoard->show();
	
	m_bSignYN = false;
	m_chSignT0 = 0;
	int x = desktop()->realWidth() - 40;
	int y = 240;
	
	if(m_bSignYN)
		pix = QPixmap(":/RenjuRes/image/sign_n.png");
	else
		pix = QPixmap(":/RenjuRes/image/sign_y.png");
	m_itemSignYN = new DJGraphicsPixmapItem(pix,0, desktop()->scene());
	m_itemSignYN->setAlignment(Qt::AlignRight );
	m_itemSignYN->setVirtualPos(QPoint(x,y));
	m_itemSignYN->setZValue(2);
	m_itemSignYN->show();
	
	if(m_chSignT0 == 0)
		pix = QPixmap(":/RenjuRes/image/sign_t1.png");
	else
		pix = QPixmap(":/RenjuRes/image/sign_t0.png");
	m_itemSignT0T1 = new DJGraphicsPixmapItem(pix,0, desktop()->scene());
	//m_itemSignT0T1->setAlignment(Qt::AlignBottom);
	m_itemSignT0T1->setVirtualPos(QPoint(x,y));
	m_itemSignT0T1->setZValue(2);
	m_itemSignT0T1->show();
	
	m_isWaitingMoveACL = false;
	
	if( prenjugroom->rule.chRuleSet & RENJU_RULESET_HASFORBID)
	{
		m_btnJinshou = new QPushButton(tr("forbidden point"),desktop());
		m_btnJinshou->hide();
		connect(m_btnJinshou,SIGNAL(clicked()),this,SLOT(clickJinshou()));
	}else
		m_btnJinshou = 0;
	
	if(prenjugroom->rule.chRuleSet & RENJU_RULESET_EXCHANGE)
	{
		m_btnJiaohuan = new QPushButton(tr("exchange"),desktop());
		m_btnJiaohuan->hide();
		connect(m_btnJiaohuan,SIGNAL(clicked()),this,SLOT(clickJiaohuan()));
		m_btnJiaohuan->setEnabled(false);
	}else
		m_btnJiaohuan = 0;
	
	m_btnQiuhe = new QPushButton(tr("request draw"),desktop());
	m_btnQiuhe->hide();
	connect(m_btnQiuhe,SIGNAL(clicked()),this,SLOT(clickQiuhe()));
	
	m_btnRenshu = new QPushButton(tr("surrender"),desktop());
	m_btnRenshu->hide();
	connect(m_btnRenshu,SIGNAL(clicked()),this,SLOT(clickRenshu()));
	
	connect( panelController->chatWidget()->textBrowser(), SIGNAL(acceptDrawChess()), SLOT(clickQiuhe()) );
	
	DJGraphicsTextItem			*axisX[15],*axisY[15];
	QFont f;
	f.setPointSize(16);
	f.setBold(true);
		
	for ( int i = 0; i < 15; i++ ) {
		axisX[i]	= new DJGraphicsTextItem(QString(QChar::fromAscii(0x41+i)),0, desktop()->scene());
		axisX[i]->setAlignment(Qt::AlignCenter);
		axisX[i]->setFont(f);
		axisX[i]->setZValue(2);
		axisX[i]->setVirtualPos( QPoint(m_boardPos.x() + 24 + i*41, m_boardPos.y() + 10) );
		axisX[i]->show();
		axisY[i]	= new DJGraphicsTextItem(QString::number(i+1),0, desktop()->scene());
		axisY[i]->setAlignment(Qt::AlignCenter);
		axisY[i]->setFont(f);
		axisY[i]->setZValue(2);
		axisY[i]->setVirtualPos( QPoint(m_boardPos.x() + 10 , m_boardPos.y() + 24 + i*41) );
		axisY[i]->show();
	}
}
RenjuDesktopController::~RenjuDesktopController()
{
	djDebug()<<"RenjuDesktopController destructor";
}
void RenjuDesktopController::createChip(quint8 chOwner,quint8 x,quint8 y,quint8 chStep)
{
	int rx,ry;
	clearChip(x,y);
	if(x < 1 || y<1 || x > DJGAME_RENJU_WIDTH || y>DJGAME_RENJU_WIDTH)
		return;
	rx = static_cast<int>((x-1)*41+24);
	ry = static_cast<int>((y-1)*41+24);
	
	int i,j;
	for(i=1;i<=DJGAME_RENJU_WIDTH;i++)
	{
		for(j=1;j<=DJGAME_RENJU_WIDTH;j++)
		{
			if(m_chips[i][j] != 0 && m_chips[i][j]->isCurrent() && m_chips[i][j]->color() != chOwner)
				m_chips[i][j]->setCurrentChip(false);
		}
	}
	m_chips[x][y] = new RenjuChip(this,chOwner,rx,ry,chStep);
	m_chips[x][y]->setCurrentChip(true);
	if(m_bSignYN)
		m_chips[x][y]->setDisplayNumber(true);
	else
		m_chips[x][y]->setDisplayNumber(false);
	m_chips[x][y]->setNumberBase(m_chSignT0);
	m_chips[x][y]->show();
}
void RenjuDesktopController::clearChip(quint8 x,quint8 y)
{
	if(x < 1 || y<1 || x > DJGAME_RENJU_WIDTH || y>DJGAME_RENJU_WIDTH)
		return;
	
	delete m_chips[x][y];
	m_chips[x][y] = 0;
}
void RenjuDesktopController::init()
{
	memset(&m_board,0,sizeof(RenjuBoard));
	
	int i,j;
	for(i=1;i<=DJGAME_RENJU_WIDTH;i++)
	{
		for(j=1;j<=DJGAME_RENJU_WIDTH;j++)
		{
			clearChip(i,j);
		}
	}
	
	m_shSteps = 0;
	m_lastOwner = 0;
	m_stepNumbers = 0;
	memset(&m_describe,0,sizeof(RenjuStepDescribe));
	m_chSignT0 = 0;
	m_hasRequestDraw	= false;
	
	if(m_btnJinshou != NULL)
	{
		m_btnJinshou->setEnabled(false);
		m_btnJinshou->show();
	}
	if(m_btnJiaohuan != NULL)
	{
		m_btnJiaohuan->setEnabled(false);
		m_btnJiaohuan->show();
	}
	m_btnQiuhe->setEnabled(true);
	m_btnQiuhe->show();
	m_btnRenshu->setEnabled(true);
	m_btnRenshu->show();
}
void RenjuDesktopController::initUnderGameInfo( const QByteArray& buf )
{	
	DJDesktopController::initUnderGameInfo(buf);
		
	init();
	
	memcpy(&m_current,buf.data(),sizeof(RenjuCurrent));
}
void RenjuDesktopController::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
	djDebug() << "RenjuDesktopController::gameWait" << mask << status;
	DJDesktopController::gameWait(mask,status,timeout);
	
	if(status == DJGAME_TABLE_STATUS_WAITSTART)
	{
		if(m_btnJinshou)
			m_btnJinshou->hide();
		if(m_btnJiaohuan)
			m_btnJiaohuan->hide();
		m_btnQiuhe->hide();
		m_btnRenshu->hide();
	}
	if ( m_stepNumbers <= 7 )
		clearAutoDisappearText();
	switch(status)
	{
		case DJGAME_RENJU_TABLE_STATUS_WAIT_MOVE :
		{
			m_isWaitingMoveACL = false;
			if( (1<<(m_current.chBlack-1)) & mask)
			{
				m_blackTimer->start();
				m_whiteTimer->pause();
			}else if( (1<<(m_current.chWhite-1)) & mask)
			{
				m_whiteTimer->start();
				m_blackTimer->pause();
			}
			PRenjuRoom prenjuroom = (PRenjuRoom)((panelController()->gameRoom())->privateRoom());
			if((prenjuroom->rule.chRuleSet & RENJU_RULESET_EXCHANGE) && m_stepNumbers == 3
					&& panelController()->seatId() == m_current.chWhite &&
					m_current.chExchanges == 0	)
			{
				m_btnJiaohuan->setEnabled(true);;
			}else
				if(m_btnJiaohuan != NULL)
					m_btnJiaohuan->setEnabled(false);
			if((prenjuroom->rule.chRuleSet & RENJU_RULESET_HASFORBID) &&
						isWaitingForMe() && panelController()->seatId() == m_current.chWhite 
						&& m_stepNumbers >= 5)
			{
				m_btnJinshou->setEnabled(true);
			}else
				if(m_btnJinshou != NULL)
					m_btnJinshou->setEnabled(false);
			bool isRIFRules	= false;
			if(prenjuroom->rule.chRuleSet & RENJU_RULESET_HASFORBID 
				&& prenjuroom->rule.chRuleSet & RENJU_RULESET_EXCHANGE
				&& prenjuroom->rule.chRuleSet & RENJU_RULESET_DOUBLE ) {
				isRIFRules	= true;
			}
			if ( !isRIFRules )
				break;
			QString hint;
			if( panelController()->seatId() == m_current.chBlack) {
				switch(m_stepNumbers) {
				case 1:
					hint	+= tr("RIF Opening Rules:\n");
					hint	+= tr("Black Plays all the first three move\n");
					hint 	+= tr("Please play the 2nd move for WHITE");
					break;
				case 2:
					hint	+= tr("RIF Opening Rules:\n");
					hint	+= tr("Black Plays all the first three move\n");
					hint 	+= tr("Please play the 3rd move for BLACK");
					break;
				case 3:
					if ( 0 == m_current.chExchanges ) {
						hint	+= tr("RIF Opening Rules:\n");
						hint	+= tr("White now has the right to change sides\n");
						hint 	+= tr("Waiting for White's decision...");
					}else if ( 1 == m_current.chExchanges ) {
						hint	+= tr("You play Black now");;
					}
					break;
				case 5:
					hint	+= tr("RIF Opening Rules:\n");
					hint	+= tr("Black has to make two different proposals for 5th move\n");
					hint 	+= tr("Please play another correct proposal");
					break;
				default:
					break;
				}
			}else if ( panelController()->seatId() == m_current.chWhite ) {
				switch(m_stepNumbers) {
				case 1:
					hint	+= tr("RIF Opening Rules:\n");
					hint	+= tr("Black Plays all the first three move\n");
					break;
				case 2:
					hint	+= tr("RIF Opening Rules:\n");
					hint	+= tr("Black Plays all the first three move\n");
					break;
				case 3:
					if ( 0 == m_current.chExchanges ) {
						hint	+= tr("RIF Opening Rules:\n");
						hint	+= tr("White now has the right to change sides\n");
					}else if ( 1 == m_current.chExchanges ) {
						DJGameUser *user	= panelController()->player(m_current.chBlack);
						if ( user ) {
							QString name	= user->userName();
							hint	+= tr("RIF Opening Rules:\n");
							hint 	+= name + tr(" has changed sides to Black\n");
							hint 	+= tr("You play White now");
						}
					}
					break;
				case 5:
					hint	+= tr("RIF Opening Rules:\n");
					hint	+= tr("Black has to make two different proposals for 5th move\n");
					hint 	+= tr("Waiting for Black to play another correct proposal");
					break;
				default:
					break;
				}
			}
			if ( !hint.isEmpty() )
				panelController()->insertText2Browser(hint);
			break;
		}
		case DJGAME_RENJU_TABLE_STATUS_WAIT_SELECT :
		{
			bool isRIFRules	= false;
			PRenjuRoom prenjuroom = (PRenjuRoom)((panelController()->gameRoom())->privateRoom());
			if(prenjuroom->rule.chRuleSet & RENJU_RULESET_HASFORBID 
				&& prenjuroom->rule.chRuleSet & RENJU_RULESET_EXCHANGE
				&& prenjuroom->rule.chRuleSet & RENJU_RULESET_DOUBLE ) {
				isRIFRules	= true;
			}
			if ( !isRIFRules )
				break;
			QString hint;
			if(panelController()->seatId() == m_current.chWhite)
			{
				hint	+= tr("RIF Opening Rules:\n");
				hint	+= tr("Please choose one of two proposals");
				
			}else if (panelController()->seatId() == m_current.chBlack) {
				hint	+= tr("RIF Opening Rules:\n");
				hint	+= tr("Waiting for White's decision...");
			}
			if ( !hint.isEmpty() )
				panelController()->insertText2Browser(hint);
			break;
		}
		default:
			break;
	}
	
}
void RenjuDesktopController::gameTraceModel( const GeneralGameTrace2Head *gameTrace)
{
	switch(gameTrace->chType)
	{
		case RENJU_GAMETRACE_SETRULE :
		{
			RenjuRule *prule = (RenjuRule *)gameTrace->chBuf;
			quint8 chOwner = (gameTrace->chSite == m_current.chWhite)?DJGAME_RENJU_OWNER_WHITE:DJGAME_RENJU_OWNER_BLACK;
			SetPlayerTimer(chOwner,&(prule->timer));
			break;
		}
		case RENJU_GAMETRACE_MOVE :
		{
			PRenjuMove pmove = (PRenjuMove)gameTrace->chBuf;
			quint8 chOwner = (gameTrace->chSite == m_current.chWhite)?DJGAME_RENJU_OWNER_WHITE:DJGAME_RENJU_OWNER_BLACK;
			RenjuRule_SetNodeChip(&m_board,pmove->x,pmove->y,chOwner);
			if(chOwner != m_lastOwner)
				m_shSteps ++;
			m_lastOwner = chOwner;
			m_steps[m_stepNumbers][0] = pmove->x;
			m_steps[m_stepNumbers][1] = pmove->y;
			m_stepNumbers ++;
			break;
		}
		case RENJU_GAMETRACE_DESCRIBE :
		{
			PRenjuStepDescribe pdes = (PRenjuStepDescribe)gameTrace->chBuf;
			memcpy(&m_describe,pdes,sizeof(RenjuStepDescribe));
			break;
		}
		case RENJU_GAMETRACE_EXCHANGE :
		{
			quint8 temp = m_current.chBlack;
			m_current.chBlack = m_current.chWhite;
			m_current.chWhite = temp;
			m_current.chExchanges ++;
			break;
		}
		case RENJU_GAMETRACE_REMOVE :
		{
			PRenjuMove pmove = (PRenjuMove)gameTrace->chBuf;
			RenjuRule_SetNodeChip(&m_board,pmove->x,pmove->y,0);
			break;
		}
	}
}
void RenjuDesktopController::gameTraceView( const GeneralGameTrace2Head *gameTrace )
{
	djDebug()<<"RenjuDesktopController::gameTraceView";
	
	switch(gameTrace->chType)
	{
		case RENJU_GAMETRACE_SETRULE :
		{
			break;
		}
		case RENJU_GAMETRACE_MOVE :
		{
			PRenjuMove pmove = (PRenjuMove)gameTrace->chBuf;
			quint8 chOwner = RenjuRule_GetNodeChip(&m_board,pmove->x,pmove->y);
			createChip(chOwner,pmove->x,pmove->y,m_shSteps);
			playWave("click.wav");
			break;
		}
		case RENJU_GAMETRACE_DESCRIBE :
		{
			PRenjuStepDescribe pdes = (PRenjuStepDescribe)gameTrace->chBuf;
			if(m_chips[pdes->x][pdes->y] != NULL)
			{
				m_chips[pdes->x][pdes->y]->setCurrentImage(pdes->des);
			}
			break;
		}
		case RENJU_GAMETRACE_EXCHANGE :
		{
			panelController()->repaintAllPlayerItems();
			break;
		}
		case RENJU_GAMETRACE_REMOVE :
		{
			PRenjuMove pmove = (PRenjuMove)gameTrace->chBuf;
			clearChip(pmove->x,pmove->y);
			break;
		}
		case RENJU_GAMETRACE_REQUESTDRAW: {
			/*
			DJGameUser *puser = c_RenjuPanel->userAtSeat(gameTrace->chSite);
			if( puser ) {
    			QString text	= puser->userName();
    			if ( m_hasRequestDraw ) {
    				text	+= tr(" accept draw");
    			}else {
    				text	+= tr(" request draw");
    			}
    			c_RenjuPanel->insertText2Browser(text);
				ShowAutoDisappearText(text);
				
				if ( gameTrace->chSite != c_RenjuPanel->selfSeatId()
					&& !m_hasRequestDraw ) {
					text	= DJSCHEME_AHREF
								.arg( DJSCHEME )
								.arg( DJSCHEME_EVENT_ACCEPT_DRAW_CHESS )
								.arg( puser->userId() )
								.arg( DJSCHEME_HOST )
								.arg( tr("Accept") );
					c_RenjuPanel->insertText2Browser( text );
				}
				m_hasRequestDraw	= true;
			}
			*/
			DJGameUser *puser = panelController()->player(gameTrace->chSite);
			if( puser ) {
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
			break;
		}
		default:
			break;
	}
}
void RenjuDesktopController::signYN()
{
	QPixmap pix;
	m_bSignYN = !m_bSignYN;
	if(m_bSignYN)
	{
		pix = QPixmap(":/RenjuRes/image/sign_n.png");
	}else
	{
		pix = QPixmap(":/RenjuRes/image/sign_y.png");
	}
	m_itemSignYN->setPixmap(pix);
	signChips(m_bSignYN);
}
void RenjuDesktopController::signChips(bool flag)
{
	int i,j;
	for(i=1;i<=DJGAME_RENJU_WIDTH;i++)
	{
		for(j=1;j<=DJGAME_RENJU_WIDTH;j++)
		{
			if(m_chips[i][j] != NULL)
				m_chips[i][j]->setDisplayNumber(flag);
		}
	}
}
void RenjuDesktopController::signT0T1()
{
	QPixmap pix;
	if(m_chSignT0 == 0 && m_shSteps > 0)
		m_chSignT0 = m_shSteps-1;
	else
		m_chSignT0 = 0;
	if(m_chSignT0 == 0)
	{
		pix = QPixmap(":/RenjuRes/image/sign_t1.png");
	}else
	{
		pix = QPixmap(":/RenjuRes/image/sign_t0.png");
	}
	m_itemSignT0T1->setPixmap(pix);
	setChipBaseNumber(m_chSignT0);
	if(!m_bSignYN)
		signYN();
}
void RenjuDesktopController::setChipBaseNumber(quint16 base)
{
	int i,j;
	for(i=1;i<=DJGAME_RENJU_WIDTH;i++)
	{
		for(j=1;j<=DJGAME_RENJU_WIDTH;j++)
		{
			if(m_chips[i][j] != NULL)
				m_chips[i][j]->setNumberBase(base);
		}
	}
}
void RenjuDesktopController::handleMousePressed( QGraphicsSceneMouseEvent *mouseEvent )
{
	djDebug()<<"RenjuDesktopController::handleMousePressed";
	QPointF scenePos	= mouseEvent->scenePos();
	QPointF pos	= desktop()->graphicsMatrix().inverted().map(scenePos);
	QGraphicsItem *item	= desktop()->desktopScene()->itemAt( scenePos );
	
	if(item == m_itemSignYN && mouseEvent->button() == Qt::LeftButton)
	{//是否显示数字
		signYN();
		return;
	}
	if(item == m_itemSignT0T1 && mouseEvent->button() == Qt::LeftButton)
	{
		signT0T1();
		return;
	}
	if(gameWaitStatus() == DJGAME_RENJU_TABLE_STATUS_WAIT_SELECT 
		&& isWaitingForMe() && !m_isWaitingMoveACL)
	{//二选一
		djDebug()<<"select";
		char chSel = 0;
		if(m_chips[m_steps[m_stepNumbers-1][0]][m_steps[m_stepNumbers-1][1]]->isMe(item))
			chSel = 2;
		if(m_chips[m_steps[m_stepNumbers-2][0]][m_steps[m_stepNumbers-2][1]]->isMe(item))
			chSel = 1;
		if(chSel != 0)
		{
			sendGameTrace(RENJU_GAMETRACE_SELECT,QByteArray(&chSel,1));
			m_isWaitingMoveACL = true;
			return;
		}
	}
	if(item == m_itemBoard && mouseEvent->button() == Qt::LeftButton)
	{//点击棋盘
		djDebug()<<"put chip";
		if(gameWaitStatus() != DJGAME_RENJU_TABLE_STATUS_WAIT_MOVE 
			|| !isWaitingForMe() || m_isWaitingMoveACL)
		{
			return;
		}
		djDebug()<<"go";
		//走棋
		int x,y;
		x = static_cast<int>((pos.x()-m_boardPos.x()+37)/41);
		y = static_cast<int>((pos.y()-m_boardPos.y()+37)/41);
		printf("board(%d,%d)\n",x,y);
		if(x < 1 || x > DJGAME_RENJU_WIDTH || y<1 || y> DJGAME_RENJU_WIDTH)
			return;
		RenjuMove move;
		move.x = (unsigned char)x;
		move.y = (unsigned char)y;
		m_isWaitingMoveACL = true;
		QVariant parameters	= QVariant::fromValue((void*)this);
		sendGameTrace(RENJU_GAMETRACE_MOVE,QByteArray((char *)&move,sizeof(RenjuMove)),HandleMoveACL,parameters);
		return;
	}
}
void RenjuDesktopController::repaintCurrentStatus()
{
	djDebug()<<"RenjuDesktopController::repaintCurrentStatus";
	
	quint8 chStep=0;
	quint8 chOwner;
	
	while(chStep<m_stepNumbers)
	{
		clearChip(m_steps[chStep][0],m_steps[chStep][1]);
		chOwner = RenjuRule_GetNodeChip(&m_board,m_steps[chStep][0],m_steps[chStep][1]);
		if(chOwner >=1 && chOwner <=2 )
		{
			createChip(chOwner,m_steps[chStep][0],m_steps[chStep][1],chStep+1);
		}
		chStep ++;
	}
	if(m_describe.x > 0 && m_describe.y>0 && m_describe.x<=DJGAME_RENJU_WIDTH && 
		m_describe.y<=DJGAME_RENJU_WIDTH )
	{
		if(m_chips[m_describe.x][m_describe.y] != NULL)
		{
			m_chips[m_describe.x][m_describe.y]->setCurrentImage(m_describe.des);
		}
	}
}
void RenjuDesktopController::clickJinshou()
{
	if(panelController()->isLookingOn())
		return;
	PRenjuRoom prenjuroom = (PRenjuRoom)((panelController()->gameRoom())->privateRoom());
	if(!(prenjuroom->rule.chRuleSet & RENJU_RULESET_HASFORBID) )
		return;
	if(panelController()->seatId() == m_current.chBlack)
		return;
	if(gameWaitStatus() != DJGAME_RENJU_TABLE_STATUS_WAIT_MOVE || !isWaitingForMe())
		return;
	sendGameTrace(RENJU_GAMETRACE_BEFORBID);
}
void RenjuDesktopController::clickJiaohuan()
{
	if(panelController()->isLookingOn())
		return;
	PRenjuRoom prenjuroom = (PRenjuRoom)((panelController()->gameRoom())->privateRoom());
	if(!(prenjuroom->rule.chRuleSet & RENJU_RULESET_EXCHANGE) || m_stepNumbers != 3)
		return ;
	sendGameTrace(RENJU_GAMETRACE_EXCHANGE);
	m_btnJiaohuan->setEnabled(false);
}
void RenjuDesktopController::clickQiuhe()
{
	if(panelController()->isLookingOn())
		return;
	sendGameTrace(RENJU_GAMETRACE_REQUESTDRAW);
}
void RenjuDesktopController::clickRenshu()
{
	if(panelController()->isLookingOn())
		return;
	sendGameTrace(RENJU_GAMETRACE_SURRENDER);
	m_btnRenshu->setEnabled(false);
}
void RenjuDesktopController::SetPlayerTimer(quint8 chOwner,PDJGameChessTimer rtimer)
{
	QPoint pa,pn,pt;
	quint8 chView,chSite=0;
	DJGraphicsChessTimerItem *curtimer=0;
	if(chOwner == DJGAME_RENJU_OWNER_BLACK)
	{
		m_blackTimer->setTimer(rtimer);
		curtimer = m_blackTimer;
		chSite = m_current.chBlack;
	}else if(chOwner == DJGAME_RENJU_OWNER_WHITE)
	{
		m_whiteTimer->setTimer(rtimer);
		curtimer = m_whiteTimer;
		chSite = m_current.chWhite;
	}
	
	if(chSite != 0)
	{
		chView = seat2View(chSite);
		if(chView == 1)
		{
			QRect showRect	= rectOfSeatShow( chSite );
			djDebug() << "view 1 showrect" << showRect;
			curtimer->setUp2Down( false );
			curtimer->setVirtualPos( QPointF(showRect.center().x(), showRect.top()) );
			curtimer->setExternalScale( desktop()->graphicsScale() );
			curtimer->adjustPos( desktop()->graphicsMatrix() );
		}else if(chView == 3)
		{
			QRect showRect	= rectOfSeatShow( chSite );
			djDebug() << "view 3 showrect" << showRect;
			curtimer->setUp2Down( true );
			curtimer->setVirtualPos( QPointF(showRect.center().x(), showRect.bottom()) );
			curtimer->setExternalScale( desktop()->graphicsScale() );
			curtimer->adjustPos( desktop()->graphicsMatrix() );
		}
	}
}
void RenjuDesktopController::handleCountdown(unsigned char countdown)
{
	QString wavName	= QString("number%1.wav").arg(countdown);
	playWave(wavName);
}
void RenjuDesktopController::locateAllNonMatrimoveItems()
{
	DJDesktopController::locateAllNonMatrimoveItems();
	
	//QPoint pos( desktop()->realWidth() - 60, desktop()->realHeight() - 30 );
	QPoint pos( 0, desktop()->realHeight() - 30 );
	QMatrix matrix	= desktop()->graphicsMatrix();
	pos	= matrix.map( pos );
	
	int space	= 4;
	int x	= pos.x();
	int y	= pos.y();
	
	if ( m_btnJinshou ) {
		//m_btnJinshou->move( x - m_btnJinshou->width(), y );
		m_btnJinshou->move( x, y );
		x	+= m_btnJinshou->width() + space;
	}
	if ( m_btnJiaohuan ) {
		//m_btnJiaohuan->move( x - m_btnJiaohuan->width(), y );
		m_btnJiaohuan->move( x, y );
		x	+= m_btnJiaohuan->width() + space;
	}
	//m_btnQiuhe->move( x - m_btnQiuhe->width(), y );
	m_btnQiuhe->move( x, y );
	x	+= m_btnQiuhe->width() + space;
	
	//m_btnRenshu->move( x - m_btnRenshu->width(), y );
	m_btnRenshu->move( x, y );
	x	+= m_btnRenshu->width() + space;
}
void RenjuDesktopController::locateStartButton()
{
	QMatrix matrix	= desktop()->graphicsMatrix();
	QPixmap pix = QPixmap(":/RenjuRes/image/board.png");
	int x = pix.width()>>1;
	int y = (desktop()->realHeight()>>1);
	QPoint pos(x,y);
	pos	= matrix.map( pos );
	pos.rx()	-= startButton()->width()>>1;
	startButton()->move( pos );
}
