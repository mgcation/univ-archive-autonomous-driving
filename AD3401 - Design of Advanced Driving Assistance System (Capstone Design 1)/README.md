# Design of Advanced Driving Assistance System

첨단운전자보조시스템 설계

## Study opencv and image processing

### week0

1. Inverse Image
2. Rewind Video

### week1

1. 히스토그램 이퀄라이제이션 라이브러리 사용
2. 가우시안 필터, 메디안 필터 구현 후 라이브러리와 비교
   1. Salt & Pepper Noising, Gaussian Noising 구현
3. Edge Detection
   1. 1차 미분 에지 검출 (Sobel, Prewitt) 구현
   2. 2차 미분 에지 검출 (LoG) 구현
   3. Canny 라이브러리 사용

### week2

1. ​Image 의 rotation, scaling, translation 을 backward mapping (보간은 bilinear interpolation) 으로 구현.

   > 각 각의 변환 중 2가지 이상을 사용해도 연산은 한 번만 해야 함. (rotation 후 scaling 후 translation 이런식이 아니라 rotation, scaling, translation 를 한 번에 할 수 있는 매트릭스를 구현 후 이를 이용하여 affine warping.)

2. Chess-board 영상(기울어진 체스판)을 직접 찾아서 16개 이상의 corner를 검출해 보기. (challenge, 최대한 지금까지의 지식을 활용하여 검출해보기)

3. 이후 해당 코너들로 projective transform을 찾아 기울어진 체스판을 위에서 본 것처럼 perspective warping 시키기.

### week3

1. Harris corner detector 를 직접 코딩해보고 라이브러리와 비교.
2. Hough transform을 이용하여 line detection을 해보고 라이브러리와 비교.

### week4

1. [라이브러리 사용] SIFT, SURF 를 라이브러리를 사용하여 특징점을 검출해보기
2. [직접 구현] RANSAC matching 기법을 사용하여 두 이미지를 매칭시키기.
3. [직접 구현] SURF를 직접 구현한 뒤 구현한 RANSAC matching을 이용하여 다시 매칭시켜보기.

### week5

MNIST data set을 이용하여 수행. (라이브러리 사용)

1. HOG + SVM을 이용하여 classification
2. HOG + MLP를 이용하여 classification
3. CNN을 이용하여 classification

## Implementation applications

### Lane Detection

- [mgcation/lane-detection](https://github.com/mgcation/lane-detection)

Around View Monitoring System

- ~~mgcation/around-view-sys~~