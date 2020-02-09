#include <iostream>
#include <vector>
#include <opencv.hpp>
#include <fstream>
#include <math.h>
#include "MyArray.h"
#include "Lines.h"

using namespace std;
using namespace cv;

void inputFile(const char *filename, vector<double> &res);

int main()
{
	double theta_max = 170.0;
	double theta_min = 10.0;
	double theta_interval = 0.2;
	int theta_num = (theta_max - theta_min) / theta_interval + 1;
	MyArray<double> mytheta(theta_num);
	for (int i = 0; i < theta_num; i++)
		mytheta.pointer[i] = theta_min + i * theta_interval;
	//for (int i = 0; i < 10; i++)
		//cout << "1~10 of theta: [" << i << "]    " << mytheta.pointer[i] << endl;

	vector<double> r;
	inputFile("result_r", r);
	MyArray<double> myrho(r.size());
	for (int i = 0; i < r.size(); i++)
		myrho.pointer[i] = r[i];
	//for (int i = 0; i < 10; i++)
		//cout << "1~10 of rho: [" << i << "]   " << myrho.pointer[i] << endl;

	//Point2f tmp(376.86, 400.4749);
	//Lines test(360, 338, mytheta, myrho, tmp, 0);
	Lines::setPara(360, 338, mytheta, myrho);
	//Lines test(tmp, 0);
	Point2f tmp1(325, 305);
	Lines test1(tmp1, 11.2366);

	cout << "point1: (" << test1.point1.x << ", " << test1.point1.y << ")" << endl;
	cout << "point2: (" << test1.point2.x << ", " << test1.point2.y << ")" << endl;
	cout << "theta: " << test1.theta << endl << "rho: " << test1.rho << endl;
	cout << "G: " << test1.G << endl;

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