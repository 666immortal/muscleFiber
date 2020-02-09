#include <iostream>
#include <opencv.hpp>
#include <cmath>
#include <vector>
#include <fstream>
#include "MyArray.h"
#include "Lines.h"

using namespace std;
using namespace cv;

// 经过验证，本函数计算结果与MATLAB一致

struct Cross
{
	int line_i[2];
	Point2f p;
};

void readLines(const char* filename, vector<Lines> &lines);
void process(const vector<Lines> &lines_set, vector<Cross> &cross_set, const int im_len, const int im_wid, const int area_len, const  int area_wid);
void process(const MyArray<Lines> &lines_set, vector<Cross> &cross_set, const int im_len, const int im_wid, const int area_len, const  int area_wid);

int main()
{
	vector<Lines> myLines;
	readLines("lines.txt", myLines);

	/*for (int i = 0; i < myLines.size(); i++)
	{
		cout << "No " << i << " : " << myLines[i].rho << endl;
	}*/

	vector<Cross> myCross;
	process(myLines, myCross, 360, 338, 2, 2);

	cout << "------------------------------------------" << endl;
	for (size_t i = 0; i < myCross.size(); i++)
	{
		cout << "No " << i << " : (" << myCross[i].line_i[0] << ", " << myCross[i].line_i[1] << ")" << endl;
	}

	return 0;
}

void process(const vector<Lines> &lines_set, vector<Cross> &cross_set, const int im_len, const int im_wid, const int area_len, const  int area_wid)
{
	int cross_i = 0;

	for (int i = 0; i < lines_set.size() - 1; i++)
	{
		Lines line1 = lines_set[i];
		for (int j = i + 1; j < lines_set.size(); j++)
		{
			Lines line2 = lines_set[j];
			double angle1 = line1.theta;
			double angle2 = line2.theta;
			if (angle1 != angle2)
			{
				double a = sin(angle1), b = -cos(angle1), c = sin(angle2);
				double d = -cos(angle2), e = -line1.rho, f = -line2.rho;
				double Y = round((d * e - b * f) / (a * d - b * c)) + floor((im_len + 1) / 2);
				double X = round((a * f - e * c) / (a * d - b * c)) + floor((im_wid + 1) / 2);

				if (1 - (im_len*(area_len - 1)) < Y
					&& im_len*area_len > Y
					&& 1 - (im_wid*(area_wid - 1)) < X
					&& im_wid *area_wid > X)
				{
					Cross tmp;
					tmp.line_i[0] = i;
					tmp.line_i[1] = j;
					tmp.p.x = Y;
					tmp.p.y = X;
					cross_set.push_back(tmp);
					cross_i++;
				}
			}
		}
	}
}

void process(const MyArray<Lines> &lines_set, vector<Cross> &cross_set, const int im_len, const int im_wid, const int area_len, const  int area_wid)
{
	int cross_i = 0;

	for (int i = 0; i < lines_set.len - 1; i++)
	{
		Lines line1 = lines_set.pointer[i];
		for (int j = i + 1; j < lines_set.len; j++)
		{
			Lines line2 = lines_set.pointer[j];
			double angle1 = line1.theta;
			double angle2 = line2.theta;
			if (angle1 != angle2)
			{
				double a = sin(angle1), b = -cos(angle1), c = sin(angle2);
				double d = -cos(angle2), e = -line1.rho, f = -line2.rho;
				double Y = round((d * e - b * f) / (a * d - b * c)) + floor((im_len + 1) / 2);
				double X = round((a * f - e * c) / (a * d - b * c)) + floor((im_wid + 1) / 2);

				if (1 - (im_len*(area_len - 1)) < Y
					&& im_len*area_len > Y
					&& 1 - (im_wid*(area_wid - 1)) < X
					&& im_wid *area_wid > X)
				{
					Cross tmp;
					tmp.line_i[0] = i;
					tmp.line_i[1] = j;
					tmp.p.x = Y;
					tmp.p.y = X;
					cross_set.push_back(tmp);
					cross_i++;
				}
			}
		}
	}
}

void readLines(const char* filename, vector<Lines> &lines)
{
	ifstream fid;
	fid.open(filename);
	if (!fid.is_open())
	{
		cout << "Could not open the file " << filename << endl;
		cout << "Program terminating." << endl;
		exit(EXIT_FAILURE);
	}

	double tmp;
	double store[7];
	int i = 0;
	fid >> tmp;
	while (fid.good())
	{
		store[i] = tmp;
		i++;
		if (i >= 7)
		{
			Lines t;
			t.point1.x = store[0];
			t.point1.y = store[1];
			t.point2.x = store[2];
			t.point2.y = store[3];
			t.theta = store[4];
			t.rho = store[5];
			t.G = store[6];
			lines.push_back(t);
			i = 0;
		}
		fid >> tmp;
	}
	if (fid.eof())
		cout << "End of file reached.\n";
	else if (fid.fail())
		cout << "Input terminated by data mismatch.\n";
	else
		cout << "Input terminated for unknown reason.\n";
	fid.close();
}