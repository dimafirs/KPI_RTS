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
	const int Nmax = 16384, Nmin = 256;
	int iteration = (int) log2(Nmax / Nmin) + 2;
	init_random();

	struct timespec start, end;
	double time_dft[iteration];
	double time_fft[iteration];
	int iter = 0;
	clock_gettime(CLOCK_REALTIME, &start);
	for (unsigned int N = Nmin; N <= Nmax;N = N << 1) {
		printf("i=%u\n", iter);
		printf("N=%u\n", N);

		double complex **dft_coeffs = malloc(sizeof(*dft_coeffs) * N);
		double complex *fft_coeffs = malloc(sizeof(*fft_coeffs) * N);
		for (unsigned int i = 0; i < N; i++)
			dft_coeffs[i] = malloc(sizeof(*(dft_coeffs[i])) * N);
		double *X = malloc(sizeof(*X) * N);

		generate_signal(X, NUM_HARMONICS, N, MAX_FREQ, MAX_AMPLITUDE);

		for (int k = 0; k < N; k++) {
			fft_coeffs[k] = cexp(-2 * M_PI * I * k / N );
			for (int n = 0; n < N; n++) {
				dft_coeffs[k][n] = cexp((-2 * I * M_PI * n * k) / N);
			}
		}

		double complex *F_fft = malloc(sizeof(*F_fft) * N);
		double complex *F_dft = malloc(sizeof(*F_dft) * N);
		for(int i = 0; i < N; i++) {
			F_fft[i] = X[i];
		}

		struct timespec now, after;

		clock_gettime(CLOCK_REALTIME, &now);
		fft(F_fft, fft_coeffs, N);
		clock_gettime(CLOCK_REALTIME, &after);
		time_fft[iter] = timespec_diff(&after, &now);

		clock_gettime(CLOCK_REALTIME, &now);
		dft(X, F_dft, dft_coeffs, N);
		clock_gettime(CLOCK_REALTIME, &after);
		time_dft[iter] = timespec_diff(&after, &now);

		for(int i = 0; i < N; i++){
			free(dft_coeffs[i]);
		}
		free(dft_coeffs);
		free(fft_coeffs);
		free(X);
		free(F_dft);
		free(F_fft);
		iter++;
	}
	clock_gettime(CLOCK_REALTIME, &end);

	const char *fft_filename = "fft_values";
	const char *dft_filename = "dft_values";
	FILE *fft_data = fopen(fft_filename, "w");
	FILE *dft_data = fopen(dft_filename, "w");
	/* Amlitude is a absolute val of F(p),  phase - argument */
	for (unsigned int i = 0; i < iteration; i++) {
		fprintf(fft_data, "%d %lf \n",(int) pow(2, i + log2(Nmin)), time_fft[i]);
		fprintf(dft_data, "%d %lf \n",(int) pow(2, i + log2(Nmin)), time_dft[i]);
	}
	fclose(fft_data);
	fclose(dft_data);

	plot_charts(Nmax, fft_filename, dft_filename, "FFT vs DFT");
	plot_onechart(Nmax, fft_filename, "FFT only");


	printf("Successfully finish calculation\nTime spent - %f s, Starting gnuplot...\n",
			(timespec_diff(&end, &start) / 1e6));
	exit(0);
}
