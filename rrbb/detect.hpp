#ifndef TRACK_DETECT_HPP
#define TRACK_DETECT_HPP

#include <list>
#include <opencv2/opencv.hpp>
#include "utility.hpp"

std::list<cv::Rect> detect(cv::Mat& text, cv::Mat& nontext);

#endif
