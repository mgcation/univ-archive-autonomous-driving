#pragma once
#include "opencv2\opencv.hpp"
#include <fstream>

using namespace cv;
using namespace std;

class CameraCalibrator
{
private:
	Mat cameraMatrix;
	Mat distortionMatrix;
	Mat sample;
	string cameraParamsPath;
	double cameraParams[8];

	void createMatrix();
	void readParameter();
	void writeParameter();

	void paramsCallback(int idx, int param);
	static void fxCallback(int, void*);
	static void cxCallback(int, void*);
	static void fyCallback(int, void*);
	static void cyCallback(int, void*);
	static void p1Callback(int, void*);
	static void p2Callback(int, void*);
	static void k1Callback(int, void*);
	static void k2Callback(int, void*);
	static void fxyCallback(int, void*);
	static void k12Callback(int, void*);
	static void tvCallback(int, void*);

public:
	// get 'cameraMatrix', 'distortionMatrix'
	// from 'cameraParamsPath'
	CameraCalibrator(const char* cameraParamsPath, const char* calibrateSampleImagePath);
	~CameraCalibrator();
	

	// get camera parameter and distortion parameter from manual gui environment with image(from 'sample')
	// will save parameter at 'cameraParamsPath'
	// calibrate 8 parameter
	void parameterSetting();

	// get camera parameter and distortion parameter from manual gui environment with image(from 'sample')
	// will save parameter at 'cameraParamsPath'
	// clibrate 2 pair of parameter
	void simpleParameterSetting();

	// return true if parameter not set.
	bool noParams();

	// undisortImage using opencv
	Mat undistortImage(const Mat source);
};
