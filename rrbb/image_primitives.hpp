#ifndef TRACK_IMAGE_PRIMITIVES_HPP
#define TRACK_IMAGE_PRIMITIVES_HPP

#include <vector>
#include "text_tools.hpp"
#include "utility.hpp"

bool isLine(const ComponentStats& cs);
bool lowCrapRatio(const cv::Mat text, const cv::Mat nontext);
bool hasLargeGraphElement(const cv::Mat nontext);
bool isTableLike(const cv::Mat& inverted);
bool hasOnewayLines(const cv::Mat horizontal, const cv::Mat vertical);
bool hasLowDensity(ComponentStats& cs);
bool mostlyText(const cv::Mat& nontext);
bool verifyReg(cv::Mat& text, cv::Mat& nontext);
bool verticalArrangement(cv::Mat& textTable);
bool verticalArrangement(cv::Mat& textTable, std::list<TextLine>& lines);

#endif
