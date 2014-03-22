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

#include "DJGraphicsAnimationItem.h"
#include "DJAnimationImage.h"

static int	countAnimationItem	= 0;

DJGraphicsAnimationItem::DJGraphicsAnimationItem( const QString& file, QGraphicsItem * parent, QGraphicsScene * scene )
	:QGraphicsPixmapItem(parent,scene)
{
	djDebug()<<"DJGraphicsAnimationItem constructor"<<countAnimationItem++;
	
	m_animationImage	= new DJAnimationImage(file,this);
	connect(m_animationImage,SIGNAL(imageChanged()),this,SLOT(handleImageChanged()));
	handleImageChanged();
	start();
}
DJGraphicsAnimationItem::~DJGraphicsAnimationItem()
{
	djDebug()<<"DJGraphicsAnimationItem destructor"<<countAnimationItem--;
}
int DJGraphicsAnimationItem::type() const
{
	return DJGraphicsAnimationItemType;
}
void DJGraphicsAnimationItem::start()
{
	m_animationImage->start();
}
void DJGraphicsAnimationItem::stop()
{
	m_animationImage->stop();
}
void DJGraphicsAnimationItem::handleImageChanged()
{
	QImage image	= m_animationImage->image();
	setPixmap( QPixmap::fromImage(image) );
}
