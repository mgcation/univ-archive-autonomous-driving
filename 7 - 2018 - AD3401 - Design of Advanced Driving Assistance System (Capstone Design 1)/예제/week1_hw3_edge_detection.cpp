#include "week1_hw3_edge_detection.h"

using namespace cv;
using namespace utility;


/*
	1차 미분을 이용한 에지 검출을 구현한다.
	2차 미분(LoG)을 이용한 에지 검출을 구현한다.
	Canny 에지 라이브러리를 찾아 적용한다.
	*/
int week1_hw3_edge_detection::run(){
	Mat source;
	my_imread(source, "resource/lena.png", 0);


	Mat sobel = sobel_edge_detection(source);
	Mat prewitt = prewitt_edge_detection(source);


	Mat diff2_LoG_kernel1 = get_LoG_kernel(0.7f);
	Mat diff2_LoG_kernel2 = get_LoG_kernel(1.0f);
	Mat diff2_LoG_kernel3 = get_LoG_kernel(1.7f);
	Mat LoG_result;
	filter2D(source, LoG_result, source.depth(), diff2_LoG_kernel1);
	imshow("week1_hw3_edge_detection : LoG 0.7f", LoG_result);
	filter2D(source, LoG_result, source.depth(), diff2_LoG_kernel2);
	imshow("week1_hw3_edge_detection : LoG 1.0f", LoG_result);
	filter2D(source, LoG_result, source.depth(), diff2_LoG_kernel3);
	imshow("week1_hw3_edge_detection : LoG 1.7f", LoG_result);


	Mat canny1, canny2, canny3, canny4;
	Canny(source, canny1, 01, 01);
	Canny(source, canny2, 01, 50);
	Canny(source, canny3, 50, 01);
	Canny(source, canny4, 200, 50);
	imshow("week1_hw3_edge_detection : canny 01, 01", canny1);
	imshow("week1_hw3_edge_detection : canny 01, 50", canny2);
	imshow("week1_hw3_edge_detection : canny 50, 01", canny3);
	imshow("week1_hw3_edge_detection : canny 200, 50", canny4);
	puts("===============");
	calc_MSE(canny2, canny3, "1 50 과 50 1의 비교");
	puts("===============");


	waitKey(0);
	return 0;
}

Mat sobel_edge_detection(Mat source){
	if (source.channels() > 1){
		source = source.clone();
		cvtColor(source, source, COLOR_BGR2GRAY);
	}
	Mat result;

	Mat sobel_kernel_horizontal = getKernel({ { 1, 2, 1 }, { 0, 0, 0 }, { -1, -2, -1 } });
	Mat sobel_kernel_vertical = getKernel({ { 1, 0, -1 }, { 2, 0, -2 }, { 1, 0, -1 } });
	int rows = source.rows;
	int cols = source.cols;

	Mat sobel_horizontal, sobel_vertical;
	filter2D(source, sobel_horizontal, source.depth(), sobel_kernel_horizontal);
	filter2D(source, sobel_vertical, source.depth(), sobel_kernel_vertical);

	result = source.clone();
	for (int y = 0; y < rows; y++){
		for (int x = 0; x < cols; x++){
			ucharAt(result, y, x) =
				sqrt(
				pow(ucharAt(sobel_horizontal, y, x), 2) + pow(ucharAt(sobel_vertical, y, x), 2)
				);
		}
	}
	imshow("week1_hw3_edge_detection : sobel horizontal", sobel_horizontal);
	imshow("week1_hw3_edge_detection : sobel vertical", sobel_vertical);
	imshow("week1_hw3_edge_detection : sobel edge detection", result);

	return result;
}

Mat prewitt_edge_detection(Mat source){
	if (source.channels() > 1){
		source = source.clone();
		cvtColor(source, source, COLOR_BGR2GRAY);
	}
	Mat result;

	Mat prewitt_kernel_horizontal = getKernel({ { 1, 1, 1 }, { 0, 0, 0 }, { -1, -1, -1 } });
	Mat prewitt_kernel_vertical = getKernel({ { 1, 0, -1 }, { 1, 0, -1 }, { 1, 0, -1 } });
	int rows = source.rows;
	int cols = source.cols;

	Mat prewitt_horizontal, prewitt_vertical;
	filter2D(source, prewitt_horizontal, source.depth(), prewitt_kernel_horizontal);
	filter2D(source, prewitt_vertical, source.depth(), prewitt_kernel_vertical);

	result = source.clone();
	for (int y = 0; y < rows; y++){
		for (int x = 0; x < cols; x++){
			ucharAt(result, y, x) =
				sqrt(
				pow(ucharAt(prewitt_horizontal, y, x), 2) + pow(ucharAt(prewitt_vertical, y, x), 2)
				);
		}
	}
	imshow("week1_hw3_edge_detection : prewitt horizontal", prewitt_horizontal);
	imshow("week1_hw3_edge_detection : prewitt vertical", prewitt_vertical);
	imshow("week1_hw3_edge_detection : prewitt edge detection", result);

	return result;
}

Mat get_LoG_kernel(float sigma){
	double kernelSize = sigma * 6;
	kernelSize += kernelSize - (int)kernelSize > 0;
	kernelSize += (int)kernelSize % 2 == 0;
	kernelSize = (int)kernelSize;
	if (kernelSize < 1){
		printf("warn :: kernelSize is 1 ; output must 0");
	}

	Mat kernel(kernelSize, kernelSize, CV_64F);
	int halfSize = (int)kernelSize / 2;
	for (int y = 0; y < halfSize+1; y++){
		for (int x = 0; x < halfSize+1; x++){
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
				= (pow2xx + pow2yy - 2*pow2sigma) / pow4sigma * pow(M_E, -(pow2xx+pow2yy)/(2*pow2sigma));
		}
	}

	return kernel;
}