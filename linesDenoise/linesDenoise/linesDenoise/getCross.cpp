#include "getCross.h"
#include <cmath>

void getCross(const vector<Lines> &lines_set, vector<Cross> &cross_set, const int im_len, const int im_wid, const int area_len, const  int area_wid)
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

void getCross(const MyArray<Lines> &lines_set, vector<Cross> &cross_set, const int im_len, const int im_wid, const int area_len, const  int area_wid)
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