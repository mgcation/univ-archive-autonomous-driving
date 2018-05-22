#include "imageProcessing.hpp"

vector<Vec2f> ImageProcessing::meanShiftRhoThetaClustering(vector<Vec2f> lines, bool reverse){
	return vector<Vec2f>();
}


//// draw clusted line
//Mat left_roi = Mat(roi, left_roi_rect);
//Mat right_roi = Mat(roi, right_roi_rect);
//vector<Vec2f> clustered_left_lines, clustered_right_lines; // length == 1
////int ran = rand() % 157;
////for (int i = 0; i < ran; i++){
////	/*
////		왼쪽 이미지는 0,0 기준으로 rho가 30이면 (30,0) 에서부터 시작.
////		theta는 0일때 일직선이고 더할수록 시계방향으로 회전. 뺄수록 반시계.
////	*/
////	clustered_left_lines.push_back(Vec2f(left_roi.cols-80, i*0.01));
////	clustered_right_lines.push_back(Vec2f(80, -i*0.01));
////}
//Mat left_theta_rho_map(91, left_roi.cols * 2, CV_32FC1, Scalar(0)), right_theta_rho_map(91, left_roi.cols * 2, CV_32FC1, Scalar(0));
//puts("===\n");
//for (auto left_line : left_lines){
//	int rho = left_line[0] + left_roi.cols;
//	int theta = left_line[1] * 180 / 3.14;
//	if (rho < 0 || left_roi.cols * 2 <= rho){
//		printf("rho < 0 || %d <= rho : %d\n", left_roi.cols, rho);
//		continue;
//	}
//	if (theta < 0 || 91 <= theta){
//		printf("theta < 0 || 91 <= theta : %d\n", theta);
//		continue;
//	}
//	//printf("%d %d\n", theta, rho);
//	left_theta_rho_map.at<float>(theta, rho) = 127;
//}
//puts("---");
//for (auto right_line : right_lines){
//	//printf("-- %.2f  - %.2f\n", right_line[0], right_line[1]);
//	int rho = right_line[0] + left_roi.cols;
//	int theta = right_line[1] * 180 / 3.14 - 90;
//	if (rho <0 || left_roi.cols * 2 <= rho){
//		printf("rho < 0 || %d <= rho : %d\n", left_roi.cols, rho);
//		continue;
//	}
//	if (theta < 0 || 91 <= theta){
//		printf("theta < 0 || 91 <= theta : %d\n", theta);
//		continue;
//	}
//	//printf("%d %d\n", theta, rho);
//	right_theta_rho_map.at<float>(theta, rho) = 127;
//}
//puts("===\n");
//int window_size = 150;
//Rect tracking_window_left_line(left_theta_rho_map.cols * 2 - 1, 0, window_size, window_size);
//Rect tracking_window_right_line(left_theta_rho_map.cols, 90, window_size, window_size);
//Mat output, centers;
//printf("\n\n>>");
//kmeans(left_theta_rho_map, 4, output, TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1.0), 3, KMEANS_PP_CENTERS, centers);
//std::cout << centers << "\n\n\n" << output;
///*meanShift(left_theta_rho_map, tracking_window_left_line, TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 100, 0.01));
//printf("%d %d %d %d\n", tracking_window_left_line.x, tracking_window_left_line.y, tracking_window_left_line.width, tracking_window_left_line.height);
//meanShift(right_theta_rho_map, tracking_window_right_line, TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 100, 0.01));
//clustered_left_lines.push_back(Vec2f(
//(tracking_window_left_line.x + window_size/2) - left_roi.cols,
//(tracking_window_left_line.y + window_size/2)*3.14 / 90));
//clustered_right_lines.push_back(Vec2f(
//(tracking_window_right_line.x + window_size/2) - left_roi.cols,
//(tracking_window_right_line.y + window_size/2 + 90)*3.14 / 90));*/
