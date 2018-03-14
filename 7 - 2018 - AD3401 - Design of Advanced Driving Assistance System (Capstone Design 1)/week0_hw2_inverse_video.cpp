#include "week0_hw2_inverse_video.h"
#include "utility.h"
#include <time.h>

using namespace cv;
using namespace utility;

void save_inverse_video(VideoCapture&, VideoWriter&);

/**
	�������� ���Ϸκ��� �Է¹޴´�.
	backward �� ����ϵ��� ���Ϸ� ����Ѵ�.
	*/
int week0_hw2_inverse_video::run(){
	VideoCapture capture("resource/sample_seq.avi");
	if (!capture.isOpened()){
		printf("invalid file");
		getchar();
		return -1;
	}

	int widthFrame = capture.get(CV_CAP_PROP_FRAME_WIDTH);
	int heightFrame = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
	double fps = capture.get(CV_CAP_PROP_FPS);
	VideoWriter writer("resource/sample_seq_w0h2_inverse_video.avi", CV_FOURCC('M', 'J', 'P', 'G'), fps, Size(widthFrame, heightFrame));

	save_inverse_video(capture, writer);
	showVideo("resource/sample_seq_w0h2_inverse_video.avi");

	capture.release();
	writer.release();

	return 0;
}

void save_inverse_video(VideoCapture& capture, VideoWriter& writer){
	int max = capture.get(CV_CAP_PROP_FRAME_COUNT);
	int position = max - 1;
	double percent = 0.0;
	printf("%d ���� �������� rewind �ؼ� �����մϴ�.\n", max);
	clock_t prev, now;
	prev = clock();
	while (0 <= position){
		Mat frame;

		if (position % 20 == 0){
			now = clock();
			if (now - prev > 1000){
				prev = now;
				int percent = (1 - (double)position / max) * 100;
				printf("%d percent complete...\n", percent);
			}
		}

		capture.set(CV_CAP_PROP_POS_FRAMES, position--);
		capture.read(frame);
		writer.write(frame);
	}
	printf("finished.\n");
}
