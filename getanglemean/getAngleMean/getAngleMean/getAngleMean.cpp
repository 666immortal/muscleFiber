#include "getAngleMean.h"

// ע�⣺��һ���������������ǡ�MyArray<Lines>ָ������顱
double getAngleMean(MyArray<Lines> *line_set[], int num)
{
	double tmp1 = 0.0, tmp2 = 0.0;
	for (int i = 0; i < num; i++)
		for (int j = 0; j < line_set[i]->len; j++)
		{
			tmp1 += line_set[i]->pointer[j].theta * line_set[i]->pointer[j].G;
			tmp2 += line_set[i]->pointer[j].G;
		}

	return tmp1 / tmp2;
}