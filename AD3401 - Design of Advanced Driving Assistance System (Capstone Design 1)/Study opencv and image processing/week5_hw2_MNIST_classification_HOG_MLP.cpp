#include "week5_hw2_MNIST_classification_HOG_MLP.h"

void week5_hw2_MNIST_classification_HOG_MLP::run()
{
	int inputSize = 10;
	int outputSize = 5;
	Mat mlp_1(inputSize, outputSize, CV_32S);
	NeuralNet_MLP mlp;
	mlp.create(mlp_1);
}
