#include "denoising.h"
#include "utility.h"
#include <algorithm>

using namespace cv;
using namespace utility;

void proj_1::median_filter(const Mat& source, Mat& result, int kernelSize, bool enhancedToImpulseImage){
	assert(0 < source.rows && 0 < source.cols);
	assert(1 < kernelSize && kernelSize % 2 == 1);

	result = source.clone();
	int border = kernelSize / 2;
	vector<uchar> nearPixel;
	vector<Vec3b> nearCPixel;
	vector<int> nearCPixelSum;
	vector<int> copy;
	int medianPosition = kernelSize*kernelSize / 2;

	for (int y = border; y < source.rows - border; y++){
		for (int x = border; x < source.cols - border; x++){
			if (enhancedToImpulseImage){
				if (source.channels() == 1){
					if (source.at<uchar>(y, x) != 0 && source.at<uchar>(y, x) != 255)
						continue;
				}
				else{
					if (source.at<Vec3b>(y, x) != Vec3b(0,0,0) && source.at<Vec3b>(y, x) != Vec3b(255,255,255))
						continue;
				}
			}
			nearPixel.clear();
			nearCPixel.clear();
			nearCPixelSum.clear();
			copy.clear();
			for (int yy = -border; yy <= border; yy++){
				for (int xx = -border; xx <= border; xx++){
					if (source.channels() == 1){
						nearPixel.push_back(source.at<uchar>(y + yy, x + xx));
					}
					else{
						Vec3b dot = source.at<Vec3b>(y + yy, x + xx);
						nearCPixelSum.push_back(dot[0] + dot[1] + dot[2]);
						nearCPixel.push_back(dot);
					}
				}
			}
			if (source.channels() == 1){
				std::sort(nearPixel.begin(), nearPixel.end());
				result.at<uchar>(y, x) = nearPixel[medianPosition];
			}
			else{
				copy = nearCPixelSum;
				std::nth_element(nearCPixelSum.begin(), nearCPixelSum.begin() + medianPosition, nearCPixelSum.end());
				auto medianIterIdx = std::find(copy.begin(), copy.end(), nearCPixelSum[medianPosition]);
				int medianIdx = medianIterIdx - copy.begin();
				result.at<Vec3b>(y, x) = nearCPixel[medianIdx];
			}
		}
	}
}
void proj_1::gaussian_filter(const Mat& source, Mat& result, int kernelSize, double sigma){
	assert(1 < kernelSize&& kernelSize % 2 == 1);

	Mat colorSource;
	if (source.channels() == 3){
		colorSource = source;
	}
	else {
		cvtColor(source, colorSource, CV_GRAY2BGR);
	}

	Mat gaussianKernel(kernelSize, kernelSize, CV_64F);
	double M_PI = 3.14159265358979323846;
	int kernelHalf = (int)kernelSize / 2;

	double pow2sigma = pow(sigma, 2);
	for (int y = 0; y < kernelHalf + 1; y++){
		for (int x = 0; x < kernelHalf + 1; x++){
			int xx = x - kernelHalf;
			int yy = y - kernelHalf;
			double pow2xx = pow(xx, 2);
			double pow2yy = pow(yy, 2);
			double value = 1 / (2 * M_PI * pow2sigma) * exp(-1 * (pow2yy + pow2xx) / (2 * pow2sigma));
			gaussianKernel.at<double>(y, x)
				= gaussianKernel.at<double>(kernelSize - y - 1, x)
				= gaussianKernel.at<double>(y, kernelSize - x - 1)
				= gaussianKernel.at<double>(kernelSize - y - 1, kernelSize - x - 1)
				= value;
		}
	}

	double sum = 0;
	for (int y = 0; y < kernelSize; y++){
		for (int x = 0; x < kernelSize; x++){
			sum +=gaussianKernel.at<double>(y, x);
		}
	}
	gaussianKernel /= sum;

	filterBased3Vec(colorSource, result, gaussianKernel);
}
void proj_1::bilateral_filter(const Mat& source, Mat& result, int kernelSize, double sigmaColor, double sigmaSpace){
	assert(1 < kernelSize&& kernelSize % 2 == 1);
	assert(source.channels() == 3);
	assert(0 < source.rows && 0 < source.cols);


	Mat colorSource;
	if (source.channels() == 3){
		colorSource = source;
	}
	else {
		cvtColor(source, colorSource, CV_GRAY2BGR);
	}


	Mat gaussianKernel(kernelSize, kernelSize, CV_64F);
	double M_PI = 3.14159265358979323846;
	int kernelHalf = (int)kernelSize / 2;

	double pow2sigmaSpace = pow(sigmaSpace, 2);
	for (int y = 0; y < kernelHalf + 1; y++){
		for (int x = 0; x < kernelHalf + 1; x++){
			int xx = x - kernelHalf;
			int yy = y - kernelHalf;
			double pow2xx = pow(xx, 2);
			double pow2yy = pow(yy, 2);
			double value = 1 / (2 * M_PI * pow2sigmaSpace) * exp(-1 * (pow2yy + pow2xx) / (2 * pow2sigmaSpace));
			gaussianKernel.at<double>(y, x)
				= gaussianKernel.at<double>(kernelSize - y - 1, x)
				= gaussianKernel.at<double>(y, kernelSize - x - 1)
				= gaussianKernel.at<double>(kernelSize - y - 1, kernelSize - x - 1)
				= value;
		}
	}

	double sum = 0;
	for (int j = 0; j < kernelSize; j++){
		for (int i = 0; i < kernelSize; i++){
			sum += gaussianKernel.at<double>(i, j);
		}
	}
	gaussianKernel /= sum;

	double pow2sigmaColor = pow(sigmaColor, 2);
	double colorDiffGaussWeight[256] = { 0, };
	for (int i = 0; i < 256; i++){
		colorDiffGaussWeight[i] = exp(-pow(i, 2) / (2 * pow2sigmaColor));
	}


	int border = kernelSize / 2;
	result = Mat::zeros(Size(colorSource.rows, colorSource.cols), colorSource.type());
	Mat colorKernel(Size(kernelSize, kernelSize), CV_64F);
	Mat bilateralKernel(Size(kernelSize, kernelSize), CV_64F);

	for (int y = border; y < colorSource.rows - border; y++){
		for (int x = border; x < colorSource.cols - border; x++){

			Vec3b select = colorSource.at<Vec3b>(y, x);
			double select_gray = 0.301 * select[0] + 0.587 * select[1] + 0.114 * select[2];

			double colorSum = 0;
			for (int yy = -border; yy <= border; yy++){
				for (int xx = -border; xx <= border; xx++){
					Vec3b target = colorSource.at<Vec3b>(y + yy, x + xx);
					double target_gray = 0.301 * target[0] + 0.587 * target[1] + 0.114 * target[2];
					int diff = int(abs(target_gray - select_gray));
					if (diff>255) diff = 255;
					double colorVal = colorDiffGaussWeight[diff];
					colorKernel.at<double>(border + yy, border + xx) = colorVal;
					colorSum += colorVal;
				}
			}
			colorKernel /= colorSum;
			Mat bilateralKernel = colorKernel.mul(gaussianKernel);

			double sum = 0;
			for (int j = 0; j < kernelSize; j++){
				for (int i = 0; i < kernelSize; i++){
					sum += bilateralKernel.at<double>(i, j);
				}
			}
			bilateralKernel /= sum;

			Vec3d value = Vec3d(0, 0, 0);

			for (int yy = -border; yy <= border; yy++){
				for (int xx = -border; xx <= border; xx++){
					double weight = bilateralKernel.at<double>(border + yy, border + xx);
					value += source.at<Vec3b>(y + yy, x + xx) * weight;
				}
			}
			if (255 < value[0]) value[0] = 255;
			if (255 < value[1]) value[1] = 255;
			if (255 < value[2]) value[2] = 255;
			if (value[0] < 0) value[0] = 0;
			if (value[1] < 0) value[1] = 0;
			if (value[2] < 0) value[2] = 0;
			result.at<Vec3b>(y, x) = value;

		}
	}
}

void library::median_filter(const Mat& source, Mat& result, int kernelSize){
	medianBlur(source, result, kernelSize);
}
void library::gaussian_filter(const Mat& source, Mat& result, int kernelSize, double sigma){
	GaussianBlur(source, result, Size(kernelSize, kernelSize), sigma);
}
void library::bilateral_filter(const Mat& source, Mat& result, int kernelSize, double sigmaColor, double sigmaSpace){
	bilateralFilter(source, result, kernelSize, sigmaColor, sigmaSpace);
}