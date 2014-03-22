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

#include "DJPanelUserItem.h"
#include "DJPanelController.h"
#include "DJGameUser.h"

static const char *winds[]={
    QT_TRANSLATE_NOOP("DJPanelUserItem","none"),
    QT_TRANSLATE_NOOP("DJPanelUserItem","east"),
    QT_TRANSLATE_NOOP("DJPanelUserItem","south"),
    QT_TRANSLATE_NOOP("DJPanelUserItem","west"),
    QT_TRANSLATE_NOOP("DJPanelUserItem","north")
};
	
DJPanelUserItem::DJPanelUserItem( DJGameUser* user, DJPanelController* panelController, QTreeWidget * parent, int type )
		: QTreeWidgetItem( parent, type),m_user(user),m_panelController(panelController)
{	
	for ( int i = 0; i < parent->columnCount(); i++ ) {
		setTextAlignment( i, Qt::AlignCenter );
	}
}
DJPanelUserItem::~DJPanelUserItem()
{
}
bool DJPanelUserItem::operator< ( const QTreeWidgetItem & other ) const
{
	const DJPanelUserItem& otherItem	= static_cast<const DJPanelUserItem &>(other);
	Qt::SortOrder	order	= treeWidget()->header()->sortIndicatorOrder();
	
	DJGameUser*	thisUser	= user();
	DJGameUser*	otherUser	= otherItem.user();
	
	if ( m_panelController->isPlayer( thisUser ) && !m_panelController->isPlayer( otherUser ) ) {
		if ( order == Qt::AscendingOrder )
			return true;
		else
			return false;
	}
	if ( !m_panelController->isPlayer( thisUser ) && m_panelController->isPlayer( otherUser ) ) {
		if ( order == Qt::AscendingOrder )
			return false;
		else
			return true;
	}
	
	return QTreeWidgetItem::operator< ( other );
}
void DJPanelUserItem::setName( const QString& name )
{
	int index	= m_panelController->indexOfSection( DJPanelController::UserName );
	if ( -1 != index ) {
		setText( index, name );
	}
}
void DJPanelUserItem::setNickName( const QString& nickname )
{
	int index	= m_panelController->indexOfSection( DJPanelController::NickName );
	if ( -1 != index ) {
		setText( index, nickname );
	}
}
void DJPanelUserItem::setSeat( quint8 seatId )
{
	int index	= m_panelController->indexOfSection( DJPanelController::Seat );
	if ( -1 != index ) {
		setText( index, QString::number( seatId ) );
	}
}
void DJPanelUserItem::setPanelScore( int score )
{
	int index	= m_panelController->indexOfSection( DJPanelController::PanelScore );
	if ( -1 != index ) {
		setText( index, QString("%1").arg(score) );
	}
}
void DJPanelUserItem::deltaPanelScore( int delta )
{
	int index	= m_panelController->indexOfSection( DJPanelController::PanelScore );
	if ( -1 != index ) {
		int score	= text( index ).toInt();
		score += delta;
		setText( index, QString("%1").arg(score) );
	}
}
void DJPanelUserItem::setPanelChips( int chips )
{
	int index	= m_panelController->indexOfSection( DJPanelController::PanelChips );
	if ( -1 != index ) {
		setText( index, QString("%1").arg(chips) );
	}
}
void DJPanelUserItem::deltaPanelChips( int delta )
{
	djDebug()<<"DJGamePlayerItem::deltaChips"<<delta;
	int index	= m_panelController->indexOfSection( DJPanelController::PanelChips );
	if ( -1 != index ) {
		int chips	= text( index ).toInt();
		chips += delta;
		setText( index, QString("%1").arg(chips) );
	}
}
void DJPanelUserItem::setDoorWind( quint8 wind )
{
	int index	= m_panelController->indexOfSection( DJPanelController::DoorWind );
	if ( -1 != index ) {
		if ( wind <= 4 ) {
			setText( index, tr(winds[wind]) );
		}
	}
}
void DJPanelUserItem::setRingWind( quint8 wind )
{
	int index	= m_panelController->indexOfSection( DJPanelController::RingWind );
	if ( -1 != index ) {
		if ( wind <= 4 ) {
			setText( index, tr(winds[wind]) );
		}
	}
}
void DJPanelUserItem::setScore()
{
	int index	= m_panelController->indexOfSection( DJPanelController::Score );
	if ( -1 != index ) {
		setText( index, QString("%1").arg(m_user->score()) );
	}
}
void DJPanelUserItem::setChips()
{
	int index	= m_panelController->indexOfSection( DJPanelController::Chips );
	if ( -1 != index ) {
		setText( index, QString("%1").arg(m_user->chips()) );
	}
}




