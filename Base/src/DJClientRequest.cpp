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

#include "DJClientRequest.h"
#include "Utility.h"

static	uint	globalId	= 0;

DJClientRequest::DJClientRequest( const QByteArray& content, AclProc aclProc, const QVariant& parameters )
	:m_content(content),m_aclProc(aclProc),m_parameters(parameters)
{
	djDebug() << "DJClientRequest constructor";
	globalId++;
	m_id	= globalId;
}
DJClientRequest::~DJClientRequest()
{
	djDebug() << "DJClientRequest destructor";
}
void DJClientRequest::invokeAclProc( const QByteArray& acl )
{
	djDebug() << "DJClientRequest::callAclProc";
	if ( m_aclProc ) {
		m_aclProc( acl, m_parameters );
	}else {
		djDebug()<<"no aclproc";
	}
}
