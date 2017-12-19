#include <vector>
#include <opencv2/opencv.hpp>

#include "mla.hpp"
#include "homogenous_regions.hpp"
#include "recursive_filter.hpp"

using namespace std;
using namespace cv;

//IMPROVEMENT
//code duplication of mlc.cpp
void remove_nontext2(Mat& text, Mat& ntext, const Mat& cc,  Rect r , const vector<int>& nontext){
  Mat mask(cc.size(), CV_8UC1, Scalar(0));
  for (int i = 0; i <  nontext.size(); i++){
    mask = mask | (cc==nontext[i]);
  }
  mask = ~mask;
  Mat local = text(r);
  local = local & mask;  
  //texttext = Mat(text.size(), CV_8UC1, Scalar(0));
  local.copyTo(text(r));
  local = ntext(r);
  local = local | ~mask;
  local.copyTo(ntext(r));
}

void multi_level_analysis(Mat& text, Mat& ntext){
  queue<Rect> q;
  q.push(Rect(0, 0, text.cols, text.rows));
  while (!q.empty()){
    Rect r = q.front();
    q.pop();
    Mat region = text(r);
    vector<Rect> homboxes = homogenous_regions(region);
    for (int i = 0; i < homboxes.size(); i++){
      Rect lr = homboxes[i]+r.tl();
      Mat box = text(lr);
      Mat cc, stats, centroids;
      int labels = connectedComponentsWithStats(box, cc, stats, centroids, 8, CV_32S);
      //      rectangle(boxes, lr,Scalar(255));
      vector<int> nontext = recursive_filter(stats);
      remove_nontext2(text, ntext, cc, lr, nontext);
      if (nontext.size() < labels - 1 && nontext.size() > 0)
	q.push(lr);
    }
  }
}
