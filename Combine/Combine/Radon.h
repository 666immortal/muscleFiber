#pragma once
#include <opencv.hpp>
#include <valarray>

#define MAX(x, y)  ((x) > (y) ? (x) : (y)) // ����꺯�����ú�������ȡ���������ֵ

class Radon
{
private:
	const double deg2rad = 3.14159265358979 / 180.0;
	inline void getIntervalDegrees(double degree_min, double degree_max, double degree_interval);
	void getIntervalRadians() { _radians = _degrees * deg2rad; }
	void getCenter()  { _center.x = MAX(0, (_pretreated_matrix.cols - 1) / 2);
								_center.y = MAX(0, (_pretreated_matrix.rows - 1) / 2); }
	void getRho();
	void pretreat(const cv::Mat &src_matrix); // �޸�ͨ�����������ͣ���һ������
	inline void incrementRadon(double *pr, double pixel, double r);
	void radonc();
protected:
	cv::Mat _pretreated_matrix;			// Ԥ�������
	std::valarray<double> _degrees;		// �Ƕ�����
	std::valarray<double> _radians;		// ��������
	cv::Mat _radon_matrix;					// radon�任����
	std::valarray<double> _rho;	
	cv::Point2i _center;						// ͼ�����ĵ�
public:
	Radon(cv::Mat src, double theta_min, double theta_max, double theta_interval);
	virtual ~Radon();
	void showRadonMatrixRows(int n_rows_l, int n_rows_r);			// ���Radon�任����
	void showPretreatedMatrixRows(int n_rows_l, int n_rows_r);	// ���Ԥ�������
	void showDegreesVector();								//	����Ƕ�����
	void showRadiansVector();								// �����������
	void showRho();												// ���Rho
	void showRadonMatrixSize();							// ���Radon�任����Ĵ�С
};
