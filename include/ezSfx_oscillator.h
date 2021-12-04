#ifndef _EZSFX_OSCILLATOR_H_
#define _EZSFX_OSCILLATOR_H_



typedef double (*EZ_Oscillator_t) (double time, double freq, double param);

double EZ_sfx_fastSine(double time, double freq, double p);
double EZ_sfx_sine    (double time, double freq, double p);
double EZ_sfx_square  (double time, double freq, double p);
double EZ_sfx_triangle(double time, double freq, double p);
double EZ_sfx_sawtooth(double time, double freq, double p);




#endif /* ezSfx_oscillator_h */