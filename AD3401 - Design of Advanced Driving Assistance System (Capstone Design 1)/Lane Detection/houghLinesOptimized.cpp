#include "imageProcessing.hpp"
#include <ctime>

void leftImage(const Mat& src, vector<Vec2f>& lines, int threshold){
	// 왼쪽 : rho(cols~0) theta(90~135) :: rho(cols*1.2 ~ 0)
	clock_t start = clock();
	int minRho = 0;
	int maxRho = src.cols*1.2;
	vector<double> sin_(46);
	vector<double> cos_(46);
	vector<vector<double>> hough(46, vector<double>(maxRho + 1, 0));

	int offset = 90;
	for (int t = 90; t <= 135; t++){
		sin_[t-offset] = sin(t);
		cos_[t-offset] = cos(t);
	}

	clock_t center = clock();

	for (int y = 0; y < src.rows; y++){
		for (int x = 0; x < src.cols; x++){
			uchar pixel = src.at<uchar>(y, x);
			for (int t = 0; t <= 45; t++){
				int rho = x * cos_[t] + y * sin_[t];
				if (minRho <= rho && rho <= maxRho){
					hough[t][rho - minRho] += pixel / 255.0;
				}
			}
		}
	}

	clock_t end = clock();
	printf("(%d %d %d)총 %d 수행 : 중간 %d 기말 %d \n", src.rows, src.cols, 45, src.rows*src.cols*45, center - start, end - center);
}

void rightImage(const Mat& src, vector<Vec2f>& lines, int threshold){
	// 오른쪽 : rho(0~cols) theta(90~45) :: rho(-cols*0.2~cols)
	int minRho = src.cols * -0.2;
	int maxRho = src.cols;
	vector<double> sin_(46);
	vector<double> cos_(46);
	vector<vector<double>> hough(46, vector<double>(maxRho - minRho + 1, 0));

	int offset = 45;
	for (int t = 45; t <= 90; t++){
		sin_[t-offset] = sin(t);
		cos_[t=offset] = cos(t);
	}

	/*for (int y = 0; y < src.rows; y++){
		for (int x = 0; x < src.cols; x++){
			uchar pixel = src.at<uchar>(y, x);
			for (int t = 0; t <= 45; t++){
				int rho = x * cos_[t] + y * sin_[t];
				if (minRho <= rho && rho <= maxRho){
					hough[t][rho - minRho] += pixel/255.0;
				}
			}
		}
	}*/

	/*for (int rho = 0; rho <= maxRho-minRho; rho++){
		for (int t = 0; t <= 45; t++){
			printf("!%.3lf ", hough[t][rho]);
		}
		puts("");
	}*/
}

void ImageProcessing::houghLinesOptimized(const Mat& src, vector<Vec2f>& lines, double rho, double theta, int threshold, double startTheta, double endTheta, double startRho, double endRho){
	double* minTheta = startTheta < endTheta ? &startTheta : &endTheta;
	double* maxTheta = startTheta < endTheta ? &endTheta : &startTheta;
	double* minRho = startRho < endRho ? &startRho : &endRho;
	double* maxRho = startRho < endRho ? &endRho : &startRho;
	assert(src.type() == CV_8UC1 || src.type() == CV_8UC3);
	if (*minTheta == -1)	*minTheta = 0;
	if (*maxTheta == -1)	*maxTheta = 180;
	if (180 < *maxTheta - *minTheta)
		*maxTheta -= int(*maxTheta - *minTheta) / 180 * 180;

	Mat gray;
	if (src.type() == CV_8UC3)
		cvtColor(src, gray, CV_BGR2GRAY);
	else
		gray = src;

	if (startTheta < endTheta)
		leftImage(gray, lines, threshold);
	else
		rightImage(gray, lines, threshold);
}