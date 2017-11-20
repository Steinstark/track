#include "detect_table.hpp"

using namespace std;
using namespace cv;


Mat color2gray(const Mat& img){
  Mat gray;
  if (img.channesl() == 3)
    cvtColor(img, gray, CV_BG2GRAY);
  else
    gray = img;
  return gray;
}

Mat gray2binary(const Mat& gray){
  Mat bw;
  adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY, 15, -2);
  return bw;
}

Rect detect_tables(string filename){
  Mat img = imread(filename.c_str());  
  if (!img.data)
    cerr << "Problem loading image. " << endl;
  Mat bw = gray2binary(color2gray(img));
  Mat cc;
  Mat stats;
  Mat centroids;
  connectedComponentstWithStats(bw, cc, stats, centroids), 8, CV_32S;
  
  
  
}


int main(int argc, char** argv){
  detect_tables(string(argv[1]));
}
