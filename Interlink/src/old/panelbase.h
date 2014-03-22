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

#ifndef __DJGAME_CLIENT_PANELBASE_H__
#define __DJGAME_CLIENT_PANELBASE_H__

#include<QtGui>
#include <Qt3Support>
#include "DJGamePanel.h"
#include "DJGameUser.h"
static const int imageRTTI = 984376;

class DominoImageItem: public Q3CanvasRectangle
{
public:
    DominoImageItem( QImage img, Q3Canvas *canvas,unsigned short tid,unsigned short sid,unsigned short type);
    int rtti () const { return imageRTTI; }
    bool hit( const QPoint&) const;
    void SetImageMark(bool b);
    
    short c_chTable,c_chSeat,c_chType;
	bool	c_bMark;
protected:
    void drawShape( QPainter & );
private:
    QImage image;
    QPixmap pixmap;
    QPixmap pixmark;
};

Q3CanvasPixmapArray * InitTimerMovie();

QString GetTempSaveFileName(unsigned short shGameID,unsigned char index=0);
void InitSaveFileHead(DJGamePanel *panel,QString &name,unsigned short shGameID,unsigned char chType);
char * GetSaveData(QString &name,unsigned short shGameID,unsigned char chType,unsigned int *iLen);
void AppendData2SaveFile(QString &name,char *data,unsigned int shLen);

#endif
