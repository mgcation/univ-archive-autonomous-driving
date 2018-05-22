#include "opencv2\opencv.hpp"
#include "utility.h"

using namespace utility;
using namespace cv;

#pragma once
class week2_hw2_find_chess_board_corners
{
public:
	static int run();
	static Mat libCornerDetection(Mat source, int maxCorner=10000, double qualityLevel=0.1, double minDistance=10, int blockSize=3);
	static Mat myCornerDetection(Mat source, bool showProcessingImage = false);
};
