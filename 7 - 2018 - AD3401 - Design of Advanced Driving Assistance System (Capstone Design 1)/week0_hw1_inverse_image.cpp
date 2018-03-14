#include "week0_hw1_inverse_image.h"
#include "utility.h"

using namespace cv;
using namespace utility;

/*
	������ ���Ϸκ��� �Է¹޴´�.
	������ ���Ʒ��� ������Ų��.
	������ ���Ϸ� ����Ѵ�.
	*/
int week0_hw1_inverse_image::run(){
	Mat source;

	my_imread(source, "resource/lena.png");
	imshow("week0_hw1_inverse_image : default lena", source);

	int rows = source.rows;
	int cols = source.cols;

	for (int y = 0; y < rows/2; y++){
		for (int x = 0; x < cols; x++){
			swapVec3b(source, y, x, source, rows - y - 1, x);
		}
	}

	imshow("week0_hw1_inverse_image : inverse lena", source);
	imwrite("resource/lena_w0h1_inverse.bmp", source);

	waitKey();

	return 0;
}