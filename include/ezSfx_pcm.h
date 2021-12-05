#ifndef _EZSFX_PCM_H_
#define _EZSFX_PCM_H_

#include <stdint.h>
#include <stddef.h>

#include "ezSfx_sample.h"


typedef struct {
	EZ_Sample_t* data; /* array of samples */
	size_t curr;       /* index to sample to be played */
	size_t size;       /* array length */
	int channels;      /* number of channels */
	int sampleRate;    /* samples per seconds * channels */

} EZ_PCM_t;


EZ_PCM_t* EZ_pcm_loadWAV(const char* fname);
void EZ_pcm_saveWAV(EZ_PCM_t* pcm);
void EZ_pcm_free(EZ_PCM_t* pcm);

/* return current sample and increment */
EZ_Sample_t EZ_pcm_next(EZ_PCM_t* array);


#endif /* ezSfx_pcm_h */