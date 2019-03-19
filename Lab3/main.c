#define NUM_HARMONICS	5
#define MAX_FREQ		1100
#define SAMPLES_CNT		256
#define MAX_AMPLITUDE	5
#define K	(SAMPLES_CNT >> 1) - 1 // cnt of freq in transform
//#define DEBUG
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
	double X[SAMPLES_CNT];
	generate_signal(X, NUM_HARMONICS, SAMPLES_CNT, MAX_FREQ, MAX_AMPLITUDE);
//	generate_sine(X, SAMPLES_CNT);
#ifdef DEBUG
	printf("Calculated values: \n");
	for(unsigned int i = 0; i < SAMPLES_CNT; i++){
		printf("X[%d] = %8lf\n", i, creal(X[i]));
	}
#endif

	double complex F[K];
	for(unsigned int k = 0; k < K; k++) {
		F[k] = 0;
		for(unsigned int n = 0; n < SAMPLES_CNT; n++) {
			F[k] += X[n] * cexp( (-2 * I * M_PI * n * k) / SAMPLES_CNT);
		}
		F[k] /= SAMPLES_CNT;
	}

#ifdef DEBUG
	printf("Calculated values: \n");
	for(unsigned int i = 0; i < K; i++){
		printf("Freal[%d] = %8lf\n", i, creal(F[i]));
	}
#endif

	const char *signal_filename = "signal_values";
	FILE *signal_data = fopen(signal_filename, "w");
	for (unsigned int i = 0; i < SAMPLES_CNT; i++) {
		fprintf(signal_data, "%d %lf \n", i, X[i]);
	}
	fclose(signal_data);

	const char *ampl_filename = "amplitude_values";
	const char *phase_filename = "phase_values";
	FILE *ampl_data = fopen(ampl_filename, "w");
	FILE *phase_data = fopen(phase_filename, "w");
	/* Amlitude is a absolute val of F(p),  phase - argument */
	for (unsigned int i = 0; i < K; i++) {
		fprintf(ampl_data, "%d %lf \n", i, cabs(F[i]));
		fprintf(phase_data, "%d %lf \n", i, carg(F[i]));
	}
	fclose(phase_data);
	fclose(ampl_data);

	plot_chart(SAMPLES_CNT, signal_filename, "Generated signal");
	plot_bargraph(K, ampl_filename, "Amplitude response");
	plot_bargraph(K, phase_filename, "Phase response");

	/* Reverse DFT */
	for(unsigned int n = 0; n < SAMPLES_CNT; n++) {
		X[n] = 0;
		for(unsigned int k = 0; k < K; k++) {
			/* Imaginary harmonic is odd and disappear in this transition */
			X[n] += 2 * F[k] * cexp( (2 * I * M_PI * n * k) / SAMPLES_CNT);
		}
	}

	const char *reverse_signal_filename = "reverse_signal_values";
	FILE *reverse_signal_data = fopen(reverse_signal_filename, "w");
	for (unsigned int i = 0; i < SAMPLES_CNT; i++) {
		fprintf(reverse_signal_data, "%d %lf \n", i, X[i]);
	}
	fclose(reverse_signal_data);
	plot_chart(SAMPLES_CNT, reverse_signal_filename, "Reversed signal");

	printf("Successfully finish calculation\nStarting gnuplot...\n");
exit:
	exit(0);
}
