#include "utility.hpp"
#include <algorithm>
#include <vector>
#include <opencv2/opencv.hpp>
#include "tree_helper.hpp"

using namespace std;
using namespace cv;
using namespace tree;

ComponentStats::ComponentStats(Rect r, int area, int i) : r(r), area(area),  index(i){
  bb_area = r.area();
  density = (double)area/bb_area;
  hwratio = (double)min(r.width, r.height)/max(r.width, r.height);
}

Rect stats2rect(const Mat& stats, int i){
  int left = stats.at<int>(i, CC_STAT_LEFT);
  int top = stats.at<int>(i, CC_STAT_TOP);
  int width = stats.at<int>(i, CC_STAT_WIDTH);
  int height = stats.at<int>(i, CC_STAT_HEIGHT);
  return Rect(left, top, width, height);
}

ComponentStats stats2component(const Mat& stats, int statsIndex){
  Rect r = stats2rect(stats, statsIndex);
  int area = stats.at<int>(statsIndex, CC_STAT_AREA);
  return ComponentStats(r, area, statsIndex);
}

vector<ComponentStats> statistics(const Mat& img){
  Mat cc;
  return statistics(img, cc);
}

vector<ComponentStats> statistics(const Mat& img, Mat& cc){
  vector<ComponentStats> components;
  statistics(img, cc, back_inserter(components));
  return components;
}
