#include "AvmsDatasetReader.h"

AvmsDatasetReader::AvmsDatasetReader(const char* avmsDatasetDir, const char* avmsDatasetListPath)
{
	this->filePath = (char*)malloc(sizeof(char)*strlen(avmsDatasetDir) + strlen(avmsDatasetListPath)+1);
	this->filePath[0] = '\0';
	strcat(this->filePath, avmsDatasetDir);
	strcat(this->filePath, avmsDatasetListPath);

	this->idx = 0;
}

AvmsDatasetReader::~AvmsDatasetReader()
{
}

bool AvmsDatasetReader::end(){
	return false;
}

Mat AvmsDatasetReader::read(){

	ifstream datasetReader(this->filePath);
	datasetReader.get
	datasetReader.close();

	return Mat();
}