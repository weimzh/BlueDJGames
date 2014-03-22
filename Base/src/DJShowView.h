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

#ifndef DJSHOWVIEW_H
#define DJSHOWVIEW_H

#include <QtGui>
#include <QtXml>
#include "DJBaseGlobal.h"

class DJMatrixCanvasText;
class DJMatrixCanvasImage;
class DJMatrixCanvasSprite;
class DJGameUser;
class DJShowView;

class DJ_BASE_EXPORT DJShowItem : public QObject
{
	Q_OBJECT
public:
	DJShowItem( DJShowView *showView, const QString& name, DJGameUser* user,
				qreal x, qreal y, qreal z, bool isAnimated, const QMatrix& matrix = QMatrix() );
	virtual ~DJShowItem();
	
	void description();
	void createItem();
	void show();
	void hide();
	bool isReady() const;
	void setLifeTime( int second );
	void setAlignment( Qt::AlignmentFlag flag );
	QString name() const { return m_name; }
	qreal x() const { return m_x; }
	qreal y() const { return m_y; }
	qreal z() const { return m_z; }
	bool isAnimated() const { return m_isAnimated; }
signals:
	void itemIsReady();
protected:
	virtual void timerEvent( QTimerEvent * event );
private slots:
	void showImageDone();
private:
	DJShowView			*m_showView;
	QString				m_name;
	qreal				m_x;
	qreal				m_y;
	qreal				m_z;
	bool				m_isAnimated;
	QString				m_localImagePath;
	bool				m_hasStartedTimer;
	int					m_lifeTime;
	QGraphicsPixmapItem	*m_item;
	QMatrix				m_matrix;
};

class DJ_BASE_EXPORT DJShowView : public QGraphicsView
{
    Q_OBJECT 
public:
    DJShowView( DJGameUser* user, QWidget* parent = 0 );
    virtual ~DJShowView();

	virtual QSize sizeHint() const;
	void setUser( DJGameUser* user );
	
	void setViewAlignment( Qt::Alignment alignment );
	void setVirtualPos( const QPoint& pos );
	//void setRealPos( const QPoint& pos );
	void adjustPos( const QMatrix& m );
	
	QSize	realSize() const { return m_realSize; }
	void setRealSize( const QSize& size );
	void setRealSize( int width, int height );
	void clearAllItems();
	
	void createDefaultItems();
	void showDefaultShow();
	void hideDefaultShow();
	
	void createCustomItems( const QString& xmlPath );
	void showCustomShow();
	void hideCustomShow();
	bool isAllCustomItemsReady();
	
	void addEffect( const QString& name, int second = 10 );
signals:
	void rightClicked( DJGameUser *user );
protected:   
	virtual void resizeEvent( QResizeEvent * event );
	virtual void mouseReleaseEvent( QMouseEvent * e );
private slots:
	void showXmlDone( const QString& filePath );
	void customItemIsReady();
	
private:
	QHash<quint32,QString>	m_showXmls;
	DJGameUser				*m_user;
	QList<DJShowItem*>		m_defaultItems;
	QList<DJShowItem*>		m_customItems;
	QHash<QString, DJShowItem*>	m_effectItems;
	QSize					m_realSize;
	Qt::Alignment			m_alignment;
	QPoint					m_virtualPos;
};

#endif
