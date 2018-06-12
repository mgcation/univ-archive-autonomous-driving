#pragma once
#include "opencv2\opencv.hpp"
#include "opencv2\nonfree\nonfree.hpp"
#include <stack>
#include <queue>

using namespace cv;

class AvmsMatcher
{
public:
	static Mat matchImage(Mat left, Mat back, Mat rear, Mat right);
	static Mat histogramMatching(Mat dataset_undisort, Mat colorTarget);

private:
	static void stitch(Mat src, Mat dest, Mat& result, Mat H);
	static Mat morphologyPyrBlending(Mat src1, Mat src2);
	static Mat pyramidBlending(vector<Mat> target);

// not used:
	static Mat findPerspectiveMatrix(Mat fromImage, Mat toImage);
};
