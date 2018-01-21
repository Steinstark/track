#ifndef TRACK_UTIL_HPP
#define TRACK_UTIL_HPP

#include <string>
#include <vector>
#include <functional>
#include <type_traits>

#include <opencv2/opencv.hpp>

bool fileHasType(std::string file, std::string  extension);

cv::Rect pos2rect(int l, int t, int r, int b);

cv::Mat gray2binary(const cv::Mat& gray);

template <
  typename V,
  typename T,
  typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
  >
double mean(std::vector<V> v, std::function<T(V)> f);

template <
  typename V,
  typename T,
  typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
  >
double variance(std::vector<V> v, std::function<T(V)> f);

int pix2a4dpi(int xPixels, int yPixels);

#endif
