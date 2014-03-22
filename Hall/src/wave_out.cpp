#include "wave_out.h"

#define MAXWAVESIZE     4294967040LU
#define MAX_WAVEBLOCKS    32

const int	SAMPLE_RATE		= 8000;//8000,16000,32000,11025,22050,44100.
const int	CHANNELS		= 1;
const int	BITS_PER_SAMPLE	= 16;

CRITICAL_SECTION  cs;
HWAVEOUT          dev                    = NULL;
int               ScheduledBlocks        = 0;
int               PlayedWaveHeadersCount = 0;          // free index
WAVEHDR*          PlayedWaveHeaders [MAX_WAVEBLOCKS];

static int
Box ( const char* msg )
{
	//MessageBox ( NULL, msg, " "VERSION_STRING": Error Message . . .", MB_OK | MB_ICONEXCLAMATION );
	return -1;
}


/*
 *  This function registers already played WAVE chunks. Freeing is done by free_memory(),
 */

void CALLBACK
wave_callback ( HWAVE hWave, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2 )
{
	if ( uMsg == WOM_DONE ) {
		EnterCriticalSection ( &cs );
		PlayedWaveHeaders [PlayedWaveHeadersCount++] = (WAVEHDR*) dwParam1;
		LeaveCriticalSection ( &cs );
	}
}

void
free_memory ( void )
{
	WAVEHDR*  wh;
	HGLOBAL   hg;

	EnterCriticalSection ( &cs );
	wh = PlayedWaveHeaders [--PlayedWaveHeadersCount];
	ScheduledBlocks--;                               // decrease the number of USED blocks
	LeaveCriticalSection ( &cs );

	waveOutUnprepareHeader ( dev, wh, sizeof (WAVEHDR) );

	hg = GlobalHandle ( wh -> lpData );       // Deallocate the buffer memory
	GlobalUnlock (hg);
	GlobalFree   (hg);

	hg = GlobalHandle ( wh );                 // Deallocate the header memory
	GlobalUnlock (hg);
	GlobalFree   (hg);
}

void WIN_Audio_open()
{
	WAVEFORMATEX  outFormat;
	UINT          deviceID = WAVE_MAPPER;

	outFormat.wFormatTag=WAVE_FORMAT_PCM;
	outFormat.nSamplesPerSec=SAMPLE_RATE;//²ÉÑùÆµÂÊ
	outFormat.nChannels=CHANNELS;
	outFormat.wBitsPerSample=BITS_PER_SAMPLE;
	
	outFormat.nBlockAlign=outFormat.wBitsPerSample/8 * outFormat.nChannels;
	outFormat.nAvgBytesPerSec=outFormat.nSamplesPerSec * outFormat.nBlockAlign;
	//outFormat.nBlockAlign=4;
	//outFormat.nAvgBytesPerSec=176400;
	outFormat.cbSize=0;

	waveOutOpen ( &dev, deviceID, &outFormat, (DWORD)wave_callback, 0, CALLBACK_FUNCTION );
	
	waveOutReset ( dev );
	InitializeCriticalSection ( &cs );
	SetPriorityClass ( GetCurrentProcess (), HIGH_PRIORITY_CLASS );
}

int
WIN_Play_Samples ( const void* data, size_t len )
{
	HGLOBAL    hg;
	HGLOBAL    hg2;
	LPWAVEHDR  wh;
	void*      allocptr;

	do {
		while ( PlayedWaveHeadersCount > 0 )                // free used blocks ...
			free_memory ();

		if ( ScheduledBlocks < sizeof(PlayedWaveHeaders)/sizeof(*PlayedWaveHeaders) ) // wait for a free block ...
			break;
		Sleep (26);
	} while (1);

	if ( (hg2 = GlobalAlloc ( GMEM_MOVEABLE, len )) == NULL )   // allocate some memory for a copy of the buffer
		return Box ( "GlobalAlloc failed." );

	allocptr = GlobalLock (hg2);
	CopyMemory ( allocptr, data, len );                         // Here we can call any modification output functions we want....

	if ( (hg = GlobalAlloc (GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof (WAVEHDR))) == NULL ) // now make a header and WRITE IT!
		return -1;

	wh                   = (LPWAVEHDR)GlobalLock (hg);
	wh -> dwBufferLength = len;
	wh -> lpData         = (LPSTR)allocptr;

	if ( waveOutPrepareHeader ( dev, wh, sizeof (WAVEHDR)) != MMSYSERR_NOERROR ) {
		GlobalUnlock (hg);
		GlobalFree   (hg);
		return -1;
	}

	if ( waveOutWrite ( dev, wh, sizeof (WAVEHDR)) != MMSYSERR_NOERROR ) {
		GlobalUnlock (hg);
		GlobalFree   (hg);
		return -1;
	}

	EnterCriticalSection ( &cs );
	ScheduledBlocks++;
	LeaveCriticalSection ( &cs );

	return len;
}
int
WIN_Audio_close ()
{
	if ( dev != NULL ) {

		while ( ScheduledBlocks > 0 ) {
			Sleep (ScheduledBlocks);
			while ( PlayedWaveHeadersCount > 0 )         // free used blocks ...
				free_memory ();
		}

		waveOutReset (dev);      // reset the device
		waveOutClose (dev);      // close the device
		dev = NULL;
	}

	DeleteCriticalSection ( &cs );
	ScheduledBlocks = 0;
	return 0;
}