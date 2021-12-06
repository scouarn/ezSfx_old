#include "ezSfx_core.h"
#include "ezGfx_utils.h"

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

/* Sound config */
static unsigned int sampleRate;
static unsigned int channels;
static unsigned int queueLength;
static unsigned int blockSize;


/* Time */
static volatile bool running;
static double globalTime;

double EZ_sfx_getTime() {
	return globalTime;
}


/* callback */
static EZ_Sample_t (*callback) (double time, int channel); 

void EZ_sfx_setCallback_sample( EZ_Sample_t (*func) (double, int) ) {
	callback = func;
}



static void fillBlock(EZ_Sample_t* block) {
	int i, chan;

	if (callback)
	for (i = 0; i < blockSize; i += channels) {

		for (chan = 0; chan < channels; chan++)
			block[i + chan] = callback(globalTime, chan);

		globalTime += 1.0 / sampleRate;

	}
	else { 
		globalTime += channels * blockSize / sampleRate;
	}
}