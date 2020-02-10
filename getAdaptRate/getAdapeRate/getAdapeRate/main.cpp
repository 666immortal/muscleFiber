#include <iostream>
#include <opencv.hpp>
#include <fstream>
#include "ImgProp.h"

using namespace std;
using namespace cv;

// 连通域函数参考自博客园《OPENCV快速连通区域分析》一篇文章

void inputFile(const char *filename, vector<double> &res);
double getAdaptRate()

int main()
{
	vector<double> G;
	inputFile("G.txt", G);
	ImgProp::setShape(497, 801);
	ImgProp::setG(G.data());

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