
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


/* interact with the audio thread */
void EZ_sfx_start();
void EZ_sfx_stop();
void EZ_sfx_join();


void EZ_sfx_setCallbackSample( EZ_Sample_t (*func) (double time, int channel) );


	
#endif
