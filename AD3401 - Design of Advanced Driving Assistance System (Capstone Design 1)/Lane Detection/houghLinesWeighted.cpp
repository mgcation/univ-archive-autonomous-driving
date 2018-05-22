#include "imageProcessing.hpp"

#define IDX(t) idx_(t, minTheta, maxTheta, thetaUnit)

int idx_(double idx, double min, double max, double unit){
	assert(min < max && min <= idx && idx <= max && unit <= max-min);
	return (idx - min) / unit + 1;
}

void ImageProcessing::houghLinesWeighted(
	Mat src,
	vector<pair<Point2i, Point2i>> dest,
	double rhoUnit,
	double thetaUnit,
	double threshold,
	double minTheta,
	double maxTheta,
	int numberOfWeight,
	double multipliedWeight)
{
	assert(thetaUnit != 0 && rhoUnit != 0 && multipliedWeight != 0 && numberOfWeight >= 1);
	assert(maxTheta > minTheta);
	assert(src.type() == CV_8UC1 && "src must be grayscale and uchar\n");

	const int rows = src.rows;
	const int cols = src.cols;
	const int MAX_RHO_IDX = sqrt(pow(rows, 2) + pow(cols, 2)) * 2;
	const int MAX_THETA_IDX = (maxTheta - minTheta) / thetaUnit + 3; // ³¶³¶ÇÏ°Ô 3 ÁÜ
	const int weightChangingPoint = rows / numberOfWeight;

	int weightLevel = 1;
	double weight = 1;
	vector<vector<double>> hough(MAX_THETA_IDX, vector<double>(MAX_RHO_IDX, 0));
	vector<double> sin_(MAX_THETA_IDX);
	vector<double> cos_(MAX_THETA_IDX);

	for (double t = minTheta; t <= maxTheta; t += thetaUnit){
		sin_[IDX(t)] = sin(t);
		cos_[IDX(t)] = cos(t);
	}

	for (double y_d = rows - 1; y_d >= 0; y_d -= weight){
		for (int x = 0; x < src.cols; x++){
			int y = y_d;

			uchar pixel = src.at<uchar>(y, x);
			// x sint + y cost = rho
			// ¹è¿­(¸Æ½º-¹Î/¼¼Å¸°£°Ý, 2*(x^2+y^2) )
			for (double t = minTheta; t <= maxTheta; t += thetaUnit){
				double r_d = y * sin_[IDX(t)] + x * cos_[IDX(t)];
				int r = r_d;
				hough[t][r] += pixel;
			}
		}
		// changing weight
		if (rows - y_d > weightLevel*weightChangingPoint){
			weight *= multipliedWeight;
			weightLevel++;
		}
	}
}
