#include "detect.hpp"
#include <list>
#include <opencv2/opencv.hpp>
#include "rlt_detect.hpp"

using namespace std;
using namespace cv;

list<Rect> detect(Mat& text, Mat& nontext){
  list<Rect> tables = findRLT(text, nontext);
  return tables;
}
