#include "week1_hw1_histogram_equalization.h"

using namespace cv;
using namespace utility;

/*
	Histogram equalization 라이브러리를 openCV 에서 찾아 적용한다.

	>> https://stackoverflow.com/a/15009815/7354469
	채널을 분리해서 개별적으로 이퀄라이징하는 것은 색상 성분이 아닌 강도 값에 의존하므로 그레이스케일에서는 잘 되지만 RGB에서는 잘 되지 않는다.
	그래서 강도 값을 구분하는 색상공간으로 변환하면 효과적인 결과를 얻을 수 있다.
	강도 사용 색상공간 : HSV / HLS , YUV , YCbCr
	이 중 YCbCr이 디지털 이미지 용으로 설계되어있어 자주 선호된다.
	Y, Cb, Cr 3개의 채널 중 강도 채널인 Y의 값을 평준화 시킨 뒤 다시 RGB 색상공간으로 변환하여 대비 균등화 효과를 얻는다.
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
