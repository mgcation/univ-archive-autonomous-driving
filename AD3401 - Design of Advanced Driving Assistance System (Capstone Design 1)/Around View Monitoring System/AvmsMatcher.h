#pragma once
#include "opencv2\opencv.hpp"
#include "opencv2\nonfree\nonfree.hpp"

using namespace cv;

class AvmsMatcher
{
public:
	static Mat matchImage(Mat left, Mat back, Mat rear, Mat right);
	static Mat histogramMatching(Mat dataset_undisort, Mat colorTarget);
};

