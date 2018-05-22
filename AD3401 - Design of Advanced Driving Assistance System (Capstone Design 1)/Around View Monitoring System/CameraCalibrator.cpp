#include "CameraCalibrator.h"

CameraCalibrator::CameraCalibrator(const char* cameraParamsPath){
	for (int i = 0; i < 8; cameraParams[i++] = 0);

	this->cameraParamsPath = cameraParamsPath;
	ifstream cameraParamsFile(cameraParamsPath);

	if (cameraParamsFile.is_open()){
		int order = 0;
		while (!cameraParamsFile.eof() && order < 8){
			cameraParamsFile >> cameraParams[order];
			cout << cameraParams[order] << endl;
			order++;
		}
		if (order == 8){
			this->createMatrix();
		}

		cameraParamsFile.close();
	}
}
CameraCalibrator::~CameraCalibrator(){}

void CameraCalibrator::createMatrix(){
	cameraMatrix = (Mat_<double>(3, 3) << cameraParams[0], 0, cameraParams[1], 0, cameraParams[2], cameraParams[3], 0, 0, 1);
	distortionMatrix = (Mat_<double>(1, 4) << cameraParams[4], cameraParams[5], cameraParams[6], cameraParams[7]);
}

void CameraCalibrator::parameterSetting(const char* calibrateSampleImagePath){
	int *paramsInt[8];
	if (!this->noParams()){
		for (int i = 0; i < 8; i++){
			paramsInt[i] = 0;
		}
	}
	else {
		for (int i = 0; i < 4; i++){
			// 0~3 idx : 0 ~ 99,999 : 0 ~ 999.99
			// 4~7 idx : 0 ~ 199,999 : -1.00000 ~ 0.99999
			*paramsInt[i] = (int)(cameraParams[i] * 100);
		}
		for (int i = 4; i < 8; i++){
			*paramsInt[i] = (int)(cameraParams[i] * 100000 + 100000);
		}
	}
	namedWindow("calibration");
	createTrackbar("fx", "calibration", paramsInt[0], 100000, this->fxCallback, this);
	createTrackbar("cx", "calibration", paramsInt[1], 100000, this->cxCallback, this);
	createTrackbar("fy", "calibration", paramsInt[2], 100000, this->fyCallback, this);
	createTrackbar("cy", "calibration", paramsInt[3], 100000, this->cyCallback, this);
	createTrackbar("k1", "calibration", paramsInt[4], 200000, this->k1Callback, this);
	createTrackbar("k2", "calibration", paramsInt[5], 200000, this->k2Callback, this);
	createTrackbar("p1", "calibration", paramsInt[6], 200000, this->p1Callback, this);
	createTrackbar("p2", "calibration", paramsInt[7], 200000, this->p2Callback, this);

	sample = imread(calibrateSampleImagePath);
	imshow("calibration", sample);
	printf("press any key on image to finish calibration\n");
	waitKey(0);

	for (int i = 0; i < 4; i++){
		// 0~3 idx : 0 ~ 99,999 : 0 ~ 999.99
		cameraParams[i] = (double)*paramsInt[i] / 100;
	}
	for (int i = 4; i < 8; i++){
		// 4~7 idx : 0 ~ 199,999 : -1.00000 ~ 0.99999
		cameraParams[i] = ((double)*paramsInt[i] - 100000) / 100000;
	}

	ofstream cameraParamsFile(this->cameraParamsPath);
	assert(("can't open cameraParamsFile", cameraParamsFile.is_open()));
	for (int i = 0; i < 8; i++){
		cameraParamsFile << this->cameraParams[i] << endl;
	}
	cameraParamsFile.close();

	this->createMatrix();
}

bool CameraCalibrator::noParams(){
	return this->cameraMatrix.empty() || this->distortionMatrix.empty();
}

Mat CameraCalibrator::undistortImage(const Mat source){
	assert(("there is no cameraParameter. must call 'parameterSetting()'", !this->noParams()));

	Mat ret;
	undistort(source, ret, this->cameraMatrix, this->distortionMatrix);
	return ret;
}

void CameraCalibrator::drawCallback(int v, void* p){

}

void CameraCalibrator::paramsCallback(int idx, int param){
	if (0<=idx && idx<4)
		cameraParams[idx] = (double)param / 100;
	else
		cameraParams[idx] = ((double)param - 100000) / 100000;

	this->createMatrix();
	Mat temp;
	undistort(sample, temp, this->cameraMatrix, this->distortionMatrix);

	if (!temp.empty()){
		printf("%d => %.6lf\n", idx, cameraParams[idx]);
		imshow("calibration", temp);
	}
	temp.release();
}

void CameraCalibrator::fxCallback(int value, void* ptr){
	CameraCalibrator* calibrator = (CameraCalibrator*)ptr;
	calibrator->paramsCallback(0, value);
}

void CameraCalibrator::cxCallback(int value, void*ptr){
	CameraCalibrator* calibrator = (CameraCalibrator*)ptr;
	calibrator->paramsCallback(1, value);
}

void CameraCalibrator::fyCallback(int value, void*ptr){
	CameraCalibrator* calibrator = (CameraCalibrator*)ptr;
	calibrator->paramsCallback(2, value);
}

void CameraCalibrator::cyCallback(int value, void*ptr){
	CameraCalibrator* calibrator = (CameraCalibrator*)ptr;
	calibrator->paramsCallback(3, value);
}

void CameraCalibrator::k1Callback(int value, void*ptr){
	CameraCalibrator* calibrator = (CameraCalibrator*)ptr;
	calibrator->paramsCallback(4, value);
}

void CameraCalibrator::k2Callback(int value, void*ptr){
	CameraCalibrator* calibrator = (CameraCalibrator*)ptr;
	calibrator->paramsCallback(5, value);
}

void CameraCalibrator::p1Callback(int value, void*ptr){
	CameraCalibrator* calibrator = (CameraCalibrator*)ptr;
	calibrator->paramsCallback(6, value);
}

void CameraCalibrator::p2Callback(int value, void*ptr){
	CameraCalibrator* calibrator = (CameraCalibrator*)ptr;
	calibrator->paramsCallback(7, value);
}
