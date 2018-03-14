#include "week1_hw2_gaussian_and_median_filter.h"

using namespace cv;
using namespace std;
using namespace utility;

/*
	필터 적용 전 노이즈를 임의로 집어넣는다.
	1. salt and pepper noising
	2. gaussian noising

	가우시안 / 메디안 필터를 직접 구현해서 적용한다.
	라이브러리를 찾아서 적용한다.
	둘을 비교해본다. (MSE)
	*/
int week1_hw2_gaussian_and_median_filter::run(){
	Mat source, result;
	my_imread(source, "resource/lena.png", 0);
	imshow("week1_hw2_gaussian_and_median_filter : source", source);


	Mat noise_saltpepper = saltpepper_noise(source, 0.10, 40);
	imshow("week1_hw2_gaussian_and_median_filter : saltpepper", noise_saltpepper);
	printf("saltpepper 에 의한 평균 노이즈 : %d\n", calc_MSE(source, noise_saltpepper, false));


	Mat noise_gaussian = gaussian_noise(source, 20);
	imshow("week1_hw2_gaussian_and_median_filter : gaussian", noise_gaussian);
	printf("gaussian 에 의한 평균 노이즈 : %d\n", calc_MSE(source, noise_gaussian, false));

	
	puts("===");


	Mat my_salt2median, my_gaus2median, my_salt2gaus, my_gaus2gaus;
	my_salt2median = myMedian_filter(noise_saltpepper);
	imshow("week1_hw2_gaussian_and_median_filter : my_salt2median", my_salt2median);
	printf("직접구현한 salt2median로 복원한 평균 노이즈 : %d\n", calc_MSE(source, my_salt2median, false));

	my_gaus2median = myMedian_filter(noise_gaussian);
	imshow("week1_hw2_gaussian_and_median_filter : my_gaus2median", my_gaus2median);
	printf("직접구현한 gaus2median로 복원한 평균 노이즈 : %d\n", calc_MSE(source, my_gaus2median, false));

	my_salt2gaus = myGaussian_filter(noise_saltpepper, 1.0);
	imshow("week1_hw2_gaussian_and_median_filter : my_salt2gaus", my_salt2gaus);
	printf("직접구현한 salt2gaus로 복원한 평균 노이즈 : %d\n", calc_MSE(source, my_salt2gaus, false));

	my_gaus2gaus = myGaussian_filter(noise_gaussian, 1.0);
	imshow("week1_hw2_gaussian_and_median_filter : my_gaus2gaus", my_gaus2gaus);
	printf("직접구현한 gaus2gaus로 복원한 평균 노이즈 : %d\n", calc_MSE(source, my_gaus2gaus, false));


	puts("===");


	Mat salt2median, gaus2median, salt2gaus, gaus2gaus;
	medianBlur(noise_saltpepper, salt2median, 3);
	imshow("week1_hw2_gaussian_and_median_filter : salt2median", salt2median);
	printf("라이브러리 salt2median로 복원한 평균 노이즈 : %d\n", calc_MSE(source, salt2median, false));

	medianBlur(noise_gaussian, gaus2median, 3);
	imshow("week1_hw2_gaussian_and_median_filter : gaus2median", gaus2median);
	printf("라이브러리 gaus2median로 복원한 평균 노이즈 : %d\n", calc_MSE(source, gaus2median, false));

	GaussianBlur(noise_saltpepper, salt2gaus, Size(7, 7), 1.0);
	imshow("week1_hw2_gaussian_and_median_filter : salt2gaus", salt2gaus);
	printf("라이브러리 salt2gaus로 복원한 평균 노이즈 : %d\n", calc_MSE(source, salt2gaus, false));

	GaussianBlur(noise_gaussian, gaus2gaus, Size(7, 7), 1.0);
	imshow("week1_hw2_gaussian_and_median_filter : gaus2gaus", gaus2gaus);
	printf("라이브러리 gaus2gaus로 복원한 평균 노이즈 : %d\n", calc_MSE(source, gaus2gaus, false));


	waitKey(0);
	return 0;
}

Mat saltpepper_noise(Mat source, float rate, double standard_deviation){
	Mat saltpepper_noise = Mat::zeros(source.rows, source.cols, CV_8U);
	randn(saltpepper_noise, 128, standard_deviation);

	int noiseNum = 255 * rate;
	Mat black = saltpepper_noise < noiseNum;
	Mat white = saltpepper_noise > 255 - noiseNum;

	Mat saltpepper_img = source.clone();
	saltpepper_img.setTo(255, white);
	saltpepper_img.setTo(0, black);

	return saltpepper_img;
}

Mat gaussian_noise(Mat source, double standard_deviation){
	Mat result = Mat::zeros(source.rows, source.cols, CV_16S);
	randn(result, 0, standard_deviation);

	Mat addSource;
	source.convertTo(addSource, CV_16S);
	result += addSource;

	result.convertTo(result, CV_8U);
	return result;
}

int partition(int* input, int p, int r)
{
	int pivot = input[r];

	while (p < r)
	{
		while (input[p] < pivot)
			p++;

		while (input[r] > pivot)
			r--;

		if (input[p] == input[r])
			p++;
		else if (p < r) {
			int tmp = input[p];
			input[p] = input[r];
			input[r] = tmp;
		}
	}

	return r;
}

int quick_select(int* input, int p, int r, int k)
{
	if (p == r) return input[p];
	int j = partition(input, p, r);
	int length = j - p + 1;
	if (length == k) return input[j];
	else if (k < length) return quick_select(input, p, j - 1, k);
	else  return quick_select(input, j + 1, r, k - length);
}

Mat myMedian_filter(Mat source){
	Mat result = source.clone();

	for (int y = 1; y < result.rows - 1; y++){
		for (int x = 1; x < result.cols - 1; x++){
			int arry[] = {
				source.at<uchar>(y - 1, x - 1),
				source.at<uchar>(y - 1, x),
				source.at<uchar>(y, x - 1),
				source.at<uchar>(y, x),
				source.at<uchar>(y, x + 1),
				source.at<uchar>(y + 1, x),
				source.at<uchar>(y + 1, x + 1),
				source.at<uchar>(y - 1, x + 1),
				source.at<uchar>(y + 1, x - 1)
			};
			ucharAt(result, y, x) = quick_select(arry, 0, 8, 5);
		}
	}

	return result;
}

Mat myGaussian_filter(Mat source, double standard_deviation){
	double kernelSize = standard_deviation * 6;
	kernelSize += kernelSize - (int)kernelSize > 0;
	kernelSize += (int)kernelSize % 2 == 0;
	kernelSize = (int)kernelSize;
	if (kernelSize < 1){
		printf("warn :: kernelSize is 1 ; output must 0");
	}

	double M_PI = 3.14159265358979323846;
	Mat kernel(kernelSize, kernelSize, CV_64F);
	int halfSize = (int)kernelSize / 2;
	for (int y = 0; y < halfSize + 1; y++){
		for (int x = 0; x < halfSize + 1; x++){
			int xx = x - halfSize;
			int yy = y - halfSize;
			double pow2xx = pow(xx, 2);
			double pow2yy = pow(yy, 2);
			double pow2sigma = pow(standard_deviation, 2);
			kernel.at<double>(y, x)
				= kernel.at<double>(kernelSize - y - 1, x)
				= kernel.at<double>(y, kernelSize - x - 1)
				= kernel.at<double>(kernelSize - y - 1, kernelSize - x - 1)
				= 1 / (2 * M_PI * pow2sigma) * exp(-1 * (pow2yy + pow2xx) / (2 * pow2sigma));
		}
	}

	Mat result = source.clone();
	filter2D(result, result, result.depth(), kernel);

	return result;
}