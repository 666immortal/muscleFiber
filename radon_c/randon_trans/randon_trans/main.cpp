#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>

using namespace std;
using namespace cv;

#define MAXX(x, y)  ((x) > (y) ? (x) : (y)) // 定义宏函数，该函数用于取两数中最大值

// radon变换外部调用接口
void radon(const Mat input, double *theta, const int len_theta, double *&P, int &len_P, double *&r, int &len_r);
// 内部调用函数
static void radonc(double *pPtr, const Mat iPtr, const double *radian, const int len_radian, const int xOrigin, const int yOrigin, const int rFirst, const int rSize);
static inline void incrementRadon(double *pr, double pixel, double r);
static void outputFile(double *P, int len_P, double *r, int len_r);

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

	double *P = nullptr, *r = nullptr;
	int len_P, len_r;
	Mat tmp;
	src.convertTo(tmp, CV_64FC1);
	//cout << tmp.size() << endl;

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
	cout << "总用时：" << ((double)(t2 - t1) / CLOCKS_PER_SEC) << "秒" << endl;
	//cout << "length of P : " << len_P << endl;
	//cout << "length of r : " << len_r << endl;  // len_r为497，对应矩阵的行row

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

	// 将结果输出到文件
	//outputFile(P, len_P, r, len_r);
	// 后期尝试改用智能指针
	delete []theta;
	delete []P;
	delete []r;

	return 0;
}

/***
**  radon变换接口函数（注意：每次调用之后要记得使用delete释放指针P、r指向的内存）
**  input：进行radon变换处理的对象
**  theta：指定进行radon变换的角度的数组，该角度就是极坐标中偏离正方向的角度
**  len_theta：theta数组中元素的个数
**  P：获取radon变换的计算结果P
**  len_P：P数组中元素的个数
**  r：获取radon变换的计算结果r
**  len_r：r数组中元素的个数
***/
void radon(const Mat input, double *theta, const int len_theta, double *&P, int &len_P, double *&r, int &len_r) 
{
	// 用于将角度转化为弧度
	const double deg2rad = 3.14159265358979 / 180.0;

	double *radian = new double[len_theta];
	double *tmp_r = radian, *tmp_t = theta;
	// 将角度转换为弧度
	for (int k = 0; k < len_theta; k++)
		*(tmp_r++) = *(tmp_t++) * deg2rad; 

	int M = input.rows;  // 输入图像的行数
	int N = input.cols;    // 输入图像的列数

	int xOrigin = MAXX(0, (N - 1) / 2);	// 选出最大值，找到图像的水平方向的中点，即坐标轴的横轴原点
	int yOrigin = MAXX(0, (M - 1) / 2);	// 选出最大值，找到图像的垂直方向的中点，即坐标轴的纵轴原点

	int temp1 = M - 1 - yOrigin;
	int temp2 = N - 1 - xOrigin;
	int rLast = (int)ceil(sqrt((double)(temp1 * temp1 + temp2 * temp2))) + 1;  //向上舍入为最接近的整数
	int rFirst = -rLast;		// 关于原点对称
	int rSize = rLast - rFirst + 1;

	//创建一个rSize行1列的矩阵，其实也就是一个数组
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
}// 调用完函数，记得释放P、r内存（这一个隐藏的bug有时间再修改）

/***
** 参数解释：
** pPtr：经过radon变换后输出的一维数组，该一维数组是其实要还原成一个rSize*numAngles的矩阵
** iPtr：需要进行radon变换的矩阵的一维数组存储形势
** radian：指定进行radon变换的弧度的数组，该弧度就是极坐标中偏离正方向的弧度
** len_theta：radian数组中元素的个数
** xOrigin：要进行radon变换的矩阵的的中心的横坐标
** yOrigin：要进行radon变换的矩阵的中心的纵坐标
** rFist：极坐标中初始点与变换原点的距离
** rSize：整个radon变换中极坐标的点之间的最远距离
***/
static void radonc(double *pPtr,  const Mat iPtr, const double *radian, const int len_radian, const int xOrigin, const int yOrigin, const int rFirst, const int rSize)
{
	double cosine, sine;									// 当前弧度的cos和sin值
	double *pr;											// 指向输出序列的指针
	double *pixelPtr;									// 指向输入序列的指针
	double *ySinTable, *xCosTable;				// 指向x*cos(angle) 和 y*sin(angle)表格的指针
	double x, y;
	double r = 0;

	int n_row = iPtr.rows;								// 行数
	int n_col = iPtr.cols;								//	列数
	xCosTable = new double[2 * n_col];			// 申请内存
	ySinTable = new double[2 * n_row];

	// 要想提速需要在这个循环里进行改良，建议使用CUDA
	for (int k = 0; k < len_radian; k++)
	{
		//clock_t t1 = clock();
		double angle = radian[k];
		pr = pPtr + k * rSize;
		cosine = cos(angle);
		sine = sin(angle);

		for (int n = 0; n < n_col; n++)
		{
			x = n - xOrigin;
			// 根据极坐标求横坐标
			xCosTable[2 * n] = (x - 0.25) * cosine;
			xCosTable[2 * n + 1] = (x + 0.25) * cosine;
		}

		for (int m = 0; m < n_row; m++)
		{
			y = yOrigin - m;
			// 根据极坐标求纵坐标
			ySinTable[2 * m] = (y - 0.25) * sine;
			ySinTable[2 * m + 1] = (y + 0.25) * sine;
		}

		pixelPtr = (double *)iPtr.data;
		// 此处原先有一个bug，处理后的数据MATLAB不一致，在交换了n_row和n_col的位置之后，结果就一致
		for (int n = 0; n < n_row; n++)
		{
			for (int m = 0; m < n_col; m++)		//遍历原矩阵中的每一个像素点
			{
				double pixel = *pixelPtr++;
				if (pixel != 0.0)						//如果该点像素值不为0，也即图像不连续
				{
					pixel *= 0.25;
					//一个像素点分解成四个临近的像素点进行计算，提高精确度
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
		/*clock_t t2 = clock();
		cout << "round" << k << " : " << ((double)(t2 - t1) / CLOCKS_PER_SEC) << "秒" << endl;*/
	}

	// 释放内存
	delete []xCosTable;
	delete []ySinTable;
}

/**
**pr：进行radon变换后输出矩阵的对于一个特定theta角的列的首地址
**pixel：要进行radon变换的像素值乘以0.25以后的值（由于每一个像素点取了相邻四个点提高精度，
**故在计算时pixel也要相应乘以0.25，类似于一个点占0.25的比例，然后四个点刚好凑足1的份额）
**r：进行radon变换的该点与初始的r值——rFirst之间的差
**/
static inline void incrementRadon(double *pr, double pixel, double r)
{
	int r1 = (int)r;
	double delta = r - r1;					//对于每一个点，r值不同，所以，通过这种方式，可以把这一列中相应行的元素的值给赋上
	pr[r1] += pixel * (1.0 - delta);		//radon变换通过记录目标平面上某一点的被映射后点的积累厚度来反推原平面上直线的存在性
	pr[r1 + 1] += pixel * delta;			//两个点互相配合，提高精度 
}

static void outputFile(double *P, int len_P, double *r, int len_r)
{
	ofstream outFile1, outFile2;
	outFile1.open("result_P");
	outFile2.open("result_r");

	if (!outFile1.is_open() && !outFile2.is_open())
	{
		cout << "Could not open the file " << endl;
		cout << "Program terminating." << endl;
		exit(EXIT_FAILURE);
	}


	for (int i = 0; i < len_P; i++)
		outFile1 << P[i] << endl;
	for (int i = 0; i < len_r; i++)
		outFile2 << r[i] << endl;
	outFile1.close();
	outFile2.close();
}