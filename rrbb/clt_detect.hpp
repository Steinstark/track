#ifndef TRACK_CLTDETECT_HPP
#define TRACK_CLTDETECT_HPP

#include <vector>
#include <opencv2/opencv.hpp>
#include "utility.hpp"

std::vector<cv::Rect> findCLT(ImageDataBox& imd, ImageMeta& im);

#endif
