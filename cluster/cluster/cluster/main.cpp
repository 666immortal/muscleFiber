#include <iostream>
#include <opencv.hpp>
#include <fstream>
#include "ImgProp.h"
#include "MyArray.h"
#include "getAdaptRate.h"

#define IM_HEIGHT 360
using namespace std;
using namespace cv;

void inputFile(const char *filename, vector<double> &res);

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

	double thresh = getAdaptRate(GG, IM_HEIGHT);

	char *peaks_mask = new char[G.size()];

	for (int k = 0; k < G.size(); k++)
		if (G[k] > thresh)
			peaks_mask[k] = 255;
		else
			peaks_mask[k] = 0;

	Mat mask = Mat(ImgProp::wid, ImgProp::len, CV_8UC1, peaks_mask);

	int border_top = floor(ImgProp::len - IM_HEIGHT * 0.99) / 2;
	int border_bottom = floor(ImgProp::len + IM_HEIGHT * 0.99) / 2;
	Mat imROI1 = mask(Range(0, border_top), Range::all());
	Mat imROI2 = mask(Range(border_bottom, mask.rows), Range::all());
	imROI1.setTo(Scalar(0));
	imROI2.setTo(Scalar(0));

	int pointsCount = countNonZero(mask);
	Mat points(pointsCount, 1, CV_32FC2), labels;
	Mat centers(pointsCount, 1, points.type());


	for (int k = 0; k < G.size(); k++)
		if (peaks_mask[k] != 0)
		{
			points.at<Vec2f>(k)[0] = k / ImgProp::wid;
			points.at<Vec2f>(k)[1] = k % ImgProp::wid;
		}
		
	cv::kmeans(points, 3, labels, cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 10, 1.0),
		                             50, KMEANS_PP_CENTERS, centers);

	delete[] peaks_mask;
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