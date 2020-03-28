#pragma once
#include <opencv.hpp>
#include <valarray>

#define MAX(x, y)  ((x) > (y) ? (x) : (y)) // 定义宏函数，该函数用于取两数中最大值

class Radon
{
private:
	const double deg2rad = 3.14159265358979 / 180.0;
	inline void getIntervalDegrees(double degree_min, double degree_max, double degree_interval);
	void getIntervalRadians() { _radians = _degrees * deg2rad; }
	void getCenter()  { _center.x = MAX(0, (_pretreated_matrix.cols - 1) / 2);
								_center.y = MAX(0, (_pretreated_matrix.rows - 1) / 2); }
	void getRho();
	void pretreat(const cv::Mat &src_matrix); // 修改通道，数据类型，归一化处理
	inline void incrementRadon(double *pr, double pixel, double r);
	void radonc();
protected:
	cv::Mat _pretreated_matrix;			// 预处理矩阵
	std::valarray<double> _degrees;		// 角度向量
	std::valarray<double> _radians;		// 弧度向量
	cv::Mat _radon_matrix;					// radon变换矩阵
	std::valarray<double> _rho;	
	cv::Point2i _center;						// 图像中心点
public:
	Radon(cv::Mat src, double theta_min, double theta_max, double theta_interval);
	virtual ~Radon();
	void showRadonMatrixRows(int n_rows_l, int n_rows_r);			// 输出Radon变换矩阵
	void showPretreatedMatrixRows(int n_rows_l, int n_rows_r);	// 输出预处理矩阵
	void showDegreesVector();								//	输出角度向量
	void showRadiansVector();								// 输出弧度向量
	void showRho();												// 输出Rho
	void showRadonMatrixSize();							// 输出Radon变换矩阵的大小
};
