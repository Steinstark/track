#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

int main(int argc, char** argv){
  Mat rsz = imread(argv[1]);
  if (!rsz.data)
    cerr << "Problem loading image. " << endl;
  Mat gray;
  if (rsz.channels() == 3){
    cvtColor(rsz, gray, CV_BGR2GRAY);
  }else
    gray = rsz;
  Mat bw;
  adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY, 15, -2);
  Mat red;
  int s = 5;
  reduce(bw, red, 1, CV_REDUCE_SUM, CV_32S);
  Mat black = Mat::zeros(bw.size(), CV_8U);
  Mat blurred;  
  blur(red, blurred, Size(1,30));
  Mat sob;
  Sobel(blurred, sob, CV_32F ,CV_SCHARR, 0, 1);
  Mat peaks = getPeaks(sob);
  
  for (int i = 0; i < dst.size().height; i++){
    for (int j = 0; j < dst.at<int>(i,0)/255; j++){
      black.at<uchar>(i,j) = 255;
    }
  }
  imshow("img", black);  
  waitKey(0);

  imshow("img", dst);
  waitKey(0);
  return 0;
}
