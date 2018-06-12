#include "AvmsDatasetReader.h"

AvmsDatasetReader::AvmsDatasetReader(const char* avmsDatasetDir, const char* avmsDatasetListPath)
{
	this->dirPath.append(avmsDatasetDir);
	string listPath;
	listPath.append(dirPath);
	listPath.append(avmsDatasetListPath);
	this->idx = 0;

	//left, back, rear, right ¼ø¼­·Î
	ifstream listFile(listPath);
	int i = 0;
	AvmsDataset temp;
	while (listFile.is_open() && !listFile.eof()){
		string str;
		getline(listFile, str);
		temp.data[i % 4].clear();
		temp.data[i % 4].append(str);
		if (i % 4 == 3)
			this->fileNames.push_back(temp);
		i++;
	}
	listFile.close();
}

AvmsDatasetReader::~AvmsDatasetReader()
{
}

bool AvmsDatasetReader::end(){
	return fileNames.size() <= idx;
}

void AvmsDatasetReader::read(Mat* dest){
	assert(("end of dataset", !end()));

	cout << "----- read once" << endl;
	for (int i = 0; i < 4; i++){
		string filePath = this->dirPath + this->fileNames[idx].data[i];
		dest[i] = imread(filePath);
		cout << filePath << endl;
	}
	cout << "-----" << endl << endl;

	this->idx++;
}