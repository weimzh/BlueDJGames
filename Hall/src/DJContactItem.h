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

#ifndef DJCONTACTITEM_H
#define DJCONTACTITEM_H

#include <QtGui>

class DJContact;
class DJChat;

class DJContactItem : public QTreeWidgetItem
{
public:
    DJContactItem( DJContact* contact, quint32 userId, const QString& userName, QTreeWidget * parent, int type = Type );
    virtual ~DJContactItem();
    virtual bool operator< ( const QTreeWidgetItem & other ) const;
    
    quint32	userId() const { return m_userId; }
    QString	userName() const { return m_userName; }
    QIcon	userIcon() const { return m_icon; }
    void userOnline( quint16 gameId, const QIcon& icon );
    void userOffline( quint16 gameId );
	quint16	firstGameId() const;

	bool isOnline() const { return !m_gameIds.isEmpty(); }
	void setBlink( bool on );
	void clearBlink();
private:
	DJContact*		m_contact;
   	quint32			m_userId;
   	QString			m_userName;
   	QSet<quint16>	m_gameIds;
   	QIcon			m_icon;
};

#endif
