#pragma once
#include "opencv2\opencv.hpp"
#include "utility.h"
#include <algorithm>
#include <vector>
class week1_hw2_gaussian_and_median_filter
{
public:
	static int run();
};

Mat saltpepper_noise(Mat source, float rate, double standard_deviation);
Mat gaussian_noise(Mat source, double standard_deviation);
Mat myMedian_filter(Mat source);
Mat myGaussian_filter(Mat source, double standard_deviation);