int compare2(int a, int b) 
{
	return (a < b) ? a : b;
}

int compare4(int a, int b, int c, int d) 
{
	return compare2(compare2(a, b), compare2(c, d));
}

/**
 * creates a truly unique number from an x,y pair
 **/
unsigned int cantor_pair(int x, int y) {
	return (x + y) * (x + y + 1) / 2 + y;
}

/**
 * cross platform rand_r function
 **/
int rand_r_portable(unsigned int *seed) {
	*seed = *seed * 1103515245 + 12345;
	return (unsigned int)(*seed / 65536) % 32768;
}
