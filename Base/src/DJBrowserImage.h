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

#ifndef DJBROWSERIMAGE_H
#define DJBROWSERIMAGE_H

#include <QtGui>
#include "DJAnimationImage.h"
#include "DJBaseGlobal.h"

class DJTextBrowser;
class DJRemoteToLocalFile;

class DJ_BASE_EXPORT DJBrowserImage : public DJAnimationImage
{
	Q_OBJECT
public:
	DJBrowserImage( const QUrl& url, int pos, int len, DJTextBrowser * browser );
	virtual ~DJBrowserImage();
	
	QUrl url() const { return m_url; };
	QList< QPair<int,int> > poslens() const { return m_poslens; }
	void addPosLen( int pos, int len );
	void increasePosLens( int startPos, int len );
	int decreasePos( int pos );
private slots:
	void handleLocalFileDone( const QString& localFile );
	void handleImageChanged();
private:
	void displayImage();
	
	QUrl					m_url;
	QList< QPair<int,int> >	m_poslens;
	DJTextBrowser			*m_browser;	
	DJRemoteToLocalFile		*m_r2l;
};

#endif
