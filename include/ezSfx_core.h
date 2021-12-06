#ifndef _EZSFX_CORE_H_
#define _EZSFX_CORE_H_


#include "ezSfx_sample.h"

/* interact with the audio thread */
void EZ_sfx_start(const char* device, unsigned int rate, unsigned int chans, unsigned int blockSize, unsigned int nbBlocks);
void EZ_sfx_stop();
void EZ_sfx_join();

#define EZ_sfx_start_default() EZ_sfx_start(NULL, 44100, 2, 1024, 16);

void EZ_sfx_setCallback_sample( EZ_Sample_t (*func) (double time, int channel) );

double EZ_sfx_getTime();


#endif /* ezSfx_core_h */
