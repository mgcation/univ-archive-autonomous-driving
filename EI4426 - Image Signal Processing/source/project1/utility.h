#include "opencv2\opencv.hpp"
#pragma once

using namespace cv;

namespace utility{
	//! source must 3 channel, kernel must 1 channel. no operation for padding.
	void filterBased3Vec(const Mat& source, Mat& result, const Mat& kernel);
	void filterBasedUchar(const Mat& source, Mat& result, const Mat& kernel);
	uchar calc_MSE(Mat target1, Mat target2, bool print, const char* msg);
	void localMaxima(const Mat& source, Mat& result, Size localSize, double minimum, double allowableError = 0);
}

