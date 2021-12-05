#ifndef _EZSFX_OSCILLATOR_H_
#define _EZSFX_OSCILLATOR_H_



typedef double (*EZ_Oscillator_t) (double time, double freq, double param);

double EZ_osc_fastSine(double time, double freq, double p);
double EZ_osc_sine    (double time, double freq, double p);
double EZ_osc_square  (double time, double freq, double p);
double EZ_osc_triangle(double time, double freq, double p);
double EZ_osc_sawtooth(double time, double freq, double p);




#endif /* ezSfx_oscillator_h */