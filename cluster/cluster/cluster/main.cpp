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

	double peaks_rate = getAdaptRate(GG, IM_HEIGHT);
	double thresh = ImgProp::getThresh(peaks_rate);
	cout << "thresh : " << thresh << endl;
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

	imshow("tmp", mask);

	int pointsCount = countNonZero(mask);
	cout << "Point num : " << pointsCount << endl;
	Mat points(pointsCount, 1, CV_32FC2), labels;
	Mat centers(pointsCount, 1, points.type());

	int tmp = 0;
	for(int row = 0; row < mask.rows; row++)
		for (int col = 0; col < mask.cols; col++)
		{
			if (mask.at<uchar>(row, col) != 0)
			{
				points.at<Vec2f>(tmp, 0)[0] = col;
				points.at<Vec2f>(tmp, 0)[1] = row;
				tmp++;
			}			
		}
		
	cv::kmeans(points, 3, labels, cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 10, 1.0),
		                             50, KMEANS_PP_CENTERS, centers);

	cv::Scalar colorTab[] = {
			cv::Scalar(0, 0, 255),
			cv::Scalar(0, 255, 0),
			cv::Scalar(255, 100, 100),
			cv::Scalar(255, 0, 255),
			cv::Scalar(0, 255, 255),
			cv::Scalar(100, 100, 255)
	};

	Mat img(ImgProp::wid, ImgProp::len, CV_8UC3);
	img = Scalar::all(0);
	for (int i = 0; i < pointsCount; i++) {
		int clusterIdx = labels.at<int>(i);
		cv::Point ipt = points.at<cv::Point2f>(i);
		cv::circle(img, ipt, 1, colorTab[clusterIdx], cv::FILLED, cv::LINE_AA);
	}

	int row[3] = { 0, 0, 0 }, col[3] = { 0, 0, 0 }, num[3] = { 0, 0, 0 };
	for (int i = 0; i < pointsCount; i++)
	{
		int clusterIdx = labels.at<int>(i);
		row[clusterIdx] += points.at<cv::Point2f>(i).x;
		col[clusterIdx] += points.at<cv::Point2f>(i).y;
		num[clusterIdx]++;
	}
	double mean_row[3], mean_col[3];
	for (int i = 0; i < 3; i++)
	{
		mean_row[i] = row[i] / num[i];
		mean_col[i] = col[i] / num[i];
	}

	cout << "mean_row : " << mean_row[0] << " " << mean_row[1] << " " << mean_row[2] << endl;
	cout << "mean_col : " << mean_col[0] << " " << mean_col[1] << " " << mean_col[2] << endl;

	cv::Point p[3] = { Point(mean_row[0], mean_col[0]),
							Point(mean_row[1], mean_col[1]),
							Point(mean_row[2], mean_col[2]), };

	cv::circle(img, p[0], 3, colorTab[4], cv::FILLED, cv::LINE_AA);
	cv::circle(img, p[1], 3, colorTab[4], cv::FILLED, cv::LINE_AA);
	cv::circle(img, p[2], 3, colorTab[4], cv::FILLED, cv::LINE_AA);

	Mat tran_img;
	transpose(img, tran_img);
	cv::imshow("clusters", tran_img);
	//imwrite("cluster_res.bmp", tran_img);
	waitKey(0);

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