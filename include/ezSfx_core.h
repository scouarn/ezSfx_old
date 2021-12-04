
/*
/
/	EZSFX
/
/
/ SCOUARN, 2021
/
/	Sound output.
/
/
/
*/


#ifndef EZSFX_CORE
#define EZSFX_CORE

#include "ezSfx_sample.h"

/* init audio */
void EZ_sfx_init(int sampleRate, int channels, int blockQueueLength, int blockSize);
#define EZ_sfx_init_default() EZ_sfx_init(CD44KHZ, 2, 16, 512);

/* interact with the audio thread */
void EZ_sfx_start();
void EZ_sfx_stop();
void EZ_sfx_join();

void EZ_sfx_setCallback_sample( EZ_Sample_t (*func) (double time, int channel) );

double EZ_sfx_getTime();
	
#endif
