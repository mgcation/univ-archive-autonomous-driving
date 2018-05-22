#include "opencv2\opencv.hpp"
#define M_PI 3.14159265358979323846264338327950288
#pragma once

using namespace cv;

namespace utility{
	//! source must 3 channel, kernel must 1 channel. no operation for padding.
	void filterBased3Vec(const Mat& source, Mat& result, const Mat& kernel);
	void filterBasedUchar(const Mat& source, Mat& result, const Mat& kernel);
	void filterBasedFloat(const Mat& source, Mat& result, const Mat& kernel, float maxVal = 1.0);
	uchar calc_MSE(Mat target1, Mat target2, bool print, const char* msg);
	void localMaxima(const Mat& source, Mat& result, Size localSize, double minimum, double allowableError = 0);
	Mat ifnot_2gray(const Mat& source);
	string type2str(int type);
}

