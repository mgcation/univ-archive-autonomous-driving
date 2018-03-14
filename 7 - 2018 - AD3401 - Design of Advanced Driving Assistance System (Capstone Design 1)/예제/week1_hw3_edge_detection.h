#pragma once
#include "opencv2\opencv.hpp"
#include "utility.h"
#include <cmath>

# define M_E 2.7182818284590452354

class week1_hw3_edge_detection
{
public:
	static int run();
};

Mat sobel_edge_detection(Mat source);
Mat prewitt_edge_detection(Mat source);
Mat get_LoG_kernel(float sigma);
