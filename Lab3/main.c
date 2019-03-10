#define NUM_HARMONICS	5
#define MAX_FREQ		1100
#define NUM_SECTIONS	256
#define MAX_AMPLITUDE	5
#define P				4 // power of transform

#include "mylib.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <complex.h>

int main(int argc, char* argv[]) {
	if(init_random()) {
		goto exit;
	}

	/* Array with signal values */
	double X[NUM_SECTIONS];
	memset(X, 0, NUM_SECTIONS * sizeof(*X));
	generate_signal(X, NUM_HARMONICS, NUM_SECTIONS, MAX_FREQ, MAX_AMPLITUDE);
#ifdef DEBUG
	printf("Calculated values: \n");
	for(unsigned int i = 0; i < NUM_SECTIONS; i++){
		printf("X[%d] = %8lf\n", i, X[i]);
	}
#endif

	/* DFT rotation coefficients */
	const double complex W[P][P] = {	{ 1.0, 1.0, 1.0, 1.0},
										{ 1.0, -I, -1.0, I},
										{ 1.0, -1.0, 1.0, -1.0},
										{ 1.0, I, -1.0, -I}
									};
	double complex F[P];
	for(unsigned int i = 0; i < NUM_SECTIONS; i++) {
		for(unsigned int j = 0; j < P; j++) {
			int k = i % P; /* calculate position of current rotation coef */
			F[j] += X[i] * W[k][j];
		}
	}

	const char *signal_filename = "signal_values";
	const char *ampl_filename = "amplitude_values";
	const char *phase_filename = "phase_values";

	FILE *signal_data = fopen(signal_filename, "w");
	for (unsigned int i = 0; i < NUM_SECTIONS; i++) {
		fprintf(signal_data, "%d %lf \n", i, X[i]);
	}
	fclose(signal_data);

	FILE *ampl_data = fopen(phase_filename, "w");
	FILE *phase_data = fopen(ampl_filename, "w");
	/* Amlitude is a real part of F(p),  phase - imaginary */
	for (unsigned int i = 0; i < P; i++) {
		fprintf(ampl_data, "%d %lf \n", i, creal(F[i]));
		fprintf(phase_data, "%d %lf \n", i, cimag(F[i]));
	}
	fclose(phase_data);
	fclose(ampl_data);

	plot_chart(NUM_SECTIONS, signal_filename, signal_filename);
	plot_bargraph(P, ampl_filename);
	plot_bargraph(P, phase_filename);

	printf("Successfully finish calculation\nStarting gnuplot...\n");
exit:
	exit(0);
}
