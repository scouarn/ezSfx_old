#ifndef _EZSFX_NOTE_H_
#define _EZSFX_NOTE_H_

#include "ezSfx_oscillator.h"
#include "ezSfx_sample.h"



typedef struct {
	EZ_Oscillator_t oscillator;	
	double osciParam; 
	double pitch;

	double length;
	double amp;

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

void EZ_note_play(EZ_Note_t* note); /* add note on the queue */
EZ_Sample_t EZ_note_output(double time, int channel); /* add in the callback */



#endif /* ezSfx_note_h */