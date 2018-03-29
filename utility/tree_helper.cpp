#include "tree_helper.hpp"
#include <algorithm>
#include <list>
#include <boost/function_output_iterator.hpp>

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

cv::Point bp2cvp(point p){
  return cv::Point(p.get<0>(), p.get<1>());
}

cv::Rect tree::bb2cvr(box b){
  return cv::Rect(bp2cvp(b.min_corner()), bp2cvp(b.max_corner()));
}

void tree::insert2tree(RT& tree, const cv::Rect& r, int index){
  tree.insert(cvr2v(r, index));
}

void tree::insert2tree(RTBox& tree, const cv::Rect& r){
  tree.insert(cvr2bb(r));
}

bool tree::cut_tree(RT& tree, const cv::Rect& r){
  box b = cvr2bb(r);
  std::list<value> boxes;
  tree.query(bgi::intersects(b) &&!bgi::covered_by(b), std::back_inserter(boxes));
  return boxes.empty();
}

std::vector<cv::Rect> tree::closestBox(RT& tree, const cv::Point cvp, const cv::Rect& r){
  point p = cvp2bp(cvp);
  box b = cvr2bb(r);
  std::vector<value> boxes;
  tree.query(bgi::nearest(p, 1) && bgi::intersects(b), std::back_inserter(boxes));
  std::vector<cv::Rect> rects;
  std::transform(boxes.begin(), boxes.end(), std::back_inserter(rects),[](value v){return bb2cvr(v.first);});
  return rects;
}

std::list<cv::Rect> tree::search_tree(RTBox& tree, const cv::Rect& r){
  std::list<cv::Rect> intersects;
  tree.query(bgi::intersects(cvr2bb(r)), boost::make_function_output_iterator([&intersects](box const& b){ intersects.push_back(bb2cvr(b));}));
  return intersects;			     
}

std::vector<int> tree::search_tree(RT& tree, const cv::Rect& r){
  box query_box = cvr2bb(r);
  std::vector<int> index;
  std::vector<value> values;
  tree.query(bgi::intersects(query_box), std::back_inserter(values));
  std::transform(values.begin(), values.end(), std::back_inserter(index),[](value v){return v.second;});
  return index;
}

void tree::remove_tree(RT& tree, const cv::Rect& r, int index){
  tree.remove(cvr2v(r, index));
}

void tree::remove_tree(RTBox& tree, const cv::Rect& r){
  tree.remove(cvr2bb(r));
}

RTBox::const_query_iterator tree::tree_begin(RTBox& tree){
  return tree.qbegin(bgi::satisfies([](box const&){ return true; }));
}

