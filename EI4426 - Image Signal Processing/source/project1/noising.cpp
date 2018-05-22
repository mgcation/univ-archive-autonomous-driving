#include "noising.h"

void proj_1::impulse_noise(const Mat& source, Mat& result, float rate, double sigma)
{
	Mat salt_pepper_bottle = Mat::zeros(source.rows, source.cols, CV_8U);
	randn(salt_pepper_bottle, 128, sigma);

	int noiseNum = 255 * rate / 2;
	Mat black = salt_pepper_bottle < noiseNum;
	Mat white = salt_pepper_bottle > 255 - noiseNum;

	result = source.clone();

	result.setTo(255, white);
	result.setTo(0, black);
}

void proj_1::gaussian_noise(const Mat& source, Mat& result, double sigma)
{
	Mat signedSource;
	source.convertTo(signedSource, CV_16S);

	result = Mat::zeros(source.rows, source.cols, signedSource.type());
	randn(result, 0, sigma);

	result += signedSource;

	Mat underflow_mask = result < 0;
	if (signedSource.channels() == 3)
		cvtColor(underflow_mask, underflow_mask, CV_RGB2GRAY);
	result.setTo(0, underflow_mask);

	result.convertTo(result, CV_8U);

	signedSource.release();
	underflow_mask.release();
}

//! errLimit : pixel error limit
void proj_1::uniform_noise(const Mat& source, Mat& result, unsigned int errLimit)
{
	Mat signedSource;
	source.convertTo(signedSource, CV_16S);

	result = Mat::zeros(source.rows, source.cols, signedSource.type());
	randu(result, -(int)errLimit, errLimit);

	result += signedSource;

	Mat underflow_mask = result < 0;
	if (signedSource.channels() == 3)
		cvtColor(underflow_mask, underflow_mask, CV_RGB2GRAY);
	result.setTo(0, underflow_mask);

	result.convertTo(result, CV_8U);

	signedSource.release();
	underflow_mask.release();
}