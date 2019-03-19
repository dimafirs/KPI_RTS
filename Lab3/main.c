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

/* Rotation coefs W[k][n] */
static double complex W[SAMPLES_CNT][SAMPLES_CNT];

/* Array with signal values */
static double X[SAMPLES_CNT];

void with_table(int smpl_cnt) {
	int local_k = (smpl_cnt >> 2) - 1;
	double complex F[local_k];
	for(unsigned int k = 0; k < local_k; k++) {
		F[k] = 0;
		for(unsigned int n = 0; n < smpl_cnt; n++) {
			F[k] += X[n] * W[k][n];
		}
		F[k] /= smpl_cnt;
	}
}

void wo_table(int smpl_cnt) {
	int local_k = (smpl_cnt >> 2) - 1;
	double complex F[local_k];
	for(unsigned int k = 0; k < local_k; k++) {
		F[k] = 0;
		for(unsigned int n = 0; n < smpl_cnt; n++) {
			F[k] += X[n] * cexp((-2 * I * M_PI * k * n) / smpl_cnt);
		}
		F[k] /= smpl_cnt;
	}
}

int main(int argc, char* argv[]) {
	if(init_random()) {
		goto exit;
	}

	generate_signal(X, NUM_HARMONICS, SAMPLES_CNT, MAX_FREQ, MAX_AMPLITUDE);
//	generate_sine(X, SAMPLES_CNT);
#ifdef DEBUG
	printf("Calculated values: \n");
	for(unsigned int i = 0; i < SAMPLES_CNT; i++){
		printf("X[%d] = %8lf\n", i, creal(X[i]));
	}
#endif

	for(unsigned int i = 0; i < SAMPLES_CNT; i++) {
		for(unsigned int j = 0; j < SAMPLES_CNT; j++){
			W[i][j] = cexp((-2 * I * M_PI * i * j) / SAMPLES_CNT);
		}
	}

	double timestmps[SAMPLES_CNT];
	double timestmps_wo[SAMPLES_CNT];
	for(int i = 0; i < SAMPLES_CNT; i++) {
		timestmps[i] = 0;
		timestmps_wo[i] = 0;
	}

	for(int i = 4; i < SAMPLES_CNT; i += 2) {
		struct timespec start, end;
		clock_gettime(CLOCK_REALTIME, &start);
		with_table(i);
		clock_gettime(CLOCK_REALTIME, &end);
		timestmps[i] = timespec_diff(&end, &start);
	}

	for(int i = 4; i < SAMPLES_CNT; i += 2) {
		struct timespec start, end;
		clock_gettime(CLOCK_REALTIME, &start);
		wo_table(i);
		clock_gettime(CLOCK_REALTIME, &end);
		timestmps_wo[i] = timespec_diff(&end, &start);
	}

	const char *time_filename = "time_values";
	FILE *time_data = fopen(time_filename, "w");
	for (unsigned int i = 0; i < SAMPLES_CNT; i++) {
		fprintf(time_data, "%d %lf \n", i, timestmps[i]);
	}
	fclose(time_data);

	const char *time_wo_filename = "time_wo_values";
	FILE *time_wo_data = fopen(time_wo_filename, "w");
	for (unsigned int i = 0; i < SAMPLES_CNT; i++) {
		fprintf(time_wo_data, "%d %lf \n", i, timestmps_wo[i]);
	}
	fclose(time_wo_data);

	plot_chart(SAMPLES_CNT, time_filename, "With table");
	plot_chart(SAMPLES_CNT, time_wo_filename, "Without table");

	printf("Successfully finish calculation\nStarting gnuplot...\n");
exit:
	exit(0);
}
