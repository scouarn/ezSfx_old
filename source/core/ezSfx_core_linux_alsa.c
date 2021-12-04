#include "ezSfx_core.h"
#include "ezGfx_utils.h"

#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <alsa/asoundlib.h>




static volatile bool running;
static pthread_t thread;
static void* sfxThread(void* arg);
static snd_pcm_t *device;

static double globalTime;
static EZ_Sample_t (*callback) (double time, int channel); 

static struct {
	int sampleRate;
	int channels;
	int blockQueueLength;
	int blockSize;
} info;



void EZ_sfx_init(int sampleRate, int channels, int blockQueueLength, int blockSize) {

		info.sampleRate = sampleRate;
		info.channels   = channels;
		info.blockSize  = blockSize;
		info.blockQueueLength = blockQueueLength;


		//init alsa  https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/Extensions/olcPGEX_Sound.h
		int rc = snd_pcm_open(&device, "default", SND_PCM_STREAM_PLAYBACK, 0);
		EZ_assert(rc >= 0, "Couldn't open sound device");


		snd_pcm_hw_params_t *params;
		snd_pcm_hw_params_alloca(&params);
		snd_pcm_hw_params_any(device, params);

		snd_pcm_hw_params_set_access(device, params, SND_PCM_ACCESS_RW_INTERLEAVED);
		snd_pcm_hw_params_set_format(device, params, SND_PCM_FORMAT_S16_LE);
		snd_pcm_hw_params_set_rate(device, params, sampleRate, 0);
		snd_pcm_hw_params_set_channels(device, params, channels);
		snd_pcm_hw_params_set_period_size(device, params, blockSize, 0);
		snd_pcm_hw_params_set_periods(device, params, blockQueueLength, 0);
		snd_pcm_hw_params(device, params);


}

void EZ_sfx_start() {
	running = true;
	pthread_create(&thread, NULL, &sfxThread, NULL);
}

void EZ_sfx_stop()  {
	running = false;
	printf("SFX STOP !!!\n");
}

void EZ_sfx_join()  {
	pthread_join(thread, NULL);
}


void* sfxThread(void* arg) {
	//!!\\ sample vs "frame" trickery

	pthread_detach(pthread_self());




	globalTime = 0.0;
	double dt = 1.0 / info.sampleRate;

	//init block
	EZ_Sample_t* block = malloc(info.blockSize*sizeof(EZ_Sample_t));
	snd_pcm_start(device);

	//main loop
	while (running) {

	//fill the block
	if (callback) {	
		for (uint32_t i = 0; i < info.blockSize; i+= info.channels) {

			for (int chan = 0; chan < info.channels; chan++)
				block[i + chan] = callback(globalTime, chan);

			globalTime += dt;
		}
	}
	else {
		globalTime += info.blockSize / info.sampleRate;
	}


	 //render the block
	 snd_pcm_uframes_t frames = info.blockSize / info.channels;
	 EZ_Sample_t *current = block;

	 while (frames > 0) {

			int rc = snd_pcm_writei(device, current, frames);
			if (rc > 0) {
				current   += rc * info.channels;
				frames -= rc;
			}

			if (rc == -EAGAIN) continue;
			if (rc == -EPIPE) // an underrun occured, prepare the device for more data
				snd_pcm_prepare(device);

	 }


	}

	snd_pcm_drain(device);
	snd_pcm_close(device);
	free(block);

	pthread_exit(NULL);
	return NULL;

}



