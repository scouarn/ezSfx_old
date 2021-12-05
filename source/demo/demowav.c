#include "ezSfx.h"

#include <stdio.h>
#include <stdlib.h>

EZ_PCM_t* audio;

EZ_Sample_t callback(double time, int channel) {
	
	EZ_Sample_t res = EZ_pcm_next(audio);

	if (audio->curr == 0) EZ_sfx_stop(); /* sample has loop once */

	return res;
}


int main(int argc, char** argv) {

	if (argc == 1) {
		printf("Usage : demowav audio_file.wav\n");
		exit(1);
	}

	audio = EZ_pcm_loadWAV(argv[1]);

	EZ_sfx_setCallback_sample(callback);
	EZ_sfx_init_default();

	EZ_sfx_start();
	EZ_sfx_join();

	return 0;
}