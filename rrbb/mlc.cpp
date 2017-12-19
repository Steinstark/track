#include <vector>
#include <opencv2/opencv.hpp>

#include "mlc.hpp"
#include "homogenous_regions.hpp"
#include "recursive_filter.hpp"

using namespace std;
using namespace cv;

//IMPROVEMENT
//duplicate code. Already in rrbb.cpp.
void remove_nontext1(Mat& text, Mat& ntext, const Mat& cc,  Rect r , const vector<int>& nontext){
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

void multi_level_classification(Mat& text, Mat& nontext){
  const double TOL = 0.99;
  while(1){
    int total = 0, removed = 0;
    vector<Rect> regions = homogenous_regions(text);
    for (int i = 0; i < regions.size(); i++){    
      Mat cc, stats, centroids;
      Mat block = text(regions[i]);
      int labels = connectedComponentsWithStats(block, cc, stats, centroids, 8, CV_32S);
      vector<int> a0 = recursive_filter(stats);
      remove_nontext1(text, nontext, cc, regions[i], a0);
      total += labels - 1;
      removed += a0.size();      
    }
    if ((double)total/removed >= TOL)
      break;
  }
}
