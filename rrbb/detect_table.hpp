#ifndef TRACK_DETECT_TABLE_HPP
#define TRACK_DETECT_TABLE_HPP

#include <list>
#include <opencv2/opencv.hpp>

std::list<cv::Rect> detect_tables(cv::Mat& text);
#endif
