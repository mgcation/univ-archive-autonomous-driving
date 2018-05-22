#pragma once

#include "opencv2\opencv.hpp"
#include "utility.h"

using namespace cv;
using namespace utility;

namespace proj_2{
	void cornerMoravec(const Mat& source, Mat& dest, double threshold);
	void cornerHarris(const Mat& source, Mat& dest, float k, float threshold);
	void cornerSUSAN(const Mat& source, Mat& dest, unsigned int threshold1, double threshold2);
}

namespace library{
	void cornerMoravec(const Mat& source, Mat& dest, double threshold);
	void cornerHarris(const Mat& source, Mat& dest, double k, double threshold);
	void cornerSUSAN(const Mat& source, Mat& dest, double threshold);
}
