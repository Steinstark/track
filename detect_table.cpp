#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(){
  string filename("scan-hund.pdf-1_1.png");
  Mat src = imread(filename);
  if (!src.data)
    cerr << "Problem loading image. " << endl;
  Mat rsz;
  Size size(800, 900);
  resize(src, rsz, size);
  Mat gray;
  if (rsz.channels() == 3){
    cvtColor(rsz, gray, CV_BGR2GRAY);
  }else
    gray = rsz;

  Mat bw;
  adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY, 15, -2);
  imshow("binary", bw);
  waitKey(0);
  return 0;
}
