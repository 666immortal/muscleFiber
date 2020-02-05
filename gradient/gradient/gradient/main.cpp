#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

void inputFile(const char *filename, vector<double> &res);
void gradient(const double *src, int row, int col, double *res);

int main()
{
	vector<double> P, r;

	inputFile("result_P", P);
	inputFile("result_r", r);

	int len_P = P.size();
	int len_r = r.size();
	double *res = new double[len_P];

	gradient(P.data(), len_r, len_P / len_r, res);

	for (int i = 0; i < r.size(); i++)
		cout << "num " << i << " r : " << r[i] << endl;

	cout << "P : " << endl;
	for (int k = 167; k < 168; k++)
		for (int j = 0; j < 497; j++)
		{
			cout << "row: " << k + 1 << " col: " << j << "__";
			cout << res[k * 497 + j] << "  " << endl;
		}

	delete[] res;

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

// row = 497, col = 801
void gradient(const double *src, const int row, const int col, double *res)
{
	for (int i = 0; i < col; i++)
	{
		res[i * row] = src[i * row + 1] - src[i * row];
		for (int j = 1; j < row - 1; j++)
		{
			res[i * row + j] = (src[i * row + j + 1] - src[i * row + j - 1]) / 2;
		}
		res[(i + 1) * row - 1] = src[(i + 1) * row - 1] - src[(i + 1) * row - 2];
	}
}

