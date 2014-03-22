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

#ifndef DJCARD_H
#define DJCARD_H

#include <QtCore>
#include "DJBaseGlobal.h"


struct CardInfo {
	quint8 	id;
	bool	selfUse;
	const char* name;
	const char*	description;
	const char* imagePath;
	const char* movieName;//path is always /images/avatars
};
struct EmotionCard {
	int			emotion;
	const char* name;
	const char* imagePath;
	const char* movieName;//path is always /images/avatars
};
class DJ_BASE_EXPORT DJCard
{
public:
	enum Emotion {
		Happy	= 1,
		Angry,
		Sad,
		Laugh,
		Cry,
	};
	DJCard( quint8 id, quint8 pages );
	virtual ~DJCard();
	
	quint8 id() const { return m_id; }
	quint8 pages() const { return m_pages; }
	int leftSeconds() const { return m_leftSeconds; }
	static QString imagePath( quint8 id );
	static QString movieName( quint8 id );
	static QString emotionImagePath( int emotion );
	static QString emotionMovieName( int emotion );
	static bool isValid( quint8 id );
	static bool isUsedOnly2Myself( quint8 id );
	static bool isEmotionCard( quint8 id );
	static QString name( quint8 id );
	static QString emotionName( int emotion );
	static QString description( quint8 id );
	//static void initCardNames();
	void setPages( quint8 pages );
	void decreasePages();
	void setLeftSeconds( int leftSeconds );
	int	life() const;
private:
	quint8		m_id;
	quint8		m_pages;
	int			m_leftSeconds;
	QDateTime	m_thenDateTime;
	//QString		m_name;
	//static	QList<quint8>	cardsOnly2Myself;
	//static	QHash<quint8,QString>	cardsInfo;
};
#endif
