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

#include "protocol.h"
#include "DJGameController.h"
#include "DJGameUser.h"
#include "DJGameRoom.h"
#include "DJTreeItemDelegate.h"
#include "DJTreeWidget.h"
#include "DJGameRank.h"


DJRankItem::DJRankItem(quint32 userId, QTreeWidget * parent , time_t time,QString & strName,quint32 iScore,quint32 iOrder,int type)
			: QTreeWidgetItem(parent,type)
{	
	m_userId = userId;
	m_time = time;
	m_strName = strName;
	m_iOrder = iOrder;
	m_iScore = iScore;
}
DJRankItem::~DJRankItem()
{
}
time_t DJRankItem::time()
{
	return m_time;
}

qint32 DJRankItem::score()
{
	return m_iScore;
}

QString DJRankItem::userName()
{
	return m_strName;
}

quint32	DJRankItem::order()
{
	return m_iOrder;
}

void DJRankItem::setOrder(quint32 iOrder)
{
	m_iOrder = iOrder;
}


//////////////////////////////////////////////////////////////////
DJGameRankWidget::DJGameRankWidget(unsigned char chRankType,unsigned char chMaxItems)
						: DJTreeWidget(NULL)
{
	c_chRankType = chRankType;
	c_chMaxItems = chMaxItems;
}

DJGameRankWidget::~DJGameRankWidget()
{
}

unsigned char DJGameRankWidget::GetRankType()
{
	return c_chRankType;
}

DJRankItem*	DJGameRankWidget::FindUserItem(quint32 userId,time_t time)
{
	for ( int i = 0; i < topLevelItemCount(); i++ )
	{
		DJRankItem *userItem	= dynamic_cast<DJRankItem *>(topLevelItem(i));
		if ( userItem && userId == userItem->userId() && time == userItem->time()) {
			return userItem;
		}
	}
	return NULL;
}

void DJGameRankWidget::incOrder(quint32 iOrder)
{
	for ( int i = 0; i < topLevelItemCount(); i++ )
	{
		DJRankItem *userItem	= dynamic_cast<DJRankItem *>(topLevelItem(i));
		if ( userItem && userItem->order() >= iOrder)
		{
			userItem->setOrder(userItem->order()+1);
			userItem->setText(0,QString("%1").arg(userItem->order()));
		}
	}
}
//////////////////////////////////////////////////////////////////

DJGameRankTabWidget::DJGameRankTabWidget(DJGameController* gameController, QWidget * parent,unsigned char chMaxItems)
			:QTabWidget(parent),m_gameController(gameController)
{
	c_chMaxItems = chMaxItems;
	setMinimumHeight(100);
}

DJGameRankTabWidget::~DJGameRankTabWidget()
{
}
void DJGameRankTabWidget::InsertIntoList(unsigned char chRankType,unsigned char iOrder,QString name,quint32 uid,qint32 iScore,time_t time)
{
	DJGameRankWidget *pwRank = FindRankWidget(chRankType);
	if(pwRank == NULL)
		return;
	
	DJRankItem* userItem	= pwRank->FindUserItem(uid,time);
	if(userItem == NULL)
	{
		pwRank->incOrder(iOrder);
		int type;
		if ( uid == m_gameController->selfUserId() )
			type	= QTreeWidgetItem::UserType;
		else
			type	= QTreeWidgetItem::Type;
		userItem = new DJRankItem(uid,pwRank,time ,name,iScore,iOrder,type);
		if(userItem)
		{
			userItem->setText(0,QString("%1").arg(userItem->order()));
			userItem->setText(1,name);
			userItem->setText(2,QString("%1").arg(userItem->score()));
			time_t time = userItem->time();
			userItem->setText(3,QString(ctime(&time)));
		}
	}
	
}

DJGameRankWidget * DJGameRankTabWidget::FindRankWidget(unsigned char chRankType)
{
	DJGameRankWidget *pwRank;
	int i = 0;
	do
	{
		pwRank = (DJGameRankWidget*)widget(i);
		if(pwRank != NULL)
		{
			if(pwRank->GetRankType() == chRankType)
				return pwRank;
		}
		i++;
	}while(pwRank != NULL);
	pwRank = new DJGameRankWidget(chRankType,c_chMaxItems);
	if(pwRank != NULL)
	{	
		pwRank->setItemDelegate(new DJTreeItemDelegate(this));
		pwRank->setRootIsDecorated(false);
		QStringList headerSections;
		headerSections <<tr("order")<< tr("name") << tr("score")<<tr("time")<<"-";
		pwRank->setColumnCount( headerSections.size() );
	
		QTreeWidgetItem* headerItem	= new QTreeWidgetItem;
		for ( i = 0; i < headerSections.size(); i++) {
			headerItem->setTextAlignment( i, Qt::AlignHCenter );
			headerItem->setText( i, headerSections.at(i) );
			
		}
		pwRank->header()->setClickable(false);
		pwRank->header()->setResizeMode(0,QHeaderView::Custom);
		pwRank->setHeaderItem( headerItem );
		
		QString strName;
		switch(chRankType)
		{
			case DJGAME_RANK_TYPE_DAILY :
				strName = tr("Daily");
				break;
			case DJGAME_RANK_TYPE_WEEKLY :
				strName = tr("Weekly");
				break;
			case DJGAME_RANK_TYPE_MONTHLY :
				strName = tr("Monthly");
				break;
			case DJGAME_RANK_TYPE_YEARLY :
				strName = tr("Yearly");
				break;
			case DJGAME_RANK_TYPE_HISTORY :
				strName = tr("History");
				break;
			default :
				strName = tr("Unkown");
				break;
		}
		addTab(pwRank,strName);
	}
	return pwRank;
}
