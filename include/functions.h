#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_
#define MIN(a,b)       (a < b) ? a : b
#define MAX(a,b)       (a > b) ? a : b

int compare4(int a, int b, int c, int d);
unsigned int cantor_pair(int x, int y);
int rand_r_portable(unsigned int *seed);
#endif
