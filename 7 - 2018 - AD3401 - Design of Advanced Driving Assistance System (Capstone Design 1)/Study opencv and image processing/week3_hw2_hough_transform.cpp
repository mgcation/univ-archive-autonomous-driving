#define M_PI 3.14159265358979323846

#include "week3_hw2_hough_transform.h"
#include "week3_hw1_harris_corner_detection.h"

/*
hough transform을 이용하여 line detection을 직접 구현하고 라이브러리와 비교
*/

int week3_hw2_hough_transform::run(){
	Mat source;
	my_imread(source, "resource/building.jpg");

	Mat myHough = myHough_lineDetection(source, 1, M_PI / 180, 150);
	Mat libHough = libHough_lineDetection(source, 1, M_PI / 180, 150);

	imshow("source", source);
	imshow("myHough", myHough);
	imshow("libHough", libHough);
	waitKey(0);

	return 0;
}

Mat week3_hw2_hough_transform::myHough_lineDetection(Mat source, double rho, double theta, int thresh){
	assert(theta != 0);

	Mat gray = ifnot_2gray(source);
	int MAX_THETA_NUM = ceil(M_PI / theta);
	int MAX_P = sqrt(pow(source.rows, 2) + pow(source.cols, 2)) * 2; //xsin + ycos 의 최대는 norm2(x,y) 이고 양수 음수 모두 사용하므로 * 2
	vector<vector<double>> storage(MAX_THETA_NUM, vector<double>(MAX_P, 0.0));
	vector<double> sin_(MAX_THETA_NUM);
	vector<double> cos_(MAX_THETA_NUM);

	for (double rad = 0; rad < M_PI; rad += theta){
		int radd = int(round(rad / theta));
		if (radd == MAX_THETA_NUM)	break;

		sin_[radd] = sin(rad);
		cos_[radd] = cos(rad);
	}

	Canny(gray, gray, 200, 200);
	imshow("canny", gray);

	for (int y = 0; y < gray.rows; y++){
		for (int x = 0; x < gray.cols; x++){
			uchar& point = ucharAt(gray, y, x);
			if (point > 0){
				for (double rad = 0; rad < M_PI; rad += theta){
					int radd = int(round(rad / theta));
					if (radd == MAX_THETA_NUM) radd = 0;

					int p = round(y*sin_[radd] + x*cos_[radd]) + MAX_P / 2;
					int floor_rho_p = int(p / rho)*rho;
					int base = floor_rho_p + rho*0.5;
					if (p >= base) p = floor_rho_p + rho;
					else p = floor_rho_p;
					storage[radd][p] += point/255; //그냥 0~1로 바꿔서 더해줘봤음.
				}
			}
		}
	}

	int gmSize = 3; //odd, 1이상
	vector<Vec2f> real;
	for (double rad = 0; rad < M_PI; rad += theta){
		for (int p = 0; p < MAX_P; p++){
			int radd = int(round(rad / theta));
			if (radd == MAX_THETA_NUM) break;

			if (storage[radd][p] > thresh){
				int startX = radd - gmSize / 2;
				int endX = radd + gmSize / 2;
				int startY = p - gmSize / 2;
				int endY = p + gmSize / 2;
				if (startX < 0) startX = 0;
				if (MAX_THETA_NUM <= endX) endX = MAX_THETA_NUM - 1;
				if (startY < 0) startY = 0;
				if (MAX_P <= endY) endY = MAX_P - 1;
				bool select = true;
				for (int x = startX; x <= endX; x++){
					for (int y = startY; y <= endY; y++){
						if (storage[radd][p] < storage[x][y])
							select = false;
					}
				}
				if (select)
					real.push_back(Vec2f(p-MAX_P/2, rad));
			}
		}
	}

	Mat lines(real);
	std::cout << "lines.rows (" << lines.rows << "), lines.cols(" << lines.cols << ")\n";
	if (!(lines.rows > 0 && lines.cols == 1))
		lines = Mat(1, 0, CV_32FC2);
	else
		transpose(lines, lines);

	//week3_hw1_harris_corner_detection::localMaxima(gray, Size(3, 3), 0);
	return drawLines(source, lines);
}

Mat week3_hw2_hough_transform::libHough_lineDetection(Mat source, double rho, double theta, int thresh){
	Mat gray = ifnot_2gray(source);
	Canny(gray, gray, 200, 200);
	Mat lines;
	HoughLines(gray, lines, rho, theta, thresh);
	std::cout << "lines.rows (" << lines.rows << "), lines.cols(" << lines.cols << ")\n";
	return drawLines(source, lines);
}

Mat week3_hw2_hough_transform::drawLines(Mat source, Mat lines){
	puts("-----");
	for (int i = 0; i < lines.cols; i++){
		Vec2f a = lines.at<Vec2f>(0, i);
		std::cout << a[0] << " , " << double(a[1]) << "\n";
	}
	puts("-----");
	Mat background = source.clone();
	for (int ch = 0; ch < lines.cols; ch++){
		float rho = lines.at<Vec2f>(0, ch)[0];
		float theta = lines.at<Vec2f>(0, ch)[1];
		int k = 1000;
		double cost = cos(theta);
		double sint = sin(theta);
		int x = cost*rho;
		int y = sint*rho;
		Point pt1(x + k*(-sint), y + k*cost),
			pt2(x - k*(-sint), y - k*cost);
		line(background, pt1, pt2, Scalar(0, 0, 255), 1);
	}
	return background;
}
