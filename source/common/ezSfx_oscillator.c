#include "ezSfx_oscillator.h"
#include "ezSfx_utils.h"
#include <math.h>


double EZ_sfx_sine(double time, double freq, double p) {
	
	return sin(time*freq*PI*2.0);
}


double EZ_sfx_fastSine(double time, double freq, double p) {

	double j = time*freq;                        // = time * 2 pi * f   / (2*pi)
	j -= (int)j;                                 //garder la partie décimale pour avoir la périodicité
	
	return 20.785 * j * (j - 0.5) * (j - 1.0f);  //approx de sin par x(x-pi)(x-2pi) en gros...
}

