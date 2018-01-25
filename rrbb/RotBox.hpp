#ifndef TRACK_ROTBOX_HPP
#define TRACK_ROTBOX_HPP

#include <vector>
#include <cmath>

using namespace std;
using namespace cv;

struct RotBox{
  vector<Point> points;
  double angle;
  RotBox(vector<Point> contour){
    if (contour.empty()){
      return;
    }
    poins(4, contour[0]);
    for (Point p : contour){
      if (point[0].x > p.x)
	point[0] = p;
      if (point[1].y > p.y)
	point[1] = p;
      if (point[2].x < p.x)
	point[2] = p;
      if (point[3].y < p.y)
	point[3] = p;
    }
    
    angle = 3.14; //pi
    vector<Point> a0 = {Point(points[0].x, points[1].y),
			Point(points[2].x, points[1].y),
			Point(points[2].x, points[3].y),
			Point(points[0].x, points[3].y)};
    for (int i = 0; i < 4; i++){
      ind = i;
      if (i == 0)
	ind = i+4;
      //technically not a point, but a vector
      Point u = a0[i] - points[i];      
      Point v = points[ind] - points[i];
      double currentAngle = u.dot(v)/(sqrt(u.dot(u))*sqrt(v.dot(v)));
      angle = min(angle, currentAngle);
    }
  }
};

#endif
