#include "opencv2\opencv.hpp"
#include "CameraCalibrator.h"
#include "AvmsDatasetReader.h"
#include "AvmsMatcher.h"

#define DEMO_VER false
#define SET_CALIBRATION false

using namespace cv;

int main(){
	/*
v	0. file reader

	1. camera calibration
v		undistortion
v		perspective warping

	---

	2. matching
		feature extract
		realtime resizing

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
	if (cameraCalibrator.noParams() || SET_CALIBRATION){
		cameraCalibrator.simpleParameterSetting();
		cameraCalibrator.parameterSetting();
	}
	
	char* avmsDatasetListDir = DEMO_VER ? "resource/demo/" : "resource/copyright/";
	AvmsDatasetReader avmsDatasetReader(avmsDatasetListDir, "list.txt");

	Mat dataset[4];
	Mat dataset_undistort[4];
	int cols = 480;
	int rows = 360;
	while (!avmsDatasetReader.end()){
		avmsDatasetReader.read(dataset);
		for (int i = 0; i < 4; i++){
			dataset_undistort[i] = cameraCalibrator.undistortImage(dataset[i]);
		}

		double val = -0.7;
		Point2f perspectiveSrc[4] = {
			Point2f(cols / 4, 0), Point2f(cols / 4, rows),
			Point2f(cols / 4 * 3, 0), Point2f(cols / 4 * 3, rows) };
		Point2f perspectiveDest[4] = {
			Point2f(cols / 4 + val*cols / 4, 0), Point2f(cols / 4 - val*cols / 4, rows),
			Point2f(cols / 4 * 3 - val*cols / 4 - (val < 0 ? 1 : 0), 0), Point2f(cols / 4 * 3 + val*cols / 4 - (val > 0 ? 1 : 0), rows) };

		for (int i = 0; i < 4; i++){
			warpPerspective(dataset_undistort[i], dataset_undistort[i], getPerspectiveTransform(perspectiveSrc, perspectiveDest), dataset_undistort[i].size());
		}

		Mat result = AvmsMatcher::matchImage(dataset_undistort[0], dataset_undistort[1], dataset_undistort[2], dataset_undistort[3]);

		imshow("left_undistortion", dataset_undistort[0]);
		imshow("back_undistortion", dataset_undistort[1]);
		imshow("rear_undistortion", dataset_undistort[2]);
		imshow("right_undistortion", dataset_undistort[3]);
		imshow("result", result);

		waitKey(0);
	}

	return 0;
}
