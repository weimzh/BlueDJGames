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

#ifndef DJCLIENTREQUEST_H
#define DJCLIENTREQUEST_H

#include <QtCore>
#include "DJBaseGlobal.h"

class DJ_BASE_EXPORT DJClientRequest
{
public:
	DJClientRequest( const QByteArray& content = QByteArray(), AclProc aclProc = 0, const QVariant& parameters = QVariant() );
	~DJClientRequest();
	
	void invokeAclProc( const QByteArray& acl );
	
	const QByteArray&	content() const { return m_content; }
	AclProc			aclProc() const { return m_aclProc; }
	const QVariant&	parameters() const { return m_parameters; }
	uint id() const { return m_id; }
private:
	QByteArray		m_content;
	AclProc			m_aclProc;
	QVariant		m_parameters;
	uint			m_id;
};
#endif
