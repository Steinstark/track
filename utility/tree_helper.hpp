#ifndef TRACK_TREE_HELPER_HPP
#define TRACK_TREE_HELPER_HPP

#include <utility>
#include <list>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <opencv2/opencv.hpp>

namespace tree{
  namespace internal{
    using point = boost::geometry::model::point<int, 2, boost::geometry::cs::cartesian>;
    using box = boost::geometry::model::box<point>;
    using value = std::pair<box, int>;
    namespace bgi = boost::geometry::index;
  }
  using RT = boost::geometry::index::rtree<internal::value, boost::geometry::index::quadratic<16> >;
  using RTBox = boost::geometry::index::rtree<internal::box, boost::geometry::index::quadratic<16> >;
  cv::Rect bb2cvr(internal::box b);
  void insert2tree(RT& tree, const cv::Rect& r, int index);
  void insert2tree(RTBox& tree, const cv::Rect& r);
  std::vector<cv::Rect> closestBox(RT& tree, const cv::Point cvp, const cv::Rect& r);
  bool cut_tree(RT& tree, const cv::Rect& r);
  std::vector<int> search_tree(RT& tree, const cv::Rect& r);
  std::list<cv::Rect> search_tree(RTBox& tree, const cv::Rect& r);
  void remove_tree(RT& tree, const cv::Rect& r, int index);
  void remove_tree(RTBox& tree, const cv::Rect& r);
  RTBox::const_query_iterator tree_begin(RTBox& tree);
}

#endif
