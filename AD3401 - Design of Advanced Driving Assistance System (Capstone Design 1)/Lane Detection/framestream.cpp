#include "framestream.h"

/*

(FS_TYPE_IMAGES case)
char* filename <- must be contain all image filename like below
{f00001.png}'\n'{f00002.png}'\n'{filename.ext}'\n'{filename.ext}'\n'...

(FS_TYPE_VIDEO case)
computer must support video's codec.

(FS_TYPE_CAM case)
not yet

*/
FrameStream::FrameStream(int frame_stream_type, char* filename, char*filepath, int recent_frame_count, int recent_frame_interval)
{
	assert("frame_stream_type : {FS_TYPE_IMAGES, FS_TYPE_VIDEO} where FrameStream's static constant.", 0 <= frame_stream_type && frame_stream_type <= 1);

	this->filepath = (char*)malloc(sizeof(char)*strlen(filepath));
	strcpy(this->filepath, filepath);
	this->frame_stream_type = frame_stream_type;
	this->recent_frame_count = recent_frame_count;
	this->recent_frame_interval = recent_frame_interval;

	switch (frame_stream_type){
	case FS_TYPE_IMAGES:{
		FILE* file;
		if (fopen_s(&file, filename, "r") != 0)
			assert(!"can't open file");

		//fread(, sizeof(char), d, file);
		char buffer[1000];
		int idx = 0;
		while (feof(file) == 0 && ferror(file) == 0) {
			char temp = fgetc(file);
			if (strlen(buffer) == 1000 - 2)
				assert(!"buffer over flow (file name length can't over 1000)");
			if (temp == '\n'){
				buffer[idx++] = '\0';
				if (1 < idx){
					imagesData.push_back(vector<char>(buffer, buffer+idx));
				}
				idx = 0;
			}
			else{
				buffer[idx++] = temp;
			}
		}
		imagesDataPosition = 0;
		fclose(file);
		break;
	}
	case FS_TYPE_VIDEO:{
		this->videoData.open(filename);
		break;
	}
	}

	nextFrame();
}

FrameStream::~FrameStream()
{
	currentFrame.release();
	for (auto f : recentFrames)
		f.release();
}

void FrameStream::nextFrame()
{
	switch (this->frame_stream_type){
	case FS_TYPE_IMAGES:{
		if (currentFrame.total() != 0){
			recentFrames.push_back(currentFrame);
			if (recentFrames.size() == recent_frame_count + 1)
				recentFrames.erase(recentFrames.begin());
		}
		if (imagesDataPosition == imagesData.size())
			this->currentFrame = Mat();
		else{
			int sizeOfString = imagesData[imagesDataPosition].size();
			char* temp = (char*)malloc(sizeof(char)*sizeOfString);
			utility::vecChar2CharPtr(imagesData[imagesDataPosition], temp, sizeOfString);

			int sizeOfFullName = sizeOfString + strlen(filepath);
			char* fp = (char*)malloc(sizeof(char)*sizeOfFullName);
			strcpy(fp, filepath);
			strcat(fp, temp);
			this->currentFrame = imread(fp);

			imagesDataPosition++;
			free(temp);
			free(fp);
		}
		break;
	}
	case FS_TYPE_VIDEO:{

		if (false)
			this->currentFrame = Mat();
		break;
	}
	default:{
		this->currentFrame = Mat();
	}
	}
}
