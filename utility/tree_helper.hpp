#ifndef TRACK_TREE_HELPER_HPP
#define TRACK_TREE_HELPER_HPP

#include <vector>
#include <opencv2/opencv.hpp>
#include <functional>
#include "RTree.h"

void insert2tree(RTree<int, int, 2, float>& tree, const cv::Rect& r, int i);

int search_tree(RTree<int, int, 2, float>& tree, cv::Rect r, std::vector<int>& vec);

int search_tree(RTree<int, int, 2, float>& tree, cv::Rect r);

int search_tree(RTree<int, int, 2, float>& tree, cv::Rect r, int& index, bool (*f)(int, void*));

void remove(RTree<int, int, 2, float>& tree, cv::Rect r, int index);

#endif
