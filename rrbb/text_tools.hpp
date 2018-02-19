#ifndef TRACK_TEXT_TOOLS_HPP
#define TRACK_TEXT_TOOLS_HPP

#include <vector>
#include <opencv2/opencv.hpp>
#include "utility.hpp"
#include "tree_helper.hpp"

struct TextLine{
  std::vector<cv::Rect> elements;  
  cv::Rect getBox() const;
  double getSpace();
  double getMeanLength();
  TextLine(){};
};

std::vector<TextLine> linesInRegion(tree::RT& tree, std::vector<ComponentStats> textData, cv::Rect region);
std::vector<TextLine> findLines(std::vector<cv::Rect>& rects);

#endif
