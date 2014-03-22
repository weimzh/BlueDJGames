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
#include "Renju.h"
#include "RenjuRule.h"
#include "gobangprofile.h"
#include "DJAutoDisappearText.h"
#include "DJGamePlayerItem.h"
#include "DJMessageBox.h"
#include "DJMatrixCanvasTimer.h"
#include "DJSchemeEvent.h"
#include "DJGroupChat.h"
#include "DJTextBrowser.h"
#include "DJGlobal.h" 

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
RenjuDesktop::RenjuDesktop(QWidget* parent,DJGamePanel *panel,quint32 w,quint32 h)
			: DJGameDesktop(parent,panel,w,h)
{
	memset(&c_current,0,sizeof(RenjuCurrent));
	memset(&c_board,0,sizeof(RenjuBoard));
	memset(c_chips,0,sizeof(RenjuChip*)*(DJGAME_RENJU_WIDTH+1)*(DJGAME_RENJU_WIDTH+1));
	
	c_RenjuPanel = (RenjuPanel*)panel;
	
	connect(this,SIGNAL(gameStarted()),this,SLOT(PlayerStarted()));
	QMatrix m = GetCurrentMatrix();
	QPixmap pix = QPixmap(QString(":/RenjuRes/image/gobang_board.png"));
	QImage img = pix.convertToImage();
	c_itemBoard = new DJGameImageItem(img,canvas(),m,RENJU_DESKTOP_TID_BOARD,0,RENJU_DESKTOP_TYPE_BOARD,false);
	
	c_itemBoard->move(0,(h-c_itemBoard->height())>>1);
	c_itemBoard->setZ(1);
	c_itemBoard->show();
	
	PRenjuRoom pgobangroom = (PRenjuRoom)((c_RenjuPanel->gameRoom())->privateRoom());
	c_blackTimer = NULL;
	c_whiteTimer = NULL;
	c_current.chBlack = 1;
	c_current.chWhite = 2;
	SetPlayerTimer(DJGAME_RENJU_OWNER_BLACK,&(pgobangroom->rule.timer));
	SetPlayerTimer(DJGAME_RENJU_OWNER_WHITE,&(pgobangroom->rule.timer));
	
	//QPoint pa,pn,pt;
	//GetAvatarPosition(1,&pa,&pn,&pt);
	//int x = pa.x()+((GetRealWidth()-pa.x())>>1);
	//int y = GetRealHeight()>>1;
	QRect rect	= idealRectOfShow( c_RenjuPanel->selfSeatId() );
	int x = rect.left()+((GetRealWidth()-rect.left())>>1);
	int y = (GetRealHeight()>>1) - 80;
	
	c_bSignYN = false;
	c_chSignT0 = 0;
	
	if(c_bSignYN)
		pix = QPixmap(QString(":/RenjuRes/image/sign_n.png"));
	else
		pix = QPixmap(QString(":/RenjuRes/image/sign_y.png"));
	img = pix.convertToImage();
	c_itemSignYN = new DJGameImageItem(img,canvas(),m,RENJU_DESKTOP_TID_BUTTON_SIGNYN,3,RENJU_DESKTOP_TYPE_BUTTON,false);
	c_itemSignYN->setVAlignment(Qt::AlignBottom);
	c_itemSignYN->setHAlignment(Qt::AlignRight);
	c_itemSignYN->move(x,y);
	c_itemSignYN->setZ(2);
	c_itemSignYN->show();
	
	if(c_chSignT0 == 0)
		pix = QPixmap(QString(":/RenjuRes/image/sign_t1.png"));
	else
		pix = QPixmap(QString(":/RenjuRes/image/sign_t0.png"));
	img = pix.convertToImage();
	c_itemSignT0T1 = new DJGameImageItem(img,canvas(),m,RENJU_DESKTOP_TID_BUTTON_SIGNYN,3,RENJU_DESKTOP_TYPE_BUTTON,false);
	c_itemSignT0T1->setVAlignment(Qt::AlignBottom);
	c_itemSignT0T1->move(x,y);
	c_itemSignT0T1->setZ(2);
	c_itemSignT0T1->show();
	
	c_b25Info = true;
	c_chAutoForbid = 0;
	if( pgobangroom->rule.chRuleSet & RENJU_RULESET_HASFORBID)
	{
		c_pbForbid = new QPushButton(this);
		QPixmap	pix = QPixmap(":/BaseRes/image/desktop/pushbutton/ShiJinShou.png");
		c_pbForbid->setIcon( pix );
		c_pbForbid->setIconSize( pix.size() );
		c_pbForbid->adjustSize();
		c_pbForbid->hide();
		connect(c_pbForbid,SIGNAL(clicked()),this,SLOT(ClickForbid()));
	}else
		c_pbForbid = NULL;
	
	if(pgobangroom->rule.chRuleSet & RENJU_RULESET_EXCHANGE)
	{
		c_pbExchange = new QPushButton(this);
		QPixmap	pix = QPixmap(":/BaseRes/image/desktop/pushbutton/JiaoHuan.png");
		c_pbExchange->setIcon( pix );
		c_pbExchange->setIconSize( pix.size() );
		c_pbExchange->adjustSize();
		c_pbExchange->hide();
		connect(c_pbExchange,SIGNAL(clicked()),this,SLOT(ClickExchange()));
		c_pbExchange->setEnabled(false);
	}else
		c_pbExchange = NULL;
	
	c_pbReqDraw = new QPushButton(this);
	pix = QPixmap(":/BaseRes/image/desktop/pushbutton/YiHe.png");
	c_pbReqDraw->setIcon( pix );
	c_pbReqDraw->setIconSize( pix.size() );
	c_pbReqDraw->adjustSize();
	c_pbReqDraw->hide();
	connect(c_pbReqDraw,SIGNAL(clicked()),this,SLOT(ClickReqDraw()));
	
	c_pbSurrender = new QPushButton(this);
	pix = QPixmap(":/BaseRes/image/desktop/pushbutton/RenShu.png");
	c_pbSurrender->setIcon( pix );
	c_pbSurrender->setIconSize( pix.size() );
	c_pbSurrender->adjustSize();
	c_pbSurrender->hide();
	connect(c_pbSurrender,SIGNAL(clicked()),this,SLOT(ClickSurrender()));
	
	c_bWaitMoveACL = false;
	
	if(pgobangroom->rule.chRuleSet & RENJU_RULESET_HASFORBID 
		&& pgobangroom->rule.chRuleSet & RENJU_RULESET_EXCHANGE
		&& pgobangroom->rule.chRuleSet & RENJU_RULESET_DOUBLE ) {
		m_isRIFRules	= true;
	}else
		m_isRIFRules	= false;
	
	for ( int i = 0; i < 15; i++ ) {
		m_axisX[i]	= new DJGameTextItem(QString(QChar::fromAscii(0x41+i)),canvas(),m,0,0,RENJU_DESKTOP_TYPE_AXIS);
		m_axisX[i]->setHAlignment(Qt::AlignHCenter);
		m_axisX[i]->setVAlignment(Qt::AlignVCenter);
		QFont f	= m_axisX[i]->font();
		f.setPointSize(16);
		f.setBold(true);
		m_axisX[i]->setFont(f);
		m_axisX[i]->setZ(2);
		m_axisX[i]->move(c_itemBoard->x() + 24 + i*41, c_itemBoard->y() + 10);
		m_axisX[i]->show();
		m_axisY[i]	= new DJGameTextItem(QString::number(i+1),canvas(),m,0,0,RENJU_DESKTOP_TYPE_AXIS);
		m_axisY[i]->setHAlignment(Qt::AlignHCenter);
		m_axisY[i]->setVAlignment(Qt::AlignVCenter);
		m_axisY[i]->setFont(f);
		m_axisY[i]->setZ(2);
		m_axisY[i]->move( c_itemBoard->x() + 10 , c_itemBoard->y() + 24 + i*41 );
		m_axisY[i]->show();
	}
}

void RenjuDesktop::ClickForbid()
{
	if(c_RenjuPanel->isLookingOn())
		return;
	PRenjuRoom pgobangroom = (PRenjuRoom)((c_RenjuPanel->gameRoom())->privateRoom());
	if(!(pgobangroom->rule.chRuleSet & RENJU_RULESET_HASFORBID) )
		return;
	if(c_RenjuPanel->selfSeatId() == c_current.chBlack)
		return;
	if(GetTableStatus() != DJGAME_RENJU_TABLE_STATUS_WAIT_MOVE || !IsWaittingForMe())
		return;
	
	SendGameTrace(RENJU_GAMETRACE_BEFORBID);
}

void RenjuDesktop::ClickExchange()
{
	if(c_RenjuPanel->isLookingOn())
		return;
	PRenjuRoom pgobangroom = (PRenjuRoom)((c_RenjuPanel->gameRoom())->privateRoom());
	if(!(pgobangroom->rule.chRuleSet & RENJU_RULESET_EXCHANGE) || c_StepNumbers != 3)
		return ;
	SendGameTrace(RENJU_GAMETRACE_EXCHANGE);
	c_pbExchange->setEnabled(false);
}

void RenjuDesktop::ClickReqDraw()
{
	if(c_RenjuPanel->isLookingOn())
		return;
	QVariant parameters	= QVariant::fromValue((void*)c_pbReqDraw);
	SendGameTrace(RENJU_GAMETRACE_REQUESTDRAW,NULL,0,aclRequestDrawProc,parameters);
}
void RenjuDesktop::ClickSurrender()
{
	if(c_RenjuPanel->isLookingOn())
		return;
	SendGameTrace(RENJU_GAMETRACE_SURRENDER);
	c_pbSurrender->setEnabled(false);
}

void RenjuDesktop::handleMatrixChanged(const QMatrix &m)
{
	DJGameDesktop::handleMatrixChanged(m);
	
	/*
	QRect rect	= idealRectOfShow( c_RenjuPanel->selfSeatId() );
	int tx,ty;
	m.map(rect.left(),GetRealHeight()>>1,&tx,&ty);
	*/
	int x,y,tx,ty;
	
	x	= c_itemBoard->width()>>1;
	y	= GetRealHeight()>>1;
	m.map(x,y,&tx,&ty);
	startButton()->move(tx-(startButton()->width()>>1),ty );
	
	x	= c_itemBoard->width() - 10;
	y	= GetRealHeight();
	m.map(x,y,&tx,&ty);
	int spacing	 = 12;
	
	if(c_pbForbid != NULL)
	{
		tx	= tx-c_pbForbid->width() - spacing;
		c_pbForbid->move(tx,ty - c_pbForbid->height() );
	}
	
	if(c_pbExchange != NULL)
	{
		tx	= tx-c_pbExchange->width() - spacing;
		c_pbExchange->move(tx,ty - c_pbExchange->height() );
	}
	
	tx	= tx - c_pbReqDraw->width() - spacing;
	c_pbReqDraw->move(tx,ty - c_pbReqDraw->height() );
	
	tx	= tx - c_pbSurrender->width() - spacing;
	c_pbSurrender->move(tx,ty - c_pbSurrender->height() );
}

void RenjuDesktop::StaticInitDesktop()
{
	memset(&c_board,0,sizeof(RenjuBoard));
	int i,j;
	for(i=1;i<=DJGAME_RENJU_WIDTH;i++)
	{
		for(j=1;j<=DJGAME_RENJU_WIDTH;j++)
		{
			ClearChip(i,j);
		}
	}
	
	c_iNumberBase = 0;
	c_shSteps = 0;
	c_LastOwner = 0;
	c_StepNumbers = 0;
	memset(&c_describe,0,sizeof(RenjuStepDescribe));
	c_chSignT0 = 0;
	
	if(c_pbForbid != NULL)
	{
		c_pbForbid->setEnabled(false);
		c_pbForbid->show();
	}
	if(c_pbExchange != NULL)
	{
		c_pbExchange->setEnabled(false);
		c_pbExchange->show();
	}
	c_pbReqDraw->setEnabled(true);
	c_pbReqDraw->show();
	c_pbSurrender->setEnabled(true);
	c_pbSurrender->show();
	m_hasRequestDraw	= false;
}

void RenjuDesktop::StaticGameTrace(GeneralGameTrace2Head *gameTrace)
{
	switch(gameTrace->chType)
	{
		case RENJU_GAMETRACE_SETRULE :
		{
			RenjuRule *prule = (RenjuRule *)gameTrace->chBuf;
			quint8 chOwner = (gameTrace->chSite == c_current.chWhite)?DJGAME_RENJU_OWNER_WHITE:DJGAME_RENJU_OWNER_BLACK;
			SetPlayerTimer(chOwner,&(prule->timer));
			break;
		}
		case RENJU_GAMETRACE_MOVE :
		{
			PRenjuMove pmove = (PRenjuMove)gameTrace->chBuf;
			quint8 chOwner = (gameTrace->chSite == c_current.chWhite)?DJGAME_RENJU_OWNER_WHITE:DJGAME_RENJU_OWNER_BLACK;
			RenjuRule_SetNodeChip(&c_board,pmove->x,pmove->y,chOwner);
			if(chOwner != c_LastOwner)
				c_shSteps ++;
			c_LastOwner = chOwner;
			c_Steps[c_StepNumbers][0] = pmove->x;
			c_Steps[c_StepNumbers][1] = pmove->y;
			c_StepNumbers ++;
			break;
		}
		case RENJU_GAMETRACE_DESCRIBE :
		{
			PRenjuStepDescribe pdes = (PRenjuStepDescribe)gameTrace->chBuf;
			memcpy(&c_describe,pdes,sizeof(RenjuStepDescribe));
			break;
		}
		case RENJU_GAMETRACE_EXCHANGE :
		{
			quint8 temp = c_current.chBlack;
			c_current.chBlack = c_current.chWhite;
			c_current.chWhite = temp;
			c_current.chExchanges ++;
			break;
		}
		case RENJU_GAMETRACE_REMOVE :
		{
			PRenjuMove pmove = (PRenjuMove)gameTrace->chBuf;
			RenjuRule_SetNodeChip(&c_board,pmove->x,pmove->y,0);
			break;
		}
	}
}

void RenjuDesktop::RepaintCurrentStatus()
{
//	RenjuBoard		board;
//	memcpy(&board,&c_board,sizeof(RenjuBoard));
	quint8 chStep=0;
	quint8 chOwner;
	
	while(chStep<c_StepNumbers)
	{
		ClearChip(c_Steps[chStep][0],c_Steps[chStep][1]);
		chOwner = RenjuRule_GetNodeChip(&c_board,c_Steps[chStep][0],c_Steps[chStep][1]);
		if(chOwner >=1 && chOwner <=2 )
		{
			CreateChip(chOwner,c_Steps[chStep][0],c_Steps[chStep][1],chStep+1);
		}
		chStep ++;
	}
	if(c_describe.x > 0 && c_describe.y>0 && c_describe.x<=DJGAME_RENJU_WIDTH && 
		c_describe.y<=DJGAME_RENJU_WIDTH )
	{
		if(c_chips[c_describe.x][c_describe.y] != NULL)
		{
			c_chips[c_describe.x][c_describe.y]->changeCurrentImage(c_describe.des);
		}
	}
}

void RenjuDesktop::PlayerStarted()
{
	//StaticInitDesktop();
}
RenjuDesktop::~RenjuDesktop()
{
	if(c_blackTimer != NULL)
		delete c_blackTimer;
	if(c_whiteTimer != NULL)
		delete c_whiteTimer;
}
QString	RenjuDesktop::playerItemNameSuffix( DJGameUser* user )
{
	if ( c_current.chWhite && c_current.chWhite == user->seatId() )
		return tr("(white)");
	else if ( c_current.chBlack && c_current.chBlack == user->seatId() )
		return tr("(black)");
	else
		return QString();
}
void RenjuDesktop::gameInfo( const unsigned char *buf )
{
	DJGameDesktop::gameInfo( buf );
	
	StaticInitDesktop();
	memcpy(&c_current,buf,sizeof(RenjuCurrent));
	SetPlayerTimer(DJGAME_RENJU_OWNER_BLACK,&(c_current.blackrule.timer));
	SetPlayerTimer(DJGAME_RENJU_OWNER_WHITE,&(c_current.whiterule.timer));
	
	SetTotalGames(letoh2(c_current.shTotalGames));	
}
void RenjuDesktop::gameWait( quint16 mask, quint8 status ,quint16 timeout)
{
//	if(status == DJGAME_RENJU_TABLE_STATUS_WAIT_MOVE)
//		SetTimerText(false);
//	else
//		SetTimerText(true);
	djDebug() << "RenjuDesktop::gameWait" << mask << status;
	djDebug() << "c_StepNumbers = " << c_StepNumbers;
	DJGameDesktop::gameWait(mask,status,timeout);
	if(status == DJGAME_TABLE_STATUS_WAITSTART)
	{
		if(c_pbForbid != NULL)
			c_pbForbid->hide();
		if(c_pbExchange != NULL)
			c_pbExchange->hide();
		c_pbReqDraw->hide();
		c_pbSurrender->hide();
	}
	if ( c_StepNumbers <= 7 )
		ClearAutoDisappearText();
	switch(status)
	{
		case DJGAME_RENJU_TABLE_STATUS_WAIT_MOVE :
		{
			c_bWaitMoveACL = false;
			if( (1<<(c_current.chBlack-1)) & mask)
			{
				c_blackTimer->startTimer();
				c_whiteTimer->pauseTimer();
			}else if( (1<<(c_current.chWhite-1)) & mask)
			{
				c_whiteTimer->startTimer();
				c_blackTimer->pauseTimer();
			}
			PRenjuRoom pgobangroom = (PRenjuRoom)((c_RenjuPanel->gameRoom())->privateRoom());
			if((pgobangroom->rule.chRuleSet & RENJU_RULESET_EXCHANGE) && c_StepNumbers == 3
					&& c_RenjuPanel->selfSeatId() == c_current.chWhite &&
					c_current.chExchanges == 0	)
			{
				c_pbExchange->setEnabled(true);;
			}else
				if(c_pbExchange != NULL)
					c_pbExchange->setEnabled(false);
			if((pgobangroom->rule.chRuleSet & RENJU_RULESET_HASFORBID) &&
						IsWaittingForMe() && c_RenjuPanel->selfSeatId() == c_current.chWhite 
						&& c_StepNumbers >= 5)
			{
				c_pbForbid->setEnabled(true);
			}else
				if(c_pbForbid != NULL)
					c_pbForbid->setEnabled(false);
			
			/*
			if( (1<<(c_current.chBlack-1))== mask  && c_LastOwner == DJGAME_RENJU_OWNER_BLACK
				&& c_RenjuPanel->selfSeatId() == c_current.chBlack)
			{
			}
			*/
			if ( !m_isRIFRules )
				break;
			QString hint;
			if( c_RenjuPanel->selfSeatId() == c_current.chBlack) {
				switch(c_StepNumbers) {
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
					if ( 0 == c_current.chExchanges ) {
						hint	+= tr("RIF Opening Rules:\n");
						hint	+= tr("White now has the right to change sides\n");
						hint 	+= tr("Waiting for White's decision...");
					}else if ( 1 == c_current.chExchanges ) {
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
			}else if ( c_RenjuPanel->selfSeatId() == c_current.chWhite ) {
				switch(c_StepNumbers) {
				case 1:
					hint	+= tr("RIF Opening Rules:\n");
					hint	+= tr("Black Plays all the first three move\n");
					break;
				case 2:
					hint	+= tr("RIF Opening Rules:\n");
					hint	+= tr("Black Plays all the first three move\n");
					break;
				case 3:
					if ( 0 == c_current.chExchanges ) {
						hint	+= tr("RIF Opening Rules:\n");
						hint	+= tr("White now has the right to change sides\n");
					}else if ( 1 == c_current.chExchanges ) {
						quint32 uid;
						uid = c_RenjuPanel->userIdOfSeat(c_current.chBlack);
						DJGameUser *user	= c_RenjuPanel->gameUser(uid);
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
			if ( !hint.isEmpty() ) {
				ShowAutoDisappearText( hint );
			}
			break;
		}
		case DJGAME_RENJU_TABLE_STATUS_WAIT_SELECT :
		{
			if ( !m_isRIFRules )
				break;
			QString hint;
			if(c_RenjuPanel->selfSeatId() == c_current.chWhite)
			{
				hint	+= tr("RIF Opening Rules:\n");
				hint	+= tr("Please choose one of two proposals");
				
			}else if (c_RenjuPanel->selfSeatId() == c_current.chBlack) {
				hint	+= tr("RIF Opening Rules:\n");
				hint	+= tr("Waiting for White's decision...");
			}
			if ( !hint.isEmpty() ) {
				ShowAutoDisappearText( hint );
			}
			break;
		}
	}
}
void RenjuDesktop::gameTrace( const GeneralGameTrace2Head *gameTrace )
{
	DJGameDesktop::gameTrace(gameTrace);
	GeneralGameTrace2Head *ptrace = (PGeneralGameTrace2Head)gameTrace;
	StaticGameTrace(ptrace);
	switch(gameTrace->chType)
	{
		case RENJU_GAMETRACE_SETRULE :
		{
			break;
		}
		case RENJU_GAMETRACE_MOVE :
		{
			PRenjuMove pmove = (PRenjuMove)gameTrace->chBuf;
			quint8 chOwner = RenjuRule_GetNodeChip(&c_board,pmove->x,pmove->y);
			CreateChip(chOwner,pmove->x,pmove->y,c_shSteps);
			//RepaintCurrentStatus();
			c_RenjuPanel->playWave("click.wav");
			break;
		}
		case RENJU_GAMETRACE_DESCRIBE :
		{
			PRenjuStepDescribe pdes = (PRenjuStepDescribe)gameTrace->chBuf;
			if(c_chips[pdes->x][pdes->y] != NULL)
			{
				c_chips[pdes->x][pdes->y]->changeCurrentImage(pdes->des);
			}
			break;
		}
		case RENJU_GAMETRACE_EXCHANGE :
		{
			c_RenjuPanel->repaintAllPlayerItems();
			break;
		}
		case RENJU_GAMETRACE_REMOVE :
		{
			PRenjuMove pmove = (PRenjuMove)gameTrace->chBuf;
			ClearChip(pmove->x,pmove->y);
			break;
		}
		case RENJU_GAMETRACE_REQUESTDRAW: {
			//if ( c_RenjuPanel->selfSeatId() != gameTrace->chSite )
			//ShowAutoDisappearText(tr("The rival request to draw\nIf you agree,click the draw button"));
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
			break;
		}
		default:
			break;
	}
}

RenjuChip * RenjuDesktop::isChip(Q3CanvasItem * item)
{
	if(item == NULL)
		return NULL;
	
	int i,j;
	for(i=1;i<=DJGAME_RENJU_WIDTH;i++)
	{
		for(j=1;j<=DJGAME_RENJU_WIDTH;j++)
		{
			if(c_chips[i][j] != NULL && c_chips[i][j]->isMe(item))
				return c_chips[i][j];
		}
	}
	return NULL;
}

void RenjuDesktop::SetChipBaseNumber(quint16 base)
{
	int i,j;
	for(i=1;i<=DJGAME_RENJU_WIDTH;i++)
	{
		for(j=1;j<=DJGAME_RENJU_WIDTH;j++)
		{
			if(c_chips[i][j] != NULL)
				c_chips[i][j]->setNumberBase(base);
		}
	}
}

void RenjuDesktop::ClearChip(quint8 x,quint8 y)
{
	if(x < 1 || y<1 || x > DJGAME_RENJU_WIDTH || y>DJGAME_RENJU_WIDTH)
		return;
	if(c_chips[x][y] != NULL)
	{
		delete c_chips[x][y];
		c_chips[x][y] = NULL;
	}
}

void RenjuDesktop::CreateChip(quint8 chOwner,quint8 x,quint8 y,quint8 chStep)
{
	int rx,ry;
	ClearChip(x,y);
	if(x < 1 || y<1 || x > DJGAME_RENJU_WIDTH || y>DJGAME_RENJU_WIDTH)
		return;
	rx = static_cast<int>((x-1)*41+24+c_itemBoard->x());
	ry = static_cast<int>((y-1)*41+24+c_itemBoard->y());
	
	int i,j;
	for(i=1;i<=DJGAME_RENJU_WIDTH;i++)
	{
		for(j=1;j<=DJGAME_RENJU_WIDTH;j++)
		{
			if(c_chips[i][j] != NULL && c_chips[i][j]->isCurrent() && c_chips[i][j]->color() != chOwner)
				c_chips[i][j]->setCurrentChip(false);
		}
	}
	c_chips[x][y] = new RenjuChip(canvas(),this,chOwner,rx,ry,chStep);
	c_chips[x][y]->setCurrentChip(true);
	if(c_bSignYN)
		c_chips[x][y]->setDisplayNumber(true);
	else
		c_chips[x][y]->setDisplayNumber(false);
	c_chips[x][y]->setNumberBase(c_chSignT0);
	c_chips[x][y]->show();
}

void RenjuDesktop::MoveItem(Q3CanvasItem* item,QPoint &ps,QPoint &pe)
{
}

void RenjuDesktop::SignChips(bool b)
{
	int i,j;
	for(i=1;i<=DJGAME_RENJU_WIDTH;i++)
	{
		for(j=1;j<=DJGAME_RENJU_WIDTH;j++)
		{
			if(c_chips[i][j] != NULL)
				c_chips[i][j]->setDisplayNumber(b);
		}
	}
}

bool RenjuDesktop::handleItemClicked(Qt::MouseButton btn,const QPoint &pos,Q3CanvasItem * item)
{
	if ( DJGameDesktop::handleItemClicked(btn,pos,item) )
		return true;
	
	if(item == c_itemSignYN && btn == Qt::LeftButton)
	{//是否显示数字
		QPixmap pix;
		QImage img;
		c_bSignYN = !c_bSignYN;
		QMatrix m;
		if(c_bSignYN)
		{
			pix = QPixmap(QString(":/RenjuRes/image/sign_n.png"));
		}else
		{
			pix = QPixmap(QString(":/RenjuRes/image/sign_y.png"));
		}
		img = pix.convertToImage();
		c_itemSignYN->changeImage(img);
		SignChips(c_bSignYN);
		m = GetCurrentMatrix();
		c_itemSignYN->setMatrix(m);
		return true;
	}
	if(item == c_itemSignT0T1 && btn == Qt::LeftButton)
	{
		QPixmap pix;
		QImage img;
		QMatrix m;
		if(c_chSignT0 == 0 && c_shSteps > 0)
			c_chSignT0 = c_shSteps-1;
		else
			c_chSignT0 = 0;
		if(c_chSignT0 == 0)
		{
			pix = QPixmap(QString(":/RenjuRes/image/sign_t1.png"));
		}else
		{
			pix = QPixmap(QString(":/RenjuRes/image/sign_t0.png"));
		}
		img = pix.convertToImage();
		c_itemSignT0T1->changeImage(img);
		SetChipBaseNumber(c_chSignT0);
		if(!c_bSignYN)
			handleItemClicked(btn,pos,(Q3CanvasItem *)c_itemSignYN);
		m = GetCurrentMatrix();
		c_itemSignT0T1->setMatrix(m);
		return true;
	}
	if(c_RenjuPanel->isLookingOn())
		return false;
	if(GetTableStatus() == DJGAME_RENJU_TABLE_STATUS_WAIT_SELECT &&
			 IsWaittingForMe() && !c_bWaitMoveACL)
	{//二选一
		char chSel = 0;
		if(c_chips[c_Steps[c_StepNumbers-1][0]][c_Steps[c_StepNumbers-1][1]]->isMe(item))
			chSel = 2;
		if(c_chips[c_Steps[c_StepNumbers-2][0]][c_Steps[c_StepNumbers-2][1]]->isMe(item))
			chSel = 1;
		if(chSel != 0)
		{
			SendGameTrace(RENJU_GAMETRACE_SELECT,&chSel,1);
			c_bWaitMoveACL = true;
			return true;
		}
	}
	if(item == c_itemBoard && btn == Qt::LeftButton)
	{//点击棋盘
		
		if(GetTableStatus() != DJGAME_RENJU_TABLE_STATUS_WAIT_MOVE 
			|| !IsWaittingForMe() || c_bWaitMoveACL)
		{
			return false;
		}
		//走棋
		int x,y;
		x = static_cast<int>((pos.x()-c_itemBoard->x()+37)/41);
		y = static_cast<int>((pos.y()-c_itemBoard->y()+37)/41);
		printf("board(%d,%d)\n",x,y);
		if(x < 1 || x > DJGAME_RENJU_WIDTH || y<1 || y> DJGAME_RENJU_WIDTH)
			return false;
		RenjuMove move;
		move.x = (unsigned char)x;
		move.y = (unsigned char)y;
		c_bWaitMoveACL = true;
		QVariant parameters	= QVariant::fromValue((void*)this);
		SendGameTrace(RENJU_GAMETRACE_MOVE,(char *)&move,sizeof(RenjuMove),HandleMoveACL,parameters);
		return true;
	}
	return false;
}
void RenjuDesktop::SetWaitMoveACLStatus(bool b)
{
	c_bWaitMoveACL = b;
}

void  HandleMoveACL( const QByteArray& buf, const QVariant& parameters )
{
    PDJGameProtocol pacl = (PDJGameProtocol)buf.data();
    RenjuDesktop* desktop = static_cast<RenjuDesktop *>(parameters.value<void*>());    
	
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
void RenjuDesktop::handleLast10Launch(unsigned char countdown)
{
	QString wavName	= QString("number%1.wav").arg(countdown);
	c_RenjuPanel->playWave(wavName);
}
void RenjuDesktop::SetPlayerTimer(quint8 chOwner,PDJGameChessTimer rtimer)
{
	QPoint pa,pn,pt;
	quint8 chView,chSite=0;
	DJGameChessTimerItem *curtimer=0;
	if(chOwner == DJGAME_RENJU_OWNER_BLACK)
	{
		if(c_blackTimer == NULL) {
			c_blackTimer = new DJGameChessTimerItem(canvas(),this,rtimer);
			connect(c_blackTimer, SIGNAL(Last10Launch(unsigned char)), SLOT(handleLast10Launch(unsigned char)));
		}else
			c_blackTimer->setTimer(rtimer);
		curtimer = c_blackTimer;
		chSite = c_current.chBlack;
	}else if(chOwner == DJGAME_RENJU_OWNER_WHITE)
	{
		if(c_whiteTimer == NULL) {
			c_whiteTimer = new DJGameChessTimerItem(canvas(),this,rtimer);
			connect(c_whiteTimer, SIGNAL(Last10Launch(unsigned char)), SLOT(handleLast10Launch(unsigned char)));
		}else
			c_whiteTimer->setTimer(rtimer);
		curtimer = c_whiteTimer;
		chSite = c_current.chWhite;
	}
	
	if(chSite != 0)
	{
		chView = MapSeat2View(chSite);
		/*
		QPoint pa,pn,pt;
		GetAvatarPosition(chSite,&pa,&pn,&pt);
		DJGameUser *puser;
		quint32 uid;
		uid = c_RenjuPanel->userIdOfSeat(chSite);
		puser = c_RenjuPanel->gameUser(uid,true);
		QPixmap pix = puser->avatar();
		*/
		if(chView == 1)
		{
			QRect showRect	= idealRectOfShow( chSite );
			djDebug() << "showrect" << showRect;
			//QRect rt(pa.x(),(GetRealHeight()>>1),pix.width(),(GetRealHeight()>>1)-pix.height()-10);
			QRect rt( showRect.left(),GetRealHeight()>>1,showRect.width(),(GetRealHeight()>>1)-showRect.height()-10);
			curtimer->move(rt,false);
			curtimer->show();
		}else if(chView == 3)
		{
			//QRect rt(pa.x(),pa.y()+pix.height()+5,pix.width(),(GetRealHeight()>>1)-pix.height()-10);
			QRect showRect	= idealRectOfShow( chSite );
			QRect rt( showRect.left(),showRect.bottom() + 5,showRect.width(),(GetRealHeight()>>1)-showRect.height()-10);
			
			curtimer->move(rt);
			curtimer->show();
		}
	}
}

bool RenjuDesktop::isCloseable()
{
	if( GetTableStatus() == DJGAME_TABLE_STATUS_WAITSTART  && IsWaittingForMe()
		&& !(GetTotalGames() & 1) )
		return true;
	if( GetTableStatus() == DJGAME_RENJU_TABLE_STATUS_WAIT_RULE)
		return true;
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////

RenjuPanel::RenjuPanel( DJGameController *gameController, DJGameRoom *room, const QList<quint32>& userIds, quint8 tableId, quint8 seatId, QWidget * parent, Qt::WFlags f)
			: DJGamePanel( gameController, room, userIds, tableId, seatId, parent, f )
{	
	djDebug() << "RenjuPanel::RenjuPanel";
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
	//RenjuDesktop* gameDesktop	= new RenjuDesktop(gameSplitter,this,760,710);
	RenjuDesktop* gameDesktop	= new RenjuDesktop(gameSplitter,this,760,710);
	
	setAccessorySplitter( accessorySplitter );
	setGameDesktop( gameDesktop );	
}
RenjuPanel::~RenjuPanel()
{
}
void RenjuPanel::createAccessoryWidgets()
{
	DJGamePanel::createAccessoryWidgets();
	
	DJGroupChat *chat	= chatWidget();
	RenjuDesktop *desktop	= static_cast<RenjuDesktop*>(gameDesktop());
	if ( chat && desktop ) {
		connect( chat->textBrowser(), SIGNAL(acceptDrawChess()), desktop, SLOT(ClickReqDraw()) );
	}
}
void RenjuPanel::closeEvent( QCloseEvent *e )
{
	DJGamePanel::closeEvent( e );
	if ( !e->isAccepted() ) {
		int result	= DJMessageBox::information(30,this, tr("Can't quit now"),tr("You must play even games for justice.\nForce quit will be treated as surrender.\nAre you still want to quit?"),QMessageBox::Yes, QMessageBox::No);
		if ( QMessageBox::Yes == result ) {
			RenjuDesktop* desktop	= static_cast<RenjuDesktop*>(gameDesktop());
			desktop->ClickStart();
			desktop->ClickSurrender();
			QByteArray buf	= makeGameData( DJGAME_PROTOCOL_SUBTYPE_QUIT, QByteArray() );
			DJClientRequest request( buf );
			sendRequest( request );
		}
	}
}
/////////////////////////////////////////////////////////
RenjuChip::RenjuChip(Q3Canvas * cv,RenjuDesktop *desktop,quint8 chOwner,int x,int y,quint16 chStep)
{
	c_Canvas = cv;
	c_chOwner = chOwner;
	c_Number  = chStep;
	c_Desktop = desktop;
	
	QPixmap pix;
	QImage img;
	QColor col;
	if(chOwner == DJGAME_RENJU_OWNER_BLACK)
	{
		pix = QPixmap(QString(":/RenjuRes/image/black.png"));
		col = QColor(0xFF,0xFF,0xFF);
	}else
	{
		pix = QPixmap(QString(":/RenjuRes/image/white.png"));
		col = QColor(0,0,0);
	}
	img = pix.convertToImage();
	QMatrix m = desktop->GetCurrentMatrix();
	c_itemChip = new DJGameImageItem(img,c_Canvas,m,RENJU_DESKTOP_TID_CHIP,chOwner,RENJU_DESKTOP_TYPE_CHIP,false);
	c_itemChip->setZ(100);
	QString str;
	str = QString("");
	c_itemNumber = new DJGameTextItem(str,c_Canvas,m,RENJU_DESKTOP_TID_CHIPNUMBER,chOwner,RENJU_DESKTOP_TYPE_CHIP);
	c_itemNumber->setZ(110);
	c_itemNumber->setColor(col);
	QFont f = c_itemNumber->font();
	f.setPointSize(14);
	c_itemNumber->setFont(f);
	
	
	c_bCurrent = false;
	c_bNumber = true;
	c_NumberBase = 0;
	
	pix = QPixmap(QString(":/RenjuRes/image/current.png"));
	img = pix.convertToImage();
	c_itemCurrent = new DJGameImageItem(img,c_Canvas,m,RENJU_DESKTOP_TID_CHIPCURRENT,chOwner,RENJU_DESKTOP_TYPE_CHIP,false);
	c_itemCurrent->setZ(120);
	c_itemCurrent->setVAlignment(Qt::AlignVCenter);
	c_itemCurrent->setHAlignment(Qt::AlignHCenter);
	c_itemCurrent->move(x,y);
	
	c_itemChip->setVAlignment(Qt::AlignVCenter);
	c_itemChip->setHAlignment(Qt::AlignHCenter);
	c_itemChip->move(x,y);
	
	c_itemNumber->setVAlignment(Qt::AlignVCenter);
	c_itemNumber->setHAlignment(Qt::AlignHCenter);
	c_itemNumber->move(x,y);
}

void RenjuChip::changeCurrentImage(quint8 ch)
{
	QPixmap pix;
	QImage img;
	if(ch == RENJU_STEP_DESCRIBE_FORBID)
	{
		pix = QPixmap(QString(":/RenjuRes/image/loseicon.png"));
	}else 
	{
		pix = QPixmap(QString(":/RenjuRes/image/winicon.png"));
	}
	img = pix.convertToImage();
	c_itemCurrent->changeImage(img);
	QMatrix m = c_Desktop->GetCurrentMatrix();
	c_itemCurrent->show();
	c_itemCurrent->setMatrix(m);
}
	
void RenjuChip::show()
{
	QMatrix m = c_Desktop->GetCurrentMatrix();
	c_itemChip->show();
	if(c_bNumber && c_Number > c_NumberBase)
	{
		c_itemNumber->setText(QString("%1").arg(c_Number-c_NumberBase));
		c_itemNumber->show();
		c_itemNumber->setMatrix(m);
	}else
		c_itemNumber->hide();
	
	if(c_bCurrent)
	{
		c_itemCurrent->show();
		c_itemCurrent->setMatrix(m);
	}else
		c_itemCurrent->hide();
	
	c_itemChip->setMatrix(m);
	c_Canvas->update();
	c_Desktop->updateContents(0,0,c_Desktop->width(),c_Desktop->height());

}

void RenjuChip::hide()
{
	c_itemChip->hide();
	c_itemNumber->hide();
	c_itemCurrent->hide();
}

void RenjuChip::setNumberBase(quint16 base)
{
	c_NumberBase = base;
	setDisplayNumber(c_bNumber);
}

quint16 RenjuChip::number()
{
	return c_Number;
}
quint8	RenjuChip::color()
{
	return c_chOwner;
}
quint16 RenjuChip::currentNumber()
{
	if(c_Number > c_NumberBase)
		return c_Number - c_NumberBase;
	return 0;
}

void RenjuChip::setDisplayNumber(bool b)
{
	c_bNumber = b;
	if(b && c_Number > c_NumberBase)
	{
		QMatrix m = c_Desktop->GetCurrentMatrix();
		c_itemNumber->setText(QString("%1").arg(c_Number-c_NumberBase));
		c_itemNumber->show();
		c_itemNumber->setMatrix(m);
	}else
		c_itemNumber->hide();
		
}

void RenjuChip::setCurrentChip(bool b)
{
	c_bCurrent = b;
	if(b)
	{
		QMatrix m = c_Desktop->GetCurrentMatrix();
		c_itemCurrent->show();
		c_itemCurrent->setMatrix(m);
	}else
	{
		c_itemCurrent->hide();
	}

}

bool RenjuChip::isCurrent()
{
	return c_bCurrent;
}
bool RenjuChip::isMe(Q3CanvasItem *item)
{
	if(item == NULL)
		return false;
	if(item == c_itemCurrent || item == c_itemChip || c_itemNumber == item)
		return true;
	return false;
}

RenjuChip::~RenjuChip()
{
	delete c_itemChip;
	delete c_itemNumber;
	delete c_itemCurrent;
}
