#ifndef TRACK_NRLTDETECT_HPP
#define TRACK_NRLTDETECT_HPP

#include <list>
#include <functional>
#include <opencv2/opencv.hpp>

std::list<cv::Rect> findNRLT(cv::Mat& text, cv::Mat& nontext, ImageMeta& im);

#endif
