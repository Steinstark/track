#ifndef TRACK_NRLTDETECT_HPP
#define TRACK_NRLTDETECT_HPP

#include <vector>
#include <opencv2/opencv.hpp>

std::vector<cv::Rect> findNRLT(cv::Mat& text, std::vector<cv::Rect> tables);

#endif
