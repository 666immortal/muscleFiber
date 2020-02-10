#pragma once
#include "Lines.h"
#include "getCross.h"
#include <cmath>

void linesDenoise(const MyArray<Lines> &lines, vector<int> &discard, const int im_len, const int im_wid, const int area_len, const int area_wid, const double angle_mean);
void linesDenoise(const vector<Lines> &lines, vector<int> &discard, const int im_len, const int im_wid, const int area_len, const int area_wid, const double angle_mean);