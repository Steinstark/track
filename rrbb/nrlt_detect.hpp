#ifndef TRACK_NRLTDETECT_HPP
#define TRACK_NRLTDETECT_HPP

#include <list>
#include <functional>
#include <opencv2/opencv.hpp>

std::list<cv::Rect> findNRLT(cv::Mat& text, std::list<cv::Rect> tables, std::function<bool(cv::Mat&, ComponentStats&)> f);

#endif
