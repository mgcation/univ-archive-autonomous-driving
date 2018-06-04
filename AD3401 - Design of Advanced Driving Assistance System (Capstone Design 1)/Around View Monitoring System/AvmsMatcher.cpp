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

	Mat result_[2] = { result - result_intersect / 2, warpDest - warpDest_intersect / 2 };
	//result = result_[0] + result_[1];

	// --- pyramid blending
	// https://docs.opencv.org/3.1.0/dc/dff/tutorial_py_pyramids.html
	vector<Mat> gp[2];
	vector<Mat> lp[2];
	int pyrLevel = 6; // 1 ÀÌ»ף
	for (int i = 0; i < 2; i++){
		gp[i].push_back(result_[i]);
		for (int j = 0; j < pyrLevel-1; j++){
			Mat downGP;
			pyrDown(gp[i][j], downGP);
			gp[i].push_back(downGP);
		}

		lp[i].push_back(gp[i][pyrLevel - 1]);
		for (int j = pyrLevel - 1; j > 0; j--){
			Mat gaussianEdge;
			pyrUp(gp[i][j], gaussianEdge, gp[i][j-1].size());
			Mat lap;
			subtract(gp[i][j - 1], gaussianEdge, lap);
			lp[i].push_back(lap);
		}
	}

	vector<Mat> lp_sum;
	for (int i = 0; i < pyrLevel; i++)
		lp_sum.push_back(lp[0][i] + lp[1][i]);

	result = lp_sum[0];
	for (int i = 1; i < pyrLevel; i++){
		pyrUp(result, result, lp_sum[i].size());
		result += lp_sum[i];
	}
}

Mat findPerspectiveMatrix(Mat fromImage, Mat toImage){
	initModule_nonfree();
	
	Mat fromClone, toClone;

	GaussianBlur(fromImage, fromClone, Size(3, 3), 0);
	GaussianBlur(toImage, toClone, Size(3, 3), 0);

	/*
	// binarize

	cvtColor(fromClone, fromClone, CV_BGR2GRAY);
	cvtColor(toClone, toClone, CV_BGR2GRAY);
	threshold(fromClone, fromClone, 0, 255, THRESH_BINARY | THRESH_OTSU);
	threshold(toClone, toClone, 0, 255, THRESH_BINARY | THRESH_OTSU);
	*/
	SurfFeatureDetector surfDetector;

	vector<KeyPoint> fromKeyPoint, toKeyPoint;
	Mat fromDescriptor, toDescriptor;

	surfDetector.detect(fromClone, fromKeyPoint);
	surfDetector.detect(toClone, toKeyPoint);
	surfDetector.compute(fromClone, fromKeyPoint, fromDescriptor);
	surfDetector.compute(toClone, toKeyPoint, toDescriptor);

	FlannBasedMatcher matcher;
	vector<DMatch> matches_pair;
	matcher.match(fromDescriptor, toDescriptor, matches_pair);
	double min_dist = matches_pair[0].distance;
	for (auto match : matches_pair){
		if (match.distance < min_dist)
			min_dist = match.distance;
	}
	vector<DMatch> good_matches_pair;
	for (auto match : matches_pair){
		if (match.distance <= 10 * min_dist)
			good_matches_pair.push_back(match);
	}
	vector<Point2f> fromPts, toPts;
	for (auto match : matches_pair){
		fromPts.push_back(fromKeyPoint[match.queryIdx].pt);
		toPts.push_back(toKeyPoint[match.trainIdx].pt);
	}
	Mat tt;
	drawMatches(fromClone, fromKeyPoint, toClone, toKeyPoint, matches_pair, tt);
	imshow("tt", tt);
	waitKey(0);
	Mat homography = findHomography(fromPts, toPts, RANSAC);

	std::vector<Point2f> fromCorners(4);
	fromCorners[0] = cvPoint(0, 0); fromCorners[1] = cvPoint(100, 0);
	fromCorners[2] = cvPoint(100, 100); fromCorners[3] = cvPoint(0, 100);
	std::vector<Point2f> toCorners(4);
	
	perspectiveTransform(fromCorners, toCorners, homography);


	line(fromImage, fromCorners[1], fromCorners[2], Scalar(0, 255, 0), 4);
	line(fromImage, fromCorners[2], fromCorners[3], Scalar(0, 255, 0), 4);
	line(fromImage, fromCorners[0], fromCorners[1], Scalar(0, 255, 0), 4);
	line(fromImage, fromCorners[3], fromCorners[0], Scalar(0, 255, 0), 4);

	line(toImage, toCorners[0], toCorners[1], Scalar(0, 0, 255), 4);
	line(toImage, toCorners[1], toCorners[2], Scalar(0, 0, 255), 4);
	line(toImage, toCorners[2], toCorners[3], Scalar(0, 0, 255), 4);
	line(toImage, toCorners[3], toCorners[0], Scalar(0, 0, 255), 4);

	return homography;
}

Mat AvmsMatcher::matchImage(Mat left, Mat back, Mat rear, Mat right){

	/*
	// surf matching

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

	/*
	// ROI matching

	Mat right_right_roi(right, Rect(370, 106, 108, 200));
	Mat rear_right_roi(rear, Rect(5, 105, 184, 202));
	Mat rear_center_roi(rear, Rect(226, 22, 253, 204));
	Mat back_center_roi(back, Rect(25, 27, 280, 176));
	Mat back_left_roi(back, Rect(268, 138, 211, 173));
	Mat left_left_roi(left, Rect(0, 68, 129, 291));
	Mat right2rear = findPerspectiveMatrix(right_right_roi, rear_right_roi);
	Mat rear2back = findPerspectiveMatrix(rear_center_roi, back_center_roi);
	Mat left2back = findPerspectiveMatrix(left_left_roi, back_left_roi);
	imshow("rr1", right_right_roi);
	imshow("rr2", rear_right_roi);
	imshow("rb1", rear_center_roi);
	imshow("rb2", back_center_roi);
	imshow("lb1", back_left_roi);
	imshow("lb2", left_left_roi);

	Mat stitch_rr_rear, stitch_rrb_back, stitch_rrbl_back;
	stitch(right, rear, stitch_rr_rear, right2rear);
	stitch(stitch_rr_rear, back, stitch_rrb_back, rear2back);
	stitch(left, stitch_rrb_back, stitch_rrbl_back, left2back);
	return stitch_rrbl_back;
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

Mat pyramidBlending(){
	// study@openCV : https://docs.opencv.org/3.1.0/dc/dff/tutorial_py_pyramids.html

	return Mat();
}

Mat AvmsMatcher::histogramMatching(Mat colorSource, Mat colorTarget){
	// study@kr : http://swprog.tistory.com/entry/Histogram-Matching-Color-mapping-Color-Transfer
	vector<Mat> source3ch;
	vector<Mat> target3ch;
	vector<Mat> result3ch(3);
	Mat result;

	split(colorSource, source3ch);
	split(colorTarget, target3ch);

	int histSize = 256;
	float range[] = { 0, 256 };
	const float* histRange = {range};
	for (int ch = 0; ch < 3; ch++){
		vector<Mat> hists(2), cdfs(2);
		calcHist(&source3ch[ch], 1, 0, Mat(), hists[0], 1, &histSize, &histRange, true, false);
		calcHist(&target3ch[ch], 1, 0, Mat(), hists[1], 1, &histSize, &histRange, true, false);

		for (auto hist : hists){
			double min, max;
			minMaxLoc(hist, &min, &max);
			if (max == 0){
				printf("err : histogram matching, hist max = 0\n");
				max = 1;
			}
			normalize(hist, hist, min / max, 1, NORM_MINMAX);
		}
		for (int i = 0; i < 2; i++){
			cdfs[i] = hists[i].clone();
			for (int r = 1; r < cdfs[i].rows; r++){
				cdfs[i].at<float>(r, 0) += cdfs[i].at<float>(r - 1, 0);
			}
			double min, max;
			minMaxLoc(cdfs[i], &min, &max);
			if (max == 0){
				printf("err : histogram matching, hist cdf max = 0\n");
				max = 1;
			}
			normalize(cdfs[i], cdfs[i], min / max, 1, NORM_MINMAX);
		}

		Mat lookUpTable(1, 256, CV_8U);
		int preIdx = 0;
		for (int r = 0; r < cdfs[0].rows; r++){
			float sourceCDFPixel = cdfs[0].at<float>(r, 0);

			for (int idx = preIdx; idx < cdfs[1].rows; idx++){
				float targetCDFPixel = cdfs[1].at<float>(idx, 0);

				if (abs(sourceCDFPixel - targetCDFPixel) < 0.001 || sourceCDFPixel < targetCDFPixel){
					lookUpTable.at<uchar>(0, r) = idx;
					preIdx = idx;
					break;
				}
			}
		}

		LUT(source3ch[ch], lookUpTable, result3ch[ch]);
	}
	merge(result3ch, result);

	return result;
}