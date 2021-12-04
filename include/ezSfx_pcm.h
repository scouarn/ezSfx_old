#ifndef _EZSFX_PCM_H_
#define _EZSFX_PCM_H_

#include <stdint.h>
#include "ezSfx_sample.h"


typedef struct {
	EZ_Sample_t* data;	/* array of samples */
	EZ_Sample_t* curr;	/* pointer to element of array (->sample to be played) */
	uint32_t   size;		/* array length */
	uint32_t   channels;	/* number of channels */
	uint32_t   sampleRate;  /* samples per seconds * channels */

} EZ_PCM_t;


EZ_PCM_t* EZ_pcm_loadWAV(const char* fname);
EZ_PCM_t* EZ_pcm_saveWAV(EZ_PCM_t* pcm);

void EZ_sfx_pcmFree(EZ_PCM_t* pcm);

EZ_Sample_t EZ_sfx_pcmNextSample(EZ_PCM_t* array);


#endif /* ezSfx_pcm_h */