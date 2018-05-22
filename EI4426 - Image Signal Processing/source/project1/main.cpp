#include "opencv2/opencv.hpp"
#include "noising.h"
#include "denoising.h"
#include "edge.h"

using namespace proj_1;

int main(){
	// noise : impulse_noise, gaussian_noise, uniform_noise (library not supported.)
	// filter : median_filter, gaussian_filter, bilateral_filter
	// edge : sobel_detector, prewitt_detector, LoG_detector (prewitt library not supported.)


	Mat source = imread("lena.png");
	Mat impulse_noise_img[4], gaussian_noise_img[3], uniform_noise_img[3];
	Mat median_filter_img, gaussian_filter_img, bilateral_filter_img;
	Mat sobel_img, prewitt_img, LoG_img;


	// ------- source -------
	imshow("source", source);
	// ------- source -------


	// ------- noise -------
	impulse_noise(source, impulse_noise_img[0], 0.1, 40);
	impulse_noise(source, impulse_noise_img[1], 0.3, 40);
	impulse_noise(source, impulse_noise_img[2], 0.1, 50);
	impulse_noise(source, impulse_noise_img[3], 0.3, 50);

	gaussian_noise(source, gaussian_noise_img[0], 10);
	gaussian_noise(source, gaussian_noise_img[1], 20);
	gaussian_noise(source, gaussian_noise_img[2], 30);

	uniform_noise(source, uniform_noise_img[0], 15);
	uniform_noise(source, uniform_noise_img[1], 30);
	uniform_noise(source, uniform_noise_img[2], 45);

	/*imshow("impulse 0", impulse_noise_img[0]);
	imshow("impulse 1", impulse_noise_img[1]);*/
	imshow("impulse 2", impulse_noise_img[2]);
	/*imshow("impulse 3", impulse_noise_img[3]);

	imshow("gaussian 0", gaussian_noise_img[0]);*/
	imshow("gaussian 1", gaussian_noise_img[1]);
	/*imshow("gaussian 2", gaussian_noise_img[2]);

	imshow("uniform 0", uniform_noise_img[0]);
	imshow("uniform 1", uniform_noise_img[1]);*/
	imshow("uniform 2", uniform_noise_img[2]);
	// ------- noise -------

	// etc
	impulse_noise_img[2];
	gaussian_noise_img[1];
	uniform_noise_img[2];
	median_filter(uniform_noise_img[2], median_filter_img, 3, false);
	gaussian_filter(uniform_noise_img[2], gaussian_filter_img, 5, 0.8);
	bilateral_filter(uniform_noise_img[2], bilateral_filter_img, 5, 25, 25);
	imshow("med", median_filter_img);
	imshow("gauss", gaussian_filter_img);
	imshow("bi", bilateral_filter_img);
	calc_MSE(source, median_filter_img, true, "median");
	calc_MSE(source, gaussian_filter_img, true, "gaussian");
	calc_MSE(source, bilateral_filter_img, true, "bilateral");
	// etc


	// ------- filter -------
	/*bool enhancedToImpulseImage = false;
	median_filter(impulse_noise_img[0], median_filter_img, 3, enhancedToImpulseImage);
	imshow("implemented impulse median 0 size(3)", median_filter_img);
	median_filter(impulse_noise_img[2], median_filter_img, 3, enhancedToImpulseImage);
	imshow("implemented impulse median 2 size(3)", median_filter_img);
	median_filter(impulse_noise_img[0], median_filter_img, 5, enhancedToImpulseImage);
	imshow("implemented impulse median 0 size(5)", median_filter_img);
	median_filter(impulse_noise_img[2], median_filter_img, 5, enhancedToImpulseImage);
	imshow("implemented impulse median 2 size(5)", median_filter_img);

	library::median_filter(impulse_noise_img[0], median_filter_img, 3);
	imshow("library impulse median 0 size(3)", median_filter_img);
	library::median_filter(impulse_noise_img[2], median_filter_img, 3);
	imshow("library impulse median 2 size(3)", median_filter_img);
	library::median_filter(impulse_noise_img[0], median_filter_img, 5);
	imshow("library impulse median 0 size(5)", median_filter_img);
	library::median_filter(impulse_noise_img[2], median_filter_img, 5);
	imshow("library impulse median 2 size(5)", median_filter_img);

	enhancedToImpulseImage = true;
	median_filter(impulse_noise_img[0], median_filter_img, 3, enhancedToImpulseImage);
	imshow("enhanced impulse median 0 (size3)", median_filter_img);
	median_filter(impulse_noise_img[2], median_filter_img, 3, enhancedToImpulseImage);
	imshow("enhanced impulse median 2 (size3)", median_filter_img);
	median_filter(impulse_noise_img[0], median_filter_img, 5, enhancedToImpulseImage);
	imshow("enhanced impulse median 0 (size5)", median_filter_img);
	median_filter(impulse_noise_img[2], median_filter_img, 5, enhancedToImpulseImage);
	imshow("enhanced impulse median 2 (size5)", median_filter_img);*/


	/*gaussian_filter(gaussian_noise_img[0], gaussian_filter_img, 3, 0.5);
	imshow("implem gaussian gaussian 00", gaussian_filter_img);
	gaussian_filter(gaussian_noise_img[0], gaussian_filter_img, 5, 0.8);
	imshow("implem gaussian gaussian 01", gaussian_filter_img);
	gaussian_filter(gaussian_noise_img[2], gaussian_filter_img, 3, 0.5);
	imshow("implem gaussian gaussian 20", gaussian_filter_img);
	gaussian_filter(gaussian_noise_img[2], gaussian_filter_img, 5, 0.8);
	imshow("implem gaussian gaussian 21", gaussian_filter_img);

	library::gaussian_filter(gaussian_noise_img[0], gaussian_filter_img, 3, 0.5);
	imshow("lib gaussian gaussian 00", gaussian_filter_img);
	library::gaussian_filter(gaussian_noise_img[0], gaussian_filter_img, 5, 0.8);
	imshow("lib gaussian gaussian 01", gaussian_filter_img);
	library::gaussian_filter(gaussian_noise_img[2], gaussian_filter_img, 3, 0.5);
	imshow("lib gaussian gaussian 20", gaussian_filter_img);
	library::gaussian_filter(gaussian_noise_img[2], gaussian_filter_img, 5, 0.8);
	imshow("lib gaussian gaussian 21", gaussian_filter_img);*/


	/*imshow("gaussian 1", gaussian_noise_img[1]);

	bilateral_filter(gaussian_noise_img[1], bilateral_filter_img, 5, 30, 30);
	imshow("implem bilateral_filter 0", bilateral_filter_img);
	bilateral_filter(gaussian_noise_img[1], bilateral_filter_img, 5, 50, 30);
	imshow("implem bilateral_filter 1", bilateral_filter_img);
	bilateral_filter(gaussian_noise_img[1], bilateral_filter_img, 5, 30, 50);
	imshow("implem bilateral_filter 2", bilateral_filter_img);
	bilateral_filter(gaussian_noise_img[1], bilateral_filter_img, 7, 40, 40);
	imshow("implem bilateral_filter 3", bilateral_filter_img);
	bilateral_filter(gaussian_noise_img[1], bilateral_filter_img, 7, 60, 60);
	imshow("implem bilateral_filter 4", bilateral_filter_img);

	library::bilateral_filter(gaussian_noise_img[1], bilateral_filter_img, 5, 30, 30);
	imshow("lib bilateral_filter 0", bilateral_filter_img);
	library::bilateral_filter(gaussian_noise_img[1], bilateral_filter_img, 5, 50, 30);
	imshow("lib bilateral_filter 1", bilateral_filter_img);
	library::bilateral_filter(gaussian_noise_img[1], bilateral_filter_img, 5, 30, 50);
	imshow("lib bilateral_filter 2", bilateral_filter_img);
	library::bilateral_filter(gaussian_noise_img[1], bilateral_filter_img, 7, 40, 40);
	imshow("lib bilateral_filter 3", bilateral_filter_img);
	library::bilateral_filter(gaussian_noise_img[1], bilateral_filter_img, 7, 60, 60);
	imshow("lib bilateral_filter 4", bilateral_filter_img);*/
	// ------- filter -------


	// ------- edge -------
	/*cvtColor(source, source, CV_BGR2GRAY);

	prewitt_detector(source, prewitt_img);
	imshow("prewitt", prewitt_img);

	sobel_detector(source, sobel_img);
	imshow("sobel", sobel_img);
	library::sobel_detector(source, sobel_img);
	imshow("sobel lib", sobel_img);


	LoG_detector(source, LoG_img, 3);
	imshow("LoG", LoG_img);

	library::LoG_detector(source, LoG_img, 3);
	imshow("LoG lib", LoG_img);*/
	// ------- edge -------


	waitKey(0);
	return 0;
}