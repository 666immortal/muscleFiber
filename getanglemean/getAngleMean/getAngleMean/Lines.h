#pragma once
#include <opencv.hpp>
#include <iostream>
#include "MyArray.h"

using namespace std;
using namespace cv;

#define PI 3.14159265358979323846

// 在main开头读入图像，生成角度等序列之后，
// 应该调用setPara函数提前设置好参数，一劳永逸，提升速度（以后只需调用短的构造函数即可）
// 该类处理的结果与MATLAB的结果存在些许误差（误差在1~2之间）
// 产生误差的原因是：MATLAB使用的是插值函数，而本程序使用数学公式的方法
class Lines {
private:
	static double s_k_theta;
	static double s_k_rho;
	static double s_b_theta;
	static double s_b_rho;
	static double s_image_length;
	static double s_image_width;
	static double s_centerx;
	static double s_centery;

public:
	Point2f point1;
	Point2f point2;
	double theta;
	double rho;
	double G;

	Lines();
	// center : 为图像的中心点的坐标，记得向下取整，如center.x = floor((image.len_x + 1) / 2);
	// peaks : 注意传入的点的顺序
	Lines(const int image_length, const int image_width, const MyArray<double> &i_theta, const MyArray<double> &i_rho, const Point2f &peaks, const double peak_G);
	static void setPara(const int image_length, const int image_width, const MyArray<double> &i_theta, const MyArray<double> &i_rho);
	Lines(const Point2f &peaks, const double peak_G);	
	void set(const Point2f &peaks, const double peak_G);
};