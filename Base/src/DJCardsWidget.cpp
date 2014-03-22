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

#include <QtGui>
#include "DJCardsWidget.h"
#include "DJGameController.h"
#include "DJPanelController.h"
#include "DJPanel.h"
#include "DJGameUser.h"
#include "DJGameRoom.h"
#include "DJCard.h"
#include "Utility.h"
#include "DJCardButton.h"
#include "UIUseCard.h"

static	int		VIEW_HEIGHT	= 60;
static 	int		VIEW_WIDTH	= 150;

DJCardsWidget::DJCardsWidget( DJGameController* gameController, QWidget * parent )
	:QScrollArea(parent),m_gameController(gameController)
{
	m_panelController	= 0;
	
	setFixedHeight( VIEW_HEIGHT );
	setFixedWidth( VIEW_WIDTH );
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setWidgetResizable(true);
	
	m_cardsView	= new QWidget;
	QHBoxLayout* layout	= new QHBoxLayout( m_cardsView );
	layout->setMargin( 0 );
	layout->setSpacing( 0 );
	
	setWidget( m_cardsView );
}
DJCardsWidget::~DJCardsWidget()
{
}
void DJCardsWidget::loadCards()
{
	//clear previous toolbuttons
    QObjectList list	= m_cardsView->children();
    foreach( QObject* obj, list ){
		if ( obj->inherits("DJCardButton") ) {
			delete obj;
		}
 	}
	QHash<quint8, DJCard*>	cards	= m_gameController->cards();
	foreach( DJCard *card, cards ) {
		quint8 id	= card->id();
		if ( DJCard::isValid( id ) ) {
        	DJCardButton *btn	= new DJCardButton( this, m_gameController, id );
        	QHBoxLayout *layout	= qobject_cast<QHBoxLayout*>(m_cardsView->layout());
			layout->addWidget( btn );
			if ( !DJCard::isEmotionCard( id ) 
				&& (m_gameController->selfRoom()->options() & DJGAME_ROOMOPTION_COUNTER) )
				btn->setEnabled( false );
        }
	}
	m_cardsView->adjustSize();
}
void DJCardsWidget::setPanelController( DJPanelController *panelController )
{
	m_panelController	= panelController;
}
QList<quint32> DJCardsWidget::otherUserIds() const
{
	QList<quint32> userIds;
	if ( m_panelController ) {
		quint32 selfUserId	= m_gameController->selfUserId();
		for( quint8 seat = 1;seat <= m_panelController->numberOfSeats(); ++seat ) {
			DJGameUser *player	= m_panelController->player(seat);
			if ( player ) {
				quint32 userId	= player->userId();
				if( selfUserId != userId ) {
					userIds << userId;
				}
			}
		}
	}
	return userIds;
}
void DJCardsWidget::cardClicked( quint8 id, int parameter )
{
	djDebug()<<"DJCardsWidget::cardClicked";
	
	if ( !m_panelController )
		return;

	ReqUseCard	reqUseCard;
	memset( &reqUseCard, 0, sizeof(ReqUseCard) );
	
	setle4( &reqUseCard.hostid, m_gameController->selfUserId() );
	setle4( &reqUseCard.targetid, m_gameController->selfUserId() );
	reqUseCard.chTable	= m_panelController->tableId();
	reqUseCard.chCard	= id;
	setle4( &reqUseCard.iParam, parameter );
	
	if ( !DJCard::isUsedOnly2Myself(id) ) {
		UIUseCard useCardDlg( id, otherUserIds(), m_gameController, m_panelController->panel() );
		useCardDlg.adjustSize();
		if ( QDialog::Accepted == useCardDlg.exec() ) {
			quint32 targetId	= useCardDlg.otherUserId();
			djDebug()<<"targetId="<<targetId;
			setle4( &reqUseCard.targetid, targetId );
			m_gameController->requestUseCard( reqUseCard );
		}
	}else {
		m_gameController->requestUseCard( reqUseCard );
	}
}

