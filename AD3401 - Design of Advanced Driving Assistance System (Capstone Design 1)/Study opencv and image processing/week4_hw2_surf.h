#pragma once
#include "week4_hw1_ransac.h"
#include "opencv2\opencv.hpp"
#include "utility.h"
#include "opencv2\nonfree\nonfree.hpp"

using namespace utility;
using namespace cv;

class week4_hw2_surf
{
public:
	static int run();
	static void match_image_my_surf(const String& object, const String& target, const String& match_algorithm, bool do_i_run_library_match = false);

	//! 64-dim ((4*4)*4), 45degrees
	class MySurf{
		double hessianThreshold;
		int nOctaves;
		int nOctaveLayers;

	public :
		MySurf(double hessianThreshold, int nOctaves=4);
		~MySurf();
		void detect(Mat image, vector<KeyPoint>& image_keyPoints);
		void compute(Mat image, vector<KeyPoint>& image_keyPoints, Mat& image_keyPoints_descriptor);
	private:
		void integral_to_fast_hessian_response(Mat integral_image, Mat& output_hessian, int hessian_size);
		void orientation_calculation(Mat image_hessian_response, int y, int x, float& angle, float& response);
	};
};

