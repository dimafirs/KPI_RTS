#define NUM_HARMONICS 8
#define MAX_FREQ 1100
#define NUM_SECTIONS 256
#define MAX_AMPLITUDE 5

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

/* return random double between 0..1 with uniform distribution */
static double rand_double() {
	return ((double) rand()) / ((double) RAND_MAX);
}

/* Calculate matimatical expectation in double subarray */
static double calc_me(double *data, int start, int end) {
	double result = 0;
	for(int i = start; i < end; i++)
		result += data[i];
	result /= end;
	return result;
}

/* Calculate variance for double subarray */
static double calc_variance(double *data, double me, int start, int end) {
	double result = 0;
	for(int i = start; i < end; i++)
		result += (data[i] - me) * (data[i] - me);
	result /= end - 1;
	return result;
}

/* Return difference in microseconds */
static double timespec_diff(struct timespec *stop, struct timespec *start)
{
	double diff = difftime(stop->tv_sec, start->tv_sec);
	diff *= 1e6;
	diff += (stop->tv_nsec - start->tv_nsec) / 1e3;
	return diff;
}

/* Plot chart via GNU Plot */
static void plot_chart(int x_limit, char *file_name){
	FILE *pipe = popen("gnuplot -persistent", "w");
	fprintf(pipe, "set title \"Lab1\" \n");
	fprintf(pipe, "plot [0:%d] '%s' with lines \n",
			x_limit, file_name);
}

int main(int argc, char* argv[]) {
	FILE *urandom = fopen("/dev/urandom", "rb");
	if (NULL == urandom)
		goto exit;
	unsigned int seed;
	fread(&seed, sizeof(seed), 1, urandom);
	if (ferror(urandom))
		goto exit;
	srand(seed);

	double X[NUM_SECTIONS], math_expect[NUM_SECTIONS], variance[NUM_SECTIONS];
	memset(X, 0, NUM_SECTIONS * sizeof(*X));
	memset(math_expect, 0, NUM_SECTIONS * sizeof(*math_expect));
	memset(variance, 0, NUM_SECTIONS * sizeof(*variance));

	double time_diffs[NUM_SECTIONS];
	memset(time_diffs, 0, NUM_SECTIONS * sizeof(*time_diffs));

	const double harmonic_step = ((double) MAX_FREQ) / ((double) NUM_HARMONICS);
	for(unsigned int i = 0; i < NUM_SECTIONS; i++){
		struct timespec now, after;
		clock_gettime(CLOCK_REALTIME, &now);
		for(unsigned int j = 0; j < NUM_HARMONICS; j++){
			/* X[i] = Ap * sin(Wp * t + fp)
			 * Ap - amplitude (random)
			 * Wp - one of harmonics
			 * fp - phase (random)
			 */
			double Ap = MAX_AMPLITUDE * rand_double();
			double Wp = (j + 1) * harmonic_step;
			double fp = 2 * M_PI * rand_double();
			X[i] += Ap * sin(Wp * i + fp);
		}
		math_expect[i] = calc_me(X, 0, i);
		variance[i] = calc_variance(X, math_expect[i], 0, i);
		clock_gettime(CLOCK_REALTIME, &after);
		time_diffs[i] = timespec_diff(&after, &now);
	}

#ifdef DEBUG
	printf("Calculated values: \n");
	for(unsigned int i = 0; i < NUM_SECTIONS; i++){
		printf("X[%d] = %8lf\n", i, X[i]);
	}
#endif

	FILE *signal_data = fopen("signal_values", "w");
	FILE *me_data = fopen("me_values", "w");
	FILE *variance_data = fopen("variance_values", "w");
	FILE *time_data = fopen("time_values", "w");
	for (unsigned int i = 0; i < NUM_SECTIONS; i++)
	{
		fprintf(signal_data, "%d %lf \n", i, X[i]);
		fprintf(me_data, "%d %lf \n", i, math_expect[i]);
		fprintf(variance_data, "%d %lf \n", i, variance[i]);
		fprintf(time_data, "%d %lf \n", i, time_diffs[i]);
	}

	plot_chart(NUM_SECTIONS, "signal_values");
	plot_chart(NUM_SECTIONS, "me_values");
	plot_chart(NUM_SECTIONS, "variance_values");
	plot_chart(NUM_SECTIONS, "time_values");

	printf("Successfully finish calculation\nStarting gnuplot...\n");
exit:
	exit(0);
}
