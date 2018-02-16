#ifndef TRACK_HOMOGENOUS_REGIONS
#define TRACK_HOMOGENOUS_REGIONS

#include <list>
#include <opencv2/opencv.hpp>

std::list<cv::Rect> homogenous_regions(const cv::Mat& img);
std::list<cv::Rect> homogenous_recursive(const cv::Mat& img);

#endif
