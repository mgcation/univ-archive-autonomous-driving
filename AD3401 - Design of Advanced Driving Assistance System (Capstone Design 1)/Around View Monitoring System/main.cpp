#include "opencv2\opencv.hpp"
#include "CameraCalibrator.h"

using namespace cv;

int main(){
	/*
	1. camera calibration
		undistortion

	2. matching
		feature extract

	3. blending
		multi band blending

	4. color correction
		histogram stretching

	5. etc

	6. report
	*/

	char* cameraParamsPath = "resource/camera.params";
	char* calibrateSampleImagePath = "resource/copyright/back/back217.jpg";
	CameraCalibrator cameraCalibrator(cameraParamsPath);
	if (!cameraCalibrator.noParams())
		cameraCalibrator.parameterSetting(calibrateSampleImagePath);
	
	Mat source = imread("resource/copyright/back/back217.jpg");
	Mat undisortImage = cameraCalibrator.undistortImage(source);

	imshow("back217 - original", source);
	imshow("back217 - undisort", undisortImage);

	waitKey(0);

	return 0;
}