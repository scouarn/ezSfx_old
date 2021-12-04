#ifndef _EZSFX_NOTE_H_
#define _EZSFX_NOTE_H_

#include "ezSfx_oscillator.h"
#include "ezSfx_sample.h"



typedef struct {
	EZ_Oscillator_t oscillator;
	
	double startTime;
	double endTime;
	double length;

	double pitch;

	double vibrato;
	double vibratoAmp;

	double tremolo;
	double tremoloAmp;

	double panning;
	double panningAmp;

	double attack;
	double decay;
	double sustain;
	double release;

} EZ_Note_t;

void EZ_sfx_play(EZ_Note_t* note); /* add note on the queue */



#endif /* ezSfx_note_h */