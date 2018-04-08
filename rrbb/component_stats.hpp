#ifndef TRACK_COMPONENT_STATS
#define TRACK_COMPONENT_STATS

#include <opencv2/opencv.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/box.hpp>
#include "binding.hpp"

struct ComponentStats{
  int index;
  cv::Rect r;
  int area, bb_area;
  double density, hwratio;
  ComponentStats(cv::Rect r, int area, int i);
};

BOOST_GEOMETRY_REGISTER_BOX(ComponentStats, cv::Point, r.tl(), r.br())

#endif

