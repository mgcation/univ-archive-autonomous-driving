#include "opencv2\opencv.hpp"
#include <fstream>
#pragma once

using namespace cv;
using namespace std;

class AvmsDatasetReader
{
private:
	struct AvmsDataset{
		string data[4];
	};

	int idx;
	string dirPath;
	vector<AvmsDataset> fileNames;

public:
	AvmsDatasetReader(const char* avmsDatasetDir, const char* avmsDatasetListPath);
	~AvmsDatasetReader();
	bool end();
	void read(Mat* dest);
};

