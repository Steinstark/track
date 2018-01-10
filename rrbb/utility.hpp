#ifndef TRACK_UTILITY_HPP
#define TRACK_UTILITY_HPP

#include <opencv2/opencv.hpp>
#include <vector>

void move2(cv::Mat& from, cv::Mat& to, const cv::Mat& cc, int i);

cv::Rect stats2rect(const cv::Mat& stats, int i);

void displayHist(std::string str, const cv::Mat& img);

struct Line{
  int l, r;
  Line(int l, int r): l(l), r(r){};
  int length() const{
    return r-l;
  }
};

struct ComponentStats{
  int index;
  cv::Rect r;
  int area, bb_area;
  double density, hwratio;
  ComponentStats(cv::Rect r, int area, int i);
};

ComponentStats stats2component(const cv::Mat& stats, int i);

void find_lines(const cv::Mat& hist, std::vector<Line>& text, std::vector<Line>& space);
void find_all_lines(const cv::Mat& hist, std::vector<Line>& text, std::vector<Line>& space);

#endif
