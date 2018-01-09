#ifndef TRACK_DETECT_TABLE_HPP
#define TRACK_DETECT_TABLE_HPP

#include <opencv2/opencv.hpp>

std::vector<cv::Rect> detect_tables(cv::Mat& text);
#endif
