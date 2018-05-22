#include "week3_hw1_harris_corner_detection.h"
/*
harris corner detection 직접 구현해보고 라이브러리와 비교
*/

int week3_hw1_harris_corner_detection::run(){
	Mat source;
	my_imread(source, "resource/chess board.jpg");

	double k = 0.04;
	int localMaximaSize = 21;
	double thresh = 0.001;

	Mat myHarris, myHarris_thresh;
	myHarris = myHarris_cornerDetection(source, k);
	myHarris = localMaxima(myHarris, Size(localMaximaSize, localMaximaSize), thresh);
	threshold(myHarris, myHarris_thresh, 0.005, 255, THRESH_BINARY);

	Mat libHarris, libHarris_thresh;
	libHarris = libHarris_cornerDetection(source, k);
	libHarris = localMaxima(libHarris, Size(localMaximaSize, localMaximaSize), thresh);
	threshold(libHarris, libHarris_thresh, 0.005, 255, THRESH_BINARY);

	imshow("source", source);
	imshow("myHarris_local_maxima", myHarris * 100);
	imshow("libHarris_local_maxima", libHarris * 100);
	imshow("myHarris_thresh", myHarris_thresh);
	imshow("libHarris_thresh", libHarris_thresh);
	waitKey(0);

	return 0;
}

//! CV32FC1 기준으로 계산하며 CV8FC1으로 반환함.
Mat week3_hw1_harris_corner_detection::localMaxima(Mat source, Size localSize, double minimum){
	Mat ret(source.rows, source.cols, CV_32FC1, Scalar(0));
	assert(localSize.height % 2 == 1 && localSize.width % 2 == 1);
	assert(source.type() == CV_32FC1);

	for (int y = 0; y < source.rows; y++){
		for (int x = 0; x < source.cols; x++){
			float& maybeCorner = source.at<float>(y, x);
			if (maybeCorner > minimum){
				int yStart = y - localSize.height / 2;
				int yEnd = y + localSize.height / 2;
				int xStart = x - localSize.width / 2;
				int xEnd = x + localSize.width / 2;
				if (yStart < 0) yStart = 0;
				if (source.rows <= yEnd) yEnd = source.rows - 1;
				if (xStart < 0) xStart = 0;
				if (source.cols <= xEnd) xEnd = source.cols - 1;
				bool corner = true;
				for (int yy = yStart; (yy <= yEnd) && corner; yy++){
					for (int xx = xStart; (xx <= xEnd) && corner; xx++){
						if (xx == x && yy == y)
							continue;
						if (source.at<float>(yy, xx) >= maybeCorner)
							corner = false;
					}
				}
				if (corner){
					ret.at<float>(y, x) = maybeCorner;
				}
			}
		}
	}

	return ret;
}

Mat week3_hw1_harris_corner_detection::myHarris_cornerDetection(Mat source, double k){
	// 0. to gray
	Mat gray = ifnot_2gray(source);

	// 1. Gradient 계산
	Mat sobelX, sobelY;
	sobelX = (Mat_<float>(3, 3) << -1, 0, 1, -3, 0, 3, -1, 0, 1);
	transpose(sobelX, sobelY);
	Mat Ix, Iy, Ix2, Iy2, Ixy, Ixy2;
	filter2D(gray, Ix, gray.depth(), sobelX);
	filter2D(gray, Iy, gray.depth(), sobelY);
	Ix.convertTo(Ix, CV_32FC1, 1.0f / 255);
	Iy.convertTo(Iy, CV_32FC1, 1.0f / 255);
	Ix2 = Ix.mul(Ix);
	Iy2 = Iy.mul(Iy);
	Ixy = Ix.mul(Iy);

	// 2. Blurring (가중치 부여)
	GaussianBlur(Ix2, Ix2, Size(3, 3), 27);
	GaussianBlur(Iy2, Iy2, Size(3, 3), 27);
	GaussianBlur(Ixy, Ixy, Size(3, 3), 27);
	/*Mat gaussianFilter = (Mat_<float>(3, 3) << 1, 4, 1, 4, 7, 4, 1, 4, 1) / 27;
	filter2D(Ix2, Ix2, Ix2.depth(), gaussianFilter);
	filter2D(Iy2, Iy2, Iy2.depth(), gaussianFilter);
	filter2D(Ixy, Ixy, Ixy.depth(), gaussianFilter);*/

	// 3. harris
	// lambda1, lambda2 가 M의 eigen values 일 때 다음과 같은 식임. : (lambda1 * lambda2) - k*(lambda1 + lambda2)^2
	// M = [Ix2 Ixy ; Ixy Iy2]
	// auto R = determinant.each(M) - (k*trace(M)*trace(M));
	Ixy2 = Ixy.mul(Ixy);
	Mat R = (Ix2.mul(Iy2) - Ixy2) - k*((Ix2 + Iy2).mul(Ix2 + Iy2));

	return R;
}

Mat week3_hw1_harris_corner_detection::libHarris_cornerDetection(Mat source, double k){
	Mat ret = ifnot_2gray(source);
	cornerHarris(ret, ret, 2, 3, k);
	return ret;
}