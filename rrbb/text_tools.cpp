#include "text_tools.hpp"
#include <set>
#include <algorithm>
#include <functional>
#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>
#include "component_stats.hpp"
#include "util.hpp"
#include "tree_helper.hpp"

using namespace std;
using namespace cv;
using namespace tree;
namespace bgi = boost::geometry::index;

Rect TextLine::getBox() const{
  return box;
}

double TextLine::getSpace(){
  std::sort(elements.begin(), elements.end(), [](const Rect& a, const Rect& b){return a.x < b.x;});
  int val = 0;
  for (int i = 1; i < elements.size(); i++){
      val += elements[i].x-elements[i-1].br().x;
  }
  return (double)val/(elements.size());
}

double TextLine::getMeanLength(){
  return mean<cv::Rect, int>(elements, [](cv::Rect r){return r.width;});
}

void TextLine::addSegment(Rect segment){
  box |= segment;
  elements.push_back(segment);
}

bool isSegment(const Rect& a, const Rect& b){
  if (a != Rect() && b != Rect())
    return min(abs(a.x-b.br().x), abs(a.br().x-b.x)) <= max(a.height, b.height);
  return true;
}

list<TextLine> findLines(Mat& img, bool expand){
  Mat cc, hist;
  list<ComponentStats> stats;
  statistics(img, cc, back_inserter(stats));
  reduce(img, hist, 1, CV_REDUCE_SUM, CV_64F);
  list<Line> text, space;
  find_lines(hist, text, space);
  bgi::rtree<ComponentStats, bgi::quadratic<16> > tree(stats);
  const int inf = 1000000;
  list<TextLine> ret;
  for (Line& line : text){
    Point p(0, (line.l + line.r)/2);
    auto it = tree.qbegin(bgi::intersects(Rect(0, line.l, inf, line.r-line.l)) && bgi::nearest(p, 1000));
    TextLine tl;
    Rect r;
    while (it != tree.qend()){
      if (expand && isSegment(r, it->r)){
	r |= it->r;
      }else{
	tl.addSegment(r);
	r = it->r;
      }	
      it++;
    }
    if (tl.elements.empty() || tl.elements.back() != r)
      tl.addSegment(r);
    ret.push_back(tl);
  }
  return ret;
}
