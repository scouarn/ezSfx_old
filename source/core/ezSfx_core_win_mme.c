#include "ezSfx_core_common.h"
#include <windows.h>

/* references : */
/* https://www.codeproject.com/Questions/354466/Simple-Waveout-API-with-one-wave */
/* https://www.codeproject.com/Articles/866347/Streaming-Audio-to-the-WaveOut-Device */
/* https://www.planet-source-code.com/vb/scripts/ShowCode.asp?txtCodeId=4422&lngWId=3 */
/* https://gist.github.com/seungin/4779216eada24a5077ca1c5e857239ce */
/* https://github.com/OneLoneCoder/synth/blob/master/olcNoiseMaker.h */
/* https://www.youtube.com/watch?v=roRH3PdTajs&feature=youtu.be */
/* https://github.com/Planet-Source-Code/david-overton-playing-audio-in-windows-using-waveout-interface__3-4422 */

/* App */
static EZ_Sample_t (*callback) (double time, int channel); 
static HANDLE thread;
static DWORD WINAPI sfxThread(LPVOID lpParam);
static CRITICAL_SECTION criticalSection;
static DWORD CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);


/* Audio context */
WAVEFORMATEX audioInfo;
static const char* deviceName = NULL; /* default (unused on windows...) */


void EZ_sfx_stop() {
	EnterCriticalSection(&criticalSection);
	running = false;
	LeaveCriticalSection(&criticalSection);
}

void EZ_sfx_join() {
	WaitForSingleObject(thread, INFINITE);
}


void EZ_sfx_start(const char* device, unsigned int rate, unsigned int chans, unsigned int size, unsigned int length) {

	if (device)
		deviceName = device;

	sampleRate  = rate;
	channels    = chans;
	blockSize   = size;
	queueLength = length;


	audioInfo.nSamplesPerSec = sampleRate;
	audioInfo.wBitsPerSample = sizeof(EZ_Sample_t) * 8;
	audioInfo.nChannels = channels;
	audioInfo.wFormatTag = WAVE_FORMAT_PCM;
	audioInfo.nBlockAlign = sizeof(EZ_Sample_t) * channels;
	audioInfo.nAvgBytesPerSec = audioInfo.nBlockAlign * sampleRate;
	audioInfo.cbSize = 0;

	InitializeCriticalSection(&criticalSection);

	running = true;
	thread = CreateThread(NULL, 0, sfxThread, NULL, 0, NULL);
	SetThreadPriority(thread, THREAD_PRIORITY_TIME_CRITICAL);

}




static DWORD WINAPI sfxThread(LPVOID lpParam) {
	int i, nbFreeBlocks = queueLength, currentBlock = 0;
	globalTime = 0.0;

	HWAVEOUT device;
	EZ_assert( 
		MMSYSERR_NOERROR == waveOutOpen(&device, WAVE_MAPPER, &audioInfo, (DWORD_PTR)waveOutProc, (DWORD_PTR)&nbFreeBlocks, CALLBACK_FUNCTION),
		"Couldn't init audio device"
	);


	/* init block queue */
	WAVEHDR* blockHeaders = calloc(queueLength, sizeof(WAVEHDR));
	EZ_Sample_t* blockQueue = malloc( queueLength * blockSize * sizeof(EZ_Sample_t) );

	for(i = 0; i < queueLength; i++) {
		blockHeaders[i].dwBufferLength = blockSize * sizeof(EZ_Sample_t);
		blockHeaders[i].lpData = (LPSTR)(blockQueue + i*blockSize);
	}



	while(running) {

		/* wait for a block to be available */
		while(nbFreeBlocks <= 0) Sleep(1000 * blockSize / sampleRate);

		/* a block is free so lock it */
		EnterCriticalSection(&criticalSection);
		nbFreeBlocks--; /* ACCESSED BY ANOTHER THREAD, SO BE CAREFUL */
		LeaveCriticalSection(&criticalSection);

		/* fill the block */
		fillBlock(blockQueue + currentBlock*blockSize);

		/* if it's already prepared, unprepare it */
		if (blockHeaders[currentBlock].dwFlags & WHDR_PREPARED)
			waveOutUnprepareHeader(device, &blockHeaders[currentBlock], sizeof(WAVEHDR));

		/* render block */
		waveOutPrepareHeader(device, &blockHeaders[currentBlock], sizeof(WAVEHDR));
		waveOutWrite(device, &blockHeaders[currentBlock], sizeof(WAVEHDR));


		/* move to next block */
		currentBlock++;
		currentBlock %= queueLength;
	}



  	/* wait for all blocks to be played */
	while (nbFreeBlocks <= 0) Sleep(10);

	/* unprepare blocks */
	for (i = 0; i < queueLength; i++) 
		if (blockHeaders[i].dwFlags & WHDR_PREPARED)
			waveOutUnprepareHeader(device, &blockHeaders[i], sizeof(WAVEHDR));


	free(blockQueue);
	free(blockHeaders);

	waveOutClose(device);
	CloseHandle(sfxThread);
	DeleteCriticalSection(&criticalSection);

	return 0;
}



DWORD CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {

	int* nbFreeBlocks = (int*)dwInstance;

	if (uMsg == WOM_DONE) {
		EnterCriticalSection(&criticalSection);
		(*nbFreeBlocks)++; /* FROM ANOTHER THREAD, SO BE CAREFUL */
		LeaveCriticalSection(&criticalSection);
	}

	return 0;
}
