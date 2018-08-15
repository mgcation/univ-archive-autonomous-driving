#include "AvmsMatcher.h"

//http://www.morethantechnical.com/2011/11/13/just-a-simple-laplacian-pyramid-blender-using-opencv-wcode/

class LaplacianBlending {
private:
	Mat_<Vec3f> left;
	Mat_<Vec3f> right;
	Mat_<float> blendMask;

	vector<Mat_<Vec3f> > leftLapPyr, rightLapPyr, resultLapPyr;
	Mat leftSmallestLevel, rightSmallestLevel, resultSmallestLevel;
	vector<Mat_<Vec3f> > maskGaussianPyramid; //masks are 3-channels for easier multiplication with RGB

	int levels;


	void buildPyramids() {
		buildLaplacianPyramid(left, leftLapPyr, leftSmallestLevel);
		buildLaplacianPyramid(right, rightLapPyr, rightSmallestLevel);
		buildGaussianPyramid();
	}

	void buildGaussianPyramid() {
		assert(leftLapPyr.size() > 0);

		maskGaussianPyramid.clear();
		Mat currentImg;
		cvtColor(blendMask, currentImg, CV_GRAY2BGR);
		maskGaussianPyramid.push_back(currentImg); //highest level

		currentImg = blendMask;
		for (int l = 1; l<levels + 1; l++) {
			Mat _down;
			if (leftLapPyr.size() > l) {
				pyrDown(currentImg, _down, leftLapPyr[l].size());
			}
			else {
				pyrDown(currentImg, _down, leftSmallestLevel.size()); //smallest level
			}

			Mat down;
			cvtColor(_down, down, CV_GRAY2BGR);
			maskGaussianPyramid.push_back(down);
			currentImg = _down;
		}
	}

	void buildLaplacianPyramid(const Mat& img, vector<Mat_<Vec3f> >& lapPyr, Mat& smallestLevel) {
		lapPyr.clear();
		Mat currentImg = img;
		for (int l = 0; l < levels; l++) {
			Mat down, up;
			pyrDown(currentImg, down);
			pyrUp(down, up, currentImg.size());
			Mat lap = currentImg - up;
			lapPyr.push_back(lap);
			currentImg = down;
		}
		currentImg.copyTo(smallestLevel);
	}

	Mat_<Vec3f> reconstructImgFromLapPyramid() {
		Mat currentImg = resultSmallestLevel;
		for (int l = levels - 1; l >= 0; l--) {
			Mat up;

			pyrUp(currentImg, up, resultLapPyr[l].size());
			currentImg = up + resultLapPyr[l];
		}
		return currentImg;
	}

	void blendLapPyrs() {
		resultSmallestLevel = leftSmallestLevel.mul(maskGaussianPyramid.back()) +
			rightSmallestLevel.mul(Scalar(1.0, 1.0, 1.0) - maskGaussianPyramid.back());
		for (int l = 0; l < levels; l++) {
			Mat A = leftLapPyr[l].mul(maskGaussianPyramid[l]);
			Mat antiMask = Scalar(1.0, 1.0, 1.0) - maskGaussianPyramid[l];
			Mat B = rightLapPyr[l].mul(antiMask);
			Mat_<Vec3f> blendedLevel = A + B;

			resultLapPyr.push_back(blendedLevel);
		}
	}

public:
	LaplacianBlending(const Mat_<Vec3f>& _left, const Mat_<Vec3f>& _right, const Mat_<float>& _blendMask, int _levels) :
		left(_left), right(_right), blendMask(_blendMask), levels(_levels)
	{
		assert(_left.size() == _right.size());
		assert(_left.size() == _blendMask.size());
		buildPyramids();
		blendLapPyrs();
	};

	Mat_<Vec3f> blend() {
		return reconstructImgFromLapPyramid();
	}
};

Mat_<Vec3f> LaplacianBlend(const Mat_<Vec3f>& l, const Mat_<Vec3f>& r, const Mat_<float>& m) {
	LaplacianBlending lb(l, r, m, 4);
	return lb.blend();
}


Mat AvmsMatcher::matchImage(Mat left, Mat back, Mat rear, Mat right){

	// try: surf matching
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

	// try: ROI matching
	/*

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
	vector<Point2f> src(4);
	//{	Point2f(240, 167),		Point2f(344, 187),		Point2f(214, 268),		Point2f(328, 287)	};
	vector<Point2f> dest(4);
	//{	Point2f(46, 147), 	Point2f(185, 150),	Point2f(66, 270),	Point2f(211, 267)	};


	// hard coded matching
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

Mat AvmsMatcher::histogramMatching(Mat colorSource, Mat colorTarget){
	// study@blog#kr : http://swprog.tistory.com/entry/Histogram-Matching-Color-mapping-Color-Transfer
	vector<Mat> source3ch;
	vector<Mat> target3ch;
	vector<Mat> result3ch(3);
	Mat result;

	split(colorSource, source3ch);
	split(colorTarget, target3ch);

	int histSize = 256;
	float range[] = { 0, 256 };
	const float* histRange = { range };
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

void AvmsMatcher::stitch(Mat src, Mat dest, Mat& result, Mat H){

	vector<Point2f> srcBoundRect(4), warpedBound(4);
	srcBoundRect[0] = Point2f(0, 0);
	srcBoundRect[1] = Point2f(src.cols, src.rows);
	srcBoundRect[2] = Point2f(0, src.rows);
	srcBoundRect[3] = Point2f(src.cols, 0);
	perspectiveTransform(srcBoundRect, warpedBound, H);

	Point2f rectMinPt = warpedBound[0],
		rectMaxPt = warpedBound[0];

	for (auto point : warpedBound){
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

	Mat warpSrc;
	warpPerspective(src, warpSrc, transition*H, stitchSize);

	Mat warpDest;
	warpPerspective(dest, warpDest, transition, stitchSize);

	// try : average blending, pyramid blending
	/*
	Mat intersectMask = Mat::zeros(stitchSize, CV_8UC1);
	Mat grayWarpSrc, grayWarpDest;
	cvtColor(warpSrc, grayWarpSrc, CV_BGR2GRAY);
	cvtColor(warpDest, grayWarpDest, CV_BGR2GRAY);
	intersectMask.setTo(255, grayWarpSrc > 0);
	intersectMask.setTo(0, grayWarpDest == 0);
	Mat warpSrc_intersect(stitchSize, warpSrc.type(), 0), warpDest_intersect(stitchSize, warpDest.type(), 0);
	warpSrc.copyTo(warpSrc_intersect, intersectMask);
	warpDest.copyTo(warpDest_intersect, intersectMask);
	vector<Mat> target = { warpSrc - warpSrc_intersect / 2, warpDest - warpDest_intersect / 2 };

	// 1. average blending
	result = target[0] + target[1];
	// 2. pyramid blending with average
	result = pyramidBlending(target);
	*/

	// try : pyramid blending with morphology segmentation
	result = morphologyPyrBlending(warpSrc, warpDest);
}

Mat AvmsMatcher::morphologyPyrBlending(Mat src1, Mat src2){
	assert(src1.size() == src2.size());
	Mat result;

	Size size = src1.size();
	Mat intersectMask = Mat::zeros(size, CV_8UC1);
	Mat graySrc1, graySrc2;
	cvtColor(src1, graySrc1, CV_BGR2GRAY);
	cvtColor(src2, graySrc2, CV_BGR2GRAY);
	intersectMask.setTo(255, graySrc1 > 0);
	intersectMask.setTo(0, graySrc2 == 0);

	Mat dilate_mask;
	dilate(intersectMask, dilate_mask, getStructuringElement(MORPH_RECT, Size(3, 3), Point(1, 1)));

	Mat non_inter_dilate_mask = dilate_mask.clone();
	non_inter_dilate_mask.setTo(0, graySrc1 > 0 | graySrc2 > 0);

	// flood fill
	int rows = non_inter_dilate_mask.rows;
	int cols = non_inter_dilate_mask.cols;
	Mat visited = Mat::zeros(non_inter_dilate_mask.size(), CV_8UC1);
	Point2f best_dilate_points[2] = { Point2f(-1, -1), Point2f(-1, -1) };
	float best_fillArea[2] = { -1, -1 };
	std::stack<Point2f> fFill_stack;
	for (int y = 0; y < rows; y++){
		for (int x = 0; x < cols; x++){
			float fillArea = 0;
			Point2f avgPoint;
			if (non_inter_dilate_mask.at<uchar>(y, x) != 0 && visited.at<uchar>(y, x) == 0){
				fFill_stack.push(Point2f(y, x));
				visited.at<uchar>(y, x) = 1;
				fillArea++;
				avgPoint = Point2f(x, y);
			}
			while (!fFill_stack.empty()){
				Point2f px = fFill_stack.top();
				fFill_stack.pop();

				int nearY, nearX;

				nearY = max(px.y - 1, 0.f);
				nearX = px.x;
				if (non_inter_dilate_mask.at<uchar>(nearY, nearX) != 0 && visited.at<uchar>(y, x) == 0){
					fFill_stack.push(Point2f(nearY, nearX));
					visited.at<uchar>(nearY, nearX) = 1;
					fillArea++;
					avgPoint = avgPoint * (fillArea - 1 / fillArea) + Point2f(nearX, nearY) * (1 / fillArea);
				}

				nearY = min(px.y + 1, rows - 1.f);
				// x = x
				if (non_inter_dilate_mask.at<uchar>(nearY, nearX) != 0 && visited.at<uchar>(y, x) == 0){
					fFill_stack.push(Point2f(nearY, nearX));
					visited.at<uchar>(nearY, nearX) = 1;
					fillArea++;
					avgPoint = avgPoint * (fillArea - 1 / fillArea) + Point2f(nearX, nearY) * (1 / fillArea);
				}

				nearY = px.y;
				nearX = max(px.x - 1, 0.f);
				if (non_inter_dilate_mask.at<uchar>(nearY, nearX) != 0 && visited.at<uchar>(y, x) == 0){
					fFill_stack.push(Point2f(nearY, nearX));
					visited.at<uchar>(nearY, nearX) = 1;
					fillArea++;
					avgPoint = avgPoint * (fillArea - 1 / fillArea) + Point2f(nearX, nearY) * (1 / fillArea);
				}

				//y = y
				nearX = min(px.x + 1, cols - 1.f);
				if (non_inter_dilate_mask.at<uchar>(nearY, nearX) != 0 && visited.at<uchar>(y, x) == 0){
					fFill_stack.push(Point2f(nearY, nearX));
					visited.at<uchar>(nearY, nearX) = 1;
					fillArea++;
					avgPoint = avgPoint * (fillArea - 1 / fillArea) + Point2f(nearX, nearY) * (1 / fillArea);
				}
			}
			if (best_fillArea[0] < fillArea){
				best_fillArea[1] = best_fillArea[0];
				best_fillArea[0] = fillArea;
				best_dilate_points[1] = best_dilate_points[0];
				avgPoint.y = max(avgPoint.y, 0.f);
				avgPoint.y = min(avgPoint.y, rows - 1.f);
				avgPoint.x = max(avgPoint.x, 0.f);
				avgPoint.x = min(avgPoint.x, cols - 1.f);
				best_dilate_points[0] = avgPoint;
			}
			else if (best_fillArea[1] < fillArea){
				best_fillArea[1] = fillArea;
				avgPoint.y = max(avgPoint.y, 0.f);
				avgPoint.y = min(avgPoint.y, rows - 1.f);
				avgPoint.x = max(avgPoint.x, 0.f);
				avgPoint.x = min(avgPoint.x, cols - 1.f);
				best_dilate_points[1] = avgPoint;
			}
		}
	}

	assert(("can't found best 2 dilate_points. try another blending at end of stitch() function.",
		best_dilate_points[0] != Point2f(-1, -1) && best_dilate_points[1] != Point2f(-1, -1)));
	// printf("2 segment point : %.2f, %.2f ...... %.2f, %.2f\n", best_dilate_points[0].y, best_dilate_points[0].x, best_dilate_points[1].y, best_dilate_points[1].x);

	Mat dilate_mask_bound_seg1 = dilate_mask.clone();
	dilate_mask_bound_seg1.setTo(0, intersectMask);
	dilate_mask_bound_seg1.setTo(0, graySrc1 == 0);
	Point maxIdx;
	minMaxLoc(dilate_mask_bound_seg1, 0, 0, 0, &maxIdx);

	Mat calc_seg1;
	calc_seg1 = dilate_mask_bound_seg1 + intersectMask;
	line(calc_seg1, best_dilate_points[0], best_dilate_points[1], 0, 2);

	Mat seg_mask1 = Mat::zeros(calc_seg1.size(), CV_8UC1);

	std::stack<Point> seg1_fFill;
	if (calc_seg1.at<uchar>(maxIdx) == 0){
		std::queue<Point> first_px_queue;
		first_px_queue.push(maxIdx);
		Mat visit_calc_seg1 = Mat::zeros(calc_seg1.size(), CV_8UC1);
		visit_calc_seg1.at<uchar>(maxIdx) = 1;
		while (true){
			Point px = first_px_queue.front();
			first_px_queue.pop();

			Point pxTo = Point(max(0, px.x - 1), px.y);
			if (calc_seg1.at<uchar>(pxTo) != 0){
				seg1_fFill.push(pxTo);
				break;
			}
			first_px_queue.push(pxTo);
			visit_calc_seg1.at<uchar>(pxTo) = 1;

			pxTo = Point(min(px.x + 1, calc_seg1.cols - 1), px.y);
			if (calc_seg1.at<uchar>(pxTo) != 0){
				seg1_fFill.push(pxTo);
				break;
			}
			first_px_queue.push(pxTo);
			visit_calc_seg1.at<uchar>(pxTo) = 1;

			pxTo = Point(px.x, min(px.y + 1, calc_seg1.rows - 1));
			if (calc_seg1.at<uchar>(pxTo) != 0){
				seg1_fFill.push(pxTo);
				break;
			}
			first_px_queue.push(pxTo);
			visit_calc_seg1.at<uchar>(pxTo) = 1;

			pxTo = Point(px.x, max(px.y - 1, 0));
			if (calc_seg1.at<uchar>(pxTo) != 0){
				seg1_fFill.push(pxTo);
				break;
			}
			first_px_queue.push(pxTo);
			visit_calc_seg1.at<uchar>(pxTo) = 1;
		}
	}
	seg1_fFill.push(maxIdx);
	seg_mask1.at<uchar>(maxIdx) = 255;
	rows = calc_seg1.rows;
	cols = calc_seg1.cols;
	while (!seg1_fFill.empty()){
		Point seg1_px = seg1_fFill.top();
		seg1_fFill.pop();

		int nearY, nearX;

		nearY = seg1_px.y;
		nearX = max(seg1_px.x - 1, 0);
		if (seg_mask1.at<uchar>(nearY, nearX) == 0 && calc_seg1.at<uchar>(nearY, nearX) != 0){
			seg1_fFill.push(Point(nearX, nearY));
			seg_mask1.at<uchar>(nearY, nearX) = 255;
		}

		// y = y
		nearX = min(seg1_px.x + 1, cols - 1);
		if (seg_mask1.at<uchar>(nearY, nearX) == 0 && calc_seg1.at<uchar>(nearY, nearX) != 0){
			seg1_fFill.push(Point(nearX, nearY));
			seg_mask1.at<uchar>(nearY, nearX) = 255;
		}

		nearY = max(seg1_px.y - 1, 0);
		nearX = seg1_px.x;
		if (seg_mask1.at<uchar>(nearY, nearX) == 0 && calc_seg1.at<uchar>(nearY, nearX) != 0){
			seg1_fFill.push(Point(nearX, nearY));
			seg_mask1.at<uchar>(nearY, nearX) = 255;
		}

		nearY = min(seg1_px.y + 1, rows - 1);
		// x = x
		if (seg_mask1.at<uchar>(nearY, nearX) == 0 && calc_seg1.at<uchar>(nearY, nearX) != 0){
			seg1_fFill.push(Point(nearX, nearY));
			seg_mask1.at<uchar>(nearY, nearX) = 255;
		}
	}
	seg_mask1.setTo(0, intersectMask == 0);

	Mat seg_mask2 = intersectMask - seg_mask1;

	Mat r1_inter, r2_inter;
	src1.copyTo(r1_inter, intersectMask);
	src2.copyTo(r2_inter, intersectMask);
	Mat ret1 = src1 - r1_inter;
	Mat ret2 = src2 - r2_inter;

	r1_inter.setTo(0, seg_mask2);
	r2_inter.setTo(0, seg_mask1);
	Mat ret_inter = pyramidBlending({ r1_inter, r2_inter });
	ret_inter.setTo(0, intersectMask == 0);

	//test
	Mat a = imread("apple.png");
	Mat b = imread("orange.png");
	Mat c = pyramidBlending({a, b});
	imshow("c", c);
	waitKey(0);

	return ret1 + ret_inter + ret2;
}

Mat AvmsMatcher::pyramidBlending(vector<Mat> target){
	// study@openCV#en : https://docs.opencv.org/3.1.0/dc/dff/tutorial_py_pyramids.html
	// --- pyramid blending
	Mat gray;
	cvtColor(target[0], gray, CV_BGR2GRAY);
	for (int i = 0; i < 2; i++){
		target[i].convertTo(target[i], CV_32F, 1.0 / 255);
	}
	Mat result;

	// use 3rd lib
	//------
	Mat_<float> testMask = Mat_<float>::zeros(target[0].size());
	testMask.setTo(1, gray);
	result = LaplacianBlending(target[0], target[1], testMask, 3).blend();
	result.convertTo(result, CV_8UC3, 255.0);
	return result;
	//------

	vector<Mat> gp[2];
	vector<Mat> lp[2];
	int pyrLevel = 6; // 1 ÀÌ»ף

	for (int i = 0; i < 2; i++){
		gp[i].push_back(target[i]);
		for (int j = 0; j < pyrLevel - 1; j++){
			Mat downGP;
			pyrDown(gp[i][j], downGP);
			gp[i].push_back(downGP);
		}

		lp[i].push_back(gp[i][pyrLevel - 1]);
		for (int j = pyrLevel - 1; j > 0; j--){
			Mat gaussianEdge;
			pyrUp(gp[i][j], gaussianEdge, gp[i][j - 1].size());
			Mat lap = gp[i][j - 1] - gaussianEdge;
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
	result.convertTo(result, CV_8UC1, 255.0);
	//return result;
}


// ----------------------------------
// below code : not used
Mat AvmsMatcher::findPerspectiveMatrix(Mat fromImage, Mat toImage){
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

