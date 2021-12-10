#include "ezSfx_core_common.h"

#include <pthread.h>
#include <alsa/asoundlib.h>


/* App */
static pthread_t thread;
static void* sfxThread(void* arg);

/* Audio context */
static snd_pcm_t *device;
static const char* deviceName = "pulse"; /* default */

/* /!\ sample vs "frame" trickery :
 * A frame is two samples with two channels */


void EZ_sfx_start(const char* device, unsigned int rate, unsigned int chans, unsigned int size, unsigned int length) {

	if (device)
		deviceName = device;

	sampleRate  = rate;
	channels    = chans;
	blockSize   = size;
	queueLength = length;

	running = true;
	pthread_create(&thread, NULL, &sfxThread, NULL);
}


void EZ_sfx_stop()  {
	running = false;
}

void EZ_sfx_join()  {
	pthread_join(thread, NULL);
}



static void renderBlock(EZ_Sample_t* block) {

	/* render the block */
	EZ_Sample_t *current = block;					 /* where the data is */
	snd_pcm_uframes_t frames = blockSize / channels; /* number of frames left to write */

	while (frames > 0) {

		int written = snd_pcm_writei(device, current, frames);

		if (written > 0) {
			current += written * channels;
			frames  -= written;
		}

		else if (written == -EPIPE) { /* an underrun occured, prepare the device for more data */
			snd_pcm_prepare(device);
			ERR_warning("Audio underrun");
		}

		else {
			ERR_warning("Couldn't write audio");
		}

	}
}


static void* sfxThread(void* arg) {

	pthread_detach(pthread_self());


	/* init alsa  */
	/* https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/Extensions/olcPGEX_Sound.h */
	/* https://soundprogramming.net/programming/alsa-tutorial-1-initialization/ */
	/* https://www.alsa-project.org/alsa-doc/alsa-lib/group___p_c_m___h_w___params.html#ga7242d7045ae23a9ae736c191030c25e8 */

	ERR_assert(
		snd_pcm_open(&device, deviceName, SND_PCM_STREAM_PLAYBACK, 0) >= 0, 
		"Couldn't open sound device"
	);

	snd_pcm_hw_params_t *params;
	snd_pcm_hw_params_malloc(&params);
	snd_pcm_hw_params_any(device, params);

	snd_pcm_hw_params_set_access(device, params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_channels(device, params, channels);
	snd_pcm_hw_params_set_rate_resample(device, params, 1); /* enabled */
	snd_pcm_hw_params_set_rate(device, params, sampleRate, 0);
	snd_pcm_hw_params_set_format(device, params, SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_period_size(device, params, blockSize, 0);
	snd_pcm_hw_params_set_periods(device, params, queueLength, 0);

	snd_pcm_hw_params(device, params);
	snd_pcm_prepare(device);
	snd_pcm_start(device);


	/* init block */
	EZ_Sample_t* block = malloc( channels * blockSize * sizeof(EZ_Sample_t) );

	globalTime = 0.0;

	/* main loop */
	while (running) {
		
		fillBlock(block);
		renderBlock(block);

	}

	snd_pcm_hw_params_free(params);
	snd_pcm_drain(device);
	snd_pcm_close(device);
	free(block);

	pthread_exit(NULL);
	return NULL;
}



