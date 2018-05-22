#include "week2_hw2_find_chess_board_corners.h"

/*
Chess-board 이미지를 불러와 Corner Detection을 한다.
배운 것들을 활용해서 도전해보기.
*/
int week2_hw2_find_chess_board_corners::run()
{
	vector<Mat> source(5);
	vector<char*> filename(5);
	filename[0] = "resource/chess board.jpg";
	filename[1] = "resource/chess board2.png";
	filename[2] = "resource/chess board3.png";
	filename[3] = "resource/chess board4.png";
	filename[4] = "resource/22.png";

	char c;
	int i = 0;
	bool start = 1;
	while ((c = waitKey(0)) != 27){
		if (c == 13 || start){
			i = (i + 1) % source.size();
			start = 0;

			my_imread(source[i], filename[i]);
			if (source[i].total() > 640 * 480)
				resize(source[i], source[i], Size(640, 480));


			Mat myCorner, libCorner;
			myCorner = myCornerDetection(source[i], false);
			libCorner = libCornerDetection(source[i]);

			imshow("source", source[i]);
			imshow("myCorner", myCorner);
			imshow("libCorner", libCorner);
		}
	}

	return 0;
}

Mat week2_hw2_find_chess_board_corners::myCornerDetection(Mat source, bool showProcessingImage){
	Mat ret = source.clone();
	ret.convertTo(ret, CV_8UC1);
	cvtColor(ret, ret, CV_BGR2GRAY);

	Mat sobel_x = (Mat_<double>(3, 3) <<
		-1, 0, 1
		, -2, 0, 2
		, -1, 0, 1
		);
	Mat sobel_y = (Mat_<double>(3, 3) <<
		-1, -2, -1
		, 0, 0, 0
		, 1, 2, 1
		);
	Mat extra_sobel_lt_rb = (Mat_<double>(5, 5) <<
		0, 0, -1, 0, 0,
		0, -2, 0, 0, 0,
		-1, 0, 0, 0, 1,
		0, 0, 0, 2, 0,
		0, 0, 1, 0, 0
		);
	Mat extra_sobel_lb_rt = (Mat_<double>(5, 5) <<
		0, 0, 1, 0, 0,
		0, 0, 0, 2, 0,
		-1, 0, 0, 0, 1,
		0, -2, 0, 0, 0,
		0, 0, -1, 0, 0
		);
	Mat retX, retY, retEX1, retEX2;

	filter2D(ret, retX, ret.depth(), sobel_x);
	filter2D(ret, retY, ret.depth(), sobel_y);
	retX.setTo(0, retX < 255 / 4);
	retY.setTo(0, retY < 255 / 4);
	retX.setTo(255, retX > 255 / 4 * 2);
	retY.setTo(255, retY > 255 / 4 * 2);
	Mat retXY = retX & retY;
	if (showProcessingImage){
		imshow("sobel x", retX);
		imshow("sobel y", retY);
		imshow("sobel x&y", retXY);
	}

	Mat temp1, temp2;
	filter2D(255 - ret, temp1, ret.depth(), extra_sobel_lt_rb);
	filter2D(255 - ret, temp2, ret.depth(), extra_sobel_lb_rt);
	filter2D(ret, retEX1, ret.depth(), extra_sobel_lt_rb);
	filter2D(ret, retEX2, ret.depth(), extra_sobel_lb_rt);
	retEX1 = (retEX1 | temp1);
	retEX2 = (retEX2 | temp2);

	retEX1.setTo(0, retEX1 < 255 / 4);
	retEX2.setTo(0, retEX2 < 255 / 4);
	retEX1.setTo(255, retEX1 > 255 / 4 * 3);
	retEX2.setTo(255, retEX2 > 255 / 4 * 3);

	if (showProcessingImage){
		imshow("sobel 변형 : 왼쪽위에서 오른쪽 아래", retEX1);
		imshow("sobel 변형 : 왼쪽아래에서 오른쪽 위", retEX2);
	}

	Mat cross = (Mat_<double>(3, 3) <<
		   0.2, -0.1,  0.2
		, -0.1,  0.3, -0.1
		,  0.2, -0.1,  0.2
		);
	Mat retExtra = (retEX1 & retEX2);
	if (showProcessingImage){
		imshow("대각선 검출", retExtra);
	}
	retExtra = 255 - retExtra;
	if (showProcessingImage){
		imshow("대각선 검출 반전", retExtra);
	}
	filter2D(retExtra, retExtra, retExtra.depth(), cross);
	retExtra.setTo(255, retExtra >= 255 * 0.3);
	retExtra.setTo(0, retExtra < 255 * 0.3);
	if (showProcessingImage){
		imshow("대각선들", retExtra);
	}

	Mat mask = retXY & retExtra;
	ret.setTo(0);
	ret.setTo(255, mask > 100);
	return ret;
}

// maxCorner = 10000     qualityLevel = 0.1     minDistance = 10     blockSize = 3
Mat week2_hw2_find_chess_board_corners::libCornerDetection(Mat source, int maxCorner, double qualityLevel, double minDistance, int blockSize){
	Mat gray;
	if (source.channels() != 1)
		cvtColor(source, gray, CV_BGR2GRAY);
	else
		gray = source.clone();
	Mat corner(gray.rows, gray.cols, gray.type());
	corner = 0;
	vector<Point2f> cornerPoint;
	goodFeaturesToTrack(gray, cornerPoint, maxCorner, qualityLevel, minDistance, noArray(), blockSize, true);
	// use harris detection algorithm : true

	for (int i = 0; i < cornerPoint.size(); i++){
		corner.at<uchar>(cornerPoint[i].y, cornerPoint[i].x) = 255;
	}
	gray.release();
	cornerPoint.clear();
	return corner;
}