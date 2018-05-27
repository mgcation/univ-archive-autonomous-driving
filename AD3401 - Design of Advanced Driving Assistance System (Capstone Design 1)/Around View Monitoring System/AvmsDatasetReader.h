#include "opencv2\opencv.hpp"
#include <fstream>
#pragma once

using namespace cv;
using namespace std;

class AvmsDatasetReader
{
private:
	int idx;
	char* filePath;

public:
	AvmsDatasetReader(const char* avmsDatasetDir, const char* avmsDatasetListPath);
	~AvmsDatasetReader();
	bool end();
	Mat read();
};

