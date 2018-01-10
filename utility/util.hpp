#ifndef TRACK_UTIL_HPP
#define TRACK_UTIL_HPP

#include <string>
#include <opencv2/opencv.hpp>

bool fileHasType(std::string file, std::string  extension);
cv::Rect pos2rect(int l, int t, int r, int b);

#endif
