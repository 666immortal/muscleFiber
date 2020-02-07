#include <iostream>
#include <vector>
#include <opencv.hpp>
#include <fstream>
#include <math.h>
#include "MyArray.h"

using namespace std;
using namespace cv;

#define PI 3.14159265358979323846

// 在main开头读入图像，生成角度等序列之后，
// 应该调用setPara函数提前设置好参数，一劳永逸，提升速度（以后只需调用短的构造函数即可）
// 该类处理的结果与MATLAB的结果存在些许误差（误差在1~2之间）
// 产生误差的原因是：MATLAB使用的是插值函数，而本程序使用数学公式的方法
class Lines {
public:
	Point2f point1;
	Point2f point2;
	double theta;
	double rho;
	double G;

	static double s_k_theta;
	static double s_k_rho;
	static double s_b_theta;
	static double s_b_rho;
	static double s_image_length;
	static double s_image_width;
	static double s_centerx;
	static double s_centery;

	Lines()
	{
		point1.x = 0;
		point1.y = 0;
		point2.x = 0;
		point2.y = 0;
		theta = 0;
		rho = 0;
		G = 0;
	}

	// center : 为图像的中心点的坐标，记得向下取整，如center.x = floor((image.len_x + 1) / 2);
	// peaks : 注意传入的点的顺序
	Lines(const int image_length, const int image_width, const MyArray<double> &i_theta, const MyArray<double> &i_rho, const Point2f &peaks, const double peak_G)
	{
		// 此处利用直线上两点算出直线的斜率和截距，构造斜截式
		// 注意，此方程不适用于与横坐标垂直的直线
		// 两点(x1, y1)、(x2, y2)
		// 在y轴上截距为：x2 * y1 - x1 * y2
		// 直线的斜率为：(y2－y1) / (x2－x1)
		double k_theta, k_rho, b_theta, b_rho;// 斜率和截距

		k_theta = (i_theta.pointer[i_theta.len - 1] - i_theta.pointer[0]) / i_theta.len;
		k_rho = (i_rho.pointer[i_rho.len - 1] - i_rho.pointer[0]) / i_rho.len;
		b_theta = (i_theta.len * i_theta.pointer[0] - i_theta.pointer[i_theta.len - 1]) / i_theta.len;
		b_rho = (i_rho.len * i_rho.pointer[0] - i_rho.pointer[i_rho.len - 1]) / i_rho.len;

		/*cout << "k_theta: " << k_theta << endl << "b_theta: " << b_theta << endl;
		cout << "k_rho: " << k_rho << endl << "b_rho: " << b_rho << endl;*/

		this->theta = (k_theta * peaks.y + b_theta) * PI / 180;
		this->rho = k_rho + peaks.x + b_rho;

		double s = sin(this->theta);
		double c = cos(this->theta);

		//cout << "sin : " << s << endl << "cos : " << c << endl;

		double center_x = floor((image_length + 1) / 2);
		double center_y = floor((image_width + 1) / 2);

		//cout << "center_x : " << center_x << endl;
		//cout << "center_y : " << center_y << endl;

		this->point1.x = 0;
		this->point2.x = image_width;
		this->point1.y = center_x - (this->rho + center_y * c) / s;
		this->point2.y = center_x - (this->rho - (image_width - center_y) * c) / s;

		this->G = peak_G;
	}

	static void setPara(const int image_length, const int image_width, const MyArray<double> &i_theta, const MyArray<double> &i_rho)
	{
		s_k_theta = (i_theta.pointer[i_theta.len - 1] - i_theta.pointer[0]) / i_theta.len;
		s_k_rho = (i_rho.pointer[i_rho.len - 1] - i_rho.pointer[0]) / i_rho.len;
		s_b_theta = (i_theta.len * i_theta.pointer[0] - i_theta.pointer[i_theta.len - 1]) / i_theta.len;
		s_b_rho = (i_rho.len * i_rho.pointer[0] - i_rho.pointer[i_rho.len - 1]) / i_rho.len;

		s_centerx = floor((image_length + 1) / 2);
		s_centery = floor((image_width + 1) / 2);

		s_image_length = image_length;
		s_image_width = image_width;
	}

	Lines(const Point2f &peaks, const double peak_G)
	{
		this->theta = (s_k_theta * peaks.y + s_b_theta) * PI / 180;
		this->rho = s_k_rho + peaks.x + s_b_rho;

		double s = sin(this->theta);
		double c = cos(this->theta);

		this->point1.x = 0;
		this->point2.x = s_image_width;
		this->point1.y = s_centerx - (this->rho + s_centery * c) / s;
		this->point2.y = s_centerx - (this->rho - (s_image_width - s_centery) * c) / s;

		this->G = peak_G;
	}

};

double Lines::s_k_theta = 0;
double Lines::s_k_rho = 0;
double Lines::s_b_theta = 0;
double Lines::s_b_rho = 0;
double Lines::s_image_length = 0;
double Lines::s_image_width = 0;
double Lines::s_centerx = 0;
double Lines::s_centery = 0;

void inputFile(const char *filename, vector<double> &res);

int main()
{
	double theta_max = 170.0;
	double theta_min = 10.0;
	double theta_interval = 0.2;
	int theta_num = (theta_max - theta_min) / theta_interval + 1;
	MyArray<double> mytheta(theta_num);
	for (int i = 0; i < theta_num; i++)
		mytheta.pointer[i] = theta_min + i * theta_interval;
	//for (int i = 0; i < 10; i++)
		//cout << "1~10 of theta: [" << i << "]    " << mytheta.pointer[i] << endl;

	vector<double> r;
	inputFile("result_r", r);
	MyArray<double> myrho(r.size());
	for (int i = 0; i < r.size(); i++)
		myrho.pointer[i] = r[i];
	//for (int i = 0; i < 10; i++)
		//cout << "1~10 of rho: [" << i << "]   " << myrho.pointer[i] << endl;

	Point2f tmp(376.86, 400.4749);
	//Lines test(360, 338, mytheta, myrho, tmp, 0);
	Lines::setPara(360, 338, mytheta, myrho);
	Lines test(tmp, 0);
	Point2f tmp1(325, 305);
	Lines test1(tmp1, 11.2366);

	cout << "point1: (" << test1.point1.x << ", " << test1.point1.y << ")" << endl;
	cout << "point2: (" << test1.point2.x << ", " << test1.point2.y << ")" << endl;
	cout << "theta: " << test1.theta << endl << "rho: " << test1.rho << endl;
	cout << "G: " << test1.G << endl;

	return 0;
}

void inputFile(const char *filename, vector<double> &res)
{
	ifstream fin;
	fin.open(filename);
	if (!fin.is_open())
	{
		cout << "Could not open the file " << filename << endl;
		cout << "Program terminating." << endl;
		exit(EXIT_FAILURE);
	}
	double tmp;
	fin >> tmp;
	while (fin.good())
	{
		res.push_back(tmp);
		fin >> tmp;
	}
	if (fin.eof())
		cout << "End of file reached.\n";
	else if (fin.fail())
		cout << "Input terminated by data mismatch.\n";
	else
		cout << "Input terminated for unknown reason.\n";
	fin.close();
}