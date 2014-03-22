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

#include <iostream>
//#include "jdebug.h"
#include "bufalloc.h"

#define BUFFER_ALLOC_HEAD_LEN	(sizeof(int)*3)

bool InitBufferForAlloc(char * buffer,unsigned int nMaxSize)
{
	if(nMaxSize < 128)
		return false;
	*((unsigned int *)(buffer+sizeof(int))) = nMaxSize;
	*((unsigned int *)buffer) = BUFFER_ALLOC_HEAD_LEN;
	return true;
}

char * BufferAlloc(char * buffer,unsigned short shAllocLen,bool bSetZero)
{
	unsigned int iMaxSize = *((unsigned int *)(buffer+sizeof(int)));
	unsigned int iCurLen = *((unsigned int *)buffer);
	
	char * p = buffer+ iCurLen;
	
	shAllocLen += sizeof(int);
	
	if(iCurLen+shAllocLen+sizeof(int) > iMaxSize)
		return NULL;
	*((int*)p) = shAllocLen;
	p += sizeof(int);
	iCurLen += shAllocLen+sizeof(int);
	*((int *)buffer) = iCurLen;
	if(bSetZero)
		memset(p,0,shAllocLen);
//	DEBUG_OUTPUT(("BufferAlloc success from %08X\n",buffer));
//	DEBUG_BUFFER((buffer,iCurLen));
	return p;
}

bool BufferFree(char *buffer,char *freep)
{
	unsigned int iCurLen = *((int *)buffer);
	unsigned int iBlockSize;
//	unsigned int iMaxSize = *((unsigned int *)(buffer+sizeof(int)));
	if(freep < buffer+sizeof(int)*3 || freep > buffer+iCurLen)
		return false;
//	DEBUG_OUTPUT(("free %08X from buffer(%08x)\n",freep,buffer));
	freep -= sizeof(int);
	if(freep < buffer+sizeof(int)*4)
		return false;
	iBlockSize = *((int *)freep);
	if(iBlockSize == 0 || freep + iBlockSize > buffer+iCurLen)
		return false;
	
	if(freep + iBlockSize >= buffer+iCurLen )
	{
		*((int *)buffer) -= (iBlockSize+sizeof(int));
	}
	
	return true;
}

char * GetFirstBlock(char *buffer)
{
	unsigned int iCurLen = *((int *)buffer);
	int iFirst = *((int *)(buffer+BUFFER_ALLOC_HEAD_LEN));
	if(iFirst == 0 || iFirst+BUFFER_ALLOC_HEAD_LEN +sizeof(int) > iCurLen)
		return NULL;
	return buffer+BUFFER_ALLOC_HEAD_LEN+sizeof(int);
}

unsigned int BufferGetAllocTotalSize(char *buffer)
{
//	unsigned int iCurLen = *((unsigned int *)buffer);
	return *((int *)buffer);
}

bool BufferSetAllocTotalSize(char *buffer,unsigned int iSize)
{
	unsigned int iMaxSize = *((unsigned int *)(buffer+sizeof(int)));
	if(iSize > iMaxSize)
		return false;
	*((int *)buffer) = iSize;
	return true;
}
