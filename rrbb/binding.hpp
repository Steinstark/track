#ifndef TRACK_BINDING_HPP
#define TRACK_BINDING_HPP

#include <opencv2/opencv.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/box.hpp>

BOOST_GEOMETRY_REGISTER_POINT_2D(cv::Point, int, cs::cartesian, x, y)
BOOST_GEOMETRY_REGISTER_BOX(cv::Rect, cv::Point, tl(), br())

#endif
