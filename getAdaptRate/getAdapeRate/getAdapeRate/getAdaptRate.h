#pragma once
#include <iostream>
#include <opencv.hpp>
#include "MyArray.h"
#include "ImgProp.h"

using namespace std;
using namespace cv;

#define LIST_MIN 0.001
#define LIST_MAX 0.003
#define LIST_INTERVAL 0.0002
#define LIST_NUM (int)((LIST_MAX - LIST_MIN) / LIST_INTERVAL + 1)

// 连通域函数参考自博客园《OPENCV快速连通区域分析》一篇文章
double getAdaptRate(const MyArray<double> &G, int im_height);
int getMax(double *data, int len);
int getMin(double *data, int len);