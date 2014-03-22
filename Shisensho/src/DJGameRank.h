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

//Please visit http://www.bluedj.com for more infomation about us
//and contact us at ggwizard@gmail.com or darkdong@gmail.com
//****************************************************************************/

#ifndef DJGAMERANK_H
#define DJGAMERANK_H

#include "DJTreeWidget.h"
#include "protocol.h"

class DJGameRankWidget;
class DJGameController;

class DJRankItem :public QTreeWidgetItem
{
public:
    DJRankItem(quint32 userId, QTreeWidget * parent , time_t time,QString & strName,
    				quint32 iScore,quint32 iOrder,int type);
    virtual ~DJRankItem();
	time_t time();
	qint32 score();
	QString userName();
	quint32 userId() const { return m_userId; }
	quint32	order();
	void setOrder(quint32 iOrder);
protected:	
	qint32		m_iScore;
	time_t		m_time;
	QString		m_strName;
	quint32		m_iOrder;
	quint32 	m_userId;
};

class DJGameRankTabWidget : public QTabWidget
{
	Q_OBJECT
public :
	DJGameRankTabWidget(DJGameController* gameController, QWidget * parent,unsigned char chMaxItems);
	~DJGameRankTabWidget();
	
	void InsertIntoList(unsigned char chRankType,unsigned char chIndex,QString name,quint32 uid,qint32 iScore,time_t time);
	DJGameRankWidget * FindRankWidget(unsigned char chRankType);
private :
	QWidget *c_Parent;
	QTabWidget *c_Tab;
	unsigned char c_chMaxItems;
	DJGameController*	m_gameController;

};

class DJGameRankWidget : public DJTreeWidget
{
	Q_OBJECT
	public :
		DJGameRankWidget(unsigned char chRankType,unsigned char chMaxItems);
		~DJGameRankWidget();
		unsigned char GetRankType();
		DJRankItem*	FindUserItem(quint32 userId,time_t time);
		void incOrder(quint32 iOrder);
	private :
		unsigned char c_chRankType;
		unsigned char c_chMaxItems;
};


#endif
