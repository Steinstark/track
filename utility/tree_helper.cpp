#include "tree_helper.hpp"
#include <algorithm>

using namespace tree;
using namespace tree::internal;

point cvp2bp(const cv::Point& p){
  return point(p.x, p.y);
}

box cvr2bb(const cv::Rect& r){
  return box(cvp2bp(r.tl()), cvp2bp(r.br()));
}

value cvr2v(const cv::Rect& r, int index){
  return value(cvr2bb(r), index);
}

void tree::insert2tree(RT& tree, const cv::Rect& r, int index){
  tree.insert(cvr2v(r, index));
}

std::vector<int> tree::search_tree(RT& tree, const cv::Rect& r){
  box query_box = cvr2bb(r);
  std::vector<int> index;
  std::vector<value> values;
  tree.query(boost::geometry::index::intersects(query_box), std::back_inserter(values));
  std::transform(values.begin(), values.end(), std::back_inserter(index),[](value v){return v.second;});
  return index;
}

void tree::remove_tree(RT& tree, const cv::Rect& r, int index){
  tree.remove(cvr2v(r, index));
}
