#include <iostream>
#include <opencv.hpp>
#include <fstream>
#include "ImgProp.h"
#include "MyArray.h"

using namespace std;
using namespace cv;

#define LIST_MIN 0.001
#define LIST_MAX 0.003
#define LIST_INTERVAL 0.0002
#define LIST_NUM (int)((LIST_MAX - LIST_MIN) / LIST_INTERVAL + 1)

// 连通域函数参考自博客园《OPENCV快速连通区域分析》一篇文章

void inputFile(const char *filename, vector<double> &res);
double getAdaptRate(const MyArray<double> &G, int im_height);
int getMax(double *data, int len);
int getMin(double *data, int len);

int main()
{
	vector<double> G;
	inputFile("G.txt", G);
	ImgProp::setShape(497, 801);
	ImgProp::setG(G.data());
	MyArray<double> GG;
	GG.reset(G.size());
	for (int i = 0; i < G.size(); i++)
		GG.pointer[i] = G[i];

	double res = getAdaptRate(GG, 360);
	cout << res << endl;

	return 0;
}

double getAdaptRate(const MyArray<double> &G, int im_height)
{
	double rate_list[LIST_NUM];
	double pixes[LIST_NUM];
	for (int i = 0; i < LIST_NUM; i++)
		rate_list[i] = LIST_MIN + i * LIST_INTERVAL;

	for (int i = 0; i < LIST_NUM; i++)
	{
		double peaks_rate = rate_list[i];
		double thresh = ImgProp::getThresh(peaks_rate);

		char *peaks_mask = new char[G.len];

		for (int k = 0; k < G.len; k++)
				if (G.pointer[k] > thresh)
					peaks_mask[k] = 255;
				else
					peaks_mask[k] = 0; 

		// 根据peaks_mask利用Mat构造函数
		// Mat(int rows, int cols, int type, void* data, size_t step=AUTO_STEP)构造图像
		// type 必须为CV_8UC1类型
		// data 指向用户数据的指针。矩阵构造器将会取数据和步长参数不分配矩阵数据。
		// 取而代之的是，仅仅初始化矩阵的头去指向具体的数据，这意味数据不会被拷贝。
		// 这个操作将会非常高效，并且可以用opencv的函数来处理外部的数据。值得注意的是，
		// 这些外部的数据不会被自动释放
		// 参考CSDN ：《OpenCV中踩过的坑系列 01- Mat(int rows, int cols, int type, void* data, size_t step=AUTO_STEP)》

		// ！！bug警告：
		// 特别注意： 因为MATLAB与C/C++处理图像的行列顺序不同，所以这里应该行列顺序想调换位置
		// 在聚类时，也应该做相应的处理
		Mat mask = Mat(ImgProp::wid, ImgProp::len, CV_8UC1, peaks_mask);
		/*imshow("tmp", mask);
		waitKey(0);*/
		// 去掉因为图像边缘产生的峰值
		int border_top = floor(ImgProp::len - im_height * 0.99) / 2;
		int border_bottom = floor(ImgProp::len + im_height * 0.99) / 2;
		Mat imROI1 = mask(Range(0, border_top), Range::all());
		Mat imROI2 = mask(Range(border_bottom, mask.rows), Range::all());
		imROI1.setTo(Scalar(0));
		imROI2.setTo(Scalar(0));

		Mat labels, stats, centroids;
		int nccomps = connectedComponentsWithStats(mask, labels, stats, centroids);
		cout << "连通域个数：" << nccomps << endl;
		
		double sum = 0;
		// 特别注意，编号0代表背景区域，应该剔除
		for (int k = 1; k < nccomps; k++)
			sum += stats.at<int>(k, CC_STAT_AREA);
		pixes[i] = sum / nccomps;

		delete[] peaks_mask;
	}

	int i;
	for (i = 0; i < 11; i++)
		if (i != getMax(pixes, i + 1))
			break;
	int index1 = i - 1;
	int index2 = getMin(pixes + index1, 11 - index1);

	// 与MATLAB不同，C/C++没有偏移，因此不用减1
	int result = index1 + index2;
	cout << "index1 : " << index1 << endl << "index2 : " << index2 << endl;
	cout << "result : " << result << endl;
	
	return rate_list[result];
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

int getMax(double *data, int len)
{
	int res = 0;
	for (int i = 1; i < len; i++)
		if (data[res] < data[i])
			res = i;
	return res;
}

int getMin(double *data, int len)
{
	int res = 0;
	for (int i = 1; i < len; i++)
		if (data[res] > data[i])
			res = i;
	return res;
}