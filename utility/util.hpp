#ifndef TRACK_UTIL_HPP
#define TRACK_UTIL_HPP

#include <string>
#include <vector>
#include <functional>
#include <opencv2/opencv.hpp>

bool fileHasType(std::string file, std::string  extension);

cv::Rect pos2rect(int l, int t, int r, int b);

Mat gray2binary(const Mat& gray){

template <typename T>
double mean(std::vector<T> v, std::function<int(T)> f);
template <typename T>
double mean(std::vector<T> v, std::function<double(T)> f);

template <typename T>
double variance(std::vector<T> v, std::function<int(T)> f);
template <typename T>
double variance(std::vector<T> v, std::function<double(T)> f);
int pix2a4dpi(int xPixels, int yPixels);

#endif
