#include "transform.h"

Mat proj_2::getTransitionMatrix(int x, int y){
	return (Mat_<double>(3, 3) <<
		1, 0, x, 
		0, 1, y, 
		0, 0, 1);
}

Mat proj_2::getRotationMatrix(double degree){
	double rad = degree / 180 * M_PI;
	double sin_ = sin(rad);
	double cos_ = cos(rad);

	return (Mat_<double>(3, 3) << 
		cos_, sin_, 0, 
		-sin_, cos_, 0,
		0, 0, 1);
}

Mat proj_2::getRotationMatrix(double degree, Point2f center){
	double rad = degree / 180 * M_PI;
	double sin_ = sin(rad);
	double cos_ = cos(rad);

	return (Mat_<double>(3, 3) <<
		cos_, sin_, (1 - cos_)*center.x - sin_*center.y,
		-sin_, cos_, sin_*center.x + (1 - cos_)*center.y,
		0, 0, 1);
}

Mat proj_2::getScalingMatrix(double xScale, double yScale){
	return (Mat_<double>(3,3) << 
		xScale, 0, 0,
		0, yScale, 0,
		0, 0, 1);
}

void proj_2::backwardMapping(const Mat& source, Mat& dest, const Mat& transform, const int INTER){
	assert(source.type() == CV_8UC3);
	assert(transform.type() == CV_64FC1);
	assert(transform.cols == 3 && (transform.rows == 2 || transform.rows == 3));
	assert(INTER == MY_INTER_BILINEAR || INTER == MY_INTER_NN);

	Mat transform3x3;
	if (transform.rows == 3){
		transform3x3 = transform;
	}
	else { // == 2
		printf("test rect : %d, %d\n", transform.rows, transform.cols);
		transform3x3 = Mat(transform3x3, Rect(0, 0, 3, 2));
		printf("test rect : %d, %d\n", transform3x3.rows, transform3x3.cols);
	}

	dest.release();
	dest = Mat(source.rows, source.cols, CV_8UC3);
	Mat invertTransform;
	invert(transform3x3, invertTransform);

	switch (INTER){
	case MY_INTER_NN: {
		for (int y = 0; y < dest.rows; y++){
			for (int x = 0; x < dest.cols; x++){
				int xx = int(round(invertTransform.at<double>(0, 0) * x + invertTransform.at<double>(0, 1) * y + invertTransform.at<double>(0, 2)));
				int yy = int(round(invertTransform.at<double>(1, 0) * x + invertTransform.at<double>(1, 1) * y + invertTransform.at<double>(1, 2)));
				if (0 <= yy && yy < dest.rows && 0 <= xx && xx < dest.cols)
					dest.at<Vec3b>(y, x) = source.at<Vec3b>(yy, xx);
			}
		}
		break;
	}
	case MY_INTER_BILINEAR: {
		for (int y = 0; y < dest.rows; y++){
			for (int x = 0; x < dest.cols; x++){
				double x_b = invertTransform.at<double>(0, 0) * x + invertTransform.at<double>(0, 1) * y + invertTransform.at<double>(0, 2);
				double y_a = invertTransform.at<double>(1, 0) * x + invertTransform.at<double>(1, 1) * y + invertTransform.at<double>(1, 2);

				int xx = int(x_b);
				int yy = int(y_a);
				int xx_1 = xx + 1;
				int yy_1 = yy + 1;

				if (xx < 0 || dest.cols - 1 <= xx || yy < 0 || dest.rows - 1 <= yy)
					continue;

				double a = y_a - yy;
				double b = x_b - xx;

				dest.at<Vec3b>(y, x) =
					(1 - b) * ((1 - a) * source.at<Vec3b>(yy, xx) + a * source.at<Vec3b>(yy_1, xx))
					+
					b * ((1 - a) * source.at<Vec3b>(yy, xx_1) + a * source.at<Vec3b>(yy_1, xx_1));
			}
		}
	}
	}
	transform3x3.release();
	invertTransform.release();
}


void library::backwardMapping(const Mat& source, Mat& dest, const Mat& transform, const int INTER){
	assert(source.type() == CV_8UC3);
	assert(transform.type() == CV_64FC1);
	assert(transform.cols == 3 && (transform.rows == 2 || transform.rows == 3));
	assert(INTER == LIB_INTER_BILINEAR || INTER == LIB_INTER_NN);

	if (transform.rows == 2){
		warpAffine(source, dest, transform, source.size(), INTER);
	}
	else {
		warpPerspective(source, dest, transform, source.size(), INTER);
	}
}