#include "ezSfx_core.h"
#include "ezSfx_utils.h"

#include <pthread.h>
#include <string.h>
#include <stdbool.h>

#include <alsa/asoundlib.h>


/* App */
static pthread_t thread;
static void* sfxThread(void* arg);
static EZ_Sample_t (*callback) (double time, int channel); 

/* Device IO */
static snd_pcm_t *device;

/* Time */
static volatile bool running;
static double globalTime;

/* Sound config */
static int sampleRate;
static int channels;
static int queueLength;
static int blockSize;


void EZ_sfx_setCallback_sample( EZ_Sample_t (*func) (double, int) ) {
	callback = func;
}

double EZ_sfx_getTime() {
	return globalTime;
}

void EZ_sfx_start() {
	running = true;
	pthread_create(&thread, NULL, &sfxThread, NULL);
}

void EZ_sfx_stop()  {
	running = false;
}

void EZ_sfx_join()  {
	pthread_join(thread, NULL);
}


void EZ_sfx_init(int rate, int chans, int nbblocks, int size) {

		sampleRate  = rate;
		channels    = chans;
		blockSize   = size;
		queueLength = nbblocks;


		/* init alsa  */
		/* https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/Extensions/olcPGEX_Sound.h */

		EZ_assert(
			snd_pcm_open(&device, "default", SND_PCM_STREAM_PLAYBACK, 0) >= 0, 
			"Couldn't open sound device"
		);


		snd_pcm_hw_params_t *params;
		snd_pcm_hw_params_alloca(&params);
		snd_pcm_hw_params_any(device, params);

		snd_pcm_hw_params_set_access(device, params, SND_PCM_ACCESS_RW_INTERLEAVED);
		snd_pcm_hw_params_set_format(device, params, SND_PCM_FORMAT_S16_LE);
		snd_pcm_hw_params_set_rate(device, params, sampleRate, 0);
		snd_pcm_hw_params_set_channels(device, params, channels);
		snd_pcm_hw_params_set_period_size(device, params, blockSize, 0);
		snd_pcm_hw_params_set_periods(device, params, queueLength, 0);
		snd_pcm_hw_params(device, params);

}

void* sfxThread(void* arg) {
	/* /!\ sample vs "frame" trickery :
	 * A frame is two samples with two channels */

	pthread_detach(pthread_self());

	globalTime = 0.0;
	double dt = 1.0 / sampleRate;


	/* init block */
	EZ_Sample_t* block = malloc( blockSize * sizeof(EZ_Sample_t) );
	snd_pcm_start(device);


	/* main loop */
	while (running) {
		int i, chan;

		/* fill the block with user data */
		if (callback) 
		for (i = 0; i < blockSize; i += channels) {

			for (chan = 0; chan < channels; chan++)
				block[i + chan] = callback(globalTime, chan);

			globalTime += dt;
		}

		else globalTime += blockSize / sampleRate;



		/* render the block */
		EZ_Sample_t *current = block;
		snd_pcm_uframes_t frames = blockSize / channels;

		while (frames > 0) {

			int rc = snd_pcm_writei(device, current, frames);

			if (rc > 0) {
				current   += rc * channels;
				frames -= rc;
			}

			if (rc == -EAGAIN) continue;
			if (rc == -EPIPE) /* an underrun occured, prepare the device for more data */
				snd_pcm_prepare(device);

		}


	}


	snd_pcm_drain(device);
	snd_pcm_close(device);
	free(block);


	pthread_exit(NULL);
	return NULL;

}



