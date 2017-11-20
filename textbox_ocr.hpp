#ifndef TRACK_TEXTBOX_OCR
#define TRACK_TEXTBOX_OCR

#include <vector>
#include <opencv2/opencv.hpp>

std::vector<std::string> textbox_content(std::string path, cv::Rect table, std::vector<cv::Rect> rv);

#endif
