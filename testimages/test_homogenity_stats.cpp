#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "homogenous_regions.cpp"

using namespace std;
using namespace cv;

Mat gray2binary(const Mat& gray){
  Mat bw;
  adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY, 15, -2);
  return bw;
}

int main(){
  string filename("test1.png");
  Mat img = imread(filename, IMREAD_GRAYSCALE);
  if (!img.data)
    cerr << "Problem loading image. " << endl;
  Mat bw = gray2binary(img);
  Mat hist;
  cout << homogenity_stats(bw, hist, 1) << endl;
  cout << homogenity_stats(bw, hist, 0) << endl;
}
