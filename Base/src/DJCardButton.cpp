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
#include "DJCardButton.h"
#include "DJCard.h"
#include "DJCardsWidget.h"
#include "DJGameController.h"
#include "Utility.h"

static	QSize	ICON_SIZE	= QSize(32,32);

DJCardButton::DJCardButton( DJCardsWidget *cardsWidget, DJGameController* gameController, quint8 id, QWidget * parent )
	:DJToolButton(parent),m_cardsWidget(cardsWidget),m_gameController(gameController),m_id(id)
{
	//djDebug()<<"DJCardButton constructor";
	
	setIconSize( ICON_SIZE );
	
	if ( DJGAME_GAMECARD_EMOTION == id ) {
		QMenu *menu	= new QMenu(this);
		for ( int i = DJCard::Happy; i <= DJCard::Cry; ++i ) {
			QAction *action	= menu->addAction( QIcon( DJCard::emotionImagePath(i) ),
												DJCard::emotionName(i) );
			action->setData( QVariant(i) );
    	}
    	setIcon( QIcon(DJCard::imagePath(id)) );
		setMenu( menu );
		setPopupMode(QToolButton::InstantPopup);
		connect( menu, SIGNAL(triggered(QAction*)), SLOT(handleCardClicked(QAction*)) );
	}else {
		setIcon( QIcon(DJCard::imagePath(m_id)) );
		connect( this, SIGNAL(clicked()), SLOT(handleCardClicked()) );
	}
}
DJCardButton::~DJCardButton()
{
	//djDebug()<<"DJCardButton destructor";
}
void DJCardButton::enterEvent ( QEvent * event )
{
	//djDebug()<<"DJCardButton::enterEvent";
	QString	text	= tr("card name : ") + DJCard::name(m_id) + "\n";
	 
	DJCard *card	= m_gameController->card(m_id);
	if ( card ) {
		text	+= tr("number of cards : ") + QString::number(card->pages()) + "\n";
		int totalSeconds	= card->life();
		if ( totalSeconds > 0 ) {
			int hours,minutes,seconds;
			ConvertSeconds2HMS( totalSeconds, hours, minutes, seconds );
			text	+= tr("this card is using now, time left : %1 hours %2 minutes %3 seconds")
										.arg(hours)
										.arg(minutes)
										.arg(seconds);
			text	+= "\n";
		}
	}
	text	+= tr("card description : ") + DJCard::description(m_id);
	setToolTip( text );
}
void DJCardButton::handleCardClicked(QAction* action)
{
	djDebug()<<"handleCardClicked"<<action;
	int parameter	= action->data().toInt();
	djDebug()<<"parameter="<<parameter;
	m_cardsWidget->cardClicked(m_id,parameter);
}
void DJCardButton::handleCardClicked()
{
	djDebug()<<"handleCardClicked";
	m_cardsWidget->cardClicked(m_id,0);
}




