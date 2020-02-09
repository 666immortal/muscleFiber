#include "Lines.h"

double Lines::s_k_theta = 0;
double Lines::s_k_rho = 0;
double Lines::s_b_theta = 0;
double Lines::s_b_rho = 0;
double Lines::s_image_length = 0;
double Lines::s_image_width = 0;
double Lines::s_centerx = 0;
double Lines::s_centery = 0;

Lines::Lines()
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
Lines::Lines(const int image_length, const int image_width, const MyArray<double> &i_theta, const MyArray<double> &i_rho, const Point2f &peaks, const double peak_G)
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

void Lines::setPara(const int image_length, const int image_width, const MyArray<double> &i_theta, const MyArray<double> &i_rho)
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

Lines::Lines(const Point2f &peaks, const double peak_G)
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

void Lines::set(const Point2f &peaks, const double peak_G)
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