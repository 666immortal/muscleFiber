#include <iostream>
#include <algorithm>

using namespace std;

double imthresh(double *img, int len_img, double rate);
bool cmp(double a, double b);

int main()
{


	return 0;
}

// 注意：该函数会破坏传入数组的顺序，应该传入一个副本
// 如果要提速，此处应做修改，因为在一个大循环中调用该函数，每次传入都需要拷贝和排序，费时费力
// 可以在循环外先排好序
double imthresh(double *img, int len_img, double rate)
{
	sort(img, img + len_img, cmp);
	int tmp = ceil(len_img * rate);
	return img[tmp];
}

bool cmp(double a, double b)
{
	return a > b;
}