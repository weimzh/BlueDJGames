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

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include "jdebug.h"

int g_debug_output_obj = JDEBUG_OUTPUT_OBJ_FILE;
#ifdef __DEBUG__
char g_debug_output_file[80]="/djgame/debug.info";

void debug2tty()
{
    g_debug_output_obj = JDEBUG_OUTPUT_OBJ_TTY;
}
void initializedebugfile(char * filename)
{
	char szHead[]="\n";
	FILE *dfile;
	strcpy(g_debug_output_file,filename);
	dfile = fopen(g_debug_output_file,"w");
	fwrite(szHead,strlen(szHead),1,dfile);
	fclose(dfile);
}

void outputdebugstring(char *pinfo,...)
{
	FILE *dfile;
	va_list	argptr;
	va_start(argptr,pinfo);
	
	
	if( g_debug_output_obj == JDEBUG_OUTPUT_OBJ_TTY )
	{
		vprintf(pinfo,argptr);
	}else
	{
		dfile = fopen(g_debug_output_file,"a");
		vfprintf(dfile,pinfo,argptr);
		if(pinfo[strlen(pinfo)-1]== 0x0a)
			fprintf(dfile,"[%d] -- ",time(NULL));
		fclose(dfile);
	}
	va_end(argptr);
}

void outputdebugbuffer(char *buf,unsigned short ushLen)
{
	int i =0;
	unsigned char ch;
	if(buf == NULL)
		return ;
	while(ushLen > 0 )
	{
		ch = *buf;
		DEBUG_OUTPUT(("%02x ",ch));
		buf ++;
		i++;
		if(i==32)
		{
			DEBUG_OUTPUT(("\n"));
			i = 0;
		}
		ushLen--;
	}
	DEBUG_OUTPUT(("\n"));
}
#endif
