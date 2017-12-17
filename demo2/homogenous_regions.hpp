#ifndef TRACK_HOMOGENOUS_REGIONS
#define TRACK_HOMOGENOUS_REGIONS

#include <vector>
#include <opencv2/opencv.hpp>

std::vector<cv::Rect> homogenous_regions(const cv::Mat& img);

#endif
