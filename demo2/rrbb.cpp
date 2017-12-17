#include <vector>
#include <algorithm>
#include <queue>

#include "detect_table.hpp"
#include "RTree.h"
#include "heuristic_filter.hpp"
#include "homogenous_regions.hpp"
#include "recursive_filter.hpp"

using namespace std;
using namespace cv;

void remove_nontext(const Mat& img, const Mat& cc,  Rect r , const vector<int>& nontext){
  Mat mask(cc.size(), CV_8UC1, Scalar(0));
  for (int i = 0; i <  nontext.size(); i++){
    mask = mask | (cc==nontext[i]);
  }
  mask = ~mask;
  Mat local = img(r);
  local = local & mask;  
  //textimg = Mat(img.size(), CV_8UC1, Scalar(0));
  local.copyTo(img(r));
}

void mla(const Mat& img){
  queue<Rect> q;
  q.push(Rect(0, 0, img.cols, img.rows));
  while (!q.empty()){
    Rect r = q.front();
    q.pop();
    Mat region = img(r);
    vector<Rect> homboxes = homogenous_regions(region);
    for (int i = 0; i < homboxes.size(); i++){
      Rect lr = homboxes[i]+r.tl();
      Mat box = img(lr);
      Mat cc;
      Mat stats;
      Mat centroids;
      int labels = connectedComponentsWithStats(box, cc, stats, centroids, 8, CV_32S);
      //      rectangle(boxes, lr,Scalar(255));
      vector<int> nontext = recursive_filter(box, stats);
      remove_nontext(img, cc, lr, nontext);
      if (nontext.size() < labels - 1 && nontext.size() > 0)
	q.push(lr);
    }
    //imshow("img", boxes);
    //    waitKey(0);
  }
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
  imshow("heuristic", text);
  waitKey(0);
  mla(text);
  imshow("mla", text);
  waitKey(0);
  //noise_removal(bw);
}

 main(int argc, char** argv){
  detect_tables(string(argv[1]));
}
