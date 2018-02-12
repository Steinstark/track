#ifndef TRACK_TREE_HELPER_HPP
#define TRACK_TREE_HELPER_HPP

#include <utility>
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
  void insert2tree(RT& tree, const cv::Rect& r, int index);
  std::vector<cv::Rect> closestBox(RT& tree, const cv::Point cvp, const cv::Rect& r);
  std::vector<int> search_tree(RT& tree, const cv::Rect& r);
  void remove_tree(RT& tree, const cv::Rect& r, int index);
}

#endif
