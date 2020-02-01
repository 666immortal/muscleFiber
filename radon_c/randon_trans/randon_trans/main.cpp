#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <ctime>

using namespace std;
using namespace cv;

#define MAXX(x, y)  ((x) > (y) ? (x) : (y)) // 定义宏函数，该函数用于取两数中最大值

static void radon(const Mat input, double *theta, const int len_theta, double *&P, int &len_P, double *&r, int &len_r);
static void radonc(double *pPtr, const Mat iPtr, const double *radian, const int len_theta, const int xOrigin, const int yOrigin, const int rFirst, const int rSize);
static void incrementRadon(double *pr, double pixel, double r);

int main()
{
	Mat src = imread("bmc1.bmp", 0);

	double theta_max = 170.0;
	double theta_min = 10.0;
	double theta_interval = 0.2;
	int theta_num = (theta_max - theta_min) / theta_interval + 1;
	double *theta = new double[theta_num];
	for (int i = 0; i < theta_num; i++)
		theta[i] = theta_min + i * theta_interval;

	double *P = NULL, *r = NULL;
	int len_P, len_r;
	Mat tmp;
	src.convertTo(tmp, CV_64FC1);
	cout << tmp.size() << endl;

	double *data;
	double mean = 0; // 求平均数
	for (int i = 0; i < tmp.rows; i++) 
	{
		data = tmp.ptr<double>(i);
		for (int j = 0; j < tmp.cols; j++)
		{
			data[j] /= 255;
			mean += data[j];
		}			
	}

	mean = mean / (tmp.rows * tmp.cols);
	// 再写一个整体减平均数的循环
	for (int i = 0; i < tmp.rows; i++)
	{
		data = tmp.ptr<double>(i);
		for (int j = 0; j < tmp.cols; j++)
		{
			data[j] -= mean;
		}
	}

	//data = tmp.ptr<double>(0);
	//for(int i = 0; i < tmp.cols; i++)
	//	cout << "data: " << data[i] << endl;
	
	// C++ PP149页计时方法（同时有讲到延时的方法）
	clock_t t1 = clock();
	radon(tmp, theta, theta_num, P, len_P, r, len_r);
	clock_t t2 = clock();
	cout << ((double)(t2 - t1) / CLOCKS_PER_SEC) << "秒" << endl;
	//cout << "length of P : " << len_P << endl;
	//cout << "length of r : " << len_r << endl;

	//cout << "r: " << endl;
	//for (int i = 0; i < len_r; i++)
	//	cout << r[i] << endl;

	// 打印输出矩阵的一部分内容，用于检测结果是否正确
	/*int sizeP = len_P / theta_num;
	int i = 0;
	cout << "P : " << endl;
	for (int k = 0; k < 3; k++)
		for (int j = 0; j < sizeP; j++)
		{
			cout << ++i << " : ";
			cout << P[k * sizeP + j] << "  " << endl;
		}		*/	

	imshow("test", src);

	waitKey(0);
	// 后期改用智能指针
	delete []theta;
	delete []P;
	delete []r;

	return 0;
}

static void radon(const Mat input, double *theta, const int len_theta, double *&P, int &len_P, double *&r, int &len_r) 
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
	double *rPtr = r;
	for (int k = rFirst; k <= rLast; k++)
		*(rPtr++) = (double)k;
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

	int n_row = iPtr.rows;
	int n_col = iPtr.cols;
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

		pixelPtr = (double *)iPtr.data;

		// 原先这里有一个bug，处理后的数据不一致，交换了n_row和n_col的位置之后，结果就与MATLAB一致了
		for (int n = 0; n < n_row; n++)
		{
			for (int m = 0; m < n_col; m++)
			{
				double pixel = *pixelPtr++;
				if (pixel != 0.0)
				{
					pixel *= 0.25;
					
					r = xCosTable[2 * m] + ySinTable[2 * n] - rFirst;
					incrementRadon(pr, pixel, r);

					r = xCosTable[2 * m + 1] + ySinTable[2 * n] - rFirst;
					incrementRadon(pr, pixel, r);

					r = xCosTable[2 * m] + ySinTable[2 * n + 1] - rFirst;
					incrementRadon(pr, pixel, r);

					r = xCosTable[2 * m + 1] + ySinTable[2 * n + 1] - rFirst;
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