#include "opencv2\opencv.hpp"
#include "opencv2\ml\ml.hpp"
#include "utility.h"
#include <fstream>
#pragma once

using namespace cv;
using namespace utility;
using namespace std;

class week5_hw1_MNIST_classification_HOG_SVM
{
public:
	static void readData(const String imageFileName, const String labelFileName, vector<Mat>& image, vector<float>& label);
	static void run();
};

