#ifndef MYLIB_H_
#define MYLIB_H_

#include <stdlib.h>

double calc_me(double *data, int start, int end);
double calc_variance(double *data, double me, int start, int end);
void plot_chart(int x_limit, const char *file_name, const char *title);
void plot_bargraph(int x_limit, const char *filename, const char *title);
void generate_signal(double *result, int harm_cnt, int section_cnt,
						int max_freq, int max_ampl);
void generate_sine(double *result, int cnt);
int init_random();


#endif /* MYLIB_H_ */
