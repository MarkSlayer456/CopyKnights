int compare2(int a, int b) 
{
	return (a < b) ? a : b;
}

int compare4(int a, int b, int c, int d) 
{
	return compare2(compare2(a, b), compare2(c, d));
}
