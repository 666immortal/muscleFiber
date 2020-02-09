#include <iostream>
#include <vector>
#include <fstream>
#include "MyArray.h"
#include "Lines.h"

using namespace std;

void readLines(const char* filename, vector<Lines> &lines);
double getAngleMean(MyArray<Lines> *line_set[], int num);

int main()
{
	vector<Lines> myLines;
	readLines("lines.txt", myLines);

	MyArray<Lines> tmp;
	tmp.reset(myLines.size());

	for (int i = 0; i < myLines.size(); i++)
		tmp.pointer[i] = myLines[i];

	MyArray<Lines> *line_set[1] = { &tmp };
	double res = getAngleMean(line_set, 1);

	cout << res;

	return 0;
}


// 注意：第一个参数传进来的是“MyArray<Lines>指针的数组”
double getAngleMean(MyArray<Lines> *line_set[], int num)
{
	double tmp1 = 0.0, tmp2 = 0.0;
	for(int i = 0; i < num; i++)
		for (int j = 0; j < line_set[i]->len; j++)
		{
			tmp1 += line_set[i]->pointer[j].theta * line_set[i]->pointer[j].G;
			tmp2 += line_set[i]->pointer[j].G;
		}

	return tmp1 / tmp2;
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