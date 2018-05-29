#include "CameraCalibrator.h"

CameraCalibrator::CameraCalibrator(const char* cameraParamsPath, const char* calibrateSampleImagePath){
	this->sample = imread(calibrateSampleImagePath);
	assert(("can't read sample file", !sample.empty()));

	for (int i = 0; i < 8; cameraParams[i++] = 0);
	this->cameraParamsPath = cameraParamsPath;
	this->readParameter();
}
CameraCalibrator::~CameraCalibrator(){
	cameraMatrix.release();
	distortionMatrix.release();
	sample.release();
}

void CameraCalibrator::createMatrix(){
	this->cameraMatrix = 
		(Mat_<double>(3, 3) << cameraParams[0], 0, cameraParams[1], 0, cameraParams[2], cameraParams[3], 0, 0, 1);
	this->distortionMatrix = 
		(Mat_<double>(1, 4) << cameraParams[4], cameraParams[5], cameraParams[6], cameraParams[7]);
}

void CameraCalibrator::readParameter(){
	ifstream paramsReader(this->cameraParamsPath);
	bool err_invalid_params = false;
	bool err_fopen_fail = !paramsReader.is_open();

	if (!err_fopen_fail){
		int i;
		for (i = 0; i < 8; i++)
			paramsReader >> this->cameraParams[i];
		if (i == 8)
			this->createMatrix();
		else
			err_invalid_params = true;
	}
	if (err_invalid_params || err_fopen_fail){
		// fx, cx, fy, cy, k1, k2, p1, p2
		this->cameraParams[0] = 500;
		this->cameraParams[1] = sample.cols / 2;
		this->cameraParams[2] = 500;
		this->cameraParams[3] = sample.rows / 2;
		this->cameraParams[4] = 0;
		this->cameraParams[5] = 0;
		this->cameraParams[6] = 0;
		this->cameraParams[7] = 0;
		printf("failed to raed parameter : fileOpenError(%c) invalidParameterError(%c)\n",
			err_fopen_fail ? 'v' : ' ',
			err_invalid_params ? 'v' : ' ');
	}
	paramsReader.close();
}

void CameraCalibrator::writeParameter(){
	ofstream paramsWriter(this->cameraParamsPath);
	bool err_fopen_fail = !paramsWriter.is_open();
	bool err_no_params = this->noParams();
	if (!err_fopen_fail && !err_no_params){
		for (int i = 0; i < 8; i++){
			paramsWriter << this->cameraParams[i] << endl;
		}
	}
	else {
		printf("failed to write parameter : fileOpenError(%c) noParameterError(%c)\n", 
			err_fopen_fail ? 'v' : ' ', 
			err_no_params ? 'v' : ' ');
	}
	paramsWriter.close();
}

void CameraCalibrator::simpleParameterSetting(){
	this->readParameter();

	int* fxy = (int*)malloc(sizeof(int));
	int* k12 = (int*)malloc(sizeof(int));
	*fxy = 50000;
	*k12 = 100000;

	int* tv = (int*)malloc(sizeof(int));
	*tv = 500;

	namedWindow("calibration");
	createTrackbar("Focal length", "calibration", fxy, 100000, this->fxyCallback, this);
	createTrackbar("Convexity", "calibration", k12, 200000, this->k12Callback, this);
	createTrackbar("TopView", "calibration", tv, 1000, this->tvCallback, this);

	imshow("calibration", sample);
	printf("press any key on image to finish calibration\n");
	waitKey(0);
	
	destroyWindow("calibration");
	free(fxy);
	free(k12);
	free(tv);
	this->writeParameter();
}

void CameraCalibrator::parameterSetting(){
	this->readParameter();

	int *paramsInt[8];
	for (int i = 0; i < 8; i++)
		paramsInt[i] = (int*)malloc(sizeof(int));

	// 0~3 idx : 0 ~ 99,999 : 0 ~ 999.99
	for (int i = 0; i < 4; i++)
		*paramsInt[i] = (int)(this->cameraParams[i] * 100);

	// 4~7 idx : 0 ~ 199,999 : -1.00000 ~ 0.99999
	for (int i = 4; i < 8; i++)
		*paramsInt[i] = (int)(this->cameraParams[i] * 100000 + 100000);
	
	namedWindow("calibration");
	createTrackbar("fx", "calibration", paramsInt[0], 100000, this->fxCallback, this);
	createTrackbar("fy", "calibration", paramsInt[2], 100000, this->fyCallback, this);
	createTrackbar("cx", "calibration", paramsInt[1], 100000, this->cxCallback, this);
	createTrackbar("cy", "calibration", paramsInt[3], 100000, this->cyCallback, this);
	createTrackbar("k1", "calibration", paramsInt[4], 200000, this->k1Callback, this);
	createTrackbar("k2", "calibration", paramsInt[5], 200000, this->k2Callback, this);
	createTrackbar("p1", "calibration", paramsInt[6], 200000, this->p1Callback, this);
	createTrackbar("p2", "calibration", paramsInt[7], 200000, this->p2Callback, this);
	imshow("calibration", sample);

	printf("press any key on image to finish calibration\n");
	waitKey(0);

	destroyWindow("calibration");

	// 0~3 idx : 0 ~ 99,999 : 0 ~ 999.99
	for (int i = 0; i < 4; i++){
		this->cameraParams[i] = (double)*paramsInt[i] / 100;
		delete(paramsInt[i]);
	}

	// 4~7 idx : 0 ~ 199,999 : -1.00000 ~ 0.99999
	for (int i = 4; i < 8; i++){
		this->cameraParams[i] = ((double)*paramsInt[i] - 100000) / 100000;
		delete(paramsInt[i]);
	}

	this->createMatrix();
	this->writeParameter();
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

void CameraCalibrator::fxyCallback(int value, void*ptr){
	CameraCalibrator* calibrator = (CameraCalibrator*)ptr;
	calibrator->cameraParams[0] = (double)value / 100;
	calibrator->cameraParams[2] = (double)value / 100;

	calibrator->createMatrix();
	Mat temp;
	undistort(calibrator->sample, temp, calibrator->cameraMatrix, calibrator->distortionMatrix);
	if (!temp.empty()){
		printf("fxy => %.6lf\n", calibrator->cameraParams[0]);
		imshow("calibration", temp);
	}
	temp.release();
}

void CameraCalibrator::k12Callback(int value, void*ptr){
	CameraCalibrator* calibrator = (CameraCalibrator*)ptr;
	calibrator->cameraParams[4] = ((double)value - 100000) / 100000;
	calibrator->cameraParams[5] = ((double)value - 100000) / 100000;

	calibrator->createMatrix();
	Mat temp;
	undistort(calibrator->sample, temp, calibrator->cameraMatrix, calibrator->distortionMatrix);
	if (!temp.empty()){
		printf("k12 => %.6lf\n", calibrator->cameraParams[4]);
		imshow("calibration", temp);
	}
	temp.release();
}

void CameraCalibrator::tvCallback(int value, void*ptr){
	//value 0~1000
	CameraCalibrator* calibrator = (CameraCalibrator*)ptr;
	Mat temp;
	undistort(calibrator->sample, temp, calibrator->cameraMatrix, calibrator->distortionMatrix);
	int cols = calibrator->sample.cols;
	int rows = calibrator->sample.rows;
	double val = ((double)value - 500) / 500; // -1 ~ 1
	Point2f perspectiveSrc[4] = {
		Point2f(cols / 4, 0), Point2f(cols / 4, rows),
		Point2f(cols / 4 * 3, 0), Point2f(cols / 4 * 3, rows) };
	Point2f perspectiveDest[4] = {
		Point2f(cols / 4 + val*cols / 4, 0), Point2f(cols / 4 - val*cols / 4, rows),
		Point2f(cols / 4 * 3 - val*cols / 4 - (val < 0 ? 1 : 0), 0), Point2f(cols / 4 * 3 + val*cols / 4 - (val > 0 ? 1 : 0), rows) };
	warpPerspective(temp, temp, getPerspectiveTransform(perspectiveSrc, perspectiveDest), temp.size());
	if (!temp.empty()){
		imshow("calibration", temp);
	}
	temp.release();
}
