#pragma once
#include "utility.h"
#include "opencv2\opencv.hpp"

using namespace cv;
using namespace utility;

class week3_hw2_hough_transform
{
public:
	static int run();
	static Mat myHough_lineDetection(Mat source, double rho, double theta, int thresh);
	static Mat libHough_lineDetection(Mat source, double rho, double theta, int thresh);
	static Mat drawLines(Mat source, Mat lines);
};

