#include "getAdaptRate.h"

double getAdaptRate(const MyArray<double> &G, int im_height)
{
	double rate_list[LIST_NUM];
	double pixes[LIST_NUM];
	for (int i = 0; i < LIST_NUM; i++)
		rate_list[i] = LIST_MIN + i * LIST_INTERVAL;

	for (int i = 0; i < LIST_NUM; i++)
	{
		double peaks_rate = rate_list[i];
		double thresh = ImgProp::getThresh(peaks_rate);

		char *peaks_mask = new char[G.len];

		for (int k = 0; k < G.len; k++)
			if (G.pointer[k] > thresh)
				peaks_mask[k] = 255;
			else
				peaks_mask[k] = 0;

		// ����peaks_mask����Mat���캯��
		// Mat(int rows, int cols, int type, void* data, size_t step=AUTO_STEP)����ͼ��
		// type ����ΪCV_8UC1����
		// data ָ���û����ݵ�ָ�롣������������ȡ���ݺͲ�������������������ݡ�
		// ȡ����֮���ǣ�������ʼ�������ͷȥָ���������ݣ�����ζ���ݲ��ᱻ������
		// �����������ǳ���Ч�����ҿ�����opencv�ĺ����������ⲿ�����ݡ�ֵ��ע����ǣ�
		// ��Щ�ⲿ�����ݲ��ᱻ�Զ��ͷ�
		// �ο�CSDN ����OpenCV�вȹ��Ŀ�ϵ�� 01- Mat(int rows, int cols, int type, void* data, size_t step=AUTO_STEP)��

		// ����bug���棺
		// �ر�ע�⣺ ��ΪMATLAB��C/C++����ͼ�������˳��ͬ����������Ӧ������˳�������λ��
		// �ھ���ʱ��ҲӦ������Ӧ�Ĵ���
		Mat mask = Mat(ImgProp::wid, ImgProp::len, CV_8UC1, peaks_mask);
		/*imshow("tmp", mask);
		waitKey(0);*/
		// ȥ����Ϊͼ���Ե�����ķ�ֵ
		int border_top = floor(ImgProp::len - im_height * 0.99) / 2;
		int border_bottom = floor(ImgProp::len + im_height * 0.99) / 2;
		Mat imROI1 = mask(Range(0, border_top), Range::all());
		Mat imROI2 = mask(Range(border_bottom, mask.rows), Range::all());
		imROI1.setTo(Scalar(0));
		imROI2.setTo(Scalar(0));

		Mat labels, stats, centroids;
		int nccomps = connectedComponentsWithStats(mask, labels, stats, centroids);
		cout << "��ͨ�������" << nccomps << endl;

		double sum = 0;
		// �ر�ע�⣬���0����������Ӧ���޳�
		for (int k = 1; k < nccomps; k++)
			sum += stats.at<int>(k, CC_STAT_AREA);
		pixes[i] = sum / nccomps;

		delete[] peaks_mask;
	}

	int i;
	for (i = 0; i < 11; i++)
		if (i != getMax(pixes, i + 1))
			break;
	int index1 = i - 1;
	int index2 = getMin(pixes + index1, 11 - index1);

	// ��MATLAB��ͬ��C/C++û��ƫ�ƣ���˲��ü�1
	int result = index1 + index2;
	cout << "index1 : " << index1 << endl << "index2 : " << index2 << endl;
	cout << "result : " << result << endl;

	return rate_list[result];
}

int getMax(double *data, int len)
{
	int res = 0;
	for (int i = 1; i < len; i++)
		if (data[res] < data[i])
			res = i;
	return res;
}

int getMin(double *data, int len)
{
	int res = 0;
	for (int i = 1; i < len; i++)
		if (data[res] > data[i])
			res = i;
	return res;
}