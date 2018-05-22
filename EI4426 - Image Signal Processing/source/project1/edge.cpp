#include "edge.h"

void proj_1::prewitt_detector(const Mat& source, Mat& result){
	Mat gray;
	if (source.channels() > 1){
		cvtColor(source, gray, COLOR_BGR2GRAY);
	}
	else{
		gray = source;
	}

	Mat prewitt_kernel_horizontal = (Mat_<double>(3, 3) << -1, -1, -1, 0, 0, 0, 1, 1, 1);
	Mat prewitt_kernel_vertical = (Mat_<double>(3, 3) << -1, 0, 1, -1, 0, 1, -1, 0, 1);
	int rows = source.rows;
	int cols = source.cols;

	Mat prewitt_horizontal, prewitt_vertical;
	filterBasedUchar(source, prewitt_horizontal, prewitt_kernel_horizontal);
	filterBasedUchar(source, prewitt_vertical, prewitt_kernel_vertical);

	prewitt_horizontal.convertTo(prewitt_horizontal, CV_64F);
	prewitt_vertical.convertTo(prewitt_vertical, CV_64F);
	result = prewitt_horizontal.mul(prewitt_horizontal) + prewitt_vertical.mul(prewitt_vertical);
	cv::sqrt(result, result);
	result.convertTo(result, CV_8U);
}
void proj_1::sobel_detector(const Mat& source, Mat& result){
	Mat gray;
	if (source.channels() > 1){
		cvtColor(source, gray, COLOR_BGR2GRAY);
	}
	else{
		gray = source;
	}
	
	Mat sobel_kernel_horizontal = (Mat_<double>(3, 3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);
	Mat sobel_kernel_vertical = (Mat_<double>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
	int rows = source.rows;
	int cols = source.cols;

	Mat sobel_horizontal, sobel_vertical;
	filterBasedUchar(source, sobel_horizontal, sobel_kernel_horizontal);
	filterBasedUchar(source, sobel_vertical, sobel_kernel_vertical);

	sobel_horizontal.convertTo(sobel_horizontal, CV_64F);
	sobel_vertical.convertTo(sobel_vertical, CV_64F);
	result = sobel_horizontal.mul(sobel_horizontal) + sobel_vertical.mul(sobel_vertical);
	cv::sqrt(result, result);
	result.convertTo(result, CV_8U);
}
void proj_1::LoG_detector(const Mat& source, Mat& result, int kernelSize){
	double sigma = (double)kernelSize / 5.3;
	Mat kernel(kernelSize, kernelSize, CV_64F);
	int halfSize = (int)kernelSize / 2;
	for (int y = 0; y < halfSize + 1; y++){
		for (int x = 0; x < halfSize + 1; x++){
			int xx = x - halfSize;
			int yy = y - halfSize;
			double pow2xx = pow(xx, 2);
			double pow2yy = pow(yy, 2);
			double pow2sigma = pow(sigma, 2);
			double pow4sigma = pow(sigma, 4);
			kernel.at<double>(y, x)
				= kernel.at<double>(kernelSize - y - 1, x)
				= kernel.at<double>(y, kernelSize - x - 1)
				= kernel.at<double>(kernelSize - y - 1, kernelSize - x - 1)
				= (pow2xx + pow2yy - 2 * pow2sigma) / pow4sigma * exp(-(pow2xx + pow2yy) / (2 * pow2sigma));
		}
	}

	Mat gray;
	if (source.channels() == 3)
		cvtColor(source, gray, CV_BGR2GRAY);
	else
		gray = source;

	Mat logResult;
	int border = kernelSize / 2;
	logResult = Mat::zeros(Size(gray.rows, gray.cols), CV_32S);
	for (int y = border; y < source.rows - border; y++){
		for (int x = border; x < source.cols - border; x++){
			double value = 0;
			for (int yy = -border; yy <= border; yy++){
				for (int xx = -border; xx <= border; xx++){
					value += gray.at<uchar>(y + yy, x + xx) * kernel.at<double>(border + yy, border + xx);
				}
			}
			logResult.at<int>(y, x) = value;
		}
	}
	imshow("before 0 connection detect * 1000", logResult * 1000);

	result = Mat::zeros(logResult.size(), CV_8U);
	for (int y = 1; y < logResult.rows-1; y++){
		for (int x = 1; x < logResult.cols-1; x++){
			if (logResult.at<int>(y - 1, x - 1) * logResult.at<int>(y + 1, x + 1) < 0){
				result.at<uchar>(y, x) = logResult.at<int>(y,x);
				continue;
			}
			else if (logResult.at<int>(y - 1, x) * logResult.at<int>(y + 1, x) < 0){
				result.at<uchar>(y, x) = logResult.at<int>(y, x);
				continue;
			}
			else if (logResult.at<int>(y - 1, x + 1) * logResult.at<int>(y + 1, x - 1) < 0){
				result.at<uchar>(y, x) = logResult.at<int>(y, x);
				continue;
			}
			else if (logResult.at<int>(y, x + 1) * logResult.at<int>(y, x - 1) < 0){
				result.at<uchar>(y, x) = logResult.at<int>(y, x);
			}
		}
	}
	imshow("before local maxima group", result);
	localMaxima(result, result, Size(3, 3), 100, 30);
	result.setTo(255, result > 100);
}

void library::sobel_detector(const Mat& source, Mat& result){
	Mat ret1,ret2;
	Sobel(source, ret1, source.depth(), 0, 1);
	Sobel(source, ret2, source.depth(), 1, 0);
	ret1.convertTo(ret1, CV_64F);
	ret2.convertTo(ret2, CV_64F);
	result = ret1.mul(ret1) + ret2.mul(ret2);
	cv::sqrt(result, result);
	result.convertTo(result, CV_8U);
	Sobel(source, ret2, source.depth(), 1, 1);
}

void library::LoG_detector(const Mat& source, Mat& result, int kernelSize){
	GaussianBlur(source, result, Size(3, 3), 0.5);
	Laplacian(result, result, source.depth(), kernelSize);
}