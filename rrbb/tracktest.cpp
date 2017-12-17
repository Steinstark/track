#include <iostream>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/opencv.hpp>
#include <stdio.h>
using namespace cv;
using namespace std;
const int alpha_slider_max = 100;
int alpha_slider;
double alpha;
double beta;
Mat dst;
static void on_trackbar(int val, void* obj)
{
  Mat src = *((Mat*) obj);
  blur(src, dst, Size(val, val), Point(-1,-1), BORDER_DEFAULT);
  imshow( "Linear Blend", dst );
}
int main( void )
{
  Mat src1 = imread("text.png", IMREAD_GRAYSCALE);
  if( src1.empty() ) { printf("Error loading src1 \n"); return -1; }
  alpha_slider = 5;
  namedWindow("Linear Blend", WINDOW_AUTOSIZE); // Create Window
  char TrackbarName[50];
  sprintf( TrackbarName, "Alpha x %d", alpha_slider_max );

  createTrackbar( TrackbarName, "Linear Blend", &alpha_slider, alpha_slider_max, on_trackbar,&src1);
  waitKey(0);
  return 0;
}




