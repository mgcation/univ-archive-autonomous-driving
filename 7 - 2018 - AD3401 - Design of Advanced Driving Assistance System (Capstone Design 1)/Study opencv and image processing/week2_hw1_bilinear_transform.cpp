#define ASCII_ESC 27
#define ASCII_ENTER 13

#include "week2_hw1_bilinear_transform.h"

/*
Image 변환(rotation, scaling, translation)을 구현한다.
보간 방법은 Bi-Linear Interpolation 을 사용한다.
*/
int week2_hw1_bilinear_transform::run()
{
	Mat source;
	my_imread(source, "resource/lena.png");

	//Mat my_transform(512, 512 * 2, CV_8UC3, Scalar(255, 255, 255, 0));

	int order = -1;
	char c;
	bool start = 1;
	while ((c = waitKey(0)) != ASCII_ESC){
		if (c == ASCII_ENTER || start){
			start = 0;
			order++;

			if (order % 5 == 0){
				// rotation
				double degree = 80;

				Mat myRotMat = get_rotation_mat(Point2f(source.rows / 2.0f, source.cols / 2.0f), degree);
				Mat libRotMat = getRotationMatrix2D(Point2f(source.rows / 2.0f, source.cols / 2.0f), degree, 1);

				Mat my_nn_transform = transformate_nearst_neighbor(source, myRotMat, source.size());
				Mat my_transform = transformate_bi_linear(source, myRotMat, source.size());

				Mat lib_transform;
				warpAffine(source, lib_transform, libRotMat, source.size(), INTER_LINEAR);


				system("cls");
				printf("rotation degree(%.2f)\n", degree);
				cv::imshow("week2_hw1_bilinear_transform : source", source);
				cv::imshow("week2_hw1_bilinear_transform : my_transform_nn", my_nn_transform);
				cv::imshow("week2_hw1_bilinear_transform : my_transform_bili", my_transform);
				cv::imshow("week2_hw1_bilinear_transform : lib_transform", lib_transform);
			}
			else if (order % 5 == 1){
				// scaling

				double x_multiplier = 2;
				double y_multiplier = 0.5;
				Mat myRotMat = get_scaling_mat(x_multiplier, y_multiplier);
				Mat libRotMat = myRotMat(Rect(0, 0, 3, 2));

				Mat my_nn_transform = transformate_nearst_neighbor(source, myRotMat, source.size());
				Mat my_transform = transformate_bi_linear(source, myRotMat, source.size());

				Mat lib_transform;
				warpAffine(source, lib_transform, libRotMat, source.size(), INTER_LINEAR);


				system("cls");
				printf("scaling x(%.2f) y(%.2f)\n", x_multiplier, y_multiplier);
				cv::imshow("week2_hw1_bilinear_transform : source", source);
				cv::imshow("week2_hw1_bilinear_transform : my_transform_nn", my_nn_transform);
				cv::imshow("week2_hw1_bilinear_transform : my_transform_bili", my_transform);
				cv::imshow("week2_hw1_bilinear_transform : lib_transform", lib_transform);
			}
			else if (order % 5 == 2){
				// translation

				int x_offset = 30;
				int y_offset = 50;
				Mat myRotMat = get_translation_mat(x_offset, y_offset);
				Mat libRotMat = myRotMat(Rect(0, 0, 3, 2));

				Mat my_nn_transform = transformate_nearst_neighbor(source, myRotMat, source.size());
				Mat my_transform = transformate_bi_linear(source, myRotMat, source.size());

				Mat lib_transform;
				warpAffine(source, lib_transform, libRotMat, source.size(), INTER_LINEAR);


				system("cls");
				printf("translation x(%d) y(%d)\n", x_offset, y_offset);
				cv::imshow("week2_hw1_bilinear_transform : source", source);
				cv::imshow("week2_hw1_bilinear_transform : my_transform_nn", my_nn_transform);
				cv::imshow("week2_hw1_bilinear_transform : my_transform_bili", my_transform);
				cv::imshow("week2_hw1_bilinear_transform : lib_transform", lib_transform);
			}
			else if (order % 5 == 3){
				// rotate -> scale -> translate

				double degree = 60;
				double x_multiplier = 0.75;
				double y_multiplier = 1.25;
				int x_offset = 30;
				int y_offset = 50;
				Mat myRotMat = get_rotation_mat(Point2f(source.cols / 2.0f, source.rows / 2.0f), degree);
				myRotMat = get_scaling_mat(myRotMat, x_multiplier, y_multiplier);
				myRotMat = get_translation_mat(myRotMat, x_offset, y_offset);
				Mat libRotMat = myRotMat(Rect(0, 0, 3, 2));

				Mat my_nn_transform = transformate_nearst_neighbor(source, myRotMat, source.size());
				Mat my_transform = transformate_bi_linear(source, myRotMat, source.size());

				Mat lib_transform;
				warpAffine(source, lib_transform, libRotMat, source.size(), INTER_LINEAR);


				system("cls");
				printf("rotate(%.2f) -> scale(%.2f, %.2f) -> translate(%d, %d)\n", degree, x_multiplier, y_multiplier, x_offset, y_offset);
				cv::imshow("week2_hw1_bilinear_transform : source", source);
				cv::imshow("week2_hw1_bilinear_transform : my_transform_nn", my_nn_transform);
				cv::imshow("week2_hw1_bilinear_transform : my_transform_bili", my_transform);
				cv::imshow("week2_hw1_bilinear_transform : lib_transform", lib_transform);
			}
			else {
				// scale -> translate -> rotate

				double degree = 60;
				double x_multiplier = 0.75;
				double y_multiplier = 1.25;
				int x_offset = 30;
				int y_offset = 50;
				Mat myRotMat = get_scaling_mat(x_multiplier, y_multiplier);
				myRotMat = get_translation_mat(myRotMat, x_offset, y_offset);
				myRotMat = get_rotation_mat(Point2f(source.cols / 2.0f, source.rows / 2.0f), myRotMat, degree);
				Mat libRotMat = myRotMat(Rect(0, 0, 3, 2));

				Mat my_nn_transform = transformate_nearst_neighbor(source, myRotMat, source.size());
				Mat my_transform = transformate_bi_linear(source, myRotMat, source.size());

				Mat lib_transform;
				warpAffine(source, lib_transform, libRotMat, source.size(), INTER_LINEAR);


				system("cls");
				printf("scale(%.2f, %.2f) -> translate(%d, %d) -> rotate(%.2f)\n", x_multiplier, y_multiplier, x_offset, y_offset, degree);
				cv::imshow("week2_hw1_bilinear_transform : source", source);
				cv::imshow("week2_hw1_bilinear_transform : my_transform_nn", my_nn_transform);
				cv::imshow("week2_hw1_bilinear_transform : my_transform_bili", my_transform);
				cv::imshow("week2_hw1_bilinear_transform : lib_transform", lib_transform);
			}
		}
	}

	return 0;
}

Mat week2_hw1_bilinear_transform::get_default_transformation_mat(){
	return Mat_<double>(3, 3) <<
		1, 0, 0,
		0, 1, 0,
		0, 0, 1;
}

Mat week2_hw1_bilinear_transform::get_rotation_mat(Point2f center, Mat transMat, double degree){
	double radian = degree / 180 * 3.14159265358979323846264338327950288;
	double _cos = cos(radian);
	double _sin = sin(radian);
	Mat rotMat = (Mat_<double>(3, 3) <<
		_cos, _sin, (1 - _cos)*center.x - _sin*center.y,
		-_sin, _cos, _sin*center.x + (1 - _cos)*center.y,
		0, 0, 1);
	return rotMat * transMat;
}

Mat week2_hw1_bilinear_transform::get_scaling_mat(Mat transMat, double x_multiplier, double y_multiplier){
	return (Mat_<double>(3, 3) <<
		x_multiplier, 0, 0,
		0, y_multiplier, 0,
		0, 0, 1
		) * transMat;
}

Mat week2_hw1_bilinear_transform::get_translation_mat(Mat transMat, double x_offset, double y_offset){
	return (Mat_<double>(3, 3) <<
		0, 0, x_offset,
		0, 0, y_offset,
		0, 0, 0
		) + transMat;
}

Mat week2_hw1_bilinear_transform::transformate_nearst_neighbor(Mat source, Mat transM, Size size){
	assert((transM.type() == CV_32F || transM.type() == CV_64F) &&
		transM.rows == 3 &&
		transM.cols == 3);

	Mat ret(size.height, size.width, CV_8UC3, Scalar(0));
	Mat transMat;
	invert(transM, transMat, 0);
	for (int y = 0; y < size.height; y++){
		for (int x = 0; x < size.width; x++){
			int xx = int(round(transMat.at<double>(0, 0) * x + transMat.at<double>(0, 1) * y + transMat.at<double>(0, 2)));
			int yy = int(round(transMat.at<double>(1, 0) * x + transMat.at<double>(1, 1) * y + transMat.at<double>(1, 2)));
			if (0 <= yy && yy < size.height && 0 <= xx && xx < size.width)
				vec3bAt(ret, y, x) = vec3bAt(source, yy, xx);
		}
	}
	transMat.release();
	return ret;
}


Mat week2_hw1_bilinear_transform::transformate_bi_linear(Mat source, Mat transM, Size size){
	assert((transM.type() == CV_32F || transM.type() == CV_64F) &&
		transM.rows == 3 &&
		transM.cols == 3);

	Mat ret(size.height, size.width, CV_8UC3, Scalar(0));
	Mat transMat;
	invert(transM, transMat, 0);
	for (int y = 0; y < size.height; y++){
		for (int x = 0; x < size.width; x++){
			double x_b = transMat.at<double>(0, 0) * x + transMat.at<double>(0, 1) * y + transMat.at<double>(0, 2);
			double y_a = transMat.at<double>(1, 0) * x + transMat.at<double>(1, 1) * y + transMat.at<double>(1, 2);

			int xx = int(x_b);
			int yy = int(y_a);
			int xx_1 = xx + 1;
			int yy_1 = yy + 1;

			if (xx < 0 || size.width-1 <= xx || yy < 0 || size.height-1 <= yy)
				continue;

			double a = y_a - yy;
			double b = x_b - xx;

			vec3bAt(ret, y, x) = (1 - b)*((1 - a)*vec3bAt(source, yy, xx) + a*vec3bAt(source, yy_1, xx)) + b*((1 - a)*vec3bAt(source, yy, xx_1) + a*vec3bAt(source, yy_1, xx_1));
		}
	}
	transMat.release();
	return ret;
}
