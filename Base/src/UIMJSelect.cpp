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

#include <QtSvg>

#include "UIMJSelect.h"
#include "mjrule.h"
#include "DJDesktopMahjongController.h"
#include "Utility.h"

UIMJSelect::UIMJSelect( DJDesktopMahjongController *mjController, const MJCardsGroups& groups, QWidget * parent, Qt::WindowFlags f )
	:QDialog( parent, f )
{
	setupUi(this);
	
	m_rows	= new QButtonGroup(this);
	
	qreal scale	= 0.5;
	for( MJCardsGroupsConstIterator groupsIt = groups.begin(); groupsIt != groups.end(); groupsIt++ ) {
		const MJCards& cards	= *groupsIt;
		QList<QPixmap> pixmaps;
		for( MJCardsConstIterator cardsIt = cards.begin(); cardsIt != cards.end(); cardsIt++ ) {
			MJCard card	= *cardsIt;
			QString resPath	= mjController->cardResPath( card, DJDesktopMahjongController::Standing, 1 );
			QSvgRenderer *renderer	= mjController->cardRenderer( resPath );
			if ( renderer ) {
				QPixmap pix	= SvgRender2Pixmap( renderer );
				pixmaps << pix;
			}
		}
		QPixmap conjointPix	= CreateConjointPixmap( pixmaps );
		ScalePixmap( conjointPix, scale );
		QRadioButton *button	= new QRadioButton;
		button->setIconSize( conjointPix.size() );
		button->setIcon( conjointPix );
		button->setChecked( true );
	
		m_rows->addButton( button );
		vboxLayout1->addWidget( button );
	}
}
UIMJSelect::~UIMJSelect()
{
}
int UIMJSelect::indexOfSelected() const
{
	QAbstractButton *checkedButton	= m_rows->checkedButton();
	
	QList<QAbstractButton *> list	= m_rows->buttons();
	for ( int i = 0; i < list.size(); i++ ) {
		if ( checkedButton == list.at(i) )
			return i;
	}
	return 0;
}
