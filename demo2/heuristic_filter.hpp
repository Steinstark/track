#ifndef TRACK_HEURISTIC_FILTER
#define TRACK_HEURISTIC_FILTER

#include <vector>
#include <opencv2/opencv.hpp>

std::vector<int> heuristic_filter(cv::Mat& in, cv::Mat& labels, cv::Mat& stats, cv::Mat& textimg, cv::Mat& nontextimg);

#endif

  
