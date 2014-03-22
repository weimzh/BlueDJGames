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

#include "DJCard.h"
#include "protocol.h"

/*
static	QList<quint8> mutualCards1	= 	QList<quint8>()		<< DJGAME_GAMECARD_ANGEL
															<< DJGAME_GAMECARD_LUCKY
															<< DJGAME_GAMECARD_10MULTIPLE
															<< DJGAME_GAMECARD_DOUBLE
															<< DJGAME_GAMECARD_NHALF
															<< DJGAME_GAMECARD_5MULTIPLE;
static	QList<quint8> mutualCards2	= 	QList<quint8>()		<< DJGAME_GAMECARD_N2MULTIPLE
															<< DJGAME_GAMECARD_N5MULTIPLE
															<< DJGAME_GAMECARD_HALF;															
static	QList< QList<quint8> > mutualCards	=	QList< QList<quint8> >()	<<	mutualCards1
																			<<	mutualCards2;
*/
static CardInfo cardsInfo[]	= 
{
	{	DJGAME_GAMECARD_ANGEL,
		true,
		QT_TRANSLATE_NOOP("DJCard","angel card"),
		QT_TRANSLATE_NOOP("DJCard","avoid attacking"),
		":/BaseRes/image/card/angel.png",
		"angel.gif",
	},
	{	DJGAME_GAMECARD_LUCKY,
		true,
		QT_TRANSLATE_NOOP("DJCard","lucky card"),
		QT_TRANSLATE_NOOP("DJCard","win double score when you win, lose half score when you lose"),
		":/BaseRes/image/card/lucky.png",
		"lucky.gif",
	},
	{	DJGAME_GAMECARD_10MULTIPLE,
		true,
		QT_TRANSLATE_NOOP("DJCard","win 10 multiple card"),
		QT_TRANSLATE_NOOP("DJCard","win 10 multiple score when you win"),
		":/BaseRes/image/card/win10.png",
		"win10.gif",
	},
	{	DJGAME_GAMECARD_ZERO,
		true,
		QT_TRANSLATE_NOOP("DJCard","clear card"),
		QT_TRANSLATE_NOOP("DJCard","clear your score"),
		":/BaseRes/image/card/clear.png",
		"clear.gif",
	},
	{	DJGAME_GAMECARD_DOUBLE,
		true,
		QT_TRANSLATE_NOOP("DJCard","win double card"),
		QT_TRANSLATE_NOOP("DJCard","win double score when you win"),
		":/BaseRes/image/card/win2.png",
		"win2.gif",
	},
	{	DJGAME_GAMECARD_NHALF,
		true,
		QT_TRANSLATE_NOOP("DJCard","lose half card"),
		QT_TRANSLATE_NOOP("DJCard","lose half score when you lose"),
		":/BaseRes/image/card/losehalf.png",
		"losehalf.gif",
	},
	{	DJGAME_GAMECARD_5MULTIPLE,
		true,
		QT_TRANSLATE_NOOP("DJCard","win 5 multiple card"),
		QT_TRANSLATE_NOOP("DJCard","win 5 multiple score when you win"),
		":/BaseRes/image/card/win5.png",
		"win5.gif",
	},
	{	DJGAME_GAMECARD_N2MULTIPLE,
		false,
		QT_TRANSLATE_NOOP("DJCard","lose double card"),
		QT_TRANSLATE_NOOP("DJCard","lose double score when you lose"),
		":/BaseRes/image/card/lose2.png",
		"lose2.gif",
	},
	{	DJGAME_GAMECARD_N5MULTIPLE,
		false,
		QT_TRANSLATE_NOOP("DJCard","lose 5 multiple card"),
		QT_TRANSLATE_NOOP("DJCard","lose 5 multiple score when you lose"),
		":/BaseRes/image/card/lose5.png",
		"lose5.gif",
	},
	{	DJGAME_GAMECARD_HALF,
		false,
		QT_TRANSLATE_NOOP("DJCard","win half card"),
		QT_TRANSLATE_NOOP("DJCard","win half score when you win"),
		":/BaseRes/image/card/winhalf.png",
		"winhalf.gif",
	},
	{	DJGAME_GAMECARD_FLOWER,
		false,
		QT_TRANSLATE_NOOP("DJCard","flower card"),
		QT_TRANSLATE_NOOP("DJCard","present flower to whom you like"),
		":/BaseRes/image/card/flower.png",
		"flower.gif",
	},
	{	DJGAME_GAMECARD_BROADCAST,
		true,
		QT_TRANSLATE_NOOP("DJCard","broadcast card"),
		QT_TRANSLATE_NOOP("DJCard","broadcast a msg in this game"),
		":/BaseRes/image/card/broadcast.png",
		"broadcast.gif",
	},
	{	DJGAME_GAMECARD_EMOTION,
		true,
		QT_TRANSLATE_NOOP("DJCard","emotion card"),
		QT_TRANSLATE_NOOP("DJCard","show your emotion"),
		":/BaseRes/image/card/emotion.png",
		"",//see emotion info
	},
	{	DJGAME_GAMECARD_EGG,
		false,
		QT_TRANSLATE_NOOP("DJCard","egg card"),
		QT_TRANSLATE_NOOP("DJCard","throw egg to whom you dislike"),
		":/BaseRes/image/card/egg.png",
		"egg.gif",
	},
};
static EmotionCard	emotions[]	= {
	{	DJCard::Happy,
		QT_TRANSLATE_NOOP("DJCard","happy"),
		":/BaseRes/image/card/happy.png",
		"happy.gif",
	},
	{	DJCard::Angry,
		QT_TRANSLATE_NOOP("DJCard","angry"),
		":/BaseRes/image/card/angry.png",
		"angry.gif",
	},
	{	DJCard::Sad,
		QT_TRANSLATE_NOOP("DJCard","sad"),
		":/BaseRes/image/card/sad.png",
		"sad.gif",
	},
	{	DJCard::Laugh,
		QT_TRANSLATE_NOOP("DJCard","laugh"),
		":/BaseRes/image/card/laugh.png",
		"laugh.gif",
	},
	{	DJCard::Cry,
		QT_TRANSLATE_NOOP("DJCard","cry"),
		":/BaseRes/image/card/cry.png",
		"cry.gif",
	},
};												
DJCard::DJCard( quint8 id, quint8 pages )
	:m_id(id),m_pages(pages)
{
	m_leftSeconds	= 0;
}
DJCard::~DJCard()
{
}
QString DJCard::imagePath( quint8 id )
{
	for ( uint i = 0; i < sizeof(cardsInfo)/sizeof(cardsInfo[0]); i++ ) {
		if ( cardsInfo[i].id == id ) {
			return QString(cardsInfo[i].imagePath);
		}
	}
	return QString();
}
QString DJCard::movieName( quint8 id )
{
	for ( uint i = 0; i < sizeof(cardsInfo)/sizeof(cardsInfo[0]); i++ ) {
		if ( cardsInfo[i].id == id ) {
			return QString(cardsInfo[i].movieName);
		}
	}
	return QString();
}
QString DJCard::emotionImagePath( int emotion )
{
	for ( uint i = 0; i < sizeof(emotions)/sizeof(emotions[0]); i++ ) {
		if ( emotions[i].emotion == emotion ) {
			return QString(emotions[i].imagePath);
		}
	}
	return QString();
}
QString DJCard::emotionMovieName( int emotion )
{
	for ( uint i = 0; i < sizeof(emotions)/sizeof(emotions[0]); i++ ) {
		if ( emotions[i].emotion == emotion ) {
			return QString(emotions[i].movieName);
		}
	}
	return QString();
}
bool DJCard::isValid( quint8 id )
{
	for ( uint i = 0; i < sizeof(cardsInfo)/sizeof(cardsInfo[0]); i++ ) {
		if ( cardsInfo[i].id == id ) {
			return true;
		}
	}
	return false;
}
bool DJCard::isUsedOnly2Myself( quint8 id )
{
	for ( uint i = 0; i < sizeof(cardsInfo)/sizeof(cardsInfo[0]); i++ ) {
		if ( cardsInfo[i].id == id ) {
			return cardsInfo[i].selfUse;
		}
	}
	return true;
}
bool DJCard::isEmotionCard( quint8 id )
{
	return (id & DJGAME_GAMECARD_TYPE_EMOTION);
}
QString DJCard::name( quint8 id )
{
	for ( uint i = 0; i < sizeof(cardsInfo)/sizeof(cardsInfo[0]); i++ ) {
		if ( cardsInfo[i].id == id ) {
			return QCoreApplication::instance()->translate("DJCard",cardsInfo[i].name);
		}
	}
	return QCoreApplication::instance()->translate("DJCard","unknown card");
}
QString DJCard::emotionName( int emotion )
{
	for ( uint i = 0; i < sizeof(emotions)/sizeof(emotions[0]); i++ ) {
		if ( emotions[i].emotion == emotion ) {
			return QCoreApplication::instance()->translate("DJCard",emotions[i].name);
		}
	}
	return QCoreApplication::instance()->translate("DJCard","unknown emotion");
}
QString DJCard::description( quint8 id )
{
	for ( uint i = 0; i < sizeof(cardsInfo)/sizeof(cardsInfo[0]); i++ ) {
		if ( cardsInfo[i].id == id ) {
			return QCoreApplication::instance()->translate("DJCard",cardsInfo[i].description);
		}
	}
	return QCoreApplication::instance()->translate("DJCard","unknown card");
}
void DJCard::setPages( quint8 pages )
{
	m_pages	= pages;
}
void DJCard::decreasePages()
{
	if ( m_pages > 0 )
		m_pages--;
}
int DJCard::life() const
{
	if ( 0 != m_leftSeconds ) {
		int deltaSecs	= m_thenDateTime.secsTo( QDateTime::currentDateTime() );
		return m_leftSeconds - deltaSecs;
	} else
		return 0;
}
void DJCard::setLeftSeconds( int leftSeconds )
{
	m_leftSeconds	= leftSeconds;
	m_thenDateTime	= QDateTime::currentDateTime();
}



