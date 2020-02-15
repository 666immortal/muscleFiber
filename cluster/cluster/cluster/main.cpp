#include <iostream>
#include <opencv.hpp>
#include <fstream>
#include "ImgProp.h"
#include "MyArray.h"
#include "getAdaptRate.h"

#define IM_HEIGHT 360
#define CLUSTER_SIZE 3

using namespace std;
using namespace cv;

void inputFile(const char *filename, vector<double> &res);
void BubbleSort(int  *p, int length, int * ind_diff);
int findMaxIndx(double *data, int len);
// 定位筋膜的坐标
vector<Point> getPointOfFascia(const vector<Point> &points, const MyArray<double> &G);
// 定位肌纤维的坐标
vector<Point> getPointOfFiber(const vector<Point> &points, const MyArray<double> &G);
// 对聚类之后的点进行分类
void classifyPoint(const Mat &points, const Mat &labels, const Mat &centers, int pointsCount, vector<Point> *point_set);

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
		
	cv::kmeans(points, CLUSTER_SIZE, labels, cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 10, 1.0),
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

	for (int i = 0; i < 3; i++)
	{
		Point pc = Point(centers.at<float>(i, 0), centers.at<float>(i, 1));
		cout << pc << endl;
		cv::circle(img, pc, 3, colorTab[4], cv::FILLED, cv::LINE_AA);
	}

	Mat tran_img;
	transpose(img, tran_img);
	cv::imshow("clusters", tran_img);
	//imwrite("cluster_res.bmp", tran_img);

	vector<Point> point_set[CLUSTER_SIZE];
	// 将点分类并按大小顺序装到不同的容器中
	classifyPoint(points, labels, centers, pointsCount, point_set);
	
	vector<Point> points_of_low_fascia = getPointOfFascia(point_set[0], GG);
	vector<Point> points_of_high_fascia = getPointOfFascia(point_set[2], GG); 
	vector<Point> points_of_fiber = getPointOfFiber(point_set[1], GG);

	// 画出寻找筋膜的结果
	Mat low_fascia = Mat(ImgProp::wid, ImgProp::len, CV_8UC1);
	low_fascia.setTo(Scalar(0));
	for (int i = 0; i < points_of_low_fascia.size(); i++)
		low_fascia.at<uchar>(points_of_low_fascia[i].y, points_of_low_fascia[i].x) = 255;

	for (int i = 0; i < points_of_high_fascia.size(); i++)
		low_fascia.at<uchar>(points_of_high_fascia[i].y, points_of_high_fascia[i].x) = 255;

	for (int i = 0; i < points_of_fiber.size(); i++)
		low_fascia.at<uchar>(points_of_fiber[i].y, points_of_fiber[i].x) = 255;

	Mat result;
	transpose(low_fascia, result);
	imshow("res", result);

	// 计算下筋膜的位置
	//double sum_y_low = 0, sum_x_low = 0, sum_G_low = 0;
	//for (int i = 0; i < points_of_low_fascia.size(); i++)
	//{
	//	double tmp_GG = G[points_of_low_fascia[i].y * ImgProp::len + points_of_low_fascia[i].x];
	//	sum_y_low += points_of_low_fascia[i].y * tmp_GG;
	//	sum_x_low += points_of_low_fascia[i].x * tmp_GG;
	//	sum_G_low += tmp_GG;
	//}

	// 计算上筋膜的位置
	//double sum_y_high = 0, sum_x_high = 0, sum_G_high = 0;
	//for (int i = 0; i < points_of_high_fascia.size(); i++)
	//{
	//	double tmp_GG = G[points_of_high_fascia[i].y * ImgProp::len + points_of_low_fascia[i].x];
	//	sum_y_high += points_of_high_fascia[i].y * tmp_GG;
	//	sum_x_high += points_of_high_fascia[i].x * tmp_GG;
	//	sum_G_high += tmp_GG;
	//}

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

// 由冒泡排序法得到索引
void BubbleSort(int  *p, int length, int * ind_diff)
{
	for (int i = 0; i < length; i++)
	{
		for (int j = 0; j < length - i - 1; j++)
		{
			// 由大到小排序
			if (p[j] < p[j + 1])
			{
				int temp = p[j];
				p[j] = p[j + 1];
				p[j + 1] = temp;

				int ind_temp = ind_diff[j];
				ind_diff[j] = ind_diff[j + 1];
				ind_diff[j + 1] = ind_temp;
			}
		}
	}
}

int findMaxIndx(double *data, int len)
{
	int res = 0;
	for (int i = 1; i < len; i++)
		if (data[res] < data[i])
			res = i;
	return res;
}

// 定位筋膜所在的坐标位置，返回点
vector<Point> getPointOfFascia(const vector<Point> &points, const MyArray<double> &G)
{
	Mat pcm = Mat(ImgProp::wid, ImgProp::len, CV_8UC1);
	pcm.setTo(Scalar(0));
	for (int i = 0; i < points.size(); i++)
		pcm.at<uchar>(points[i].y, points[i].x) = 255;

	Mat con_labels, stats, centroids;
	int nccomps = connectedComponentsWithStats(pcm, con_labels, stats, centroids);

	double *pclw = new double[nccomps - 1]();
	vector<Point> *points_of_con = new vector<Point>[nccomps - 1];

	int tmp_indx = 0;
	for (int j = 0; j < con_labels.rows; j++)
		for (int k = 0; k < con_labels.cols; k++)
		{
			if ((tmp_indx = con_labels.at<int>(j, k)) != 0)
			{
				// 累加权值
				pclw[tmp_indx - 1] += G.pointer[j * con_labels.cols + k];
				// 保存点的坐标位置到相应的容器中
				points_of_con[tmp_indx - 1].push_back(Point(k, j));
			}
		}

	// 找到权值序列中的最大值的索引
	int max_ind = findMaxIndx(pclw, nccomps - 1);
	vector<Point> points_of_low_fascia = points_of_con[max_ind];

	delete[] points_of_con;
	delete[] pclw;

	return points_of_low_fascia;
}

vector<Point> getPointOfFiber(const vector<Point> &points, const MyArray<double> &G)
{
	Mat pcm = Mat(ImgProp::wid, ImgProp::len, CV_8UC1);
	pcm.setTo(Scalar(0));
	for (int i = 0; i < points.size(); i++)
		pcm.at<uchar>(points[i].y, points[i].x) = 255;

	Mat con_labels, stats, centroids;
	int nccomps = connectedComponentsWithStats(pcm, con_labels, stats, centroids);

	vector<Point> *points_of_con = new vector<Point>[nccomps - 1];

	int tmp_indx = 0;
	for (int j = 0; j < con_labels.rows; j++)
		for (int k = 0; k < con_labels.cols; k++)
		{
			if ((tmp_indx = con_labels.at<int>(j, k)) != 0)
			{
				// 保存点的坐标位置到相应的容器中
				points_of_con[tmp_indx - 1].push_back(Point(k, j));
			}
		}

	Mat im_G = Mat(ImgProp::wid, ImgProp::len, CV_64FC1, G.pointer);
	vector<Point> fiber_points;

	for (int i = 0; i < nccomps - 1; i++)
	{
		Mat im_mask = Mat(ImgProp::wid, ImgProp::len, CV_8UC1);
		im_mask.setTo(Scalar(0));
		for (int j = 0; j < points_of_con[i].size(); j++)
			im_mask.at<uchar>(points_of_con[i][j].y, points_of_con[i][j].x) = 255;
		double tmp;
		Point tmp_p;
		minMaxLoc(im_G, NULL, &tmp, NULL, &tmp_p, im_mask);
		fiber_points.push_back(tmp_p);
	}

	delete[] points_of_con;
	return fiber_points;
}

void classifyPoint(const Mat &points, const Mat &labels, const Mat &centers, int pointsCount, vector<Point> *point_set)
{
	int clu_sort[CLUSTER_SIZE];
	int clu_ind[CLUSTER_SIZE];
	for (int i = 0; i < CLUSTER_SIZE; i++)
	{
		clu_sort[i] = centers.at<float>(i, 0);
		clu_ind[i] = i;
	}

	BubbleSort(clu_sort, CLUSTER_SIZE, clu_ind);
	int clu_ind_inv[CLUSTER_SIZE];
	// 调换索引的关系，使得分配更快
	for (int i = 0; i < CLUSTER_SIZE; i++)
		clu_ind_inv[clu_ind[i]] = i;
	// 将聚类得到的各个结果分别存放在各个容器内
	for (int i = 0; i < pointsCount; i++)
	{
		point_set[clu_ind_inv[labels.at<int>(i)]].push_back(points.at<cv::Point2f>(i));
	}
}