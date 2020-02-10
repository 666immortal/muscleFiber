#pragma once
#include <opencv.hpp>
#include <vector>
#include "MyArray.h"
#include "Lines.h"

using namespace std;
using namespace cv;

struct Cross
{
	int line_i[2];
	Point2f p;
};

// ������֤����������������MATLABһ��
void getCross(const vector<Lines> &lines_set, vector<Cross> &cross_set, const int im_len, const int im_wid, const int area_len, const  int area_wid);
void getCross(const MyArray<Lines> &lines_set, vector<Cross> &cross_set, const int im_len, const int im_wid, const int area_len, const  int area_wid);