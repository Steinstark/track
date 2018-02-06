#ifndef TRACK_RLTDETECT_HPP
#define TRACK_RLTDETECT_HPP

#include <vector>
#include <opencv2/opencv.hpp>
#include "utility.hpp"

std::vector<cv::Rect> findRLT(ImageDataBox& imd, ImageMeta& im);
bool verifyRLT(cv::Mat& region, ImageMeta& im, std::vector<ComponentStats>& textData, ComponentStats& cs);

#endif
