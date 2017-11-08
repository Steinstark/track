#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

int main(){
  const string filename("file0.png");
  Mat small = imread(filename.c_str()), rgb;
  pyrDown(small, rgb);
  cvtColor(rgb, small, CV_BGR2GRAY);
  Mat grad;
  Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
  morphKernel = getStructuringElement(MORPH_RECT, Size(9, 1));
  morphologyEx(small, grad, MORPH_GRADIENT, morphKernel);
  Mat bw;
  threshold(grad, bw, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU);
  Mat connected;
  morphKernel = getStructuringElement(MORPH_RECT, Size(9, 1));
  morphologyEx(bw, connected, MORPH_CLOSE, morphKernel);
  Mat mask = Mat::zeros(bw.size(), CV_8UC1);
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  findContours(connected, contours, hierarchy,  CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
  for (int idx = 0;  idx >= 0; idx = hierarchy[idx][0]){
    Rect rect =  boundingRect(contours[idx]);
    Mat maskROI(mask, rect);
    maskROI = Scalar(0, 0, 0);
    drawContours(mask, contours, idx, Scalar(255,255,255), CV_FILLED);
    double r = (double)countNonZero(maskROI)/(rect.width*rect.height);
    if (r > .45 && rect.height > 8 && rect.width > 8){
      rectangle(rgb, rect, Scalar(0, 255, 0), 2);
    }
  }
  imshow("marked", rgb);
  waitKey(0);
  return 0;
}
