#include "week4_hw2_surf.h"


int week4_hw2_surf::run(){
	initModule_nonfree();

	char* object = "resource/object.png";
	char* target = "resource/target.png";
	char* cite_object = "resource/cite_a.jpg";
	char* cite_target = "resource/cite_b.jpg";

	week4_hw2_surf::match_image_my_surf(object, target, "FlannBased", true);

	waitKey(0);
	return 0;
}

void week4_hw2_surf::match_image_my_surf(
	const String& object, const String& target, 
	const String& match_algorithm,
	bool do_i_run_library_match)
{
	int failed = 7;
	// refer : https://github.com/opencv/opencv/blob/master/modules/features2d/src/matchers.cpp
	failed -= match_algorithm.compare("FlannBased") == 0;
	failed -= match_algorithm.compare("BruteForce") == 0; //L2
	failed -= match_algorithm.compare("BruteForce-SL2") == 0; //Squeared L2
	failed -= match_algorithm.compare("BruteForce-L1") == 0;
	failed -= match_algorithm.compare("BruteForce-Hamming") == 0;
	failed -= match_algorithm.compare("BruteForce-HammingLUT") == 0;
	failed -= match_algorithm.compare("BruteForce-Hamming(2)") == 0;
	if (!failed){
		std::cout << "bad argument : " << match_algorithm << " (use : FlannBased, BruteForce, "\
			<< "BruteForce-SL2, BruteForce-L1, BruteForce-Hamming, BruteForce-HammingLUT, BruteForce-Hamming(2)\n";
		getchar();
		return;
	}

	Mat object_mat, target_mat;
	vector<KeyPoint> object_kp, target_kp;
	Mat object_descriptor, target_descriptor;

	my_imread(object_mat, object.c_str(), 0);
	my_imread(target_mat, target.c_str(), 0);

	// --------------------------------------------------
	// my surf implementation

	MySurf ms(400);
	//SurfFeatureDetector ms(400);
	ms.detect(object_mat, object_kp);
	ms.detect(target_mat, target_kp);
	ms.compute(object_mat, object_kp, object_descriptor);
	ms.compute(target_mat, target_kp, target_descriptor);

	// --------------------------------------------------

	if (do_i_run_library_match){
		week4_hw1_ransac::draw_library_ransac_match(
			object_mat, target_mat,
			object_kp, target_kp,
			object_descriptor, target_descriptor,
			match_algorithm
			);
	}
	else {
		week4_hw1_ransac::draw_my_ransac_match(
			object_mat, target_mat,
			object_kp, target_kp, 
			object_descriptor, target_descriptor,
			match_algorithm
			);
	}
}

week4_hw2_surf::MySurf::MySurf(double hessianThreshold, int nOctaves){
	this->hessianThreshold = hessianThreshold;
	this->nOctaves = nOctaves;
}

week4_hw2_surf::MySurf::~MySurf(){

}

void week4_hw2_surf::MySurf::detect(Mat image, vector<KeyPoint>& image_keyPoints){
	int class_id = -1;

	// 1. interesting point(KeyPoint) detection

	// 1-1. calc integral image pyramid (gray)
	Mat integral_image;
	Mat grayImage;
	if (image.type() == CV_8UC1)
		grayImage = image.clone();
	else
		cvtColor(image, grayImage, CV_BGR2GRAY);
	integral(grayImage, integral_image);

	// 1-2. hessian matrix filtering
	// hessian size : 9, 15, 21, 27, ...(+6)
	vector<Mat> image_hessian_response(nOctaves);
	int hessian_size = 9;
	for (auto ihr : image_hessian_response){
		integral_to_fast_hessian_response(integral_image, ihr, hessian_size);
		//Dxx.mul(Dyy) - 0.81*Dxy.mul(Dxy);
		hessian_size += 6;
	}

	// 1-3. local extrema (KeyPoint) detection
	for (int z = 0; z < nOctaves; z++){
		for (int y = 1; y < grayImage.rows-1; y++){
			for (int x = 1; x < grayImage.cols - 1; x++){

				int local_extrema_when_is_26 = 0;
				if (z == 0 || z == nOctaves - 1)
					local_extrema_when_is_26 += 9;

				for (int zz = -1; zz <= 1; zz++)
					for (int yy = -1; yy <= 1; yy++)
						for (int xx = -1; xx <= 1; xx++)
							if (image_hessian_response[z].at<double>(y, x) > image_hessian_response[z + zz].at<double>(y + yy, x + xx))
								local_extrema_when_is_26++;

				if (local_extrema_when_is_26 == 26){
					// 1-4. calculating orientation, response from
					float size = (9 + z * 6) / 2;
					float angle = 359;
					float response = 3342;

					orientation_calculation(image_hessian_response[z], y, x, angle, response);
					image_keyPoints.push_back(KeyPoint(x, y, size, angle, response, z, class_id));
				}
			}
		}
	}
}

double getPixelAtPoint(Mat image, int x, int y){
	int row = image.rows;
	int col = image.cols;
	if (x < 0 || y<0)
		return 0;
	if (col <= x){
		x = col - 1;
	}
	if (row <= y){
		y = row - 1;
	}
	return image.at<double>(y, x);
}

double getPixelAt2Point(Mat image, int y, int x, Point lt, Point rb){
	double A, B, C, D;
	int Ai, Aj, Bi, Bj, Ci, Cj, Di, Dj;

	Ai = Ci = lt.y + x;
	Aj = Bj = lt.x + y;
	Bi = Di = rb.y + x;
	Cj = Dj = rb.x + y;

	A = getPixelAtPoint(image, Ai, Aj);
	B = getPixelAtPoint(image, Bi, Bj);
	C = getPixelAtPoint(image, Ci, Cj);
	D = getPixelAtPoint(image, Di, Dj);

	return D - B - C + A;
}

void week4_hw2_surf::MySurf::integral_to_fast_hessian_response(Mat integral_image, Mat& output_hessian, int hessian_size){
	output_hessian = Mat(integral_image.size(), CV_64FC1);

	Mat Dxx(integral_image.size(), CV_64FC1);
	Mat Dyy(integral_image.size(), CV_64FC1);
	Mat Dxy(integral_image.size(), CV_64FC1);

	int row = integral_image.rows;
	int col = integral_image.cols;

	for (int y = 0; y < row; y++){
		for (int x = 0; x < col; x++){
			int padding = hessian_size / 4;
			int center = hessian_size / 2;


			Point lt(padding - 1 - center, -1 - center);
			Point rb(hessian_size - padding - 1 - center, hessian_size - 1 - center);
			Dyy.at<double>(y,x) = getPixelAt2Point(Dyy, y, x, lt, rb);


			lt = Point(lt.y, lt.x);
			rb = Point(rb.y, rb.x);
			Dxx.at<double>(y, x) = getPixelAt2Point(Dxx, y, x, lt, rb);
			lt = Point(hessian_size / 3 - 1 - center, lt.y);
			rb = Point(hessian_size / 3 * 2 - 1 - center, rb.y);
			Dxx.at<double>(y, x) -= 3 * getPixelAt2Point(Dxx, y, x, lt, rb);


			lt = Point(lt.y, lt.x);
			rb = Point(rb.y, rb.x);
			Dyy.at<double>(y, x) -= 3 * getPixelAt2Point(Dyy, y, x, lt, rb);


			lt = Point(-hessian_size / 3, -hessian_size / 3);	rb = Point(-1, -1);
			Point lt2 = Point(1, -hessian_size / 3), rb2 = Point(hessian_size / 3, -hessian_size / 3);
			Point lt3 = Point(-hessian_size / 3, 1), rb3 = Point(-1, hessian_size / 3);
			Point lt4 = Point(1, 1), rb4 = Point(hessian_size / 3, hessian_size / 3);
			Dxy.at<double>(y, x) =
				getPixelAt2Point(Dxy, y, x, lt, rb)-
				getPixelAt2Point(Dxy, y, x, lt2, rb2)-
				getPixelAt2Point(Dxy, y, x, lt3, rb3)+
				getPixelAt2Point(Dxy, y, x, lt4, rb4);
		}
	}

	double w = 0.9 * 0.9;
	output_hessian = Dxx.mul(Dyy) - w*Dxy.mul(Dxy);
}

void week4_hw2_surf::MySurf::orientation_calculation(Mat image_hessian_response, int y, int x, float& angle, float& response){

}

void week4_hw2_surf::MySurf::compute(Mat image, vector<KeyPoint>& image_keyPoints, Mat& image_keyPoints_descriptor){
	image_keyPoints_descriptor = Mat(Size(image_keyPoints.size(), 64),CV_32FC1,1);

	// 2. descriptor calculation & normalization

	// 2-1. 머야 시발 어케해. 방향에 따라서 픽셀 16x16 구해? 그리고 걍 때려박아?

}