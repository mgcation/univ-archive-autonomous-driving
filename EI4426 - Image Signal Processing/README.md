# Image signal Processing

영상처리

## Project

### 1. Image denoising & Edge Detection

1. 영상에 noise 추가
   1. Impulse noise
   2. Gaussian noise
   3. Uniform noise
2. Denoising filter
   1. Gaussian Filter
   2. Bilateral Filter
   3. Median Filter
3. Sobel, Prewitt edge detection
4. LoG edge detection

### 2. Transformation & Corner Detection

1. 이미지의 확대, 회전, 이동 (Nearest neighbor interpolation)
2. 이미지의 확대, 회전, 이동 (Bilinear interpolation)
3. Moravec corner detection
4. Harris corner detection
5. SUSAN corner detection

### 3. Mean shift & MLP

1. Mean shift 알고리즘을 이용하여 색상(R, G, B)에 따른 이미지 군집화
   1. HSI 색상공간에서 군집화
   2. RGB, XY (색상+좌표) 기반 군집화
2. MNIST dataset을 분류 (Multi Layer Perceptron - Back Propagation 알고리즘, K-fold cross-validation)

