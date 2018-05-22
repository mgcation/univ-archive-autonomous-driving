#include "utility.h"

void utility::filterBased3Vec(const Mat& source, Mat& result, const Mat& kernel){
	assert(kernel.rows == kernel.cols && kernel.rows % 2 == 1);
	assert(kernel.channels() == 1 && kernel.type() == CV_64F);
	assert(source.channels() == 3);
	assert(0 < source.rows && 0 < source.cols);

	int border = kernel.rows / 2;
	Mat ret = Mat::zeros(Size(source.rows, source.cols), source.type());
	for (int y = border; y < source.rows - border; y++){
		for (int x = border; x < source.cols - border; x++){
			Vec3d value = Vec3d(0, 0, 0);
			for (int yy = -border; yy <= border; yy++){
				for (int xx = -border; xx <= border; xx++){
					value += source.at<Vec3b>(y+yy, x+xx) * kernel.at<double>(border + yy, border + xx);
				}
			}
			if (255 < value[0]) value[0] = 255;
			if (255 < value[1]) value[1] = 255;
			if (255 < value[2]) value[2] = 255;
			if (value[0] < 0) value[0] = 0;
			if (value[1] < 0) value[1] = 0;
			if (value[2] < 0) value[2] = 0;
			ret.at<Vec3b>(y, x) = value;
		}
	}
	result.release();
	result = ret;
}

void utility::filterBasedUchar(const Mat& source, Mat& result, const Mat& kernel){
	assert(kernel.rows == kernel.cols && kernel.rows % 2 == 1);
	assert(kernel.channels() == 1 && (kernel.type() == CV_64F || kernel.type() == CV_32F));
	assert(source.channels() == 1);
	assert(0 < source.rows && 0 < source.cols);

	int border = kernel.rows / 2;
	Mat ret = Mat::zeros(Size(source.rows, source.cols), CV_8U);

	if (kernel.type() == CV_64F){
		for (int y = border; y < source.rows - border; y++){
			for (int x = border; x < source.cols - border; x++){
				double value = 0;
				for (int yy = -border; yy <= border; yy++){
					for (int xx = -border; xx <= border; xx++){
						value += source.at<uchar>(y + yy, x + xx) * kernel.at<double>(border + yy, border + xx);
					}
				}
				if (255 < value) value = 255;
				if (value < 0) value = 0;
				ret.at<uchar>(y, x) = value;
			}
		}
	}
	else {
		for (int y = border; y < source.rows - border; y++){
			for (int x = border; x < source.cols - border; x++){
				double value = 0;
				for (int yy = -border; yy <= border; yy++){
					for (int xx = -border; xx <= border; xx++){
						value += source.at<uchar>(y + yy, x + xx) * kernel.at<float>(border + yy, border + xx);
					}
				}
				if (255 < value) value = 255;
				if (value < 0) value = 0;
				ret.at<uchar>(y, x) = value;
			}
		}
	}
	result.release();
	result = ret;
}

void utility::filterBasedFloat(const Mat& source, Mat& result, const Mat& kernel, float maxVal){
	assert(kernel.rows == kernel.cols && kernel.rows % 2 == 1);
	assert(kernel.channels() == 1 && kernel.type() == CV_32F);
	assert(source.channels() == 1 && source.type() == CV_32F);
	assert(0 < source.rows && 0 < source.cols);

	int border = kernel.rows / 2;
	Mat ret = Mat::zeros(Size(source.rows, source.cols), CV_32F);
	for (int y = border; y < source.rows - border; y++){
		for (int x = border; x < source.cols - border; x++){
			float value = 0;
			for (int yy = -border; yy <= border; yy++){
				for (int xx = -border; xx <= border; xx++){
					value += source.at<float>(y + yy, x + xx) * kernel.at<float>(border + yy, border + xx);
				}
			}
			if (maxVal < value) value = maxVal;
			if (value < 0) value = 0;
			ret.at<float>(y, x) = value;
		}
	}
	result.release();
	result = ret;
}

uchar utility::calc_MSE(Mat target1, Mat target2, bool print, const char* msg){
	bool err_diff_channels = target1.channels() != target2.channels();

	if (err_diff_channels){
		printf("channel not same");
		getchar();
		exit(0);
	}
	else {
		long long err = 0;
		vector<Mat> target1_vec, target2_vec;
		int rows = target1.rows;
		int cols = target1.cols;
		split(target1, target1_vec);
		split(target2, target2_vec);
		int chs = target1_vec.size();

		for (int ch = 0; ch < chs; ch++){
			for (int y = 0; y < rows; y++){
				for (int x = 0; x < cols; x++){
					err += pow(target1_vec[ch].at<uchar>(y, x) - target2_vec[ch].at<uchar>(y, x), 2);
				}
			}
		}
		if (print){
			printf("calc_MSE : %s\n", msg);
			printf("errSum = %d\n", err);
			printf("size = y,x,ch (%d, %d, %d)\n", rows, cols, chs);
			err = err / rows / cols / chs;
			err = sqrt(err);
			printf("avg err = %d\n", err);
		}
		else {
			err = err / rows / cols / chs;
			err = sqrt(err);
		}
		return err;
	}
}

void utility::localMaxima(const Mat& source, Mat& result, Size localSize, double minimum, double allowableError){
	assert(localSize.height % 2 == 1 && localSize.width % 2 == 1);
	assert(source.channels() == 1);

	Mat source32;
	if (source.type() != CV_32F){
		source.convertTo(source32, CV_32F);
	}
	else{
		source32 = source.clone();
	}

	result = 0;
	for (int y = 0; y < source32.rows; y++){
		for (int x = 0; x < source32.cols; x++){
			float maybeMaxima = source32.at<float>(y, x);
			if (maybeMaxima > minimum){
				int yStart = y - localSize.height / 2;
				int yEnd = y + localSize.height / 2;
				int xStart = x - localSize.width / 2;
				int xEnd = x + localSize.width / 2;
				if (yStart < 0) yStart = 0;
				if (source32.rows <= yEnd) yEnd = source32.rows - 1;
				if (xStart < 0) xStart = 0;
				if (source32.cols <= xEnd) xEnd = source32.cols - 1;
				bool maxima = true;
				for (int yy = yStart; (yy <= yEnd) && maxima; yy++){
					for (int xx = xStart; (xx <= xEnd) && maxima; xx++){
						if (xx == x && yy == y)
							continue;
						if (source32.at<float>(yy, xx) > maybeMaxima+allowableError)
							maxima = false;
					}
				}
				if (maxima){
					result.at<uchar>(y, x) = maybeMaxima;
				}
			}
		}
	}
}

Mat utility::ifnot_2gray(const Mat& source){
	Mat gray;
	if (source.channels() == 3)
		cvtColor(source, gray, CV_BGR2GRAY);
	else
		gray = source.clone();
	return gray;
}

string utility::type2str(int type) {
	// https://stackoverflow.com/a/17820615/7354469
	string r;

	uchar depth = type & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (type >> CV_CN_SHIFT);

	switch (depth) {
	case CV_8U:  r = "8U"; break;
	case CV_8S:  r = "8S"; break;
	case CV_16U: r = "16U"; break;
	case CV_16S: r = "16S"; break;
	case CV_32S: r = "32S"; break;
	case CV_32F: r = "32F"; break;
	case CV_64F: r = "64F"; break;
	default:     r = "User"; break;
	}

	r += "C";
	r += (chans + '0');

	return r;
}