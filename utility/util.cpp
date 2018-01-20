#include "util.hpp"
#include <cmath>
#include <functional>

using namespace std;
using namespace cv;

bool fileHasType(string file, string extension){
  int pos = file.find_last_of(".");
  return file.substr(pos+1) == extension;
}

Rect pos2rect(int l, int t, int r, int b){
  return Rect(l, t, r-l, b-t);
}

int pix2a4dpi(int xPixels, int yPixels){
  double xDPI = xPixels/8.27, yDPI = yPixels/11.69;
  return (xDPI + yDPI)/2;
}

template <typename T>
double mean(vector<T> v, function<int(T)> f){
  int val = 0;
  for (int i = 0; i < v.size(); i++){
    val += f(v[i]);
  }
  return (double)val/v.size();
}

template <typename T>
double mean(vector<T> v, function<double(T)> f){
  double val = 0;
  for (int i = 0; i < v.size(); i++){
    val += f(v[i]);
  }
  return (double)val/v.size();
}

template <typename T>
double variance(vector<T> v, function<int(T)> f){
  double m2 = pow(mean(v, f),2);
  double m1 = mean(v, [&f](T e){ return pow(f(e),2);});
  return m1 - m2;
}

template <typename T>
double variance(vector<T> v, function<double(T)> f){
  double m2 = pow(mean(v, f),2);
  double m1 = mean(v, [&f](T e){ return pow(f(e),2);});
  return m1 - m2;
}

cv::Mat gray2binary(const cv::Mat& gray){
  Mat bw;
  adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY, 15, -2);
  return bw;
}Mat gray2binary(const Mat& gray){
  Mat bw;
  adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY, 15, -2);
  return bw;
}
