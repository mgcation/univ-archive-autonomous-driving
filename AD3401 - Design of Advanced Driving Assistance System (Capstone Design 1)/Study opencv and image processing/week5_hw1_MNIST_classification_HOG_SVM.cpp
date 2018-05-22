#include "week5_hw1_MNIST_classification_HOG_SVM.h"

void reverseEndianInt(int& value){
	unsigned int val = value;
	int ch1 = val & 255;
	int ch2 = (val >> 8) & 255;
	int ch3 = (val >> 16) & 255;
	int ch4 = (val >> 24) & 255;
	value = (ch1 << 24) + (ch2 << 16) + (ch3 << 8) + ch4;
}

void week5_hw1_MNIST_classification_HOG_SVM::readData(const String imageFileName, const String labelFileName, vector<Mat>& images, vector<float>& labels){
/*
mnist from : http://yann.lecun.com/exdb/mnist/


Image file
----------
[offset] [type]          [value]          [description]
0000     32 bit integer  0x00000803(2051) magic number
0004     32 bit integer  10000            number of images
0008     32 bit integer  28               number of rows
0012     32 bit integer  28               number of columns
0016     unsigned byte   ??               pixel
...


Label file
----------
[offset] [type]          [value]          [description]
0000     32 bit integer  0x00000801(2049) magic number (MSB first)
0004     32 bit integer  10000            number of items
0008     unsigned byte   ??               label
0009     unsigned byte   ??               label
...
*/
	ifstream imageStream(imageFileName, std::ios::binary);
	ifstream labelStream(labelFileName);

	int magic_number;
	int number_of_images, rows, cols;
	int number_of_labels;

	imageStream.read((char*)&magic_number, sizeof(int));
	labelStream.read((char*)&magic_number, sizeof(int));

	imageStream.read((char*)&number_of_images, sizeof(int));
	imageStream.read((char*)&rows, sizeof(int));
	imageStream.read((char*)&cols, sizeof(int));
	labelStream.read((char*)&number_of_labels, sizeof(int));

	reverseEndianInt(number_of_images);
	reverseEndianInt(rows);
	reverseEndianInt(cols);
	reverseEndianInt(number_of_labels);

	if (number_of_images){
		Mat test(rows, cols, CV_8U);
		unsigned char pixel;
		char label;

		double percent = 0.1;
		for (int n = 0; n < number_of_images; n++){
			if (n >= number_of_images*percent) {
				printf("reading file : %.1f percent 완료\n", percent*100);
				percent+=0.1;
			}
			for (int y = 0; y < rows; y++){
				for (int x = 0; x < cols; x++){
					imageStream.read((char*)&pixel, sizeof(char));
					test.at<uchar>(y, x) = pixel;
				}
			}
			images.push_back(test.clone());

			labelStream.read((char*)&label, sizeof(char));
			labels.push_back(label);
		}
	}

	imageStream.close();
	labelStream.close();
	imageStream.clear();
	labelStream.clear();
}

void training(vector<Mat> train_images, vector<float> train_labels, const int train_size, const char* saveFileName){

	HOGDescriptor hog(train_images[0].size(), Size(24, 24), Size(2, 2), Size(8, 8), 9);
	Mat train_descriptor(train_size, hog.getDescriptorSize(), CV_32F);
	Mat train_desc_label(train_size, 1, CV_32F);

	double percent = 0.1;
	for (int i = 0; i < train_size; i++){
		if (i >= train_size*percent) {
			printf("reading file : %.1f percent 완료\n", percent * 100);
			percent += 0.1;
		}

		vector<float> hog_descriptor;
		hog.compute(train_images[i], hog_descriptor);
		transpose(Mat(hog_descriptor), train_descriptor.row(i));

		train_desc_label.at<float>(i, 0) = train_labels[i];
	}

	SVMParams params;
	params.svm_type = SVM::C_SVC; // 2개 이상 검출 가능.
	params.kernel_type = CvSVM::LINEAR;
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);

	SVM classifier;
	classifier.train(train_descriptor, train_desc_label, Mat(), Mat(), params);
	classifier.save(saveFileName);
	/*for (auto train_i : train_image){
	hog.compute(train_i, hog_descriptor);
	classifier.train(Mat(hog_descriptor), Mat(hog_descriptor));
	}*/
}

void test(vector<Mat> test_images, vector<float>& test_labels, const int test_size, const char* savedFileName){
	Mat result;
	
	HOGDescriptor hog(test_images[0].size(), Size(24, 24), Size(2, 2), Size(8, 8), 9);
	Mat test_descriptor(test_size, hog.getDescriptorSize(), CV_32F);

	double percent = 0.1;
	for (int i = 0; i < test_size; i++){
		if (i >= test_size*percent) {
			printf("reading file : %.1f percent 완료\n", percent * 100);
			percent += 0.1;
		}

		vector<float> hog_descriptor;
		hog.compute(test_images[i], hog_descriptor);
		transpose(Mat(hog_descriptor), test_descriptor.row(i));
	}

	//! size
	SVM classifier;
	classifier.load(savedFileName);
	float a = 3; classifier.predict(test_descriptor, test_labels);
	printf("?? : %.2f ??", a);
}

void week5_hw1_MNIST_classification_HOG_SVM::run()
{
	// test case(10k) image : "t10k-images.idx3-ubyte"
	// test case(10k) label : "t10k-labels.idx1-ubyte"
	// train case image : "train-images.idx3-ubyte"
	// train case label : "train-labels.idx1-ubyte"


	vector<Mat> train_images;
	vector<float> train_labels;
	readData("resource/train-images.idx3-ubyte", "resource/train-labels.idx1-ubyte", train_images, train_labels);
	int trainSize = train_images.size();
	training(train_images, train_labels, trainSize, "hog_svm_param.yaml");


	vector<Mat> test_images;
	vector<float> test_labels;
	readData("resource/t10k-images.idx3-ubyte", "resource/t10k-labels.idx1-ubyte", test_images, test_labels);
	int testSize = test_images.size();
	vector<float> test_results;
	test(test_images, test_results, testSize, "hog_svm_param.yaml");


	double sum_of_err = 0;
	float cnt_err = 0;
	for (int i = 0; i < testSize; i++){
		imshow("test", test_images[i]);
		cnt_err += (test_labels[i] != test_results[i]);
		printf("real : (%.1f) / predict : (%.1f) / average of err : (%.2f)\n", test_labels[i], test_results[i], cnt_err / (i == 0 ? 1 : i));
		if (test_labels[i] != test_results[i]){
			waitKey(0);
		}
	}


	waitKey(0);
}
