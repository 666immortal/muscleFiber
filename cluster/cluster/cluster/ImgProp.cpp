#include "ImgProp.h"

int ImgProp::len = 0;
int ImgProp::wid = 0;
int ImgProp::G_len = 0;
double * ImgProp::G_sort = nullptr;

static bool cmp(double a, double b)
{
	return a > b;
}

void ImgProp::set(const int len_i, const int wid_i, const double *G)
{
	len = len_i;
	wid = wid_i;
	G_len = len * wid;

	delete[] G_sort;
	G_sort = new double[G_len];
	memcpy(G_sort, G, G_len * sizeof(double));
	sort(G_sort, G_sort + G_len, cmp);
}

void ImgProp::setShape(const int len_i, const int wid_i)
{
	len = len_i;
	wid = wid_i;
	G_len = len * wid;
}

void ImgProp::setG(const double *G)
{
	delete[] G_sort;
	G_sort = new double[G_len];
	memcpy(G_sort, G, G_len * sizeof(double));
	sort(G_sort, G_sort + G_len, cmp);
}

double ImgProp::getThresh(const double rate)
{
	int tmp = ceil(G_len * rate);
	// 因为C与MATLAB的起始下标不同，所以要减1
	return G_sort[tmp - 1];
}