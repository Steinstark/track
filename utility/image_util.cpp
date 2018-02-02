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

double counterRotAngle(vector<Point> contour){
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

void move2(Mat& from, Mat& to, const Mat& cc, int i){
  Mat mask(cc.size(), CV_8UC1, Scalar(0));
  mask = mask | (cc==i);
  mask = mask & from;
  to = to | mask;
  from = from & ~mask;
}

void displayHist(string str, const Mat& img){
  int m = 0;
  int length = max(img.rows, img.cols);
  for (int i = 0; i < length; i++){
    int val = img.at<double>(i);
    m = max(m,val);
  }
  Mat hist;
  if (img.rows > img.cols){
    hist = Mat::zeros(img.rows, m+50, CV_8U);
    for (int i = 0; i < img.rows; i++){
      for (int j = 0; j < img.at<double>(i,0); j++){
	hist.at<uchar>(i,j) = 255;
      }
    }
  }else{
    hist = Mat::zeros(m+50, img.cols, CV_8U);
    for (int i = 0; i < img.cols; i++){
      for (int j = 0; j < img.at<double>(0, i); j++){
	hist.at<uchar>(j,i) = 255;
      }
    }
  }
  imshow(str, hist);
}
