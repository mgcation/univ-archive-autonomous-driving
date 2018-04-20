#include "opencv2\opencv.hpp"
#include "utility.h"
#include <ctime>

using namespace utility;
using namespace cv;

#pragma once
class week2_hw1_bilinear_transform
{
public:
	static int run();

	static Mat get_default_transformation_mat();

	static Mat get_rotation_mat(Point2f center, double degree){ return get_rotation_mat(center, get_default_transformation_mat(), degree); }
	static Mat get_scaling_mat(double x_multiplier, double y_multiplier){ return get_scaling_mat(get_default_transformation_mat(), x_multiplier, y_multiplier); }
	static Mat get_translation_mat(double x_offset, double y_offset){ return get_translation_mat(get_default_transformation_mat(), x_offset, y_offset); }

	static Mat get_rotation_mat(Point2f center, Mat transMat, double degree);
	static Mat get_scaling_mat(Mat transMat, double x_multiplier, double y_multiplier);
	static Mat get_translation_mat(Mat transMat, double x_offset, double y_offset);

	static Mat transformate_bi_linear(Mat source, Mat transMat, Size size);
	static Mat transformate_nearst_neighbor(Mat source, Mat transM, Size size);
};

