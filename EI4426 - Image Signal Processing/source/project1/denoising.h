#include "opencv2\opencv.hpp"

using namespace cv;

#pragma once

namespace proj_1{
	void median_filter(const Mat& source, Mat& result, int kernelSize, bool enhancedToImpulseImage);
	void gaussian_filter(const Mat& source, Mat& result, int kernelSize, double sigma);
	void bilateral_filter(const Mat& source, Mat& result, int kernelSize, double sigmaColor, double sigmaSpace);
}

namespace library{
	void median_filter(const Mat& source, Mat& result, int kernelSize);
	void gaussian_filter(const Mat& source, Mat& result, int kernelSize, double sigma);
	void bilateral_filter(const Mat& source, Mat& result, int kernelSize, double sigmaColor, double sigmaSpace);
}
