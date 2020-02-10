#pragma once
#include <iostream>
#include <algorithm>

using namespace std;

class ImgProp {
private:
	static double *G_sort;
public:
	static int len;
	static int wid;
	static int G_len;

	static void set(const int len_i, const int wid_i, const double *G);
	static void setShape(const int len_i, const int wid_i);
	static void setG(const double *G);
	static double getThresh(const double rate);
};