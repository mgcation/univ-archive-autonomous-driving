#include "proj2_main.h"

/*
# project2 dependency
 - (default : Proj2 folder)
 - utility
 - main (프로그램 메인)
*/

int proj2_main()
{
	//Mat source = imread("resource/lena.png");
	Mat source = imread("resource/chess_board.png");
	//Mat source = imread("resource/qrcode.png");
	//Mat source = imread("resource/leaf.png");



	//-------- transform
	/*double degree = 460;
	double transitionX = 100;
	double transitionY = 0;
	double scaleX = 0.7;
	double scaleY = 0.5;
	Point2f center(source.rows / 2, source.cols / 2);

	Mat tm = Mat::eye(3, 3, CV_64FC1);
	tm = getScalingMatrix(scaleX, scaleY) * tm;
	tm = getRotationMatrix(degree, center) * tm;
	tm = getTransitionMatrix(transitionX, transitionY) * tm;

	Mat my_nn, my_bilinear, lib_nn, lib_bilinear;
	backwardMapping(source, my_nn, tm, MY_INTER_NN);
	backwardMapping(source, my_bilinear, tm, MY_INTER_BILINEAR);
	library::backwardMapping(source, lib_nn, tm, library::LIB_INTER_NN);
	library::backwardMapping(source, lib_bilinear, tm, library::LIB_INTER_BILINEAR);

	imshow("my_implementation_nearest_neighbor", my_nn);
	imshow("my_implementation_bilinear", my_bilinear);
	imshow("library_nearest_neighbor", lib_nn);
	imshow("library_bilinear", lib_bilinear);*/
	//--------




	//-------- corner detection
	Mat my_moravec, my_harris, my_susan, lib_moravec, lib_harris, lib_susan;
	double harris_k = 0.04;
	double harris_threshold = 0.0001;
	int moravec_threshold = 7500; // max : 585225 (255*255*9)
	unsigned int susan_threshold1 = 20; // 클수록 적게 / 명암값 차이 10까지 같다고 인정
	double susan_threshold2 = 0.50; // 클수록 많이 / 마스크 넓이의 75%

	cornerMoravec(source, my_moravec, moravec_threshold);
	cornerHarris(source, my_harris, harris_k, harris_threshold);
	library::cornerHarris(source, lib_harris, harris_k, harris_threshold);
	cornerSUSAN(source, my_susan, susan_threshold1, susan_threshold2);

	imshow("source", source);
	imshow("my moravec", my_moravec);
	imshow("my harris", my_harris);
	imshow("library harris", lib_harris);
	imshow("my susan", my_susan);
	//--------

	waitKey(0);
	return 0;
}

