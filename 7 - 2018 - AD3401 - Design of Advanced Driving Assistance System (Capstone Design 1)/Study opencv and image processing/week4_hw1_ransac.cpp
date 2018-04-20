#include "week4_hw1_ransac.h"
#include "opencv2/nonfree/nonfree.hpp"

int number;
int getNum(){
	return number++;
}

int week4_hw1_ransac::run(){
	initModule_nonfree();

	char* object = "resource/object.png";
	char* target = "resource/target.png";
	char* cite_object = "resource/cite_a.jpg";
	char* cite_target = "resource/cite_b.jpg";

	//match_image_library_feature(object, target, "SIFT", "FlannBased", true);
	//match_image_library_feature(object, target, "SURF", "FlannBased", true);
	//match_image_library_feature(cite_object, cite_target, "SURF", "FlannBased", true);
	
	match_image_library_feature(object, target, "SURF", "FlannBased", false);
	//match_image_library_feature(cite_object, cite_target, "SURF", "FlannBased", false);

	waitKey(0);
	return 0;
}

void week4_hw1_ransac::match_image_library_feature(
	const String& object_filename, const String& target_filename,
	const String& detector_type, const String& match_algorithm,
	bool do_i_run_library_match)
{
	int failed = 2;
	failed -= detector_type.compare("SIFT") == 0;
	failed -= detector_type.compare("SURF") == 0;
	if (!failed){
		std::cout << "bad argument : " << detector_type << " (use : SIFT, SURF)\n";
		getchar();
		return;
	}

	failed = 7;
	// refer : https://github.com/opencv/opencv/blob/master/modules/features2d/src/matchers.cpp
	failed -= match_algorithm.compare("FlannBased") == 0;
	failed -= match_algorithm.compare("BruteForce") == 0; //L2
	failed -= match_algorithm.compare("BruteForce-SL2") == 0; //Squeared L2
	failed -= match_algorithm.compare("BruteForce-L1") == 0;
	failed -= match_algorithm.compare("BruteForce-Hamming") == 0;
	failed -= match_algorithm.compare("BruteForce-HammingLUT") == 0;
	failed -= match_algorithm.compare("BruteForce-Hamming(2)") == 0;
	if (!failed){
		std::cout << "bad argument : " << match_algorithm << " (use : FlannBased, BruteForce, "\
			<< "BruteForce-SL2, BruteForce-L1, BruteForce-Hamming, BruteForce-HammingLUT, BruteForce-Hamming(2)\n";
		getchar();
		return;
	}

	Mat object, target, cite_object, cite_target;
	my_imread(object, object_filename.c_str(), IMREAD_GRAYSCALE);
	my_imread(target, target_filename.c_str(), IMREAD_GRAYSCALE);

	vector<KeyPoint> object_kp, target_kp;
	Mat object_descriptor, target_descriptor;

	Ptr<FeatureDetector> surfDetector = FeatureDetector::create(detector_type);
	surfDetector->detect(object, object_kp);
	surfDetector->detect(target, target_kp);
	Ptr<DescriptorExtractor> surf_extractor = DescriptorExtractor::create(detector_type);
	surf_extractor->compute(object, object_kp, object_descriptor);
	surf_extractor->compute(target, target_kp, target_descriptor);

	if (do_i_run_library_match){
		week4_hw1_ransac::draw_library_ransac_match(
			object, target,
			object_kp, target_kp,
			object_descriptor, target_descriptor,
			match_algorithm);
	}
	else {
		draw_my_ransac_match(
			object, target, 
			object_kp, target_kp, 
			object_descriptor, target_descriptor,
			match_algorithm);
	}
}

int week4_hw1_ransac::draw_library_ransac_match(
	Mat object, Mat target,
	vector<KeyPoint> object_kp, vector<KeyPoint> target_kp,
	Mat object_descriptor, Mat target_descriptor,
	const String& match_algorithm)
{
	// refer below example
	// 1. https://docs.opencv.org/2.4/doc/tutorials/features2d/feature_homography/feature_homography.html
	// 2. https://docs.opencv.org/3.3.1/d7/dff/tutorial_feature_homography.html 

	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(match_algorithm);

	vector<DMatch> dmatch;
	matcher->match(object_descriptor, target_descriptor, dmatch);

	double min = 100;
	for (int i = 0; i < object_descriptor.rows; i++){
		min = min <= dmatch[i].distance ? min : dmatch[i].distance;
	}
	if (min == 0) min = 1;
	std::cout << "min : " << min << "\n";
	vector<DMatch> min_3_match;
	for (auto match : dmatch){
		if (match.distance < min * 3){
			min_3_match.push_back(match);
		}
	}

	Mat min_3_match_image;
	drawMatches(object, object_kp, target, target_kp, min_3_match, min_3_match_image);

	// -- Localize the object
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;
	for (int i = 0; i < min_3_match.size(); i++)
	{
		//-- Get the keypoints from the good matches
		obj.push_back(object_kp[min_3_match[i].queryIdx].pt);
		scene.push_back(target_kp[min_3_match[i].trainIdx].pt);
		/*std::cout << "min3match query : (" << min_3_match[i].queryIdx << \
			") / train : (" << min_3_match[i].trainIdx << \
			") / distance : (" << min_3_match[i].distance << \
			") / image : (" << min_3_match[i].imgIdx << ")\n";*/
	}

	Mat H = findHomography(obj, scene, RANSAC);
	//-- Get the corners from the image_1 ( the object to be "detected" )
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0); obj_corners[1] = cvPoint(object.cols, 0);
	obj_corners[2] = cvPoint(object.cols, object.rows); obj_corners[3] = cvPoint(0, object.rows);
	std::vector<Point2f> scene_corners(4);
	perspectiveTransform(obj_corners, scene_corners, H);
	//-- Draw lines between the corners (the mapped object in the scene - image_2 )
	line(min_3_match_image, scene_corners[0] + Point2f(object.cols, 0), scene_corners[1] + Point2f(object.cols, 0), Scalar(0, 255, 0), 4);
	line(min_3_match_image, scene_corners[1] + Point2f(object.cols, 0), scene_corners[2] + Point2f(object.cols, 0), Scalar(0, 255, 0), 4);
	line(min_3_match_image, scene_corners[2] + Point2f(object.cols, 0), scene_corners[3] + Point2f(object.cols, 0), Scalar(0, 255, 0), 4);
	line(min_3_match_image, scene_corners[3] + Point2f(object.cols, 0), scene_corners[0] + Point2f(object.cols, 0), Scalar(0, 255, 0), 4);
	//-- Show detected matches

	char winNum[4];
	_itoa(getNum(), winNum, 4);
	String winName = "draw_min_3_match_image_";
	winName.append(winNum);
	imshow(winName, min_3_match_image);

	return min_3_match.size();
}

Point2f dotPerspectiveWarping(Point2f dot, Mat m){
	double z = dot.x*m.at<double>(2, 0) 
		+ dot.y*m.at<double>(2, 1)
		+ m.at<double>(2, 2);
	return Point2f(
		(dot.x*m.at<double>(0, 0) + dot.y*m.at<double>(0, 1) + m.at<double>(0, 2)) / z,
		(dot.x*m.at<double>(1, 0) + dot.y*m.at<double>(1, 1) + m.at<double>(1, 2)) / z
		);
}

int week4_hw1_ransac::draw_my_ransac_match(
	Mat object, Mat target,
	vector<KeyPoint> object_kp, vector<KeyPoint> target_kp,
	Mat object_descriptor, Mat target_descriptor,
	const String& match_algorithm, 
	unsigned int ransacIterCount,
	bool step_by_step_iteration_esc)
{
	// -------------------------------------------------
	// using library when match descriptor

	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(match_algorithm);

	vector<DMatch> dmatch;
	matcher->match(object_descriptor, target_descriptor, dmatch);

	double min = 100;
	for (int i = 0; i < object_descriptor.rows; i++){
		min = min <= dmatch[i].distance ? min : dmatch[i].distance;
	}
	std::cout << "min : " << min << "\n";
	vector<DMatch> min_3_match;
	for (auto match : dmatch){
		if (match.distance < min * 3){
			min_3_match.push_back(match);
		}
	}

	// -------------------------------------------------

	Mat lineObject;
	Mat lineTarget;
	Mat bestTransformMatrix;
	Mat bestTransformedTarget;
	double bestDistance = 10000000000;
	double bestInlinerCount = 0;
	double distanceThreshold = 15;

	int matchLength = min_3_match.size();
	int rand4[4] = { 0, };
	srand(getTickCount());
	for (int ransacIter = 0; ransacIter < ransacIterCount; ransacIter++){
		// find 4 dot (to make perspectiveTransform Matrix)
		bool _4_dot_found = false;

		printf("\n\n%d(ransacIter)", ransacIter);
		while (!_4_dot_found){
			printf("finding..\t");
			// random 4 dot selection without duplicates
			for (int i = 0; i < 4; i++){
				bool isValidRandomVariable;
				do{
					rand4[i] = rand() % matchLength;
					isValidRandomVariable = true;
					for (int j = 0; j < i; j++){
						if (rand4[j] == rand4[i]){
							isValidRandomVariable = false;
							break;
						}
					}
				} while (!isValidRandomVariable);
			}
			
			// make convex hull at object_kp
			// shape like 0-1-2-3-0
			vector<Point2f> object_pt;
			vector<Point2f> target_pt;
			for (auto r : rand4){
				object_pt.push_back(object_kp[min_3_match[r].queryIdx].pt);
				target_pt.push_back(target_kp[min_3_match[r].trainIdx].pt);
			}

			vector<Point2f> object_ch_pt;
			vector<Point2f> target_ch_pt;
			convexHull(object_pt, object_ch_pt, false, false);
			convexHull(target_pt, target_ch_pt, false, false);
			if (!(object_ch_pt.size() == 4 && target_ch_pt.size() == 4)){
				continue;
			}
			
			int valid = 4;
			for (int i = 0; i < 4; i++){
				for (int j = 0; j < 4; j++){
					if (object_kp[min_3_match[rand4[i]].queryIdx].pt == object_ch_pt[j] &&
						target_kp[min_3_match[rand4[i]].trainIdx].pt == target_ch_pt[j]){
						valid--;
					}
				}
			}

			
			if (valid == 0){
				_4_dot_found = true;

				lineObject.release();
				lineTarget.release();
				cvtColor(object, lineObject, CV_GRAY2BGR);
				cvtColor(target, lineTarget, CV_GRAY2BGR);
				line(lineObject, object_ch_pt[0], object_ch_pt[1], Scalar(0, 0, 255));
				line(lineObject, object_ch_pt[1], object_ch_pt[2], Scalar(0, 0, 255));
				line(lineObject, object_ch_pt[2], object_ch_pt[3], Scalar(0, 0, 255));
				line(lineTarget, target_ch_pt[0], target_ch_pt[1], Scalar(0, 0, 255));
				line(lineTarget, target_ch_pt[1], target_ch_pt[2], Scalar(0, 0, 255));
				line(lineTarget, target_ch_pt[2], target_ch_pt[3], Scalar(0, 0, 255));
				Mat transformedTarget;
				
				Mat transformMatrix = getPerspectiveTransform(target_ch_pt, object_ch_pt);
				warpPerspective(target, transformedTarget, transformMatrix, Size(object.size()));
				cvtColor(transformedTarget, transformedTarget, CV_GRAY2BGR);

				double distance=0;
				int inLinerCount = 0;
				for (int i = 0; i < min_3_match.size(); i++){
					Point2f targetPt = target_kp[min_3_match[i].trainIdx].pt;
					Point2f objectPt = object_kp[min_3_match[i].queryIdx].pt;

					Point2f transformedTargetPt = dotPerspectiveWarping(targetPt, transformMatrix);
					if (distance < distanceThreshold){
						distance += norm(transformedTargetPt - objectPt);
						inLinerCount++;

						circle(transformedTarget, transformedTargetPt, 4, Scalar(0, 0, 255)); //빨간게 추정값
						circle(transformedTarget, objectPt, 2, Scalar(255, 000, 0)); //파란색이 원래 목적값
						line(transformedTarget, transformedTargetPt, objectPt, Scalar(255, 0, 255));
					}
				}
				std::cout << "distance : " << distance << "\n";

				if (bestInlinerCount < inLinerCount ||
					(bestInlinerCount == inLinerCount && distance < bestDistance)){
					bestInlinerCount = inLinerCount;
					bestDistance = distance;
					bestTransformedTarget = transformedTarget.clone();
					bestTransformMatrix = transformMatrix.clone();
				}

				vector<Point> nowDetection(4);
				vector<Point> detectionLine = {
					Point(0, 0), Point(0, object.rows - 1),
					Point(object.cols - 1, object.rows - 1), Point(object.cols - 1, 0) };
				Mat reverseTransformMat, nowReverse;
				invert(bestTransformMatrix, reverseTransformMat, 1);
				invert(transformMatrix, nowReverse);
				for (int i = 0; i < 4; i++){
					nowDetection[i] = dotPerspectiveWarping(detectionLine[i], nowReverse);
					detectionLine[i] = dotPerspectiveWarping(detectionLine[i], reverseTransformMat);
				}
				line(lineTarget, detectionLine[0], detectionLine[1], Scalar(0, 255, 0), 3);
				line(lineTarget, detectionLine[1], detectionLine[2], Scalar(0, 255, 0), 3);
				line(lineTarget, detectionLine[2], detectionLine[3], Scalar(0, 255, 0), 3);
				line(lineTarget, detectionLine[3], detectionLine[0], Scalar(0, 255, 0), 3);

				line(lineTarget, nowDetection[0], nowDetection[1], Scalar(30, 150, 30), 2);
				line(lineTarget, nowDetection[1], nowDetection[2], Scalar(30, 150, 30), 2);
				line(lineTarget, nowDetection[2], nowDetection[3], Scalar(30, 150, 30), 2);
				line(lineTarget, nowDetection[3], nowDetection[0], Scalar(30, 150, 30), 2);

				imshow("line object", lineObject);
				imshow("line target", lineTarget);
				imshow("transformed target", transformedTarget);
				imshow("best transformed target", bestTransformedTarget);
				
				if (step_by_step_iteration_esc)
					while (!checkEsc());
			}
		}

	}

	return 0;
}
