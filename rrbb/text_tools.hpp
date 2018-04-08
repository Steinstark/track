#ifndef TRACK_TEXT_TOOLS_HPP
#define TRACK_TEXT_TOOLS_HPP

#include <vector>
#include <opencv2/opencv.hpp>
#include "utility.hpp"
#include "tree_helper.hpp"

struct TextLine{
  std::vector<cv::Rect> elements;
  cv::Rect box;
  cv::Rect getBox() const;
  double getSpace();
  double getMeanLength();
  void addSegment(cv::Rect r);
};

std::list<TextLine> findLines(cv::Mat& img);

#endif
