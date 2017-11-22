#include "detect_table.hpp"
#include <vector>

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
  nLabels = connectedComponentstWithStats(bw, cc, stats, centroids, 8, CV_32S);
  vector<Rect> bb;
  vector<int> areas;
  vector<double> density;
  //TODO
  //get stats of number of overlapping boundingboxes for each component
  //might be possible to do efficiently with an r-tree. This should prob
  //been mentioned in the original article
  for (int i = 1; i < nLabels; i++){
    int left = stats.at<int>(i, CC_STAT_LEFT);
    int top = stats.at<int>(i, CC_STAT_TOP);
    int width = stats.at<int>(i, CC_STAT_WIDTH);
    int height = stats.at<int>(i, CC_STAT_HEIGHT);
    bb.push_back(Rect(left, top, width, height));
    areas.push_back(stats.at<int>(i, CC_STAT_AREA));
    density.push_back(areas[i-1]/bb[i-1].area());
  }  
}
 main(int argc, char** argv){
  detect_tables(string(argv[1]));
}
