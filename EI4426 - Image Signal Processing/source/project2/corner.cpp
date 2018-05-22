#include "corner.h"


void proj_2::cornerMoravec(const Mat& source, Mat& dest, double threshold){
	assert(source.type() == CV_8UC3 || source.type() == CV_8UC1);

	int v[4] = {-1, 0, 1, 0};
	int u[4] = {0, -1, 0, 1};

	Mat gray = ifnot_2gray(source);
	dest = Mat(source.rows, source.cols, CV_8UC1, Scalar(0));

	for (int y = 2; y < source.rows - 2; y++){
		for (int x = 2; x < source.cols - 2; x++){
			double C;
			for (int i = 0; i < 4; i++){
				double SSD = 0;
				for (int yy = -1; yy <= 1; yy++){
					for (int xx = -1; xx <= 1; xx++){
						uchar S_00 = gray.at<uchar>(y + yy, x + xx);
						uchar S_vu = gray.at<uchar>(y + v[i] + yy, x + u[i] + xx);
						SSD += pow(S_00 - S_vu, 2);
					}
				}
				if (!i || SSD < C)
					C = SSD;
			}
			dest.at<uchar>(y, x) = (threshold <= C) * 255;
		}
	}
	gray.release();
}

void proj_2::cornerHarris(const Mat& source, Mat& dest, float k, float threshold){
	// 0. to gray
	Mat gray = ifnot_2gray(source);


	// 1. edge
	Mat sobelX, sobelY;
	sobelX = (Mat_<float>(3, 3) << -1, 0, 1, -3, 0, 3, -1, 0, 1);
	transpose(sobelX, sobelY);
	Mat Ix, Iy, Ix2, Iy2, Ixy, Ixy2;
	filterBasedUchar(gray, Ix, sobelX);
	filterBasedUchar(gray, Iy, sobelY);
	Ix.convertTo(Ix, CV_32FC1, 1.0 / 255);
	Iy.convertTo(Iy, CV_32FC1, 1.0 / 255);
	Ix2 = Ix.mul(Ix);
	Iy2 = Iy.mul(Iy);
	Ixy = Ix.mul(Iy);

	// 2. edge blurring (가중치 부여)
	int kernelSize = 3;
	float sigma = 2.1 / 6.0;
	Mat gaussianKernel(kernelSize, kernelSize, CV_32FC1);
	int kernelHalf = kernelSize / 2;
	float pow2sigma = pow(sigma, 2);
	for (int y = 0; y < kernelHalf + 1; y++){
		for (int x = 0; x < kernelHalf + 1; x++){
			int xx = x - kernelHalf;
			int yy = y - kernelHalf;
			float pow2xx = pow(xx, 2);
			float pow2yy = pow(yy, 2);
			float value = 1 / (2 * M_PI * pow2sigma) * exp(-1 * (pow2yy + pow2xx) / (2 * pow2sigma));
			gaussianKernel.at<float>(y, x)
				= gaussianKernel.at<float>(kernelSize - y - 1, x)
				= gaussianKernel.at<float>(y, kernelSize - x - 1)
				= gaussianKernel.at<float>(kernelSize - y - 1, kernelSize - x - 1)
				= value;
		}
	}
	filterBasedFloat(Ix2, Ix2, gaussianKernel);
	filterBasedFloat(Iy2, Iy2, gaussianKernel);
	filterBasedFloat(Ixy, Ixy, gaussianKernel);


	// 3. harris
	// lambda1, lambda2 가 M의 eigen values 일 때 다음과 같은 식임. : (lambda1 * lambda2) - k*(lambda1 + lambda2)^2
	// M = [Ix2 Ixy ; Ixy Iy2]
	// auto R = determinant.each(M) - (k*trace(M)*trace(M));
	dest = (Ix2.mul(Iy2) - Ixy.mul(Ixy)) - k*((Ix2 + Iy2).mul(Ix2 + Iy2));


	// 4. binarize
	dest.setTo(0, dest < threshold);
	dest.setTo(255, dest >= threshold);
	dest.convertTo(dest, CV_8UC1);
}

void proj_2::cornerSUSAN(const Mat& source, Mat& dest, unsigned int threshold1, double threshold2){
	const int mask[7][7] = {
		0, 0, 1, 1, 1, 0, 0,
		0, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 0,
		0, 0, 1, 1, 1, 0, 0
	};
	const int padding = 3;
	const int maskArea = 37;
	const int q = 0.75 * maskArea;
	const int threshold2Area = threshold2 * maskArea;

	Mat gray = ifnot_2gray(source);
	Mat ret = Mat(gray.rows, gray.cols, CV_8UC1, Scalar(0));
	for (int y = 3; y < gray.rows - 3; y++){
		for (int x = 3; x < gray.cols - 3; x++){
			uchar cur = gray.at<uchar>(y, x);
			int usan_area = 0;
			for (int yy = -padding; yy <= padding; yy++){
				for (int xx = -padding; xx <= padding; xx++){
					if (!mask[padding + yy][padding + xx])
						continue;
					double neighbor = gray.at<uchar>(y + yy, x + xx);
					if (abs(neighbor - cur) <= threshold1)
						usan_area++;
				}
			}
			if (usan_area <= threshold2Area)
				ret.at<uchar>(y, x) = 255; // (q - usan_area) * 255 / q;
		}
	}

	dest.release();
	dest = ret;
}

void library::cornerHarris(const Mat& source, Mat& dest, double k, double threshold){
	dest = ifnot_2gray(source);
	cv::cornerHarris(dest, dest, 2, 3, k);
	cv::threshold(dest, dest, threshold, 255, THRESH_BINARY);
}
