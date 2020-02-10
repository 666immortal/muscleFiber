#include "gradient.h"

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