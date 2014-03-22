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

#include<QtGui>
#include <Qt3Support>

#include "protocol.h"
#include "panelbase.h"
#include "DJGamePanel.h"
#include "DJGameUser.h"
#include "DJGameRoom.h"


/*
Q3CanvasPixmapArray * InitTimerMovie()
{
	Q3ValueList	<QPixmap> pixlist;
	int frame;
	
	pixlist.clear();
	frame	= 1;
	while (frame <= 8 ) {
		pixlist.push_back(QPixmap(QString(":/BaseRes/image/movie/timer/timer%1.png").arg(frame)) );
		pixlist.push_back(QPixmap(QString(":/BaseRes/image/movie/timer/timer%1.png").arg(frame)) );
		pixlist.push_back(QPixmap(QString(":/BaseRes/image/movie/timer/timer%1.png").arg(frame)) );
		frame++;
	}
	return new Q3CanvasPixmapArray(pixlist);
}
*/
//////////////////////////////////////////////////
DominoImageItem::DominoImageItem( QImage img, Q3Canvas *canvas,unsigned short tid,unsigned short sid,unsigned short type)
    : Q3CanvasRectangle( canvas ), image(img),pixmark( QPixmap() )
{
    setSize( image.width(), image.height() );
    c_chTable = tid;
    c_chSeat  = sid;
    c_chType  = type;
#if !defined(Q_WS_QWS)
    //pixmap.convertFromImage(image, OrderedAlphaDither);
    pixmap	= QPixmap::fromImage(image,Qt::OrderedAlphaDither);
    pixmark.resize(pixmap.width(),pixmap.height());
    pixmark.fill(Qt::white);
    QPainter p(&pixmark);
    p.drawPixmap(0,0,pixmap);
    p.setPen(QPen(QColor(0,255,0),2));
    p.drawRect(0,0,width()-1,height()-1);
    //p.end();
#endif
    setWinding(FALSE);
    c_bMark = false;
}

void DominoImageItem::drawShape( QPainter &p )
{
// On Qt/Embedded, we can paint a QImage as fast as a QPixmap,
// but on other platforms, we need to use a QPixmap.
#if defined(Q_WS_QWS)
    p.drawImage( int(x()), int(y()), image, 0, 0, -1, -1, OrderedAlphaDither );
#else
    if(c_bMark)
        p.drawPixmap( int(x()), int(y()), pixmark);
    else
        p.drawPixmap( int(x()), int(y()), pixmap );
#endif
}

void DominoImageItem::SetImageMark(bool b)
{
    c_bMark = b;
	hide();
	show();
}
bool DominoImageItem::hit( const QPoint &p ) const
{
    int ix = p.x()-int(x());
    int iy = p.y()-int(y());
    if ( !image.valid( ix , iy ) )
	return FALSE;
    QRgb pixel = image.pixel( ix, iy );
    return qAlpha( pixel ) != 0;
}


///////////////////////////////////////////////

///////////////////////////////////////////////


QString GetTempSaveFileName(unsigned short shGameID,unsigned char index)
{
	QDir dir = QDir::home();
    if (!dir.exists(QString("DJGameSave")))
	{
		 dir.mkdir("DJGameSave");
	}
	dir.cd("DJGameSave");
	QString name =dir.path();
	char buffer[32];
	sprintf(buffer,"%04x%02x_%d.sav",shGameID,index,rand());
	
	name += QString(buffer);
	return name;
}

typedef struct __tagDJGameSavePlayer
{
	unsigned char chGender;
	unsigned char chSeat;
	unsigned char szName[40];
}DJGameSavePlayer,*PDJGameSavePlayer;

typedef struct __tagDJGameSaveHead
{
	unsigned short shHeadLen;
	unsigned short shGameID;
	unsigned char  chType;
	unsigned char  chSeats;
	unsigned char  chMySeat;
	DJGameSavePlayer players[1];
}DJGameSaveHead,*PDJGameSaveHead;

void InitSaveFileHead(DJGamePanel *panel,QString &name,unsigned short shGameID,unsigned char chType)
{
	char *p;
	unsigned short shSize;
	PDJGameSaveHead	phead;
	
	if(panel == NULL)
		return ;
	DJGameRoom* nroom = panel->gameRoom();
	if(nroom->numberOfSeats() < 2)
			return ;
		
	QFile file(name);

	if (file.open(QIODevice::WriteOnly))
	{
		
		shSize = sizeof(DJGameSaveHead)+ sizeof(DJGameSavePlayer)* (nroom->numberOfSeats()-1);
		
		p = (char *)malloc(shSize+0xFF);
		memset(p,0,shSize+0xFF);
		phead = (PDJGameSaveHead)p;
		phead->shHeadLen = shSize;
		phead->shGameID = shGameID;
		phead->chType = chType;
		phead->chMySeat = panel->selfSeatId();
		phead->chSeats = nroom->numberOfSeats();
		quint32 uid,nLen,i;
		DJGameUser *puser;
		QString strUserName;
		for(i=0;i<nroom->numberOfSeats();i++)
		{
			uid = panel->userIdOfSeat(i+1);
			puser = panel->gameUser(uid);
			phead->players[i].chSeat = i+1;
			if(puser!=NULL)
			{
				phead->players[i].chGender = puser->gender();
				strUserName = puser->userName();
				QByteArray ba=strUserName.toUtf8();
				nLen = strlen(ba.data());
				if(nLen > 36)
					nLen = 36;
				memcpy(phead->players[i].szName,ba.data(),nLen);
			}
		}
		file.write((const char *)p,shSize);
		file.flush();
	}
}

char * GetSaveData(QString &name,unsigned short shGameID,unsigned char chType,unsigned int *iLen)
{
	QFile file(name);
	char *p,*ph;
	DJGameSaveHead *phead;
	
	if (file.open(QIODevice::ReadOnly))
	{
		if(file.size()<=3)
		{
			return NULL;
		}
		unsigned short shHeadLen=0,shSize;
		file.read((char *)&shHeadLen,sizeof(short));
		if(shHeadLen < sizeof(DJGameSaveHead) || file.size() < shHeadLen+3)
			return NULL;
		file.seek(0);
		ph = (char *)malloc(shHeadLen);
		file.read(ph,shHeadLen);
		phead =(PDJGameSaveHead)ph;
		
		if(shGameID != phead->shGameID || chType != phead->chType || 
				shHeadLen != sizeof(DJGameSaveHead)+ sizeof(DJGameSavePlayer)* (phead->chSeats-1) )
		{
			return NULL;
		}
		
		shSize = file.size()-shHeadLen;
		p = (char *)malloc(shSize + 0xFF);
		file.read(p,file.size());
		*iLen = shSize;
		return p;
	}
	return NULL;
}

void AppendData2SaveFile(QString &name,char *data,unsigned int shLen)
{
	QFile file(name);

	if (file.open(QIODevice::Append))
	{
//    	QDataStream out(&file);
//		out<<(unsigned short)shLen; 
		file.write((const char *)data,shLen);
		file.flush();
	}
}
