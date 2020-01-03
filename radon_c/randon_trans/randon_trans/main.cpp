#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

using namespace std;
using namespace cv;

#define MAXX(x, y)  ((x) > (y) ? (x) : (y)) // 定义宏函数，取最大值

static void radon(const Mat input, double *theta, const int len_theta, double *P, int &len_P, double *r, int &len_r);
static void radonc(double *pPtr, const Mat iPtr, const double *radian, const int len_theta, const int xOrigin, const int yOrigin, const int rFirst, const int rSize);
static void incrementRadon(double *pr, double pixel, double r);

int main()
{
	Mat src = imread("bmc1.bmp");

	double theta_max = 170.0;
	double theta_min = 10.0;
	double theta_interval = 0.2;
	int theta_num = (theta_max - theta_min) / theta_interval;
	double *theta = new double[theta_num];
	for (int i = 0; i < theta_num; i++)
		theta[i] = theta_min + i * theta_interval;

	double *P = NULL, *r = NULL;
	int len_P, len_r;

	radon(src, theta, theta_num, P, len_P, r, len_r);
	imshow("test", src);
	waitKey(0);
	delete []theta;
	delete []P;
	delete []r;

	return 0;
}

static void radon(const Mat input, double *theta, const int len_theta, double *P, int &len_P, double *r, int &len_r) 
{
	const double deg2rad = 3.14159265358979 / 180.0;

	double *radian = new double[len_theta];
	double *tmp_r = radian, *tmp_t = theta;
	for (int k = 0; k < len_theta; k++)
		*(tmp_r++) = *(tmp_t++) * deg2rad;

	int M = input.rows;  // 输入图像的行数
	int N = input.cols;    // 输入图像的列数

	int xOrigin = MAXX(0, (N - 1) / 2);
	int yOrigin = MAXX(0, (M - 1) / 2);

	int temp1 = M - 1 - yOrigin;
	int temp2 = N - 1 - xOrigin;
	int rLast = (int)ceil(sqrt((double)(temp1 * temp1 + temp2 * temp2))) + 1;
	int rFirst = -rLast;
	int rSize = rLast - rFirst + 1;

	r = new double[rSize];
	for (int k = rFirst; k <= rLast; k++)
		*(r++) = (double)k;
	len_r = rSize;

	// len_P是P的整体长度，如果要遍历P，则要注意存放顺序，角度数为行数
	len_P = len_theta * rSize;
	P = new double[len_P];

	radonc(P, input, radian, len_theta, xOrigin, yOrigin, rFirst, rSize);
	delete []radian;
}// 记得释放P、r内存（这一个隐藏的bug有时间再修改）

static void radonc(double *pPtr,  const Mat iPtr, const double *radian, const int len_theta, const int xOrigin, const int yOrigin, const int rFirst, const int rSize)
{
	double cosine, sine;		// cosine and sine of current angle
	double *pr;				// points inside output array
	double *pixelPtr;		// point inside input array
	double *ySinTable, *xCosTable; // tables for x*cos(angle) and y*sin(angle)
	double x, y;
	double r = 0;

	Mat tmp;
	iPtr.convertTo(tmp, CV_64FC1);

	int n_row = tmp.rows;
	int n_col = tmp.cols;
	xCosTable = new double[2 * n_col];
	ySinTable = new double[2 * n_row];

	for (int k = 0; k < len_theta; k++)
	{
		double angle = radian[k];
		pr = pPtr + k * rSize;
		cosine = cos(angle);
		sine = sin(angle);

		for (int n = 0; n < n_col; n++)
		{
			x = n - xOrigin;
			xCosTable[2 * n] = (x - 0.25) * cosine;
			xCosTable[2 * n + 1] = (x + 0.25) * cosine;
		}

		for (int m = 0; m < n_row; m++)
		{
			y = yOrigin - m;
			ySinTable[2 * m] = (y - 0.25) * sine;
			ySinTable[2 * m + 1] = (y + 0.25) * sine;
		}

		pixelPtr = (double *)tmp.data;
		for (int n = 0; n < n_col; n++)
		{
			for (int m = 0; m < n_row; m++)
			{
				double pixel = *pixelPtr++;
				if (pixel != 0.0)
				{
					pixel *= 0.25;
					
					r = xCosTable[2 * n] + ySinTable[2 * m] - rFirst;
					incrementRadon(pr, pixel, r);

					r = xCosTable[2 * n + 1] + ySinTable[2 * m] - rFirst;
					incrementRadon(pr, pixel, r);

					r = xCosTable[2 * n] + ySinTable[2 * m + 1] - rFirst;
					incrementRadon(pr, pixel, r);

					r = xCosTable[2 * n + 1] + ySinTable[2 * m + 1] - rFirst;
					incrementRadon(pr, pixel, r);
				}
			}
		}
	}

	// 释放内存
	delete []xCosTable;
	delete []ySinTable;
}

static void incrementRadon(double *pr, double pixel, double r)
{
	int r1 = (int)r;
	double delta = r - r1;
	pr[r1] += pixel * (1.0 - delta);
	pr[r1 + 1] += pixel * delta;
}