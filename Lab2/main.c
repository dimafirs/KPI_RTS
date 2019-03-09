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

/* Generate static random process with parameters */
static void generate_signal(double *result, int harm_cnt,
						int section_cnt, int max_freq, int max_ampl)
{
	const double harmonic_step = ((double) max_freq) / ((double) harm_cnt);
	for(unsigned int i = 0; i < section_cnt; i++){
		for(unsigned int j = 0; j < harm_cnt; j++){
			/* X[i] = Ap * sin(Wp * t + fp)
			 * Ap - amplitude (random)
			 * Wp - one of harmonics
			 * fp - phase (random)
			 */
			double Ap = max_ampl * rand_double();
			double Wp = (j + 1) * harmonic_step;
			double fp = 2 * M_PI * rand_double();
			result[i] += Ap * sin(Wp * i + fp);
		}
	}
}

static double calc_correlation(double *X, double *Y, int size, double Mx, double My, int tau) {
	double result;
	for(int i = 0; i < size; i++) {
		result += (X[i] - Mx) * (Y[i + tau] - My);
	}
	return result/(double)(size-1);
}
/* Calculate matimatical expectation in double subarray */
static double calc_me(double *data, int start, int end) {
	double result = 0;
	for(int i = start; i < end; i++)
		result += data[i];
	result /= end - start;
	return result;
}

/* Plot chart via GNU Plot */
static void plot_chart(int x_limit, char *file_name) {
	FILE *pipe = popen("gnuplot -persistent", "w");
	fprintf(pipe, "set title \"Lab2\" \n");
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
	fclose(urandom);

	double X[NUM_SECTIONS], Y[NUM_SECTIONS];
	memset(X, 0, NUM_SECTIONS * sizeof(*X));
	memset(Y, 0, NUM_SECTIONS * sizeof(*Y));

	generate_signal(X, NUM_HARMONICS, NUM_SECTIONS, MAX_FREQ, MAX_AMPLITUDE);
	generate_signal(Y, NUM_HARMONICS, NUM_SECTIONS, MAX_FREQ, MAX_AMPLITUDE);
	double Mx = calc_me(X, 0, NUM_SECTIONS);
	double My = calc_me(Y, 0, NUM_SECTIONS);
	double *correl_xx = malloc(sizeof(*correl_xx) * (NUM_SECTIONS >> 1));
	double *correl_xy = malloc(sizeof(*correl_xy) * (NUM_SECTIONS >> 1));
	for (int tau = 0; tau < (NUM_SECTIONS >> 1) - 1; tau++){
		correl_xx[tau] = calc_correlation(X, X, NUM_SECTIONS >> 1, Mx, Mx, tau);
		correl_xy[tau] = calc_correlation(X, Y, NUM_SECTIONS >> 1, Mx, My, tau);
	}

#ifdef DEBUG
	printf("Calculated values: \n");
	for(unsigned int i = 0; i < NUM_SECTIONS; i++){
		printf("X[%d] = %8lf\n", i, X[i]);
		printf("Y[%d] = %8lf\n", i, Y[i]);
	}
#endif

	FILE *signal_x_data = fopen("signal_x_values", "w");
	FILE *signal_y_data = fopen("signal_y_values", "w");
	FILE *correl_xx_data = fopen("correl_xx_values", "w");
	FILE *correl_xy_data = fopen("correl_xy_values", "w");
	for (unsigned int i = 0; i < NUM_SECTIONS; i++)
	{
		fprintf(signal_x_data, "%d %lf \n", i, X[i]);
		fprintf(signal_y_data, "%d %lf \n", i, Y[i]);
	}
	for(unsigned int i = 0; i < (NUM_SECTIONS >> 1) - 1; i++) {
		fprintf(correl_xx_data, "%d %lf \n", i, correl_xx[i]);
		fprintf(correl_xy_data, "%d %lf \n", i, correl_xy[i]);
	}
	fclose(signal_x_data);
	fclose(signal_y_data);
	fclose(correl_xx_data);
	fclose(correl_xy_data);

	free(correl_xx);
	free(correl_xy);

	plot_chart(NUM_SECTIONS, "signal_x_values");
	plot_chart(NUM_SECTIONS, "signal_y_values");
	plot_chart(NUM_SECTIONS >> 1, "correl_xx_values");
	plot_chart(NUM_SECTIONS >> 1, "correl_xy_values");

	printf("Successfully finish calculation\nStarting gnuplot...\n");
exit:
	exit(0);
}
