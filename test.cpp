#include <algorithm>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

bool isExtrema(double a, double b){
  return (a < 0 && b > 0) || (a > 0 && b < 0);
}

double absol(double v){
  if (v < 0) return -v;
  return v;
}


void displayHist(const Mat& img, string str){

  int m = 0;
  for (int i = 0; i < img.rows; i++){
    int val = img.at<double>(i,0);
    m = max(m,val);
  }
  Mat hist = Mat::zeros(img.rows, m+50, CV_8U);
  for (int i = 0; i < img.rows; i++){
    for (int j = 0; j < img.at<double>(i,0); j++){
      hist.at<uchar>(i,j) = 255;
    }
  }
  imshow(str, hist);
  waitKey(0);
}

int main(int argc, char** argv){
  Mat img = imread(argv[1], IMREAD_GRAYSCALE);
  Mat bw;
  adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY, 15, -2);
  Mat red;
  int s = 5;
  //histogram
  int scale = 255;
  reduce(bw, red, 1, CV_REDUCE_SUM, CV_64F);
  Mat sred = red / scale;
  displayHist(sred, "red");

  Mat blurred;
  //medelkernel 
  blur(sred, blurred, Size(1,30));
  displayHist(blurred, "blurred");
  Mat sob;
  //gradienten
  Scharr(blurred, sob, -1, 0, 1);
  displayHist(sob, "sobel");
  vector<int> a0;  
  for (int i = 1; i < sob.rows ; i++){
    if (isExtrema(sob.at<double>(i,0), sob.at<double>(i-1,0))){
      a0.push_back(i);
    }
  }  
  Mat peaks(a0.size(), 1, CV_32S, a0.data());
  vector<int> a1;
  for (int i = 1; i < a0.size(); i++){
    a1.push_back(a0[i] - a0[i-1]);
  }
  Mat delta(a0.size()-1, 1, CV_32S, a1.data());
  Mat mean, stddev;
  meanStdDev(delta, mean, stddev);
  //cout << "Mean: " << mean.at<double>(0,0) << " stddev: " << stddev.at<double>(0,0) << endl;
  double st = stddev.at<double>(0,0);
  double var = st*st;
  cout << "Variance: " << var << endl;
  return 0;
}
