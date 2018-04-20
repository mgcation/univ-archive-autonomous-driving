#pragma once
#include "week4_hw2_surf.h"
#include "opencv2/opencv.hpp"
#include "utility.h"
#include "opencv2/nonfree/features2d.hpp"
#define ITERCOUNT 100

using namespace cv;
using namespace utility;

class week4_hw1_ransac
{
public:
	static int run();
	static void match_image_library_feature(const String& object_filename, const String& target_filename, const String& detector_type, const String& match_algorithm, bool do_i_run_library_match);
	static int draw_library_ransac_match(Mat object, Mat target, vector<KeyPoint> object_kp, vector<KeyPoint> target_kp, Mat object_descriptor, Mat target_descriptor, const String& match_algorithm);
	static int draw_my_ransac_match(Mat object, Mat target, vector<KeyPoint> object_kp, vector<KeyPoint> target_kp, Mat object_descriptor, Mat target_descriptor, const String& match_algorithm, unsigned int ransacIterCount = ITERCOUNT, bool step_by_step_iteration_esc = false);
};

