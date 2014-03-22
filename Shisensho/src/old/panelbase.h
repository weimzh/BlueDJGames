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

#ifndef __DJGAME_CLIENT_PANELBASE_H__
#define __DJGAME_CLIENT_PANELBASE_H__

#include<QtGui>
#include <Qt3Support>
#include "DJGamePanel.h"
#include "DJGameUser.h"
static const int imageRTTI = 984376;

class SHSSImageItem: public Q3CanvasRectangle
{
public:
    SHSSImageItem( QImage img, Q3Canvas *canvas,unsigned short tid,unsigned short sid,unsigned short type);
    int rtti () const { return imageRTTI; }
    bool hit( const QPoint&) const;
    void SetImageMark(bool b);
    bool GetImageMark();
    short c_chTable,c_chSeat,c_chType;
protected:
    void drawShape( QPainter & );
private:
    QImage image;
    QPixmap pixmap;
    QPixmap pixmark;
    bool	c_bMark;
};

char GetSeatPostion(unsigned char chRoomSeats,unsigned char chMySeat,unsigned char chSeat,bool bClockwise);
void GetPosition(QWidget *parent,quint8 chPos,int *x,int *y);

/*
Q3CanvasPixmapArray * InitTimerMovie();


void AppendUser2List(DJGamePanel *panel,QTreeWidget * sv,quint8 seatHeader,quint32 uid,quint8 chColor);
void SetUserScore(DJGamePanel *panel,QTreeWidget *sv,quint32 uid,int iScore,quint8 scoreHeader,bool bSet);

void AddUser(DJGamePanel *panel,QTreeWidget * sv,quint8 seatHeader,quint32 userId,quint8 seatId);
void RemoveUser( DJGamePanel *panel,QTreeWidget * sv,quint32 userId );

QString GetTempSaveFileName(unsigned short shGameID,unsigned char index=0);
void InitSaveFileHead(DJGamePanel *panel,QString &name,unsigned short shGameID,unsigned char chType);
char * GetSaveData(QString &name,unsigned short shGameID,unsigned char chType,unsigned int *iLen);
void AppendData2SaveFile(QString &name,char *data,unsigned int shLen);
*/

#endif
