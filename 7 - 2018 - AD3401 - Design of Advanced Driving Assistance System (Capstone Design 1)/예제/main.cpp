#include "opencv2\opencv.hpp"
#include "week0_hw1_inverse_image.h"
#include "week0_hw2_inverse_video.h"
#include "week1_hw1_histogram_equalization.h"
#include "week1_hw2_gaussian_and_median_filter.h"
#include "week1_hw3_edge_detection.h"

int main(){
	//week0_hw1_inverse_image::run();
	//week0_hw2_inverse_video::run();
	//week1_hw1_histogram_equalization::run();
	//week1_hw2_gaussian_and_median_filter::run();
	week1_hw3_edge_detection::run();

	cv::destroyAllWindows();
	return 0;
}