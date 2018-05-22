#include "opencv2\opencv.hpp"

using namespace cv;

#pragma once

namespace proj_1{
	//! rate must be at (0,1]
	void impulse_noise(const Mat& source, Mat& result, float rate, double sigma);
	void gaussian_noise(const Mat& source, Mat& result, double sigma);
	void uniform_noise(const Mat& source, Mat& result, unsigned int errLimit);
}
