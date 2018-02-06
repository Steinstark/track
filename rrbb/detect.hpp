#ifndef TRACK_DETECT_HPP
#define TRACK_DETECT_HPP

#include <vector>
#include <opencv2/opencv.hpp>
#include "utility.hpp"

std::vector<cv::Rect> detect(cv::Mat& text, cv::Mat& nontext);

#endif
