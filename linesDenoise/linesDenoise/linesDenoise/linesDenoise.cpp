#include "linesDenoise.h"

void linesDenoise(const MyArray<Lines> &lines, vector<int> &discard, const int im_len, const int im_wid, const int area_len, const int area_wid, const double angle_mean)
{
	vector<Cross> crosses;
	getCross(lines, crosses, im_len, im_wid, area_len, area_wid);

	for (int i = 0; i < crosses.size(); i++)
	{
		double t_d1 = abs(lines.pointer[crosses[i].line_i[0]].theta - angle_mean);
		double t_d2 = abs(lines.pointer[crosses[i].line_i[1]].theta - angle_mean);

		if (t_d1 > t_d2)
			discard.push_back(crosses[i].line_i[0]);
		else
			discard.push_back(crosses[i].line_i[1]);
	}

	sort(discard.begin(), discard.end());
	discard.erase(unique(discard.begin(), discard.end()), discard.end());
}

void linesDenoise(const vector<Lines> &lines, vector<int> &discard, const int im_len, const int im_wid, const int area_len, const int area_wid, const double angle_mean)
{
	vector<Cross> crosses;
	getCross(lines, crosses, im_len, im_wid, area_len, area_wid);

	for (int i = 0; i < crosses.size(); i++)
	{
		double t_d1 = abs(lines[crosses[i].line_i[0]].theta - angle_mean);
		double t_d2 = abs(lines[crosses[i].line_i[1]].theta - angle_mean);

		if (t_d1 > t_d2)
			discard.push_back(crosses[i].line_i[0]);
		else
			discard.push_back(crosses[i].line_i[1]);
	}

	sort(discard.begin(), discard.end());
	discard.erase(unique(discard.begin(), discard.end()), discard.end());
}