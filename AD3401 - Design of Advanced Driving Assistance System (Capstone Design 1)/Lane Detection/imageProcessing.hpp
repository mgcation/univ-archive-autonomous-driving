#pragma once
#include "opencv2\opencv.hpp"
#include "utility.h"
#include <numeric>

using namespace cv;
using namespace std;

namespace ImageProcessing{
	//! src must bgr or gray
	// 각도는 점점 벌어진다. 차선검출용 hough line.
	void houghLinesOptimized(const Mat& src, vector<Vec2f>& lines, double rho, double theta, int threshold, double minTheta, double maxTheta, double minRho = -1, double maxRho = -1);
	Point2f findVanishingPoint(Point2f line1Point1, Point2f line1Point2, Point2f line2Point1, Point2f line2Point2);
	//! dest return n lines[ vector<pair<startPoint, endPoint>> ] which is generalized lines of each lines group.
	void houghLinesWeighted(Mat src, vector<pair<Point2i, Point2i>> dest, double rhoUnit, double thetaUnit, double threshold, double minTheta = -2 * DEGREE, double maxTheta = 2 * DEGREE, int numberOfWeight = 4, double multipliedWeight = 2.0);

	void drawLines(Mat& src, vector<Vec2f> lines, Scalar color, int thickness);
	//! use reverse when calc left image.
	vector<Vec2f> meanShiftRhoThetaClustering(vector<Vec2f> lines, bool reverse);
	//! rhoThreshold recommended 0.05 * src.cols() (where src is roi)
	//  degreeThreshold recommended 10 degree.
	//  first fitted line does not exist where return Vec2f(0, 0).
	Vec2f findFirstAverageLine(const Mat& src, vector<Vec2f>& lines, double startRho, double endRho, double startDegree, double endDegree, double rhoThreshold /*=src.cols*0.05*/, double degreeThreshold = 10);
}