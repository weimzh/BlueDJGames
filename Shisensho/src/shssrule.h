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

#ifndef __DJGAME_SHISENSHO_RULE_H__
#define __DJGAME_SHISENSHO_RULE_H__

#define SHISENSHO_MAX_MAP								320

//消去2张牌后,是否向某方向做调整;
//周围牌的调整方式从0开始每过一关加1,直到 > SHISENSHO_ADJUST_MAX后归0
#define SHISENSHO_ADJUST_NULL			0x00
#define SHISENSHO_ADJUST_BOTTOM			0x01
#define SHISENSHO_ADJUST_TOP			0x02
#define SHISENSHO_ADJUST_LEFT			0x03
#define SHISENSHO_ADJUST_RIGHT			0x04
#define SHISENSHO_ADJUST_MAX			0x04

typedef struct __tabShisenshoRule
{
	unsigned char chAdjustOri;
	unsigned char chWidth;
	unsigned char chHeight;
	unsigned char chLifes;
	unsigned char chBaseScore;
	unsigned char chXX;
	unsigned short shXX1;
	unsigned short shXX2;
	unsigned short timeout;
}ShisenshoRule,*PShisenshoRule;

#define SHISENSHO_RULE_SEARCH_ORI_UP			0x01
#define SHISENSHO_RULE_SEARCH_ORI_DOWN			0x02
#define SHISENSHO_RULE_SEARCH_ORI_LEFT			0x04
#define SHISENSHO_RULE_SEARCH_ORI_RIGHT			0x08

bool ShisenshoRule_Check(unsigned char *pmap,unsigned char chWidth,unsigned char chHeight,
							unsigned char *pxs,unsigned char *pys);

void ShisenshoRule_DeleteCard(unsigned char *pmap,unsigned char chWidth,unsigned char chHeight,
								unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2,
								unsigned char chAdjustOri,
								void *pcallback, 
								void (*SHSSRule_DelCallBack)(void *pcallback,unsigned char *pmap,unsigned char chWidth,unsigned char chHeight,unsigned char x,unsigned char y,char dx,char dy));
bool ShisenshoRule_IsNull(unsigned char *pmap,unsigned char chWidth,unsigned char chHeight);

bool ShisenshoRule_CheckPipe(unsigned char *pmap,unsigned char chWidth,unsigned char chHeight,
							unsigned char sx,unsigned char sy,unsigned char ex,unsigned char ey);

unsigned char * ShisenshoRule_SearchNull(unsigned char *pmap,unsigned char chWidth,unsigned char chHeight,
							unsigned char x,unsigned char y ,unsigned char chOri,unsigned char *pxs,unsigned char *pys,unsigned char *pnumbers);

bool ShisenshoRule_SearchPath(unsigned char *pmap,unsigned char chWidth,unsigned char chHeight,
							unsigned char *pxs,unsigned char *pys);

bool ShisenshoRule_CopyLines(unsigned char *pmap,unsigned char chWidth,unsigned char chHeight,
				unsigned char chStartLine,unsigned char chEndLine,unsigned char *pbuffer);
bool ShisenshoRule_SetLines(unsigned char *pmap,unsigned char chWidth,unsigned char chHeight,
				unsigned char chStartLine,unsigned char chEndLine,unsigned char *pbuffer);
				
unsigned char * ShisenshoRule_GetPoint(unsigned char *pmap,unsigned char chWidth,unsigned char chHeight,
											unsigned char x,unsigned char y);

bool ShisenshoRule_SetPoint(unsigned char *pmap,unsigned char chWidth,unsigned char chHeight,
											unsigned char x,unsigned char y,unsigned char chCard);


void ShisenshoRule_CreateAllCards(unsigned char *pcardlist,unsigned short shMax);
bool ShisenshoRule_InitializeMap(unsigned char *pcardlist,
								unsigned char *pmap,unsigned char chWidth,unsigned char chHeight);
void ShisenshoRule_ResetMap(unsigned char *pmap,unsigned char chWidth,unsigned char chHeight);

								
#endif
