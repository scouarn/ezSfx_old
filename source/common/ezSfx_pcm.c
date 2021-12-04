#include "ezSfx_pcm.h"

#include <stdlib.h>


EZ_Sample_t EZ_sfx_pcmNextSample(EZ_PCM_t* arr) {

	if (arr->data == NULL || arr->curr - arr->data >= arr->size) return 0;

	else return *++arr->curr;

}

void EZ_sfx_pcmFree(EZ_PCM_t* array) {
	free(array->data);
}
