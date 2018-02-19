#ifndef TRACK_RLTDETECT_HPP
#define TRACK_RLTDETECT_HPP

#include <list>
#include <functional>
#include <opencv2/opencv.hpp>
#include "utility.hpp"


std::list<cv::Rect> findRLT(ImageDataBox& imd, ImageMeta& im, std::function<bool(cv::Mat&, ComponentStats&)> f);

#endif
