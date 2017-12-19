#include <vector>
#include <opencv2/opencv.hpp>
#include "noise.hpp"
#include "RTree.h"
#include "tree_helper.hpp"
#include "utility.hpp"

using namespace std;
using namespace cv;
using RT = RTree<int, int, 2, float>;

Rect stats2rect(const Mat& stats, int i){
  int left = stats.at<int>(i, CC_STAT_LEFT);
  int top = stats.at<int>(i, CC_STAT_TOP);
  int width = stats.at<int>(i, CC_STAT_WIDTH);
  int height = stats.at<int>(i, CC_STAT_HEIGHT);
  return Rect(left, top, width, height);
}

void remove_noise(Mat& text, Mat& nontext){
  Mat t_cc, t_stats, t_centroids;
  int t_labels = connectedComponentsWithStats(text, t_cc, t_stats, t_centroids, 8, CV_32S);
  RT t_tree;
  for (int i = 1; i < t_labels; i++){
    insert2tree(t_tree, stats2rect(t_stats, i), i);
  }
  int s = 3;
  Mat element = getStructuringElement( MORPH_RECT, Size(s,1), Point(-1,-1));
  Mat ntd;
  dilate(nontext, ntd, element);
  Mat nt_cc, nt_stats, nt_centroids;
  int nt_labels = connectedComponentsWithStats(ntd, nt_cc, nt_stats, nt_centroids, 8, CV_32S);
  RT nt_tree;
  for (int i = 1; i < nt_labels; i++){
    insert2tree(nt_tree, stats2rect(nt_stats, i), i);
  }
  for (int i = 1; i < t_labels; i++){
    vector<int> overlap;
    search_tree(nt_tree, stats2rect(t_stats, i), overlap);
    int t_area = t_stats.at<int>(i, CC_STAT_AREA);
    Rect tr = stats2rect(t_stats, i);
    for (int j = 0; j < overlap.size(); j++){
      Rect ntr = stats2rect(nt_stats, overlap[j]);
      Rect intersect = tr & ntr;
      if (intersect != tr){
	int nt_area = nt_stats.at<int>(overlap[j], CC_STAT_AREA);
	if (t_area > nt_area){
	  move2(nontext, text, nt_cc, overlap[j]);
	}else{
	  move2(text, nontext, t_cc, i);
	}
      }
    }
  }
}
