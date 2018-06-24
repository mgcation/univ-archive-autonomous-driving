#include "opencv2\opencv.hpp"
#include "CameraCalibrator.h"
#include "AvmsDatasetReader.h"
#include "AvmsMatcher.h"
#include <ctime>

#define DEMO_VER true
#define SET_CALIBRATION false
#define SPEED_TEST false

using namespace cv;
using namespace std;

int main(){

	// left - back - rear - rigt 순서로 붙임.
	char* cameraParamsPath = "resource/camera.params";
	char* calibrateSampleImagePath = "resource/demo/back217.jpg";
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
	long startTime = clock();
	long preTime = startTime;
	long minTime = 10000000;
	long maxTime = 0;
	int count = 0;
	while (!avmsDatasetReader.end()){
		avmsDatasetReader.read(dataset);
		for (int i = 0; i < 4; i++){
			dataset_undistort[i] = cameraCalibrator.undistortImage(dataset[i]);
		}

		Mat colorTarget = dataset_undistort[3];
		dataset_undistort[0] = AvmsMatcher::histogramMatching(dataset_undistort[0], colorTarget);
		dataset_undistort[1] = AvmsMatcher::histogramMatching(dataset_undistort[1], colorTarget);
		dataset_undistort[2] = AvmsMatcher::histogramMatching(dataset_undistort[2], colorTarget);
		dataset_undistort[3] = AvmsMatcher::histogramMatching(dataset_undistort[3], colorTarget);

		double val = -0.7; // -0.7;
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

		long wait_ms = 50;
		waitKey(wait_ms);

		if (SPEED_TEST){
			count++;
			long now = clock();
			long ms = now - preTime - wait_ms;
			minTime = min(minTime, ms);
			maxTime = max(maxTime, ms);
			double fps = 1000.0 / ms;
			long avg_ms = (now - startTime) / count - wait_ms;
			double avg_fps = 1000.0 / avg_ms;
			printf("%ldms (%.2lffps) / 평균(%ld, %.2lf)\n", ms, fps, avg_ms, avg_fps);
			preTime = now;
		}
	}
	if (SPEED_TEST){
		printf("min : %ldms / max : %ldms\n", minTime, maxTime);
	}
	puts("end");
	waitKey(0);

	return 0;
}
