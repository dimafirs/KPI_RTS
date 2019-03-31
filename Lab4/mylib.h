#ifndef MYLIB_H_
#define MYLIB_H_

#include <stdlib.h>
#include <complex.h>

double calc_me(double *data, int start, int end);
double calc_variance(double *data, double me, int start, int end);
void plot_charts(int x_limit, const char *file_name, const char* sec_filename, const char *title);
void plot_onechart(int x_limit, const char *filename, const char *title);
void generate_signal(double *result, int harm_cnt, int section_cnt,
						int max_freq, int max_ampl);
void generate_sine(double *result, int cnt);
int init_random();
void fft(double complex *F, double complex *coeffs, int N);
void dft(double *X, double complex *F,
		double complex **coeffs, int N);

double timespec_diff(struct timespec *stop, struct timespec *start);




#endif /* MYLIB_H_ */
