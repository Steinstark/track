#ifndef TRACK_IMAGE_PRIMITIVES_HPP
#define TRACK_IMAGE_PRIMITIVES_HPP

#include <vector>
#include "text_tools.hpp"
#include "utility.hpp"
#include "tree_helper.hpp"

bool isLine(const ComponentStats& cs);
bool regionIsRectangle(const ComponentStats& cs);
bool containsManyElements(tree::RT& tree, const ComponentStats& cs);

bool hasLowDensity(ComponentStats& cs);
bool verticalArrangement(cv::Mat& textTable);
bool verticalArrangement(cv::Mat& textTable, std::list<TextLine>& lines);
bool manySmallRect(cv::Mat& text, ComponentStats& cs);
bool verifyReg(cv::Mat& text, cv::Mat& nontext, int count);
bool mostlyText(const cv::Mat& nontext);
bool hasLargeGraphElement(const cv::Mat nontext);

#endif
