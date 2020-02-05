#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>

using namespace std;
using namespace cv;

#define MAXX(x, y)  ((x) > (y) ? (x) : (y)) // ����꺯�����ú�������ȡ���������ֵ

// radon�任�ⲿ���ýӿ�
void radon(const Mat input, double *theta, const int len_theta, double *&P, int &len_P, double *&r, int &len_r);
// �ڲ����ú���
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
	double mean = 0; // ��ƽ����
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
	// ��дһ�������ƽ������ѭ��
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
	
	// C++ PP149ҳ��ʱ������ͬʱ�н�����ʱ�ķ�����
	clock_t t1 = clock();
	radon(tmp, theta, theta_num, P, len_P, r, len_r);
	clock_t t2 = clock();
	cout << "����ʱ��" << ((double)(t2 - t1) / CLOCKS_PER_SEC) << "��" << endl;
	//cout << "length of P : " << len_P << endl;
	//cout << "length of r : " << len_r << endl;  // len_rΪ497����Ӧ�������row

	//cout << "r: " << endl;
	//for (int i = 0; i < len_r; i++)
	//	cout << r[i] << endl;

	// ��ӡ��������һ�������ݣ����ڼ�����Ƿ���ȷ
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

	// �����������ļ�
	//outputFile(P, len_P, r, len_r);
	// ���ڳ��Ը�������ָ��
	delete []theta;
	delete []P;
	delete []r;

	return 0;
}

/***
**  radon�任�ӿں�����ע�⣺ÿ�ε���֮��Ҫ�ǵ�ʹ��delete�ͷ�ָ��P��rָ����ڴ棩
**  input������radon�任����Ķ���
**  theta��ָ������radon�任�ĽǶȵ����飬�ýǶȾ��Ǽ�������ƫ��������ĽǶ�
**  len_theta��theta������Ԫ�صĸ���
**  P����ȡradon�任�ļ�����P
**  len_P��P������Ԫ�صĸ���
**  r����ȡradon�任�ļ�����r
**  len_r��r������Ԫ�صĸ���
***/
void radon(const Mat input, double *theta, const int len_theta, double *&P, int &len_P, double *&r, int &len_r) 
{
	// ���ڽ��Ƕ�ת��Ϊ����
	const double deg2rad = 3.14159265358979 / 180.0;

	double *radian = new double[len_theta];
	double *tmp_r = radian, *tmp_t = theta;
	// ���Ƕ�ת��Ϊ����
	for (int k = 0; k < len_theta; k++)
		*(tmp_r++) = *(tmp_t++) * deg2rad; 

	int M = input.rows;  // ����ͼ�������
	int N = input.cols;    // ����ͼ�������

	int xOrigin = MAXX(0, (N - 1) / 2);	// ѡ�����ֵ���ҵ�ͼ���ˮƽ������е㣬��������ĺ���ԭ��
	int yOrigin = MAXX(0, (M - 1) / 2);	// ѡ�����ֵ���ҵ�ͼ��Ĵ�ֱ������е㣬�������������ԭ��

	int temp1 = M - 1 - yOrigin;
	int temp2 = N - 1 - xOrigin;
	int rLast = (int)ceil(sqrt((double)(temp1 * temp1 + temp2 * temp2))) + 1;  //��������Ϊ��ӽ�������
	int rFirst = -rLast;		// ����ԭ��Գ�
	int rSize = rLast - rFirst + 1;

	//����һ��rSize��1�еľ�����ʵҲ����һ������
	r = new double[rSize];
	double *rPtr = r;
	for (int k = rFirst; k <= rLast; k++)
		*(rPtr++) = (double)k;
	len_r = rSize;

	// len_P��P�����峤�ȣ����Ҫ����P����Ҫע����˳�򣬽Ƕ���Ϊ����
	len_P = len_theta * rSize;
	P = new double[len_P];
	radonc(P, input, radian, len_theta, xOrigin, yOrigin, rFirst, rSize);
	
	delete []radian;
}// �����꺯�����ǵ��ͷ�P��r�ڴ棨��һ�����ص�bug��ʱ�����޸ģ�

/***
** �������ͣ�
** pPtr������radon�任�������һά���飬��һά��������ʵҪ��ԭ��һ��rSize*numAngles�ľ���
** iPtr����Ҫ����radon�任�ľ����һά����洢����
** radian��ָ������radon�任�Ļ��ȵ����飬�û��Ⱦ��Ǽ�������ƫ��������Ļ���
** len_theta��radian������Ԫ�صĸ���
** xOrigin��Ҫ����radon�任�ľ���ĵ����ĵĺ�����
** yOrigin��Ҫ����radon�任�ľ�������ĵ�������
** rFist���������г�ʼ����任ԭ��ľ���
** rSize������radon�任�м�����ĵ�֮�����Զ����
***/
static void radonc(double *pPtr,  const Mat iPtr, const double *radian, const int len_radian, const int xOrigin, const int yOrigin, const int rFirst, const int rSize)
{
	double cosine, sine;									// ��ǰ���ȵ�cos��sinֵ
	double *pr;											// ָ��������е�ָ��
	double *pixelPtr;									// ָ���������е�ָ��
	double *ySinTable, *xCosTable;				// ָ��x*cos(angle) �� y*sin(angle)����ָ��
	double x, y;
	double r = 0;

	int n_row = iPtr.rows;								// ����
	int n_col = iPtr.cols;								//	����
	xCosTable = new double[2 * n_col];			// �����ڴ�
	ySinTable = new double[2 * n_row];

	// Ҫ��������Ҫ�����ѭ������и���������ʹ��CUDA
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
			// ���ݼ������������
			xCosTable[2 * n] = (x - 0.25) * cosine;
			xCosTable[2 * n + 1] = (x + 0.25) * cosine;
		}

		for (int m = 0; m < n_row; m++)
		{
			y = yOrigin - m;
			// ���ݼ�������������
			ySinTable[2 * m] = (y - 0.25) * sine;
			ySinTable[2 * m + 1] = (y + 0.25) * sine;
		}

		pixelPtr = (double *)iPtr.data;
		// �˴�ԭ����һ��bug������������MATLAB��һ�£��ڽ�����n_row��n_col��λ��֮�󣬽����һ��
		for (int n = 0; n < n_row; n++)
		{
			for (int m = 0; m < n_col; m++)		//����ԭ�����е�ÿһ�����ص�
			{
				double pixel = *pixelPtr++;
				if (pixel != 0.0)						//����õ�����ֵ��Ϊ0��Ҳ��ͼ������
				{
					pixel *= 0.25;
					//һ�����ص�ֽ���ĸ��ٽ������ص���м��㣬��߾�ȷ��
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
		cout << "round" << k << " : " << ((double)(t2 - t1) / CLOCKS_PER_SEC) << "��" << endl;*/
	}

	// �ͷ��ڴ�
	delete []xCosTable;
	delete []ySinTable;
}

/**
**pr������radon�任���������Ķ���һ���ض�theta�ǵ��е��׵�ַ
**pixel��Ҫ����radon�任������ֵ����0.25�Ժ��ֵ������ÿһ�����ص�ȡ�������ĸ�����߾��ȣ�
**���ڼ���ʱpixelҲҪ��Ӧ����0.25��������һ����ռ0.25�ı�����Ȼ���ĸ���պô���1�ķݶ
**r������radon�任�ĸõ����ʼ��rֵ����rFirst֮��Ĳ�
**/
static inline void incrementRadon(double *pr, double pixel, double r)
{
	int r1 = (int)r;
	double delta = r - r1;					//����ÿһ���㣬rֵ��ͬ�����ԣ�ͨ�����ַ�ʽ�����԰���һ������Ӧ�е�Ԫ�ص�ֵ������
	pr[r1] += pixel * (1.0 - delta);		//radon�任ͨ����¼Ŀ��ƽ����ĳһ��ı�ӳ����Ļ��ۺ��������ԭƽ����ֱ�ߵĴ�����
	pr[r1 + 1] += pixel * delta;			//�����㻥����ϣ���߾��� 
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