#include "opencv2\opencv.hpp"
#include "utility.h"
#pragma once

using namespace cv;

class FrameStream
{
	int frame_stream_type;
	VideoCapture videoData;
	int imagesDataPosition;
public:
	vector<vector<char>> imagesData;
	char* filepath;
	static const int FS_TYPE_IMAGES = 0;
	static const int FS_TYPE_VIDEO = 1;
	//static const int FS_TYPE_CAM = 2;
	
	Mat currentFrame;
	vector<Mat> recentFrames;
	unsigned int recent_frame_count;
	unsigned int recent_frame_interval;
	const char* filename;

	FrameStream(int frame_stream_type, char* filename, char* filepath, int recent_frame_count, int recent_frame_interval);
	~FrameStream();

	void nextFrame();

};
