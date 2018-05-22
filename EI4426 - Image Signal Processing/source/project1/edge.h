#include "opencv2\opencv.hpp"
#include "utility.h"

using namespace cv;
using namespace utility;

#pragma once

namespace proj_1{
	void prewitt_detector(const Mat& source, Mat& result);
	void sobel_detector(const Mat& source, Mat& result);
	void LoG_detector(const Mat& source, Mat& result, int kernelSize);
}


namespace library{
	void sobel_detector(const Mat& source, Mat& result);
	void LoG_detector(const Mat& source, Mat& result, int kernelSize);
}
