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

#include "DJContactItem.h"
#include "DJContact.h"
#include "DJChat.h"
#include "DJGlobal.h"

DJContactItem::DJContactItem( DJContact* contact, quint32 userId, const QString& userName, QTreeWidget * parent, int type )
	:QTreeWidgetItem(parent,type),m_contact(contact),m_userId(userId),m_userName(userName)
{
	djDebug() << "DJContactItem constructor";
	setText( 0, userName );
}
DJContactItem::~DJContactItem()
{
	djDebug() << "DJContactItem destructor";
	m_contact->removePendingChat(this);
}

bool DJContactItem::operator< ( const QTreeWidgetItem & other ) const
{	
	const DJContactItem& otherItem	= static_cast<const DJContactItem &>(other);
	
	Qt::SortOrder	order	= treeWidget()->header()->sortIndicatorOrder();
	quint32	selfUserId	= m_contact->selfUserId();

	if ( m_userId == selfUserId ) {
		if ( order == Qt::AscendingOrder )
			return true;
		else
			return false;
	}else if ( otherItem.m_userId == selfUserId ) {
		if ( order == Qt::AscendingOrder )
			return false;
		else
			return true;
	}

	if ( otherItem.isOnline() ) {
		//other is online
		if ( isOnline() ) {
			//this is online too
			return QTreeWidgetItem::operator< ( other );
		}else {
			//this is offline
			if ( order == Qt::AscendingOrder )
				return false;
			else
				return true;
		}
	}else {
		//other is offline
		if ( isOnline() ) {
			//this is online 
			if ( order == Qt::AscendingOrder )
				return true;
			else
				return false;
		}else {
			//this is offline too
			return QTreeWidgetItem::operator< ( other );
		}
	}
}
void DJContactItem::userOnline( quint16 gameId, const QIcon& icon )
{
	djDebug() << "DJContactItem::userOnline"<<"gameid="<<gameId;
	m_icon	= icon;
	//QIcon disabledIcon	= icon.pixmap( 16, QIcon::Disabled );
	//m_icon.addPixmap( disabledIcon, QIcon::Disabled );
	m_gameIds.insert( gameId );
	setIcon( 0, m_icon );
}
void DJContactItem::userOffline( quint16 gameId )
{
	djDebug() << "DJContactItem::userOffline"<<"gameid="<<gameId;
	m_gameIds.remove( gameId );
	if ( !isOnline() )
		setIcon( 0, QIcon() );
}
quint16	DJContactItem::firstGameId() const
{
	if ( m_gameIds.isEmpty() )
		return 0;
	else
		return *(m_gameIds.constBegin());
}
void DJContactItem::setBlink( bool on )
{
	if ( on ) {
		setIcon( 0, m_icon );
	}else {
		setIcon( 0, m_icon.pixmap( 16, QIcon::Disabled ) );
	}
}
void DJContactItem::clearBlink()
{
	if ( isOnline() ) {
		setIcon( 0, m_icon );
	}else {
		setIcon( 0, QIcon() );
	}
}

