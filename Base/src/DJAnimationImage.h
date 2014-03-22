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

#ifndef	DJANIMATIONIMAGE_H
#define DJANIMATIONIMAGE_H

#include <QtGui>
#include "DJBaseGlobal.h"

class DJ_BASE_EXPORT DJAnimationImage : public QObject
{
	Q_OBJECT
public:
	DJAnimationImage( QObject *parent = 0 );
	DJAnimationImage( const QString& file, QObject *parent = 0 );
	virtual ~DJAnimationImage();
	
	QImage image() const;
	void setImagePath( const QString& file );
	bool isValid() const;
	bool isAnimation() const;
signals:
	void imageChanged();
public slots:
	void start();
	void stop();
private slots:
	void handleAnimation();
private:
	QList<QImage>			m_images;
	QList<int>				m_delays;
	int						m_currentFrame;
	QTimer					*m_timer;
};
#endif
