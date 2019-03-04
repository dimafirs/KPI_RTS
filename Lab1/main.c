#define NUM_HARMONICS 8
#define MAX_FREQ 1100
#define NUM_SECTIONS 256
#define MAX_AMPLITUDE 5

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

/* return random double between 0..1 with uniform distribution */
static double rand_double() {
	return ((double) rand()) / ((double) RAND_MAX);
}

int main(int argc, char* argv[]){
	FILE *urandom = fopen("/dev/urandom", "rb");
	if (NULL == urandom)
		goto exit;
	unsigned int seed;
	fread(&seed, sizeof(seed), 1, urandom);
	if (ferror(urandom))
		goto exit;
	srand(seed);

	double X[NUM_SECTIONS];
	memset(&X, 0, NUM_SECTIONS*sizeof(*X));

	const double harmonic_step = ((double) MAX_FREQ) / ((double) NUM_HARMONICS);
	for(unsigned int i = 0; i < NUM_SECTIONS; i++){
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
	}

#ifdef DEBUG
	printf("Calculated values: \n");
	for(unsigned int i = 0; i < NUM_SECTIONS; i++){
		printf("X[%d] = %8lf\n", i, X[i]);
	}
#endif

	FILE *data = fopen("data_values", "w");
	FILE *plot_pipe = popen("gnuplot -persistent", "w");
	for (unsigned int i = 0; i < NUM_SECTIONS; i++)
	{
		fprintf(data, "%d %lf \n", i, X[i]);
	}
	fprintf(plot_pipe, "%s \n", "set title \"Lab1\"");
	fprintf(plot_pipe, "plot [0:%d] 'data_values' with lines \n",
				NUM_SECTIONS + 10);

	printf("Successfully finish calculation\nStarting gnuplot...\n");
exit:
	exit(0);
}
