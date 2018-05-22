//#include "opencv2\opencv.hpp"
//#include "framestream.h"
//#include "utility.h"
//#include "vanishingPoint.h"
//#include <ctime>
//
//#define RECENT_FRAME_COUNT 3
//#define RECENT_FRAME_INTERVAL 3
//#define TEST_FILE_IO false
//#define FPS 10
//#define IMSHOW_DEBUG true //with out current frame
//
//using namespace cv;
//
//int main(){
//	FrameStream fStream(FrameStream::FS_TYPE_IMAGES, "resource/cordova/2/list.txt", "resource/cordova/2/", RECENT_FRAME_COUNT, RECENT_FRAME_INTERVAL);
//	//FrameStream fStream2(FrameStream::FS_TYPE_VIDEO, "resource/sample_seq.mp4", RECENT_FRAME_COUNT);
//
//	// print file list
//	if (TEST_FILE_IO){
//		printf("path : %s\n", fStream.filepath);
//		for (auto a : fStream.imagesData){
//			int sizeOfString = a.size();
//			char* temp = (char*)malloc(sizeof(char)*sizeOfString);
//			utility::vecChar2CharPtr(a, temp, sizeOfString);
//			printf("%s\t", temp);
//		}
//	}
//
//
//	Point2i center(fStream.currentFrame.cols / 2, fStream.currentFrame.rows / 2);
//	Mat weightedSumRoi;
//	Rect ROI(center.x - 200, 250, 400, 100);
//	cvtColor(fStream.currentFrame, weightedSumRoi, CV_BGR2GRAY);
//	weightedSumRoi = Mat(weightedSumRoi, ROI);
//	float weight = 0.1;
//	while (fStream.currentFrame.total() != 0){
//		clock_t start = clock();
//
//		// perspective warping manually
//		int cols = fStream.currentFrame.cols;
//		Point2f leftTop(310, 190), leftBottom(158, 415);
//		Point2f src[4] = {
//			leftTop, Point2f(cols - leftTop.x, leftTop.y),
//			leftBottom, Point2f(cols - leftBottom.x, leftBottom.y) };
//		/*Point2f src[4] = {
//		Point2f(290, 240), Point2f(310,240),
//		Point2f(116, 508), Point2f(546, 508) };*/
//		float leftMiddle = (src[0].x + src[2].x) / 2;
//		float rightMiddle = (src[1].x + src[3].x) / 2;
//		Point2f dest[4] = {
//			Point2f(leftMiddle, src[0].y), Point2f(rightMiddle, src[0].y),
//			Point2f(leftMiddle, src[2].y), Point2f(rightMiddle, src[2].y) };
//		//-------------------
//
//		// vanishing point for extended perspective warping.
//		// 2/3 지점에 안착시켜본다.
//		Point2f vanishingPoint = ImageProcessing::findVanishingPoint(src[0], src[2], src[1], src[3]);
//		Point2f newLeftTop = Point2f(cols / 2.25, src[0].y);
//		Point2f newLeftBottom = ImageProcessing::findVanishingPoint(newLeftTop, vanishingPoint, src[2], Point2f(vanishingPoint.x, src[2].y));
//		Point2f newSrc[4] = {
//			newLeftTop, Point2f(cols - newLeftTop.x, newLeftTop.y),
//			newLeftBottom, Point2f(cols - newLeftBottom.x, newLeftBottom.y) };
//		float newLeftMiddle = (newSrc[0].x + newSrc[2].x) / 2;
//		float newRightMiddle = (newSrc[1].x + newSrc[3].x) / 2;
//		Point2f newDest[4] = {
//			Point2f(newLeftMiddle, newSrc[0].y), Point2f(newRightMiddle, newSrc[0].y),
//			Point2f(newLeftMiddle, newSrc[2].y), Point2f(newRightMiddle, newSrc[2].y) };
//		for (int i = 0; i < 4; i++){
//			src[i] = newSrc[i];
//			dest[i] = newDest[i];
//		}
//		//-------------------
//
//		Mat c_edge = Mat(fStream.currentFrame, Rect(0, src[0].y, cols, src[2].y - src[0].y));
//		imshow("c", c_edge);
//		float src0y = src[0].y;
//		for (int i = 0; i < 4; i++){
//			src[i].y -= src0y;
//			dest[i].y -= src0y;
//		}
//		Mat perspectiveTransform = getPerspectiveTransform(src, dest);
//		Mat perspectiveFrame;
//		warpPerspective(c_edge, perspectiveFrame, perspectiveTransform, c_edge.size());
//		resize(perspectiveFrame, perspectiveFrame, Size(), 1, 3);
//		imshow("perpective", perspectiveFrame);
//		Mat lining = c_edge.clone();
//		line(lining, src[0], src[2], Scalar(255, 0, 0));
//		line(lining, src[1], src[3], Scalar(255, 0, 0));
//		line(lining, dest[0], dest[2], Scalar(0, 255, 0));
//		line(lining, dest[1], dest[3], Scalar(0, 255, 0));
//		imshow("line", lining);
//
//		//Canny(fStream.currentFrame, fStream.currentFrame, 500, 600);
//		Mat roi = Mat(fStream.currentFrame, ROI);
//		Mat hsv;
//		cvtColor(roi, hsv, CV_BGR2HSV);
//		vector<Mat> hsv_vector;
//		split(hsv, hsv_vector);
//		Mat yellow = Mat::zeros(roi.size(), roi.type());
//		yellow.setTo(255, (30 / 2 <= hsv_vector[0]) & (hsv_vector[0] < 64 / 2) & (hsv_vector[1] >= 255 * 0.15) & (hsv_vector[2] > 0.3 * 255));
//		Mat white = Mat::zeros(roi.size(), roi.type());
//		white.setTo(255, (hsv_vector[1] < 255 * 0.15) & (hsv_vector[2] > 0.8 * 255));
//		yellow = white | yellow;
//		//imshow("before", yellow);
//		//Canny(yellow, yellow, 800, 400);
//		imshow("yellow", yellow);
//		cvtColor(yellow, yellow, CV_BGR2GRAY);
//		weightedSumRoi = (1 - weight) * weightedSumRoi + weight * yellow;
//
//
//		Mat lines;
//		Mat otsu_weightedSumRoi;
//		threshold(weightedSumRoi, otsu_weightedSumRoi, 0, 255, CV_THRESH_OTSU | THRESH_BINARY);
//		HoughLinesP(otsu_weightedSumRoi, lines, 1, 3.141592 / 360.0, 75);
//		imshow("otsh", otsu_weightedSumRoi);
//		for (int i = 0; i < lines.cols; i++){
//			Vec4i point = lines.at<Vec4i>(0, i);
//			line(roi, Point(point[0], point[1]), Point(point[2], point[3]), Scalar(0, 0, 255), 2);
//		}
//		/*vector<Vec2f> lines;
//		Mat otsu_weightedSumRoi;
//		threshold(weightedSumRoi, otsu_weightedSumRoi, 0, 255, CV_THRESH_OTSU | THRESH_BINARY);
//		HoughLines(otsu_weightedSumRoi, lines, 1, 3.141592 / 360.0, 40, 0, 0);
//		imshow("otsh", otsu_weightedSumRoi);
//		if (lines.size() < 200)
//		for (int i = 0; i < lines.size(); i++){
//		float rho = lines[i][0];
//		float theta = lines[i][1];
//		Point pt1, pt2;
//		double a = cos(theta), b = sin(theta);
//		double x0 = a*rho, y0 = b*rho;
//		pt1.x = cvRound(x0 + 1000 * (-b));
//		pt1.y = cvRound(y0 + 1000 * (a));
//		pt2.x = cvRound(x0 - 1000 * (-b));
//		pt2.y = cvRound(y0 - 1000 * (a));
//		line(roi, pt1, pt2, Scalar(0, 0, 255), 3, CV_AA);
//		}
//		else
//		printf("%d 검출\n", lines.size());*/
//
//		imshow("yellow + white", yellow);
//		imshow("line-drawed roi", roi);
//		imshow("weightedSumRoi", weightedSumRoi);
//
//		int i = RECENT_FRAME_COUNT;
//		if (IMSHOW_DEBUG){
//			for (auto f : fStream.recentFrames){
//				char buf[2];
//				String a = "recent";
//				_itoa(--i, buf, 10);
//				a.append(buf);
//				resize(f, f, Size(), 0.5, 0.5);
//				imshow(a, f);
//			}
//		}
//
//		fStream.nextFrame();
//
//		clock_t end = clock();
//		int mSPF = 1000 / FPS - (end - start); //ms per frame
//		if (mSPF < 1){
//			printf("frame loss : %d ms\n", -mSPF);
//			waitKey(1);
//		}
//		else{
//			printf("wait %d ms\n", mSPF);
//			waitKey(mSPF);
//		}
//	}
//
//	system("pause");
//	return 0;
//}