#include "ezSfx.h"

#include <stdio.h>
#include <stdlib.h>

EZ_Sample_t callback(double time, int channel) {

	return SAMPLE_MAX * 0.5 * EZ_note_output(time, channel);
}


int main() {

	EZ_Note_t note = {0};

	note.oscillator = EZ_osc_sine;
	note.pitch = 440.0;
	note.length = 1.0;
	note.amp = 1.0;

	EZ_note_play(&note);

	EZ_sfx_setCallback_sample(callback);
	EZ_sfx_start_default();
	EZ_sfx_join();

	return 0;
}