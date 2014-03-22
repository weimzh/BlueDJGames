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

#ifndef PLAYWAVE_H
#define PLAYWAVE_H

#include <QtGui>
#include "DJBaseGlobal.h"

#ifdef Q_WS_X11
typedef struct waveformat_tag {
    unsigned short wFormatTag;        /* format type */
    unsigned short nChannels;         /* number of channels (i.e. mono, stereo, etc.) */
    unsigned int    nSamplesPerSec;    /* sample rate */
    unsigned int nAvgBytesPerSec;   /* for buffer estimation */
    unsigned short nBlockAlign;       /* block size of data */
} WAVEFORMAT, *PWAVEFORMAT;

#define WAVE_FORMAT_PCM     1

/* specific waveform format structure for PCM data */
typedef struct pcmwaveformat_tag {
    unsigned short wFormatTag;        /* format type */
    unsigned short nChannels;         /* number of channels (i.e. mono, stereo, etc.) */
    unsigned int    nSamplesPerSec;    /* sample rate */
    unsigned int nAvgBytesPerSec;   /* for buffer estimation */
    unsigned short nBlockAlign;       /* block size of data */
    unsigned short wBitsPerSample;
} PCMWAVEFORMAT, *PPCMWAVEFORMAT;

typedef struct _WAVE_FILE_HEADER
{
 unsigned int            dwRiff;
 unsigned int      dwFileSize;
 unsigned int   dwWave;
 unsigned int   dwFormat;
 unsigned int   dwFormatLength;
 PCMWAVEFORMAT    waveFormat;
 unsigned int   dwData;
 unsigned int   dwDataLength;
} WAVE_FILE_HEADER;

void 	AsynPlayWaveFile(char *filename);
void* 	PlayWaveFile(void *filename);
#endif

DJ_BASE_EXPORT void PlayWave( const char* filename );

#endif
