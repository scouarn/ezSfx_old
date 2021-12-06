#include "ezSfx.h"

#include <stdio.h>
#include <stdlib.h>

EZ_Sample_t callback(double time, int channel) {

	return SAMPLE_MAX * 0.5 * EZ_osc_sine(time, 440, 0);
}


int main() {

	EZ_sfx_setCallback_sample(callback);
	EZ_sfx_start_default();
	EZ_sfx_join();

	return 0;
}