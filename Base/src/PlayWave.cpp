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

#include "PlayWave.h"

#ifdef Q_WS_X11

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <linux/soundcard.h>
#include <pthread.h>
#include <sys/stat.h>
#include <errno.h>

pthread_t 		g_X11PlayWaveThread= 0;
char 			g_X11PlayWaveFileName[255]={0};
pthread_mutex_t 	g_mutex_PlayWave = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t		g_cond_PlayWave =PTHREAD_COND_INITIALIZER;

void AsynPlayWaveFile(const char *filename)
{
    pthread_attr_t attr;
  //  printf("AsynPlayWaveFile\n");
	if(g_X11PlayWaveThread == 0)
	{
    	pthread_attr_init(&attr);
    	pthread_create(&g_X11PlayWaveThread,&attr,PlayWaveFile,(void *)NULL);
	}
	int iRet = pthread_mutex_trylock(&(g_mutex_PlayWave));
	if(iRet == EBUSY)
	{
	//	printf("busy\n");
			return ;
	}
	//printf("signal\n");
	strcpy(g_X11PlayWaveFileName,filename);
	pthread_cond_signal(&(g_cond_PlayWave));
	pthread_mutex_unlock(&(g_mutex_PlayWave));

}
void * PlayWaveFile(void *p)
{
     int wavfd;
	int fd;
  char *buf;
  struct stat statbuf; 
  int i;
  WAVE_FILE_HEADER wavhead;
 
  pthread_mutex_lock(&(g_mutex_PlayWave));
  //printf("PlayWaveFile start\n");
  while(true)
  {
	  pthread_cond_wait(&(g_cond_PlayWave),&(g_mutex_PlayWave));
	//  printf("playwave\n");
	  if(strlen(g_X11PlayWaveFileName) == 0)
	  {
		  pthread_mutex_unlock(&(g_mutex_PlayWave));
		  continue;
	  }
  
   	fd = open("/dev/dsp", O_WRONLY);
	if (fd == -1)
    {
		memset(g_X11PlayWaveFileName,0,255);
		  pthread_mutex_unlock(&(g_mutex_PlayWave));
		  continue;
	  }
	
	if( stat((char *)g_X11PlayWaveFileName,&statbuf) == -1) 
    {
		memset(g_X11PlayWaveFileName,0,255);
		  pthread_mutex_unlock(&(g_mutex_PlayWave));
		  close(fd);
		  continue;
	  }
  	if(statbuf.st_size <= sizeof(WAVE_FILE_HEADER))
    {
		memset(g_X11PlayWaveFileName,0,255);
		  pthread_mutex_unlock(&(g_mutex_PlayWave));
		  close(fd);
		  continue;
	  }
 
  	wavfd = open((char *)g_X11PlayWaveFileName,O_RDONLY);
  	read(wavfd,&wavhead,sizeof(WAVE_FILE_HEADER));

   	i=0;
  
   i=wavhead.waveFormat.nSamplesPerSec;
   ioctl (fd,SNDCTL_DSP_SPEED,(char *)&i);
   i=wavhead.waveFormat.nChannels;
   ioctl (fd,SNDCTL_DSP_CHANNELS,(char *)&i);
   if(wavhead.waveFormat.wBitsPerSample == 0x08)
       i= AFMT_S8;
   else
   	i=AFMT_S16_LE;
   ioctl (fd,SNDCTL_DSP_SETFMT,(char *)&i);

	i = wavhead.dwDataLength;
	if( i > (statbuf.st_size - sizeof(WAVE_FILE_HEADER)))
			i = statbuf.st_size - sizeof(WAVE_FILE_HEADER);
  buf = (char *)malloc(i+0x0F);
  read(wavfd,buf,i);
  write(fd, buf,i); 
  free(buf);
  close(wavfd);
	close(fd);
	memset(g_X11PlayWaveFileName,0,255);
	pthread_mutex_unlock(&(g_mutex_PlayWave));
}
 return NULL;
}
#endif

void PlayWave( const char* filename )
{
#ifdef Q_WS_X11
	AsynPlayWaveFile(filename);
#else
	QSound::play(filename);
#endif
}
