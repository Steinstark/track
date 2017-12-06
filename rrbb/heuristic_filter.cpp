#include "heuristic_filter.hpp"
#include "RTree.h"
#include <vector>

using namespace std;
using namespace cv;

double getRatio(Rect r){
  return (double)min(r.width, r.height)/max(r.width, r.height);
}

void getOverlapCount(const vector<Rect>& bb,
			    vector<int>& overlap,
			    vector<int>& onRow,
			    vector<int>& onCol){
  const int inf = 1000000;
  RTree<int, int, 2, float> tree;
  for (int i = 0; i < bb.size(); i++){
    int tl[] = {bb[i].x, bb[i].y};
    int br[] = {bb[i].br().x, bb[i].br().y};
    tree.Insert(tl, br, i);
  }
  for (int i = 0; i < bb.size(); i++){
    int tl[] = {bb[i].x, bb[i].y};
    int br[] = {bb[i].br().x, bb[i].br().y};
    int count = tree.Search(tl, br, NULL, NULL);
    overlap.push_back(count-1);
    tl[0] = -inf;
    br[0] = inf;
    count = tree.Search(tl,br, NULL, NULL);
    onRow.push_back(count-1);
    tl[0] = bb[i].x;
    tl[1] = -inf;
    br[0] = bb[i].br().x;
    br[1] = inf;
    count = tree.Search(tl, br, NULL, NULL);
    onCol.push_back(count-1);    
  }
}

vector<int> heuristic_filter(Mat& in, Mat& labels, Mat& stats, Mat& textimg, Mat& nontextimg){
  int nLabels = stats.rows;
  vector<Rect> bb(nLabels);
  bb[0] = Rect(-1, -1, 0, 0);
  vector<int> areas(nLabels);
  vector<double> density(nLabels);
  vector<double> hwratio(nLabels);
  for (int i = 1; i < nLabels; i++){
    int left = stats.at<int>(i, CC_STAT_LEFT);
    int top = stats.at<int>(i, CC_STAT_TOP);
    int width = stats.at<int>(i, CC_STAT_WIDTH);
    int height = stats.at<int>(i, CC_STAT_HEIGHT);
    bb[i] = Rect(left, top, width, height);
    areas[i] = stats.at<int>(i, CC_STAT_AREA);
    hwratio[i] = getRatio(bb[i]);
    density[i] = (double)areas[i]/bb[i].area();    
  }
  vector<int> onRow;
  vector<int> onCol;
  vector<int> overlap;
  getOverlapCount(bb, overlap, onRow, onCol);

  vector<int> text;
  vector<int> nontext;
  for (int i = 1; i < nLabels; i++){
    if (areas[i] < 6 ||
	overlap[i] > 4 ||
	(hwratio[i] < 0.06 && bb[i].width > bb[i].height) ||      
	density[i] < 0.06){
      nontext.push_back(i);
    }
    else
      text.push_back(i);
  }
  Mat mask(labels.size(), CV_8UC1, Scalar(0));
  for (int i = 0; i <  nontext.size(); i++){
    mask = mask | (labels==nontext[i]);
  }
  textimg = Mat(in.size(), CV_8UC1, Scalar(0));
  in.copyTo(textimg, ~mask);
  for (int i = 0; i <  text.size(); i++){
    mask = mask | (labels== text[i]);
  }
  nontextimg = Mat(in.size(), CV_8UC1, Scalar(0));
  in.copyTo(nontextimg, ~mask);
  return text;
}
