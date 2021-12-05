#include "ezSfx_pcm.h"
#include "ezGfx_utils.h"
#include <stdlib.h>


EZ_Sample_t EZ_pcm_next(EZ_PCM_t* arr) {

	if (arr->data == NULL) {
		return 0;
	}

	EZ_Sample_t res = arr->data[arr->curr];
	
	arr->curr ++;
	arr->curr %= arr->size;

	return res;

}


void EZ_pcm_free(EZ_PCM_t* array) {
	free(array->data);
	free(array);
}



/* http://soundfile.sapp.org/doc/WaveFormat/ */
/* https://gist.github.com/Jon-Schneider/8b7c53d27a7a13346a643dac9c19d34f */

/* Magic numbers with adjusted endianness */
#define RIFF_MAGIC_NUMBER 0x46464952
#define WAVE_MAGIC_NUMBER 0x45564157
#define fmt__MAGIC_NUMBER 0x20746d66
#define data_MAGIC_NUMBER 0x61746164

struct __attribute__((__packed__)) riff_header {
	
	uint32_t ChunkID;   /* "RIFF" */
	int32_t  ChunkSize; /* Size of the rest of the file */
	uint32_t Format;    /* "WAVE" */
};


struct __attribute__((__packed__)) fmt_subchunk {

	uint32_t  SubchunkID;  /* "fmt " */
	int32_t SubchunkSize;  /*  Should be 16 for PCM */
	int16_t AudioFormat;   /*  1 -> PCM */
	int16_t NumChannels;
	int32_t SampleRate;
	int32_t ByteRate;      /*  SampleRate * NumChannels * sizeof(sample) */
	int16_t BlockAlign;    /*  NumChannels * sizeof(sample) -> bytes per alsa "frame" */
	int16_t BitsPerSample; /*  8 * sizeof(sample) */
};

struct __attribute__((__packed__)) data_subchunk {

	uint32_t SubchunkID;   /* "data" */
	int32_t  SubchunkSize; /* Number of bytes in data.  */
	/*  -> samples * num_channels * sizeof(sample) */
};




EZ_PCM_t* EZ_pcm_loadWAV(const char* fname) {

	/* open file */
	FILE *file = fopen(fname,"rb");

	if (file == NULL) {
		EZ_throw("Couldn't open file", fname);
		return NULL;
	}


	/* read riff header */
	struct riff_header riff;
	fread(&riff, sizeof(struct riff_header), 1, file);

	if (riff.ChunkID != RIFF_MAGIC_NUMBER || riff.Format != WAVE_MAGIC_NUMBER) {
		EZ_throw("Wrong magic numbers in riff header", fname);
		fclose(file);

		return NULL;
	}


	/* read ftm chunk */
	struct fmt_subchunk fmt;
	fread(&fmt, sizeof(struct fmt_subchunk), 1, file);

	if (fmt.SubchunkID != fmt__MAGIC_NUMBER) {
		EZ_throw("Wrong magic number in fmt chunk", fname);
		fclose(file);

		return NULL;
	}

	if (fmt.SubchunkSize != 16 || fmt.AudioFormat != 1) {
		EZ_throw("Only PCM wave file are supported", fname);
		fclose(file);

		return NULL;
	}

	if (fmt.NumChannels == 0) {
		EZ_throw("This audio file has 0 channels", fname);
		fclose(file);

		return NULL;
	}
	
	if (fmt.SampleRate == 0 || fmt.ByteRate == 0) {
		EZ_throw("This audio file has a sample rate of 0", fname);
		fclose(file);

		return NULL;
	}

	if (fmt.BitsPerSample != 16) {
		EZ_throw("Only 16bits signed integer audio is supported", fname);
		fclose(file);

		return NULL;
	}


	/* read data chunk */
	struct data_subchunk data;
	fread(&data, sizeof(struct data_subchunk), 1, file);

	if (data.SubchunkID != data_MAGIC_NUMBER) {
		EZ_throw("Wrong magic number in data section",fname);
		fclose(file);

		return NULL;
	}

	if (data.SubchunkSize == 0) {
		EZ_throw("This audio file has no samples", fname);
		fclose(file);

		return NULL;
	}



	/* Init pcm array */
	EZ_PCM_t* arr = malloc( sizeof(EZ_PCM_t) );
	
	arr->data = malloc(data.SubchunkSize);
	arr->curr = 0;
	arr->size = data.SubchunkSize / fmt.NumChannels;
	arr->sampleRate = fmt.SampleRate;
	arr->channels = fmt.NumChannels;


	/* read samples */	
	fread(arr->data, 1, data.SubchunkSize, file);

	return arr;

}


void EZ_save_WAV(EZ_PCM_t* arr, const char* fname) {

	/* open file */
	FILE *file = fopen(fname,"wb");

	if (file == NULL) {
		EZ_throw("Couldn't save file", fname);
		return;
	}


	/* init headers */
	struct riff_header riff = {
		.ChunkID = RIFF_MAGIC_NUMBER,
		.ChunkSize = 36 + arr->size * sizeof(EZ_Sample_t),
		.Format  = WAVE_MAGIC_NUMBER
	};

	struct fmt_subchunk fmt = {
		.SubchunkID = fmt__MAGIC_NUMBER,
		.SubchunkSize = 16,
		.AudioFormat = 1,
		.NumChannels = arr->channels,
		.SampleRate  = arr->sampleRate,
		.ByteRate = arr->channels * arr->sampleRate * sizeof(EZ_Sample_t),
		.BlockAlign = arr->channels * sizeof(EZ_Sample_t),
		.BitsPerSample = 8 * sizeof(EZ_Sample_t)
	};

	struct data_subchunk data = {
		.SubchunkID = data_MAGIC_NUMBER,
		.SubchunkSize = arr->size * sizeof(EZ_Sample_t)
	};


	/* write headers */
	fwrite(&riff, sizeof(struct riff_header),   1, file);
	fwrite(&fmt,  sizeof(struct fmt_subchunk),  1, file);
	fwrite(&data, sizeof(struct data_subchunk), 1, file);

	/* write pcm data */
	fwrite(arr->data, sizeof(EZ_Sample_t), arr->size, file);

}