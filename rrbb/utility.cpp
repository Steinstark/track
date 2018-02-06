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

ComponentStats stats2component(const Mat& stats, int statsIndex, int compIndex){
  if (compIndex < 0)
    compIndex = statsIndex;
  Rect r = stats2rect(stats, statsIndex);
  int area = stats.at<int>(statsIndex, CC_STAT_AREA);
  return ComponentStats(r, area, compIndex);
}

vector<ComponentStats> statistics(Mat& img){
  Mat cc, stats, centroids;
  int labels = connectedComponentsWithStats(img, cc, stats, centroids, 8, CV_32S);
  vector<ComponentStats> components;
  for (int i = 1; i < labels; i++){
    components.push_back(stats2component(stats, i, i-1)); 
  }
  return components;
}

ImageMeta::ImageMeta(int width, int height, vector<ComponentStats>& text, vector<ComponentStats>& nontext): width(width), height(height){
  for (int i = 0; i < text.size(); i++)
    insert2tree(t_tree, text[i].r, i);
  for (int i = 0; i < nontext.size(); i++)
    insert2tree(nt_tree, nontext[i].r, i);    
}

ImageDataBox::ImageDataBox(Mat& text, Mat& nontext): text(text), nontext(nontext){
  textData = statistics(text);
  nontextData = statistics(nontext);
}

void find_lines(const Mat& hist, vector<Line>& text, vector<Line>& space){
  int length = max(hist.rows, hist.cols);
  int t = 0;
  for (int i = 1; i < length; i++){
    if (!hist.at<double>(i-1) && hist.at<double>(i)){
      t = i;
    }
    else if (hist.at<double>(i-1) && !hist.at<double>(i)){
      text.push_back(Line(t,i-1));
      t = -1;
    }
  }
  if (t != -1)
    text.push_back(Line(t,length-1));
  for (int i = 1; i < text.size(); i++){
    space.push_back(Line(text[i-1].r+1, text[i].l-1));
  }
}

void find_all_lines(const Mat& hist, vector<Line>& text, vector<Line>& space){
  find_lines(hist, text, space);
  Line a = text.front();
  Line b = text.back();
  int length = max(hist.rows, hist.cols);
  if (a.l > 1)
    space.push_back(Line(0, a.l-1));
  if (b.r < length-1)
    space.push_back(Line(b.r, length-1));      
}

