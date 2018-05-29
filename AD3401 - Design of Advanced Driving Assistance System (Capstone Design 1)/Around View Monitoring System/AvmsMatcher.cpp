#include "AvmsMatcher.h"

void stitch(Mat src, Mat dest, Mat& result, Mat H){

	vector<Point2f> boundingRect(4), boundedRect(4);
	boundingRect[0] = Point2f(0, 0);
	boundingRect[1] = Point2f(src.cols, src.rows);
	boundingRect[2] = Point2f(0, src.rows);
	boundingRect[3] = Point2f(src.cols, 0);
	perspectiveTransform(boundingRect, boundedRect, H);

	Point2f rectMinPt = boundedRect[0],
		rectMaxPt = boundedRect[0];

	for (auto point : boundedRect){
		if (point.x < rectMinPt.x)
			rectMinPt.x = point.x;
		if (rectMaxPt.x < point.x)
			rectMaxPt.x = point.x;
		if (point.y < rectMinPt.y)
			rectMinPt.y = point.y;
		if (rectMaxPt.y < point.y)
			rectMaxPt.y = point.y;
	}
	double transitionX, transitionY;
	if (rectMinPt.x > 0)
		rectMaxPt.x += rectMinPt.x, transitionX = 0;
	else
		transitionX = -rectMinPt.x;
	if (rectMinPt.y > 0)
		rectMaxPt.y += rectMinPt.y, transitionY = 0;
	else
		transitionY = -rectMinPt.y;

	Mat transition = (Mat_<double>(3, 3) << 1, 0, transitionX, 0, 1, transitionY, 0, 0, 1);
	Size stitchSize = Size(max(rectMaxPt.x - rectMinPt.x, dest.cols - rectMinPt.x), max(rectMaxPt.y - rectMinPt.y, dest.rows - rectMinPt.y));
	warpPerspective(src, result, transition*H, stitchSize);

	Mat warpDest;
	warpPerspective(dest, warpDest, transition, stitchSize);

	Mat intersect = Mat::zeros(result.size(), CV_8UC1);
	Mat grayResult, grayWarpDest;
	cvtColor(result, grayResult, CV_BGR2GRAY);
	cvtColor(warpDest, grayWarpDest, CV_BGR2GRAY);
	intersect.setTo(255, grayResult > 0);
	intersect.setTo(0, grayWarpDest == 0);

	Mat result_intersect(result.size(), result.type(), 0), warpDest_intersect(result.size(), result.type(), 0);
	result.copyTo(result_intersect, intersect);
	warpDest.copyTo(warpDest_intersect, intersect);

	result = (result - result_intersect / 2) + (warpDest - warpDest_intersect / 2);
}

Mat AvmsMatcher::matchImage(Mat left, Mat back, Mat rear, Mat right){

	/*
	initModule_nonfree();

	GaussianBlur(rear, rear, Size(5, 5), 0);
	GaussianBlur(back, back, Size(5, 5), 0);

	SurfFeatureDetector surfDetector;
	vector<KeyPoint> rearKeyPoint, backKeyPoint;
	Mat rearDescriptor, backDescriptor;

	surfDetector.detect(rear, rearKeyPoint);
	surfDetector.detect(back, backKeyPoint);
	surfDetector.compute(rear, rearKeyPoint, rearDescriptor);
	surfDetector.compute(back, backKeyPoint, backDescriptor);

	FlannBasedMatcher matcher;
	vector<DMatch> matches_rb;
	matcher.match(rearDescriptor, backDescriptor, matches_rb);
	double min_dist = matches_rb[0].distance;
	for (auto match : matches_rb){
		if (match.distance < min_dist)
			min_dist = match.distance;
	}
	vector<DMatch> good_matches_rb;
	for (auto match : matches_rb){
		if (match.distance <= 10*min_dist)
			good_matches_rb.push_back(match);
	}
	vector<Point2f> rearPts, backPts;
	for (auto match : good_matches_rb){
		rearPts.push_back(rearKeyPoint[match.queryIdx].pt);
		backPts.push_back(backKeyPoint[match.trainIdx].pt);
	}
	Mat tt;
	drawMatches(rear, rearKeyPoint, back, backKeyPoint, good_matches_rb, tt);
	imshow("tt", tt);
	Mat homography_rb = findHomography(rearPts, backPts, RANSAC);
	
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0); obj_corners[1] = cvPoint(100, 0);
	obj_corners[2] = cvPoint(100, 100); obj_corners[3] = cvPoint(0, 100);
	std::vector<Point2f> scene_corners(4);
	//perspectiveTransform(obj_corners, scene_corners, homography_rb);

	
	line(rear, obj_corners[0], obj_corners[1], Scalar(0, 255, 0), 4);
	line(rear, obj_corners[1], obj_corners[2], Scalar(0, 255, 0), 4);
	line(rear, obj_corners[2], obj_corners[3], Scalar(0, 255, 0), 4);
	line(rear, obj_corners[3], obj_corners[0], Scalar(0, 255, 0), 4);

	line(back, scene_corners[0], scene_corners[1], Scalar(0, 255, 0), 4);
	line(back, scene_corners[1], scene_corners[2], Scalar(0, 255, 0), 4);
	line(back, scene_corners[2], scene_corners[3], Scalar(0, 255, 0), 4);
	line(back, scene_corners[3], scene_corners[0], Scalar(0, 255, 0), 4);

	*/

	Mat H;
	vector<Point2f> src{
		Point2f(240, 167),
		Point2f(344, 187),
		Point2f(214, 268),
		Point2f(328, 287) };
	vector<Point2f> dest{
		Point2f(46, 147),
		Point2f(185, 150),
		Point2f(66, 270),
		Point2f(211, 267) };
	//Mat stitch_r_b;
	//H = getPerspectiveTransform(src, dest);
	//stitch(rear, back, stitch_r_b, H);


	src[0] = Point2f(127, 189);		src[1] = Point2f(93, 321);
	src[2] = Point2f(184, 195);		src[3] = Point2f(173, 330);
	dest[0] = Point2f(407, 286);	dest[1] = Point2f(269, 293);
	dest[2] = Point2f(421, 335);	dest[3] = Point2f(284, 344);
	Mat stitch_b_l;
	H = getPerspectiveTransform(src, dest);
	stitch(left, back, stitch_b_l, H);

	src[0] = Point2f(241, 168);		src[1] = Point2f(216, 268);
	src[2] = Point2f(345, 188);		src[3] = Point2f(330, 288);
	dest[0] = Point2f(48, 162);	dest[1] = Point2f(67, 293);
	dest[2] = Point2f(182, 166);	dest[3] = Point2f(212, 285);
	Mat stitch_r_bl;
	H = getPerspectiveTransform(src, dest);
	stitch(rear, stitch_b_l, stitch_r_bl, H);

	src[0] = Point2f(347, 189);		src[1] = Point2f(368, 310);
	src[2] = Point2f(251, 347);		src[3] = Point2f(228, 194);
	dest[0] = Point2f(205, 417);	dest[1] = Point2f(461, 385);
	dest[2] = Point2f(549, 542);	dest[3] = Point2f(185, 583);
	Mat stitch_r_rbl;
	H = getPerspectiveTransform(src, dest);
	stitch(right, stitch_r_bl, stitch_r_rbl, H);

	return stitch_r_rbl;
}