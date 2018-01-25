#ifndef TRACK_UTILITY_HPP
#define TRACK_UTILITY_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include "RTree.h"
#include "util.hpp"

void move2(cv::Mat& from, cv::Mat& to, const cv::Mat& cc, int i);

cv::Rect stats2rect(const cv::Mat& stats, int i);

void displayHist(std::string str, const cv::Mat& img);

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
  RTree<int, int, 2, float> t_tree, nt_tree;
  ImageMeta(int width, int height, std::vector<ComponentStats> text, std::vector<ComponentStats> nontext);
};

struct TextLine{
  std::vector<cv::Rect> elements;
  cv::Rect getBox(){
    if (elements.empty())
      return cv::Rect();
    cv::Rect r = elements[0];
    for (int i = 1; i < elements.size(); i++){
      r |= elements[i];
    }
    return r;
  }
  
  double getSpace(){
    std::sort(elements.begin(), elements.end(), [](cv::Rect a, cv::Rect b){return a.x < b.x;});
    int val = 0;
    for (int i = 1; i < elements.size(); i++){
      val += elements[i].x-elements[i-1].br().x;
    }
    return (double)val/elements.size();
  }

  double getMeanLength(){
    return mean<cv::Rect, int>(elements, [](cv::Rect r){return r.width;});
  }
};

ComponentStats stats2component(const cv::Mat& stats, int i);
std::vector<ComponentStats> statistics(cv::Mat& img);

void find_lines(const cv::Mat& hist, std::vector<Line>& text, std::vector<Line>& space);
void find_all_lines(const cv::Mat& hist, std::vector<Line>& text, std::vector<Line>& space);

#endif
