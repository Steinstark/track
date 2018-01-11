#ifndef FIND_GRID_HPP
#define FIND_GRID_HPP

#include <vector>
#include <string>
#include <utility>
#include <opencv2/opencv.hpp>


struct Cell{
  std::string type, text;
  std::pair<int,int> xSpan, ySpan;    
};

std::vector<std::pair<int,int>> find_grid(std::vector<cv::Rect>& bb, std::vector<std::string>& text);
  
#endif
