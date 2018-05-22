#pragma once
#include "opencv2\opencv.hpp"
#include "imageProcessing.hpp"
#include "framestream.h"
#include "utility.h"
#include <ctime>

#define RECENT_FRAME_COUNT 3
#define RECENT_FRAME_INTERVAL 3
#define TEST_FILE_IO false
#define FPS 30
#define IMSHOW_DEBUG true //with out current frame

using namespace cv;

int main(){
	int minTime = 10000;
	int maxTime = 0;
	int sumTime = 0;
	int cnt = 0;

	// washington, cordova
	FrameStream fStream(FrameStream::FS_TYPE_IMAGES, "resource/cordova/1/list.txt", "resource/cordova/1/", RECENT_FRAME_COUNT, RECENT_FRAME_INTERVAL);
	//FrameStream fStream2(FrameStream::FS_TYPE_VIDEO, "resource/sample_seq.mp4", RECENT_FRAME_COUNT);


	// print file list
	if (TEST_FILE_IO){
		printf("path : %s\n", fStream.filepath);
		for (auto a : fStream.imagesData){
			int sizeOfString = a.size();
			char* temp = (char*)malloc(sizeof(char)*sizeOfString);
			utility::vecChar2CharPtr(a, temp, sizeOfString);
			printf("%s\t", temp);
		}
	}


#pragma message("notice : image size must be same.")
	// declare initial settings
	// ROI : width(70%), height(위부터:40%지점, 아래부터:30%지점(부동영역))
	//	todo : 부동영역 계산
	// image크기는 항상 같아야 함.
	int rows = fStream.currentFrame.rows;
	int cols = fStream.currentFrame.cols;
	Point2i center(fStream.currentFrame.cols / 2, fStream.currentFrame.rows / 2);
	int roi_height = rows*(1 - 0.4 - 0.3);
	int roi_width = cols*0.7;
	Rect ROI(center.x - roi_width / 2, rows*0.4, roi_width, roi_height); //Rect ROI(center.x - 200, center.y - 50, 400, 150);
	// threshold for line nearest.
	double degree_threshold = 10;
	double rho_threshold = roi_width * 0.05;

	while (fStream.currentFrame.total() != 0){
		clock_t start = clock();


		// --------------------------
		// detection lane line app

		// gaussian_blur
		Mat roi = Mat(fStream.currentFrame, ROI);
		Mat gaussian_roi = roi.clone();
		GaussianBlur(roi, gaussian_roi, Size(7, 7), 0);

		// hsv
		Mat hsv;
		cvtColor(gaussian_roi, hsv, CV_BGR2HSV);

		// yellow and white detection
		// H : 0~179, S : 0~255, V : 0~255
		// yellow from hsv, white from otsu binarization
		Mat yellow, white;
		inRange(hsv, Scalar(11, 48, 64), Scalar(41, 255, 255), yellow);
		vector<Mat> hsv_vec;
		split(hsv, hsv_vec);
		threshold(hsv_vec[2], white, 0, 255, THRESH_BINARY | THRESH_OTSU);
		Mat yellow_white = yellow | white;

		// canny & {yellow and white}
		// canny parameter recommended {high:row = 1:2 or 1:3}
		Mat canny;
		Canny(roi, canny, 125, 375, 3);
		Mat canny_and_yellow_white = canny & yellow_white;

		// hough line detection
		Rect left_roi_rect(0, 0, roi.cols / 2, roi.rows);
		Rect right_roi_rect(roi.cols / 2, 0, roi.cols / 2, roi.rows);
		Mat canny_left_roi = Mat(canny_and_yellow_white, left_roi_rect);
		Mat canny_right_roi = Mat(canny_and_yellow_white, right_roi_rect);
		vector<Vec2f> left_lines, right_lines;
		HoughLines(canny_left_roi, left_lines, 1, DEGREE / 2, 30);
		HoughLines(canny_right_roi, right_lines, 1, DEGREE / 2, 30);

		// find first average line.
		int half_roi_cols = canny_left_roi.cols;
		Vec2f left_first_line = ImageProcessing::findFirstAverageLine(canny_left_roi, left_lines, 1.2*half_roi_cols, 0.2*half_roi_cols, 0, 45, rho_threshold, degree_threshold);
		Vec2f right_first_line = ImageProcessing::findFirstAverageLine(canny_right_roi, right_lines, -half_roi_cols*0.2, half_roi_cols*0.8, 180, 135, rho_threshold, degree_threshold);

		// interpolate loss line
		/*현재 없는 경우{
			좌우에 2개의 직선이 생길 때 직선 정보를 저장한다;
			reliability_right += 5;
			reliability_right += 5;
			}
			현재 있는 경우{
			각각 하나가 검출될 때 threshold 를 벗어나지 않는다면 움직인 각도만큼 예측한다;
			reliability_oneside += 5;
			(maximum = 10 * 2 << 4);
			저장직선정보를 갱신;
			둘다 검출 안되면 신뢰도를 줄인다;
			reliability_left /= 2;
			reliability_right /= 2;
			만약 10 이하면 0 으로 버리고 직선정보를 삭제한다.
			}*/
		Vec2f left_saved_line(0, 0), right_saved_line(0, 0);
		bool need_interpolate = left_first_line == Vec2f(0, 0) || right_first_line == Vec2f(0, 0);
		unsigned int reliability_right = 0;
		unsigned int reliability_left = 0;
		unsigned int reliability_max = 2 << 3; // 3 frame 의 오류까지 신뢰함
		if (left_saved_line == Vec2f(0, 0) && right_saved_line == Vec2f(0, 0)){
			if (!need_interpolate){
				reliability_left = 2;
				reliability_right = 2;
				left_saved_line = left_first_line;
				right_saved_line = right_first_line;
			}
		}
		else{ // saved line exist.
			double radian_threshold = degree_threshold * DEGREE;
			double left_radian_diff = left_first_line[1] - left_saved_line[1];
			double left_rho_diff = left_first_line[0] - left_saved_line[0];
			double right_radian_diff = right_first_line[1] - right_saved_line[1];
			double right_rho_diff = right_first_line[0] - right_saved_line[0];
			bool left_line_overflow = radian_threshold < abs(left_radian_diff) || rho_threshold < abs(left_rho_diff);
			bool right_line_overflow = radian_threshold < abs(right_radian_diff) || rho_threshold < abs(right_rho_diff);

			if (!need_interpolate){ // two line detected.
				if (!left_line_overflow && !right_line_overflow){
					reliability_left+=2;
					reliability_right+=2;
					if (reliability_max < reliability_left)
						reliability_left = reliability_max;
					if (reliability_max < reliability_right)
						reliability_right = reliability_max;

					left_saved_line = left_first_line;
					right_saved_line = right_first_line;
				}
				else if (!left_line_overflow){
					reliability_left+=2;
					reliability_right++;
					if (reliability_max < reliability_left)
						reliability_left = reliability_max;
					if (reliability_max < reliability_right)
						reliability_right = reliability_max;

					left_saved_line = left_first_line;
					right_saved_line[0] = right_first_line[0] + left_rho_diff;
					right_saved_line[1] = right_first_line[1] + left_radian_diff;
				}
				else if (!right_line_overflow){
					reliability_left++;
					reliability_right+=2;
					if (reliability_max < reliability_left)
						reliability_left = reliability_max;
					if (reliability_max < reliability_right)
						reliability_right = reliability_max;

					left_saved_line[0] = left_first_line[0] + right_rho_diff;
					left_saved_line[1] = left_first_line[1] + right_radian_diff;
					right_saved_line = right_first_line;
				}
				else{
					reliability_left /= 2;
					reliability_right /= 2;

					if (reliability_left < 2 && reliability_right < 2){
						left_saved_line = Vec2f(0, 0);
						right_saved_line = Vec2f(0, 0);
					}
					else {
						left_saved_line = left_first_line;
						right_saved_line = right_first_line;
					}
				}
			}
			else if (left_first_line != Vec2f(0, 0)){
				if (!left_line_overflow){
					reliability_left+=2;
					reliability_right++;
					if (reliability_max < reliability_left)
						reliability_left = reliability_max;
					if (reliability_max < reliability_right)
						reliability_right = reliability_max;

					left_saved_line = left_first_line;
					right_saved_line[0] = right_first_line[0] + left_rho_diff;
					right_saved_line[1] = right_first_line[1] + left_radian_diff;
				}
				else{
					reliability_left /= 2;
					reliability_right /= 2;
					if (reliability_left < 2){
						left_saved_line = left_first_line;
					}
				}
			}
			else if (right_first_line != Vec2f(0, 0)){
				if (!right_line_overflow){
					reliability_left++;
					reliability_right+=2;
					if (reliability_max < reliability_left)
						reliability_left = reliability_max;
					if (reliability_max < reliability_right)
						reliability_right = reliability_max;

					left_saved_line[0] = left_first_line[0] + right_rho_diff;
					left_saved_line[1] = left_first_line[1] + right_radian_diff;
					right_saved_line = right_first_line;
				}
				else{
					reliability_left /= 2;
					reliability_right /= 2;
					if (reliability_right < 2){
						right_saved_line = right_first_line;
					}
				}
			}
			else { // 0 line detected.
				reliability_left /= 2;
				reliability_right /= 2;
				if (reliability_left < 2){
					left_saved_line = Vec2f(0, 0);
				}
				if (reliability_right < 2){
					right_saved_line = Vec2f(0, 0);
				}
			}
		}

		// draw lines
		if ((left_saved_line == Vec2f(0, 0)) || (right_saved_line == Vec2f(0, 0)));
		// do not draw.
		else{
			Mat left_roi = Mat(roi, left_roi_rect);
			Mat right_roi = Mat(roi, right_roi_rect);
			ImageProcessing::drawLines(left_roi, vector<Vec2f>(1, left_saved_line), Scalar(0, 255, 0), 3);
			ImageProcessing::drawLines(right_roi, vector<Vec2f>(1, right_saved_line), Scalar(0, 255, 0), 3);
		}

		// imshow
		if (IMSHOW_DEBUG){
			// debug for fitted line
			// draw all of line, then draw fitted line.
			// line : red
			// fitted line : green
			Mat debug_roi = roi.clone();
			Mat debug_left_roi = Mat(debug_roi, left_roi_rect);
			Mat debug_right_roi = Mat(debug_roi, right_roi_rect);
			ImageProcessing::drawLines(debug_left_roi, left_lines, Scalar(0, 0, 255), 1);
			ImageProcessing::drawLines(debug_right_roi, right_lines, Scalar(0, 0, 255), 1);
			ImageProcessing::drawLines(debug_left_roi, vector<Vec2f>(1, left_saved_line), Scalar(0, 255, 0), 4);
			ImageProcessing::drawLines(debug_right_roi, vector<Vec2f>(1, right_saved_line), Scalar(0, 255, 0), 4);
			ImageProcessing::drawLines(debug_left_roi, vector<Vec2f>(1, left_first_line), Scalar(255, 0, 0), 2);
			ImageProcessing::drawLines(debug_right_roi, vector<Vec2f>(1, right_first_line), Scalar(255, 0, 0), 2);
			imshow("debug : roi line clustering", debug_roi);

			// show for debug
			imshow("gaussian_roi", gaussian_roi);
			imshow("yellow_white", yellow_white);
			imshow("canny", canny);
			imshow("canny&yellow_white", canny_and_yellow_white);
			imshow("canny - canny&yellow_white", canny - canny_and_yellow_white);

			// show recent frame image
			int i = RECENT_FRAME_COUNT;
			for (auto f : fStream.recentFrames){
				char buf[2];
				String a = "recent";
				_itoa(--i, buf, 10);
				a.append(buf);
				resize(f, f, Size(), 0.5, 0.5);
				imshow(a, f);
			}
		}
		imshow("observation", fStream.currentFrame);

		// detection lane line app --- end
		// --------------------------


		fStream.nextFrame();
		clock_t end = clock();

		int period = (end - start);
		int mSPF = 1000 / FPS - period; //ms per frame for wait
		if (mSPF < 1){
			printf("frame loss : %d ms\n", -mSPF);
			waitKey(1);
		}
		else{
			printf("wait %d ms\n", mSPF);
			waitKey(mSPF);
		}
		minTime = period < minTime ? period : minTime;
		maxTime = period > maxTime ? period : maxTime;
		sumTime += period;
		cnt++;
	}

	printf("maxTime : %d ms \n", maxTime);
	printf("minTime : %d ms \n", minTime);
	printf("averageTime : %d ms (FPS : %.2f)\n", sumTime / cnt, 1000.0 / sumTime * cnt);
	system("pause");
	return 0;
}
