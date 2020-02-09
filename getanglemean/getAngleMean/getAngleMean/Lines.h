#pragma once
#include <opencv.hpp>
#include <iostream>
#include "MyArray.h"

using namespace std;
using namespace cv;

#define PI 3.14159265358979323846

// ��main��ͷ����ͼ�����ɽǶȵ�����֮��
// Ӧ�õ���setPara������ǰ���úò�����һ�����ݣ������ٶȣ��Ժ�ֻ����ö̵Ĺ��캯�����ɣ�
// ���ദ��Ľ����MATLAB�Ľ������Щ���������1~2֮�䣩
// ��������ԭ���ǣ�MATLABʹ�õ��ǲ�ֵ��������������ʹ����ѧ��ʽ�ķ���
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
	// center : Ϊͼ������ĵ�����꣬�ǵ�����ȡ������center.x = floor((image.len_x + 1) / 2);
	// peaks : ע�⴫��ĵ��˳��
	Lines(const int image_length, const int image_width, const MyArray<double> &i_theta, const MyArray<double> &i_rho, const Point2f &peaks, const double peak_G);
	static void setPara(const int image_length, const int image_width, const MyArray<double> &i_theta, const MyArray<double> &i_rho);
	Lines(const Point2f &peaks, const double peak_G);	
	void set(const Point2f &peaks, const double peak_G);
};