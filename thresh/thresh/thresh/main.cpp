#include <iostream>
#include <algorithm>

using namespace std;

double imthresh(double *img, int len_img, double rate);
bool cmp(double a, double b);

int main()
{


	return 0;
}

// ע�⣺�ú������ƻ����������˳��Ӧ�ô���һ������
// ���Ҫ���٣��˴�Ӧ���޸ģ���Ϊ��һ����ѭ���е��øú�����ÿ�δ��붼��Ҫ���������򣬷�ʱ����
// ������ѭ�������ź���
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