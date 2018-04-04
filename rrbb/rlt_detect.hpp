#ifndef TRACK_RLTDETECT_HPP
#define TRACK_RLTDETECT_HPP

#include <list>
#include <opencv2/opencv.hpp>


std::list<cv::Rect> findRLT(cv::Mat& text, cv::Mat& nontext);

#endif
