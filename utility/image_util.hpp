#ifndef IMAGE_UTIL_HPP
#define IMAGE_UTIL_HPP

#include <string>
#include <opencv2/opencv.hpp>

void move2(cv::Mat& from, cv::Mat& to, const cv::Mat& cc, int i);
void remove_lines(cv::Mat& from, cv::Mat& to);

void lineSep(const cv::Mat& img, cv::Mat& line, cv::Mat& element);
cv::Mat lineMask(cv::Mat& img);
  

double counterRotAngle(cv::Mat& img);
double counterRotAngle(std::vector<cv::Point>& contour);

void rotate(cv::Mat& src, cv::Mat& dst, double angle);

cv::Rect pos2rect(int l, int t, int r, int b);

cv::Mat color2binary(const cv::Mat& img);

cv::Mat gray2binary(const cv::Mat& gray);

cv::Rect pos2rect(int l, int t, int r, int b);

int pix2a4dpi(int xPixels, int yPixels);

cv::Mat gray2binary(const cv::Mat& gray);
#endif
