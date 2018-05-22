#include "imageProcessing.hpp"

// 1. theta, rho 가 제한 안에 있고
// 2. 직선 결정의 소스 분포도가 크면서
// 3. 첫번째 찾아지는 직선그룹의 평균 직선을 찾는다.
Vec2f ImageProcessing::findFirstAverageLine(
	const Mat& src, vector<Vec2f>& lines,
	double startRho, double endRho,
	double startDegree, double endDegree,
	double rhoThreshold, double degreeThreshold)
{
	assert(src.channels() == 1);
	Mat blurred;
	GaussianBlur(src, blurred, Size(9, 9), 0);

	double startTheta = startDegree / 180 * M_PI;
	double endTheta = endDegree / 180 * M_PI;
	double thetaThreshold = degreeThreshold / 180 * M_PI;

	double minRho = startRho < endRho ? startRho : endRho;
	double maxRho = startRho < endRho ? endRho : startRho;
	double minTheta = startTheta < endTheta ? startTheta : endTheta;
	double maxTheta = startTheta < endTheta ? endTheta : startTheta;

	for (auto iter = lines.begin(); iter != lines.end();){
		auto line = *iter;
		double rho = line[0];
		double theta = line[1];
		if ((minRho <= rho && rho <= maxRho) && (minTheta <= theta && theta <= maxTheta)){
			iter++;
			continue;
		}
		iter = lines.erase(iter);
	}

	// sort : rho -> theta
	// left (rho Desc, theta Asc)
	// right (rho Asc, theta Desc)
	bool rhoAsc = startRho < endRho;
	bool thetaAsc = startTheta < endTheta;
	std::sort(lines.begin(), lines.end(), [rhoAsc, thetaAsc](const Vec2f& a, const Vec2f& b){
		return a[0] == b[0] ? (a[1] < b[1]) == thetaAsc : (a[0] < b[0]) == rhoAsc;
	});
	

	// 최초와 비교하여 rhoThreshold, degreeThreshold 가 넘지않는 직선의 평균 직선을 구함.
	// (최초 직선이 분포도도 일정하면서 outlier 일 경우 : 가드레일? -> 에러 발생)
	bool firstDetected = false;
	Vec2f firstLine;
	double rhoSum = 0;
	double thetaSum = 0;
	for (auto iter = lines.begin(); iter != lines.end();){
		auto line = *iter;
		// double rho = line[0];
		// double theta = line[1];

		if (firstDetected && (rhoThreshold < abs(firstLine[0] - line[0]))){
			// threshold 를 넘긴 rho, theta 에 대한 직선 모두 제거
			while (iter != lines.end()){
				iter = lines.erase(iter);
			}
			break;
		}

		double deviation = 0;
		if (!firstDetected){
			// 직선 분포도 계산
			// 분산을 구해서 일정 이상인 것을 취함.
			// xcost + ysint = rho :: y=mx+b
			vector<int> storage;

			Point startPt = Point(line[0] / cos(line[1]), 0);
			Point endPt = Point((line[0] - src.rows * sin(line[1])) / cos(line[1]), src.rows);
			double m = (startPt.y - endPt.y) / (startPt.x - endPt.x);
			double b = startPt.y - m * startPt.x;
			for (int y = 0; y < src.rows; y++){
				double x_double = (y - b) / m;
				int x = std::round(x_double);
				if (x < 0 || src.cols <= x){
					continue;
				}

				if (0 < blurred.at<uchar>(y, x)){
					storage.push_back(y);
				}
				else if (0.1 < std::abs(x - x_double)){
					int near_x = x_double < x ? x - 1 : x + 1;
					if (near_x < src.cols){
						if (0 < blurred.at<uchar>(y, near_x)){
							storage.push_back(y);
						}
					}
				}
			}
			if (rhoAsc){
				/*printf("storage size >>>>> %d\n", storage.size());
				Mat test = src.clone();
				cvtColor(test, test, CV_GRAY2BGR);
				for (auto data : storage){
					int x = (data - b)/m;
					circle(test, Point(x, data), 2, Scalar(0, 0, 255), 3);
				}
				imshow("test right", test);*/
			}
			else {
				/*printf("storage size <<<<< %d\n", storage.size());
				Mat test = src.clone();
				cvtColor(test, test, CV_GRAY2BGR);
				for (auto data : storage){
					int x = (data - b) / m;
					circle(test, Point(x, data), 2, Scalar(255, 0, 0), 3);
				}
				imshow("test left", test);*/
			}
			// storage의 분산을 구하기
			if (storage.size() < src.rows * 0.25){
				deviation = 0;
			}
			else {
				// referred : https://stackoverflow.com/a/12405793/7354469
				double sum = std::accumulate(storage.begin(), storage.end(), 0.0);
				double mean = sum / storage.size();
				double variance = 0;
				std::for_each(storage.begin(), storage.end(), [&variance, mean](const double d){
					variance += (d - mean) * (d - mean);
				});
				deviation = variance / (storage.size() - 1);
			}
		}

		if (!firstDetected && src.rows * 1.5 < deviation){
			//printf("%.4f\n",deviation);
		}

		if (firstDetected || src.rows * 1.5 < deviation){
			if (!firstDetected){
				//printf("---\n");
				if (rhoAsc){
					//printf("============> deviation : %.4f\n", deviation);
				}
				else {
					//printf("<============ deviation : %.4f\n\n\t=========================\t\n\n\n", deviation);
				}
				//waitKey(0);

				firstDetected = true;
				firstLine = line;
				rhoSum += line[0];
				thetaSum += line[1];
				iter++;
				continue;
			}
			else{
				if (abs(firstLine[1] - line[1]) <= thetaThreshold){
					rhoSum += line[0];
					thetaSum += line[1];
					iter++;
					continue;
				}
			}
		}
		iter = lines.erase(iter);
	}

	return firstDetected ? Vec2f(rhoSum / lines.size(), thetaSum / lines.size()) : Vec2f(0, 0);
}
