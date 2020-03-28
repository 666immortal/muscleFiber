#include <opencv.hpp>
#include "Radon.h"
//#include <opencv2/core/ocl.hpp>
#include <algorithm>

class Base_Processor : protected Radon {
private:
	void gradientRadonMatrix();   // 使用指针的方法处理，用时0.01s
	//void gradientRadonKernel();  // 使用卷积的方法处理，相比指针方法要慢，用时0.038s
	void createAdaptRateMatrix();
	double getThread(const double rate);
protected:
	cv::Mat &_gradient_radon_matrix = _radon_matrix;
	cv::Mat _adapt_rate_matrix;
	double *_ptr_adapt_rate_matrix;
	int pixel_total_amount;
public:
	Base_Processor(cv::Mat src, double degrees_min, double degrees_max, double degrees_interval)
		: Radon(src, degrees_min, degrees_max, degrees_interval) 
	{
		pixel_total_amount = _adapt_rate_matrix.rows * _adapt_rate_matrix.cols; 
		gradientRadonMatrix();
		createAdaptRateMatrix();
	}
	Radon::showRadonMatrixRows;
	virtual ~Base_Processor() {}
	double getAdaptRate();
};

// 使用指针的方法处理，用时0.01s
void Base_Processor::gradientRadonMatrix()
{
	cv::Mat _tmp(_radon_matrix.rows, _radon_matrix.cols, CV_64FC1);
	double *src = (double *)_radon_matrix.data;
	double *dst = (double *)_tmp.data;
	for (int i = 0; i < _radon_matrix.rows; i++)
	{
		dst[i * _radon_matrix.cols] = src[i * _radon_matrix.cols + 1] - src[i * _radon_matrix.cols];
		for (int j = 1; j < _radon_matrix.cols - 1; j++)
			dst[i * _radon_matrix.cols + j] = (src[i * _radon_matrix.cols + j + 1] - src[i * _radon_matrix.cols + j - 1]) / 2;
		dst[(i + 1) * _radon_matrix.cols - 1] = src[(i + 1) * _radon_matrix.cols - 1] - src[(i + 1) * _radon_matrix.cols - 2];
	}
	cv::swap(_tmp, _radon_matrix);
}

// 使用卷积的方法处理，相比指针方法要慢，用时0.038s
//void Base_Processor::gradientRadonKernel()
//{
//	// 卷积核为
//	// 0,     0,    0
//	// -0.5, 0, 0.5
//	// 0,     0,    0
//	cv::Mat kernel_y_down = (cv::Mat_<double>(3, 3) << 0, 0, 0, -0.5, 0, 0.5, 0, 0, 0);
//	cv::filter2D(_radon_matrix, _radon_matrix, _radon_matrix.depth(), kernel_y_down);
//}

void Base_Processor::createAdaptRateMatrix()
{
	_gradient_radon_matrix.copyTo(_adapt_rate_matrix);
	_ptr_adapt_rate_matrix = (double *)_adapt_rate_matrix.data;
	// 从大到小排列
	std::sort(_ptr_adapt_rate_matrix, 
					_ptr_adapt_rate_matrix + pixel_total_amount,
				std::greater<double>());
}

double Base_Processor::getThread(const double rate)
{
	int index = ceil(pixel_total_amount * rate);
	// 因为C与MATLAB的起始下标不同，所以要减1
	return _ptr_adapt_rate_matrix[index - 1];
}

double Base_Processor::getAdaptRate()
{

}

int main()
{
	cv::Mat src = cv::imread("bmc1.bmp", 0);
	Base_Processor test(src, 10.0, 170.0, 0.2);
	//test.showRadonMatrixRows(152, 155);
	cv::imshow("Show", src);

	//cv::UMat usrc = cv::imread("bmc1.bmp", 0).getUMat(cv::ACCESS_READ);
	
	/*std::vector<cv::ocl::PlatformInfo> plats;
	cv::ocl::getPlatfomsInfo(plats);
	const cv::ocl::PlatformInfo *platform = &plats[0];
	std::cout << "Platform name: " << platform->name().c_str() << std::endl;
	cv::ocl::Device current_device;
	platform->getDevice(current_device, 0);
	std::cout << "Device name: " << current_device.name().c_str() << std::endl;
	current_device.set(0);
	cv::ocl::setUseOpenCL(true);*/

	cv::waitKey(0);
	return 0;
}