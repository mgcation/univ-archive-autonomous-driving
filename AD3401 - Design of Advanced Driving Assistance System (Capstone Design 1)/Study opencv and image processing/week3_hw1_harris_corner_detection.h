#pragma once
#include "utility.h"
#include "opencv2\opencv.hpp"

using namespace cv;
using namespace utility;

class week3_hw1_harris_corner_detection
{
public:
	static Mat localMaxima(Mat source, Size localSize, double minimum);
	static Mat myHarris_cornerDetection(Mat source, double k);
	static Mat libHarris_cornerDetection(Mat source, double k);
	static int run();
};

