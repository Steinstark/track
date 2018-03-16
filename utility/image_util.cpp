#include "image_util.hpp"
#include <cmath>

using namespace std;
using namespace cv;

double rotAngle(RotatedRect rr){
  Rect bb = rr.boundingRect();
  Point2f cp = rr.center;
  double dxl = cp.x - bb.x;
  double dxr = bb.br().x - cp.x;
  if (abs(rr.angle) >= 90)
    return 0;
  if ( dxl > dxr)
    return -rr.angle;
  return rr.angle;  
}

double counterRotAngle(Mat& img){
  vector<vector<Point> > contours;  
  findContours(img, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
  if (contours.empty())
    return 0;
  return rotAngle(minAreaRect(contours[0]));
}

double counterRotAngle(vector<Point>& contour){
  return rotAngle(minAreaRect(contour));  
}

void rotate(Mat& src, Mat& dst, double angle){
    Point2f ptCp(src.cols*0.5, src.rows*0.5);
    Mat M = getRotationMatrix2D(ptCp, angle, 1.0);
    warpAffine(src, dst, M, src.size(), INTER_CUBIC);
}

Rect pos2rect(int l, int t, int r, int b){
  return Rect(l, t, r-l, b-t);
}

int pix2a4dpi(int xPixels, int yPixels){
  double xDPI = xPixels/8.27, yDPI = yPixels/11.69;
  return (xDPI + yDPI)/2;
}


Mat gray2binary(const Mat& gray){
  Mat bw;
  adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY, 15, -2);
  return bw;
}

Mat color2binary(const Mat& img){
  Mat gray;
  cvtColor(img, gray, COLOR_BGR2GRAY);
  return gray2binary(gray);  
}

Mat lineMask(Mat& img){
  Mat element = getStructuringElement(MORPH_RECT, Size(1, 29), Point(-1, -1));
  Mat vertical, horizontal;
  erode(img, vertical, element);
  dilate(vertical, vertical, element);
  transpose(element, element);
  erode(img, horizontal, element);
  dilate(horizontal, horizontal, element);
  return horizontal | vertical;
}

void remove_lines(Mat& from, Mat& to){
  Mat lines = lineMask(from);
  to |= lines;
  bitwise_not(lines, from, from);
}

void move2(Mat& from, Mat& to, const Mat& cc, int i){
  Mat mask(cc.size(), CV_8UC1, Scalar(0));
  mask = mask | (cc==i);
  mask = mask & from;
  to = to | mask;
  from = from & ~mask;
}
