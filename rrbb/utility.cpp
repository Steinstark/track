#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#include "utility.hpp"

void move2(Mat& from, Mat& to, const Mat& cc, int i){
  Mat mask(cc.size(), CV_8UC1, Scalar(0));
  mask = mask | (cc==i);
  mask = mask & from;
  to = to | mask;
  from = from & ~mask;
}
