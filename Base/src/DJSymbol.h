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

#ifndef DJSYMBOL_H
#define DJSYMBOL_H

#include <QtGui>
#include "DJBaseGlobal.h"

class DJ_BASE_EXPORT DJSymbolTable : public QTableWidget
{
    Q_OBJECT 
public:
    DJSymbolTable( int type, QWidget* parent = 0, const QSize symbolSize = QSize(36,36) );
    virtual ~DJSymbolTable();
    
    void setSymbolSize( const QSize& size );
    void setDirPath( const QString& dirPath );
    void createSymbolItems();
    void setSymbolItems( int rows, int column );
	void reload();

signals:
	void playerSymbolSelected( const QString& name );
	void memberSymbolSelected( const QString& name );
	
public slots:
	void handleItemClicked( QTableWidgetItem * item );
	void handleItemDoubleClicked( QTableWidgetItem * item );

protected:   
	virtual void resizeEvent ( QResizeEvent * event );
	
private:
	int							m_type;
	QString						m_dirPath;
	QSize						m_symbolSize;
	QList<QTableWidgetItem *> 	m_symbolItems;	
};

class DJ_BASE_EXPORT DJSymbol : public QTabWidget
{
	Q_OBJECT
public:
    DJSymbol( QWidget * parent = 0 );
    virtual ~DJSymbol();
	
   	void removeMemberWidget();
public slots:   
	void reloadMemberWidget();
signals:
	void playerSymbolSelected( const QString& name );
	void memberSymbolSelected( const QString& name );

private:
	DJSymbolTable	*m_playerSymbolWidget;
	DJSymbolTable	*m_memberSymbolWidget;
};
#endif

