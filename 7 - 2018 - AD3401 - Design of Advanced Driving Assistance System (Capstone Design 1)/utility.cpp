#include "utility.h"

uchar &utility::ucharAt(Mat image, int y, int x){
	return image.at<uchar>(y, x);
}

Vec3b &utility::vec3bAt(Mat image, int y, int x){
	return image.at<Vec3b>(y, x);
}

void utility::swapVec3b(Mat source, int y1, int x1, Mat dest, int y2, int x2){
	Vec3b temp = vec3bAt(source, y1, x1);
	vec3bAt(source, y1, x1) = vec3bAt(dest, y2, x2);
	vec3bAt(dest, y2, x2) = temp;
}

char utility::my_imread(Mat& img, const char* filename, int IMREAD_FLAGS){
	img = imread(filename, IMREAD_FLAGS);
	if (img.total() == 0){
		if (img.empty()){
			printf("image load failed : there is no file?");
			getchar();
			exit(-1);
		}
		printf("image load failed : no img?");
		getchar();
		exit(-1);
	}
	return 0;
}

bool utility::checkEsc(){
	char c = (char)waitKey(30);
	if (c == 27)
		return true;
	return false;
}

void utility::showVideo(const char* filename){
	VideoCapture capture(filename);

	while (true){
		Mat frame;

		capture >> frame;
		if (frame.empty()){
			break;
		}
		imshow(filename, frame);

		if (checkEsc()){
			break;
		}
	}
}

Mat utility::getKernel(vector<vector<float>> kernelVector){
	if (kernelVector.size() == 0){
		printf("kernel size error (size:0)\n");
		waitKey(0);
		exit(-1);
	}

	int kernelSize = kernelVector[0].size();
	bool error = kernelVector.size() != kernelSize;

	if (!error){
		for (auto k : kernelVector){
			if (k.size() != kernelSize)
				error = true;
		}
	}

	if (error){
		printf("kernel size error (non square)\n");
		waitKey(0);
		exit(-1);
	}

	Mat kernel(kernelSize, kernelSize, CV_32F);
	int y = 0;
	int x;
	for (auto rows : kernelVector){
		x = 0;
		for (auto cell : rows){
			kernel.at<float>(y, x) = cell;
			x++;
		}
		y++;
	}

	return kernel;
}

uchar utility::calc_MSE(Mat target1, Mat target2, bool print, const char* msg){
	bool err_diff_channels = target1.channels() != target2.channels();

	if (err_diff_channels){
		printf("channel not same");
		getchar();
		exit(0);
	}
	else {
		long long err = 0;
		vector<Mat> target1_vec, target2_vec;
		int rows = target1.rows;
		int cols = target1.cols;
		split(target1, target1_vec);
		split(target2, target2_vec);
		int chs = target1_vec.size();

		for (int ch = 0; ch < chs; ch++){
			for (int y = 0; y < rows; y++){
				for (int x = 0; x < cols; x++){
					err += pow(ucharAt(target1_vec[ch], y, x) - ucharAt(target2_vec[ch], y, x),2);
				}
			}
		}
		if (print){
			printf("calc_MSE : %s\n", msg);
			printf("errSum = %d\n", err);
			printf("size = y,x,ch (%d, %d, %d)\n", rows, cols, chs);
			err = err / rows / cols / chs;
			err = sqrt(err);
			printf("avg err = %d\n", err);
		}
		else {
			err = err / rows / cols / chs;
			err = sqrt(err);
		}
		return err;
	}
}
