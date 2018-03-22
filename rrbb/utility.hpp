#ifndef TRACK_UTILITY_HPP
#define TRACK_UTILITY_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include "tree_helper.hpp"

cv::Rect stats2rect(const cv::Mat& stats, int i);

struct Line{
  int l, r;
  Line(int l, int r): l(l), r(r){};
  int length() const{
    return r-l;
  }
};

struct ComponentStats{
  int index;
  cv::Rect r;
  int area, bb_area;
  double density, hwratio;
  ComponentStats(cv::Rect r, int area, int i);
};

struct ImageMeta{
  int width, height;
  tree::RT t_tree, nt_tree;
  ImageMeta(int width, int height, std::vector<ComponentStats>& text, std::vector<ComponentStats>& nontext);
};

struct ImageDataBox{
  cv::Mat text, nontext;
  std::vector<ComponentStats> textData, nontextData;
  ImageDataBox(cv::Mat& text, cv::Mat& nontext);
};

ComponentStats stats2component(const cv::Mat& stats, int statsIndex, int compIndex = -1);
std::vector<ComponentStats> statistics(cv::Mat& img);

template <typename T>
void boundingVector(const cv::Mat& img, T bb){
  cv::Mat cc, stats, centroids;
  int labels = cv::connectedComponentsWithStats(img, cc, stats, centroids, 8, CV_32S);
  for (int i = 1; i < labels; i++){
    bb = stats2rect(stats, i); 
  }
}

template <typename LineContainer>
void find_lines(const cv::Mat& hist, LineContainer& text, LineContainer& space){
  int length = std::max(hist.rows, hist.cols);
  int t = 0;
  for (int i = 1; i < length; i++){
    if (!hist.at<double>(i-1) && hist.at<double>(i)){
      t = i;
    }
    else if (hist.at<double>(i-1) && !hist.at<double>(i)){
      text.push_back(Line(t,i));
      t = -1;
    }
  }
  if (t != -1)
    text.push_back(Line(t,length));
  auto it = text.begin();
  Line& prev = *it++;
  while (it !=text.end()){
    Line current = *it++;
    space.push_back(Line(prev.r, current.l));
    prev = current;
  }
}

void find_all_lines(const cv::Mat& hist, std::vector<Line>& text, std::vector<Line>& space);

std::list<cv::Rect> mergeIntersecting(std::list<cv::Rect>& tables);

#endif
