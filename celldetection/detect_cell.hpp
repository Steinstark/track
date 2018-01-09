#ifndef TRACK_DETECT_CELL
#define TRACK_DETECT_CELL

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

std::vector<cv::Rect> detect_cells(const cv::Mat& bw, cv::Rect r);
#endif
