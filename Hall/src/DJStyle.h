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

#ifndef DJSTYLE_H
#define DJSTYLE_H

#include <QtGui>
//QMotifStyle,QWindowsXPStyle,QWindowsStyle,QPlastiqueStyle
#ifdef Q_WS_MAC
#include <QMacStyle>
#define	BASE_STYLE	QMacStyle
#else
#define	BASE_STYLE	QWindowsStyle
#endif

class DJStyle : public BASE_STYLE
{
	Q_OBJECT
public:
	DJStyle( const QString& stylePath );
	virtual ~DJStyle();
	QString stylePath() const { return m_stylePath; }
	
	virtual void drawComplexControl ( ComplexControl control, const QStyleOptionComplex * option, QPainter * painter, const QWidget * widget = 0 ) const;
#ifndef Q_WS_MAC
	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const;
	virtual void drawControl(ControlElement control, const QStyleOption *option, QPainter *painter, const QWidget *widget) const;
	virtual QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const;
	virtual QPixmap standardPixmap ( StandardPixmap standardPixmap, const QStyleOption * option = 0, const QWidget * widget = 0 ) const;
#endif
private:
	QString	m_stylePath;
	int		m_pbHeight;
};

#endif
