#include "opencv2\opencv.hpp"
#include "CameraCalibrator.h"
#include "AvmsDatasetReader.h"

#define DEMO_VER true

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

	// left - back - rear - rigt 순서로 붙임.
	char* cameraParamsPath = "resource/camera.params";
	char* calibrateSampleImagePath = "resource/copyright/back/back217.jpg";
	CameraCalibrator cameraCalibrator(cameraParamsPath, calibrateSampleImagePath);
	if (cameraCalibrator.noParams()){
		cameraCalibrator.simpleParameterSetting();
		cameraCalibrator.parameterSetting();
	}
	
	char* avmsDatasetListDir = DEMO_VER ? "resource/demo/" : "resource/copyright/";
	AvmsDatasetReader avmsDatasetReader(avmsDatasetListDir, "list.txt");
	
	Mat left, back, rear, right;
	Mat left_dist, back_dist, rear_dist, right_dist;
	while (!avmsDatasetReader.end()){
		left = avmsDatasetReader.read();
		back = avmsDatasetReader.read();
		rear = avmsDatasetReader.read();
		right = avmsDatasetReader.read();
		left_dist = cameraCalibrator.undistortImage(left);
		back_dist = cameraCalibrator.undistortImage(back);
		rear_dist = cameraCalibrator.undistortImage(rear);
		right_dist = cameraCalibrator.undistortImage(right);

		imshow("left_undistortion", left_dist);
		imshow("back_undistortion", back_dist);
		imshow("rear_undistortion", rear_dist);
		imshow("right_undistortion", right_dist);

		waitKey(0);
	}

	return 0;
}