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
double imthresh(double *data, int len_data, double rate)
{
	sort(data, data + len_data, cmp);
	int tmp = ceil(len_data * rate);
	return data[tmp];
}

bool cmp(double a, double b)
{
	return a > b;
}