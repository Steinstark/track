#ifndef IMAGE_UTIL_HPP
#define IMAGE_UTIL_HPP

#include <opencv2/opencv.hpp>

void move2(cv::Mat& from, cv::Mat& to, const cv::Mat& cc, int i);

double counterRotAngle(cv::Mat& img);

void rotate(cv::Mat& src, cv::Mat& dst, double angle);

cv::Rect pos2rect(int l, int t, int r, int b);

cv::Mat gray2binary(const cv::Mat& gray);

cv::Rect pos2rect(int l, int t, int r, int b);

int pix2a4dpi(int xPixels, int yPixels);

cv::Mat gray2binary(const cv::Mat& gray);

#endif
