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

#ifndef DJDESKTOP_H
#define DJDESKTOP_H

#include <QtGui>
#include "DJBaseGlobal.h"

class DJDesktopScene;
class DJDesktopController;

class DJ_BASE_EXPORT DJDesktop : public QGraphicsView
{
    Q_OBJECT 
public:
    DJDesktop( DJDesktopController *desktopController, const QSize& realSize, QWidget *parent );
    virtual ~DJDesktop();

	DJDesktopScene* desktopScene() const;
    int realWidth() const { return m_realWidth; }
    int realHeight() const { return m_realHeight; }
    void setRealSize( int realWidth, int realHeight );
    QMatrix graphicsMatrix() const { return m_matrix; }
    qreal graphicsScale() const { return m_scale; }
signals:
	void matrixChanged();
//public slots:
	//void setDesktopBrush( const QBrush& brush );
protected :
	virtual void resizeEvent( QResizeEvent * );
private slots:    
	void calculateMatrix();
private:
	DJDesktopController 	*m_desktopController;	
	QTimer					*m_resizeTimer;
	int						m_realWidth,m_realHeight;
	QMatrix					m_matrix;
	qreal					m_scale;
};

#endif
