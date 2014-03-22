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

#ifndef __JERRY_LINUX_DEBUG__
#define __JERRY_LINUX_DEBUG__

#ifndef FALSE
#define FALSE		0
#endif
#ifndef TRUE
#define TRUE		 1
#endif

//#define __DEBUG__

#define JDEBUG_OUTPUT_OBJ_TTY		 0
#define JDEBUG_OUTPUT_OBJ_FILE		1

#ifdef __DEBUG__

#define DEBUG_OUTPUT(arg)		outputdebugstring arg
#define DEBUG_INITFILE(arg)		initializedebugfile arg
#define DEBUG_TTY()			debug2tty()
#define DEBUG_BUFFER(arg)		outputdebugbuffer arg
#else

#define DEBUG_OUTPUT(arg)
#define DEBUG_INITFILE(arg)
#define DEBUG_TTY()
#define DEBUG_BUFFER(arg)
#endif 
// __DEBUG__


void outputdebugstring(char *pinfo,...);
void initializedebugfile(char * filename);
void debug2tty();
void outputdebugbuffer(char *buf,unsigned short ushLen);

#endif
