#include <iostream>
#include <algorithm>
#include <opencv.hpp>
#include <fstream>
#include "ImgProp.h"

using namespace std;
using namespace cv;

void inputFile(const char *filename, vector<double> &res);
double imthresh(double *img, int len_img, double rate);

static bool cmp(double a, double b)
{
	return a > b;
}

int main()
{
	vector<double> G;
	inputFile("G.txt", G);

	ImgProp::set(497, 801, G.data());

	cout << ImgProp::getThresh(0.001) << endl;

	return 0;
}

// 注意：该函数会破坏传入数组的顺序，应该传入一个副本
// 如果要提速，此处应做修改，因为在一个大循环中调用该函数，每次传入都需要拷贝和排序，费时费力
// 可以在循环外先排好序
double imthresh(double *data, int len_data, double rate)
{
	sort(data, data + len_data, cmp);
	int tmp = ceil(len_data * rate);
	return data[tmp];
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