#ifndef TRACK_IMAGE_PRIMITIVES_HPP
#define TRACK_IMAGE_PRIMITIVES_HPP

#include <vector>
#include "text_tools.hpp"
#include "utility.hpp"
#include "tree_helper.hpp"

bool isLine(const ComponentStats& cs);
bool hasLargeGraphElement(const cv::Mat nontext);
bool hasLowDensity(ComponentStats& cs);
bool mostlyText(const cv::Mat& nontext);
bool verifyReg(cv::Mat& text, cv::Mat& nontext, int count);
bool verticalArrangement(cv::Mat& textTable);
bool verticalArrangement(cv::Mat& textTable, std::list<TextLine>& lines);

#endif
