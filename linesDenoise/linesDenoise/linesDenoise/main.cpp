#include <iostream>
#include <vector>
#include <fstream>
#include "linesDenoise.h"

using namespace std;

void readLines(const char* filename, vector<Lines> &lines);

int main()
{
	vector<Lines> tmp;
	readLines("lines.txt", tmp);

	vector<int> dis;
	linesDenoise(tmp, dis, 360, 338, 2, 2, 1.3182);

	for (int i = 0; i < dis.size(); i++)
		cout << "No. " << i << " : " << dis[i] << endl;

	return 0;
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