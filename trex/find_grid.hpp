#ifndef FIND_GRID_HPP
#define FIND_GRID_HPP

#include <vector>
#include <string>
#include <utility>
#include <opencv2/opencv.hpp>
#include <iostream>

enum CellType{TITLE, DATA};

struct Cell{
  CellType type;
  std::pair<int,int> xSpan, ySpan;
  cv::Rect rect;
  Cell(CellType type, std::pair<int, int> xSpan, std::pair<int, int> ySpan, cv::Rect rect): type(type), xSpan(xSpan), ySpan(ySpan), rect(rect){};
  friend std::ostream& operator<<(std::ostream& os, const Cell& c);
};

std::vector<Cell> find_grid(std::vector<cv::Rect>& bb);
  
#endif
