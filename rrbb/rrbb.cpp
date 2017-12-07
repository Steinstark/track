#include <vector>
#include <algorithm>
#include <queue>

#include "detect_table.hpp"
#include "RTree.h"
#include "heuristic_filter.hpp"
#include "homogenous_regions.hpp"

using namespace std;
using namespace cv;


//TODO
//implement
void recursiveFilter(Rect r){
  return;
}

void mla(const Mat& img){
  vector<Rect> homboxes = homogenous_regions(img);
  Mat boxes = img.clone();
  for (int i = 0; i < homboxes.size(); i++){
    rectangle(boxes, homboxes[i],Scalar(255));
  }
  imshow("img", boxes);
  waitKey(0);
}

Mat gray2binary(const Mat& gray){
  Mat bw;
  adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY, 15, -2);
  return bw;
}

vector<Rect> detect_tables(string filename){
  Mat img = imread(filename.c_str(), IMREAD_GRAYSCALE);  
  if (!img.data)
    cerr << "Problem loading image. " << endl;
  Mat bw = gray2binary(img);
  Mat cc;
  Mat stats;
  Mat centroids;
  int nLabels = connectedComponentsWithStats(bw, cc, stats, centroids, 8, CV_32S);
  Mat text, nontext;
  heuristic_filter(bw, cc, stats,text, nontext);
  //  imshow("text", text);
  //  waitKey(0);
  mla(text);
  //noise_removal(bw);
}

 main(int argc, char** argv){
  detect_tables(string(argv[1]));
}
