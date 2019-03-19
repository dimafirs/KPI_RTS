#include "mylib.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

/* return random double between 0..1 with uniform distribution */
static inline double rand_double() {
	return ((double) rand()) / ((double) RAND_MAX);
}
/* Calculate matimatical expectation in double subarray */
double calc_me(double *data, int start, int end) {
	double result = 0;
	for(int i = start; i < end; i++)
		result += data[i];
	result /= end - start;
	return result;
}

/* Calculate variance for double subarray */
double calc_variance(double *data, double me, int start, int end) {
	double result = 0;
	for(int i = start; i < end; i++)
		result += (data[i] - me) * (data[i] - me);
	result /= end - start - 1;
	return result;
}

/* Plot chart via GNU Plot */
void plot_chart(int x_limit, const char *file_name, const char *title) {
	FILE *pipe = popen("gnuplot -persistent", "w");
	fprintf(pipe, "set title \"%s\" \n", title);
	fprintf(pipe, "plot [0:%d] '%s' with lines, 0 with lines \n",
			x_limit, file_name);
}

void plot_bargraph(int x_limit, const char *filename, const char *title) {
	FILE *pipe = popen("gnuplot -persistent", "w");
	/* Some hardcode :( */
	fprintf(pipe, "set boxwidth 0.1 \n");
	fprintf(pipe, "set style fill solid \n");
	fprintf(pipe, "set grid\n");
	fprintf(pipe, "set title \"%s\"\n", title);
	fprintf(pipe, "plot [-1:%d] \"%s\" w boxes, 0 with lines\n", x_limit, filename);
}

/* Generate static random process with parameters */
void generate_signal(double *result, int harm_cnt,
						int section_cnt, int max_freq, int max_ampl)
{
	const double harmonic_step = ((double) max_freq) / ((double) harm_cnt);
	for(unsigned int i = 0; i < section_cnt; i++){
		result[i] = 0;
		for(unsigned int j = 0; j < harm_cnt; j++){
			/* X[i] = Ap * sin(Wp * t + fp)
			 * Ap - amplitude (random)
			 * Wp - one of harmonics
			 * fp - phase (random)
			 */
			double Ap = max_ampl * rand_double();
			double Wp = (j + 1) * harmonic_step;
			double fp = 2 * M_PI * rand_double();
			result[i] += Ap * sin(Wp * j + fp);
		}
	}
}

/* DFT debug function to see 2 harmonic on transition */
void generate_sine(double *result, int cnt) {
	for(int i = 0; i < cnt; i++)
		result[i] = sin(i*M_PI/5) + 3*sin(i*M_PI/32);
}

/* Return difference in microseconds */
double timespec_diff(struct timespec *stop, struct timespec *start) {
	double diff = difftime(stop->tv_sec, start->tv_sec);
	diff *= 1e6;
	diff += (stop->tv_nsec - start->tv_nsec) / 1e3;
	return diff;
}

int init_random() {
	FILE *urandom = fopen("/dev/urandom", "rb");
	if (NULL == urandom)
		return -1;
	unsigned int seed;
	fread(&seed, sizeof(seed), 1, urandom);
	if (ferror(urandom))
		return -1;
	srand(seed);
	fclose(urandom);
	return 0;
}
