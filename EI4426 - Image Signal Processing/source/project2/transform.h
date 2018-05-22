#pragma once

#include "opencv2\opencv.hpp"
#include "utility.h"

using namespace cv;

namespace proj_2{
	const int MY_INTER_NN = 1;
	const int MY_INTER_BILINEAR = 2;

	Mat getTransitionMatrix(int x, int y);
	Mat getRotationMatrix(double degree);
	Mat getRotationMatrix(double degree, Point2f center);
	Mat getScalingMatrix(double xScale, double yScale);
	void backwardMapping(const Mat& source, Mat& dest, const Mat& transform, const int INTER);
}

namespace library{
	const int LIB_INTER_NN = INTER_NEAREST;
	const int LIB_INTER_BILINEAR = INTER_LINEAR;

	void backwardMapping(const Mat& source, Mat& dest, const Mat& transform, const int INTER);
}