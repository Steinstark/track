#ifndef TRACK_IMAGE_PRIMITIVES_HPP
#define TRACK_IMAGE_PRIMITIVES_HPP

#include <vector>
#include "text_tools.hpp"
#include "utility.hpp"

bool isHorizontalLine(ImageMeta& im, const ComponentStats& cs);
bool regionIsRectangle(const ComponentStats& cs);
bool isColorBlock(ImageMeta& im, const ComponentStats& cs);
bool containsManyTextElements(ImageMeta& im, const ComponentStats& cs);

bool hasLowDensity(ComponentStats& cs);
bool verticalArrangement(cv::Mat& textTable, std::vector<TextLine>& lines);
bool manySmallRect(cv::Mat& text, ComponentStats& cs);
bool noCut(ImageMeta& im, std::vector<ComponentStats>& textData, cv::Rect r);
bool onlyText(ImageMeta& im, cv::Rect r);
#endif
