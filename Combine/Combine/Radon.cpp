#include "Radon.h"
#include <iostream>

Radon::Radon(cv::Mat src, double degree_min, double degree_max, double degree_interval)
{
	getIntervalDegrees(degree_min, degree_max, degree_interval);
	getIntervalRadians();
	pretreat(src);
	getCenter();
	getRho();
	radonc();
}

Radon::~Radon()
{
	
}

inline void Radon::getIntervalDegrees(double degree_min, double degree_max, double degree_interval)
{
	_degrees.resize((degree_max - degree_min) / degree_interval + 1);
	_degrees[0] = degree_min;
	for (int i = 1, j = 0; i < _degrees.size(); i++, j++)
		_degrees[i] = _degrees[j] + degree_interval;
}

void Radon::getRho()
{
	int x_len = _pretreated_matrix.cols - 1 - _center.x;
	int y_len = _pretreated_matrix.rows - 1 - _center.y;
	int rLast = (int)std::ceil(std::sqrt((double)(x_len * x_len + y_len * y_len))) + 1;
	int rFirst = -rLast;
	_rho.resize(rLast - rFirst + 1);
	for (int k = rFirst, i = 0; k <= rLast; k++, i++)
		_rho[i] = (double)k;
}

void Radon::pretreat(const cv::Mat &src_matrix)
{
	src_matrix.convertTo(_pretreated_matrix, CV_64FC1);
	double _mean = 0, *pixel;
	
	// ����ƽ��ֵ
	for (int i = 0; i < _pretreated_matrix.rows; i++)
	{
		pixel = _pretreated_matrix.ptr<double>(i);
		for (int j = 0; j < _pretreated_matrix.cols; j++, pixel++)
		{
			*pixel /= 255;
			_mean += *pixel;
		}
	}
	_mean = _mean / (_pretreated_matrix.rows  * _pretreated_matrix.cols);
	// �����ؼ�ȥƽ��ֵ
	for (int i = 0; i < _pretreated_matrix.rows; i++)
	{
		pixel = _pretreated_matrix.ptr<double>(i);
		for (int j = 0; j < _pretreated_matrix.cols; j++, pixel++)
			*pixel -= _mean;
	}
}

/**
**pr������radon�任���������Ķ���һ���ض�theta�ǵ��е��׵�ַ
**pixel��Ҫ����radon�任������ֵ����0.25�Ժ��ֵ������ÿһ�����ص�ȡ�������ĸ�����߾��ȣ�
**���ڼ���ʱpixelҲҪ��Ӧ����0.25��������һ����ռ0.25�ı�����Ȼ���ĸ���պô���1�ķݶ
**r������radon�任�ĸõ����ʼ��rֵ����rFirst֮��Ĳ�
**/
inline void Radon::incrementRadon(double *pr, double pixel, double r)
{
	int r1 = (int)r;
	double delta = r - r1;					//����ÿһ���㣬rֵ��ͬ�����ԣ�ͨ�����ַ�ʽ�����԰���һ������Ӧ�е�Ԫ�ص�ֵ������
	pr[r1] += pixel * (1.0 - delta);		//radon�任ͨ����¼Ŀ��ƽ����ĳһ��ı�ӳ����Ļ��ۺ��������ԭƽ����ֱ�ߵĴ�����
	pr[r1 + 1] += pixel * delta;			//�����㻥����ϣ���߾��� 
}

void Radon::radonc()
{
	_radon_matrix = cv::Mat::zeros(_radians.size(), _rho.size(), CV_64FC1);
	double *xCosTable = new double[2 * _pretreated_matrix.cols];
	double *ySinTable = new double[2 * _pretreated_matrix.rows];

	for (int k = 0; k < _radians.size(); k++)
	{
		double *pr = _radon_matrix.ptr<double>(k);
		double cosine = std::cos(_radians[k]);
		double sine = std::sin(_radians[k]);

		for (int n = 0; n < _pretreated_matrix.cols; n++)
		{
			double x = n - _center.x;
			// ���ݼ������������
			xCosTable[2 * n] = (x - 0.25) * cosine;
			xCosTable[2 * n + 1] = (x + 0.25) * cosine;
		}

		for (int m = 0; m < _pretreated_matrix.rows; m++)
		{
			double y = _center.y - m;
			// ���ݼ�������������
			ySinTable[2 * m] = (y - 0.25) * sine;
			ySinTable[2 * m + 1] = (y + 0.25) * sine;
		}

		double *pixelPtr = (double *)_pretreated_matrix.data;
		for (int m = 0; m < _pretreated_matrix.rows; m++)
		{
			for (int n = 0; n < _pretreated_matrix.cols; n++)		//����ԭ�����е�ÿһ�����ص�
			{
				double pixel = *pixelPtr++;
				if (pixel != 0.0)						//����õ�����ֵ��Ϊ0��Ҳ��ͼ������
				{
					double r = 0;
					pixel *= 0.25;
					//һ�����ص�ֽ���ĸ��ٽ������ص���м��㣬��߾�ȷ��
					r = xCosTable[2 * n] + ySinTable[2 * m] - _rho[0];
					incrementRadon(pr, pixel, r);

					r = xCosTable[2 * n + 1] + ySinTable[2 * m] - _rho[0];
					incrementRadon(pr, pixel, r);

					r = xCosTable[2 * n] + ySinTable[2 * m + 1] - _rho[0];
					incrementRadon(pr, pixel, r);

					r = xCosTable[2 * n + 1] + ySinTable[2 * m + 1] - _rho[0];
					incrementRadon(pr, pixel, r);
				}
			}
		}
	}
	delete[]xCosTable;
	delete[]ySinTable;
}

void Radon::showRadonMatrixRows(int n_rows_l, int n_rows_r)
{
	cv::Mat tmp = _radon_matrix(cv::Range(n_rows_l, n_rows_r), cv::Range::all());
	std::cout << cv::format(tmp, cv::Formatter::FMT_PYTHON) << std::endl;
}

void Radon::showRho()
{
	std::cout << "The rho : ";
	for (auto &var : _rho)
		std::cout << var << " ";
	std::cout << std::endl;
}

void Radon::showRadonMatrixSize()
{
	std::cout << "Radon Matrix size : ";
	std::cout << "(" << _radon_matrix.rows << ", " << _radon_matrix.cols << ")" << std::endl;
}

void Radon::showDegreesVector()
{
	std::cout << "Degrees Vector : ";
	for (auto &var : _degrees)
		std::cout << var << " ";
	std::cout << std::endl;
}

void Radon::showPretreatedMatrixRows(int n_rows_l, int n_rows_r)
{
	cv::Mat tmp = _pretreated_matrix(cv::Range(n_rows_l, n_rows_r), cv::Range::all());
	std::cout << cv::format(tmp, cv::Formatter::FMT_PYTHON) << std::endl;
}

void Radon::showRadiansVector()
{
	std::cout << "Radians Vector : ";
	for (auto &var : _radians)
		std::cout << var << " ";
	std::cout << std::endl;
}