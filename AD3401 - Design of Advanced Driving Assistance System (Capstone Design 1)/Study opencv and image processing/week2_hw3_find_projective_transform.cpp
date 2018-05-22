#include "week2_hw3_find_projective_transform.h"
#include "week2_hw2_find_chess_board_corners.h"
//#define debug true

/*
Chess-board 모서리를 가지고 projective 변환 매트릭스를 구한다.
(직사각형이 되도록 복원도 해본다.)
(너무 복잡하게 생각하지 말고 간단하게 조금만 휘어졌다고 생각하고 구현한다. projective 변환 매트릭스를 구하는데 의의가 있다.)
*/

void testPrint(char* text){
#ifdef debug
	if (debug){
		std::cout << text;
	}
#endif
}

vector<vector<Point2i>> harris2corners(Mat arg, bool printCorners = true){
	Mat harris = arg.clone();
	vector<Point2i> corners;

	for (int y = 0; y < harris.rows; y++){
		for (int x = 0; x < harris.cols; x++){
			uchar& dot = ucharAt(harris, y, x);
			if (dot == 0)
				continue;
			corners.push_back(Point2i(x, y));
		}
	}

	// corner 들 중 가장 가까운 코너를 구함.
	unsigned int minDistance = sqrt(
		pow((corners[0].x - corners[1].x), 2) +
		pow((corners[0].y - corners[1].y), 2)
		);;
	Point2i minPoint1 = corners[0];
	Point2i minPoint2 = corners[1];
	for (int i = 0; i < corners.size() - 1; i++){
		for (int j = i + 1; j < corners.size(); j++){
			unsigned int d = sqrt(
				pow((corners[i].x - corners[j].x), 2) +
				pow((corners[i].y - corners[j].y), 2)
				);
			if (d < minDistance){
				minDistance = d;
				minPoint1 = corners[i];
				minPoint2 = corners[j];
			}
		}
	}

	// 가장 가까운 코너와의 거리 minDistance의 반만큼 크기를 가지는 커널을 갖고 첫번째 코너와 같은 행에 있는 코너들을 구한다.
	// 이어서 두번째 행 한 점도 찾고 똑같은 방법으로 두번째 행의 코너들을 구한다. ... 반복.
	vector<vector<Point2i>> ret;
	unsigned int seekSize = minDistance / 2;

	Point2i firstCorner = corners[0];
	ucharAt(harris, firstCorner.y, firstCorner.x) = 0;
	bool firstCornerFound = true;

	while (firstCornerFound){
		vector<Point2i> rowCorners;

		//왼쪽으로 seek
		int leftSeekCount = firstCorner.x / seekSize;
		for (int xStep = 0; xStep < leftSeekCount; xStep++){
			for (int y = firstCorner.y - seekSize / 2; y < int(firstCorner.y + seekSize / 2); y++){
				if (y < 0)	y = 0;
				if (harris.rows <= y)	break;
				for (int x = xStep*seekSize; x<(xStep + 1)*seekSize; x++){
					uchar& corner = ucharAt(harris, y, x);
					if (corner == 255){
						corner = 0;
						rowCorners.push_back(Point2i(x, y));
					}
				}
			}
		}

		rowCorners.push_back(firstCorner);

		//오른쪽으로 seek
		int rightSeekCount = (harris.cols - firstCorner.x - 1) / seekSize;
		int seekStartX = harris.cols - rightSeekCount * seekSize;
		for (int xStep = 0; xStep < rightSeekCount; xStep++){
			for (int y = firstCorner.y - seekSize / 2; y < int(firstCorner.y + seekSize / 2); y++){
				if (y < 0)	y = 0;
				if (harris.rows <= y)	break;
				for (int x = seekStartX + xStep*seekSize; x < seekStartX + (xStep + 1)*seekSize; x++){
					uchar& corner = ucharAt(harris, y, x);
					if (corner == 255){
						corner = 0;
						rowCorners.push_back(Point2i(x, y));
					}
				}
			}
		}

		ret.push_back(rowCorners);

		firstCornerFound = false;
		for (int i = 0; i < corners.size(); i++){
			uchar& cornerVal = ucharAt(harris, corners[i].y, corners[i].x);
			if (cornerVal != 0){
				firstCornerFound = true;
				firstCorner = corners[i];
				cornerVal = 0;
				break;
			}
		}
	}

	if (printCorners){
		puts("-----corners-----");
		for (auto a : ret){
			for (auto b : a){
				std::cout << "(" << b.x << ", " << b.y << "),\t";
			}
			std::cout << std::endl;
		}
	}

	return ret;
}

Mat getProjectionMatrix(Mat source, vector<vector<Point2i>> corners, bool printMatrix=true){
	Mat projection(3,3,CV_64FC1);

	Point2i dot[4]; // lt, rt, lb, rb
	dot[0] = corners[0][0];
	dot[1] = corners[0][corners[0].size()-1];
	dot[2] = corners[corners.size()-1][0];
	dot[3] = corners[corners.size() - 1][corners[corners.size() - 1].size() - 1];

	int paddingRow = 25;
	int paddingCol = 100;
	Point2i resultDot[4];
	resultDot[0] = Point2i(paddingCol, paddingRow);
	resultDot[1] = Point2i(source.cols - paddingCol, paddingRow);
	resultDot[2] = Point2i(paddingCol, source.rows - paddingRow);
	resultDot[3] = Point2i(source.cols - paddingCol, source.rows - paddingRow);

	Mat solvingForm = (Mat)(Mat_<double>(8, 8) <<
		dot[0].x, dot[0].y, 1, 0, 0, 0, -resultDot[0].x * dot[0].x, -resultDot[0].x*dot[0].y,
		dot[1].x, dot[1].y, 1, 0, 0, 0, -resultDot[1].x * dot[1].x, -resultDot[1].x*dot[1].y,
		dot[2].x, dot[2].y, 1, 0, 0, 0, -resultDot[2].x * dot[2].x, -resultDot[2].x*dot[2].y,
		dot[3].x, dot[3].y, 1, 0, 0, 0, -resultDot[3].x * dot[3].x, -resultDot[3].x*dot[3].y,
		0, 0, 0, dot[0].x, dot[0].y, 1, -dot[0].x * resultDot[0].y, -dot[0].y*resultDot[0].y,
		0, 0, 0, dot[1].x, dot[1].y, 1, -dot[1].x * resultDot[1].y, -dot[1].y*resultDot[1].y,
		0, 0, 0, dot[2].x, dot[2].y, 1, -dot[2].x * resultDot[2].y, -dot[2].y*resultDot[2].y,
		0, 0, 0, dot[3].x, dot[3].y, 1, -dot[3].x * resultDot[3].y, -dot[3].y*resultDot[3].y
		);
	Mat resultDotMat = (Mat)(Mat_<double>(8, 1) << 
		resultDot[0].x, resultDot[1].x, resultDot[2].x, resultDot[3].x,
		resultDot[0].y, resultDot[1].y, resultDot[2].y, resultDot[3].y
		);
	invert(solvingForm, solvingForm);
	Mat solved = solvingForm * resultDotMat;
	
	int i = 0;
	for (int y = 0; y < 3; y++){
		for (int x = 0; x < 3; x++){
			if (i == 8)
				break;
			projection.at<double>(y, x) = solved.at<double>(i++, 0);
		}
	}
	projection.at<double>(2, 2) = 1;

	if (printMatrix){
		puts("-----corner for find projectionMatrix-----");
		for (int i = 0; i < 4; i++){
			printf("(%d,%d)   \t", dot[i].x, dot[i].y);
		}
		puts("");
		puts("-----dot(estimate) for find projectionMatrix-----");
		for (int i = 0; i < 4; i++){
			printf("(%d,%d)\t", resultDot[i].x, resultDot[i].y);
		}
		puts("");
		puts("-----projectionMatrix-----");
		for (int y = 0; y < projection.rows; y++){
			for (int x = 0; x < projection.cols; x++){
				printf("%.2f\t", projection.at<double>(y, x));
			}
			puts("");
		}
	}
	return projection;
}

int week2_hw3_find_projective_transform::run()
{
	Mat source, harris;
	my_imread(source, "resource/chess board.jpg");
	harris = week2_hw2_find_chess_board_corners::libCornerDetection(source, 10000, 0.05, 20, 3);

	Mat bin_harris;
	threshold(harris, bin_harris, 0, 255, THRESH_BINARY | THRESH_OTSU);

	// corner 들을 배열에 넣음.
	vector<vector<Point2i>> corners;
	corners = harris2corners(bin_harris);

	//find projection matrix
	Mat proj = getProjectionMatrix(source, corners);
	Mat topView;
	warpPerspective(source, topView, proj, Size(source.cols, source.rows));

	imshow("source", source);
	imshow("harris detector", harris);
	imshow("bin_harris", bin_harris);
	imshow("topView", topView);
	waitKey(0);
	return 0;
}
