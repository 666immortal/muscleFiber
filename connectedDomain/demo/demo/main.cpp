#include <opencv.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace cv;

int getMax(double *data, int len);
int getMin(double *data, int len);

int main()
{
	double pixes[11];
	char filename[30];
	for (int i = 0; i < 11; i++)
	{
		sprintf(filename, "peaks_mask%d.bmp", i + 1);
		Mat im = imread(filename, 0);
		Mat labels, stats, centroids;

		int nccomps = connectedComponentsWithStats(im, labels, stats, centroids);
		cout << "连通域个数：" << nccomps - 1 << endl;

		double sum = 0;
		// 特别注意，编号0代表背景区域，应该剔除
		for (int k = 1; k < nccomps; k++)
			sum += stats.at<int>(k, CC_STAT_AREA);

		pixes[i] = sum / (nccomps - 1);
	}
	
	for(int i = 0; i < 11; i++)
		cout << "pixe " << i << " : " << pixes[i] << endl;

	int i;
	for (i = 0; i < 11; i++)
		if (i != getMax(pixes, i + 1))
			break;
	int index1 = i - 1;
	int index2 = getMin(pixes + index1, 11 - index1);

	// 与MATLAB不同，C/C++没有偏移，因此不用减1
	int result = index1 + index2;
	cout << "index1 : " << index1 << " index2 : " << index2 << endl;
	cout << "result: " << result << endl;

	return 0;
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