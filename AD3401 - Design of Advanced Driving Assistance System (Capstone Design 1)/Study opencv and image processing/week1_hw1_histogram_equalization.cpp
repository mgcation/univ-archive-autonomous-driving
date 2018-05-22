#include "week1_hw1_histogram_equalization.h"

using namespace cv;
using namespace utility;

/*
	Histogram equalization ���̺귯���� openCV ���� ã�� �����Ѵ�.

	>> https://stackoverflow.com/a/15009815/7354469
	ä���� �и��ؼ� ���������� ��������¡�ϴ� ���� ���� ������ �ƴ� ���� ���� �����ϹǷ� �׷��̽����Ͽ����� �� ������ RGB������ �� ���� �ʴ´�.
	�׷��� ���� ���� �����ϴ� ����������� ��ȯ�ϸ� ȿ������ ����� ���� �� �ִ�.
	���� ��� ������� : HSV / HLS , YUV , YCbCr
	�� �� YCbCr�� ������ �̹��� ������ ����Ǿ��־� ���� ��ȣ�ȴ�.
	Y, Cb, Cr 3���� ä�� �� ���� ä���� Y�� ���� ����ȭ ��Ų �� �ٽ� RGB ����������� ��ȯ�Ͽ� ��� �յ�ȭ ȿ���� ��´�.
*/
int week1_hw1_histogram_equalization::run()
{
	Mat source;
	vector<Mat> channels;
	Mat result;


	my_imread(source, "resource/lena.png");
	imshow("week1_hw1_histogram_equalization : source", source);


	split(source, channels);
	for (auto channel : channels){
		equalizeHist(channel, channel);
	}
	merge(channels, result);
	imshow("week1_hw1_histogram_equalization : result1 just BGR", result);


	cvtColor(source, source, COLOR_BGR2YCrCb);
	split(source, channels);
	equalizeHist(channels[0], channels[0]);
	merge(channels, result);
	cvtColor(result, result, COLOR_YCrCb2BGR);
	imshow("week1_hw1_histogram_equalization : result2 pass YCrCb", result);

	waitKey(0);
	return 0;
}
