#include "nrlt_detect.hpp"
#include <list>
#include <set>
#include <opencv2/opencv.hpp>
#include <boost/geometry.hpp>
#include "homogenous_regions.hpp"
#include "image_primitives.hpp"
#include "text_tools.hpp"

using namespace std;
using namespace cv;
using namespace tree;
namespace bgi = boost::geometry::index;

list<Rect> expand(Mat& text){
  list<Rect> kernels = homogenous_recursive(text);
  int prevCols = 0;
  list<Rect> expanded;
  expanded.push_back(Rect());
  for (Rect& kernel : kernels){
    Mat local = text(kernel);
    list<TextLine> tls = findLines(local);
    map<int, int> columnToCount;
    int cols, num = 0;
    for (TextLine& tl : tls){
      int val = ++columnToCount[tl.elements.size()];
      if (val > num){
	num = val;
	cols = tl.elements.size();
      }
    }
    Rect r;
    for (TextLine& tl : tls){
      if (tl.elements.size() == cols){
        r |= tl.getBox() + kernel.tl();
      }
    }    
    if (cols == prevCols){
      expanded.back() |= r;
    }else{
      expanded.push_back(r);
    }
      prevCols = cols;
  }
  return expanded;
}

struct IntRect{
  int cols;
  Rect r;
  IntRect(int cols, Rect& r): cols(cols), r(r) {}
  bool operator < (const IntRect& other) const {
    if (cols == other.cols)
      return r.y < other.r.y;
    return cols > other.cols;
  }    
};

list<Rect> expand2(Mat& text){
  list<Rect> expanded;
  list<TextLine> tls = findLines(text);
  if (tls.empty())
    return expanded;
  int prev = -1, lim = 0;
  list<IntRect> boxes;
  for (TextLine& tl : tls){
    Rect r = tl.getBox();
    lim += r.height;
    if (prev == tl.elements.size()){
      boxes.back().r |= r;
    }else {
      prev = tl.elements.size();
      boxes.push_back(IntRect(prev, r));
    }
  }
  lim = 5*lim/tls.size();
  boxes.sort();
  prev = -1;
  for (IntRect& b : boxes){
    if (b.cols < 3)
      continue;
    if (prev == b.cols && b.r.y - expanded.back().br().y < lim){
      expanded.back() |= b.r;      
    }else {
      prev = b.cols;
      expanded.push_back(b.r);
    }    
  }
  expanded.sort([](const Rect& a, const Rect& b){return a.area() > b.area();});
  bgi::rtree<Rect, bgi::quadratic<16> > tableTree;
  list<Rect> ret;
  for (Rect& r : expanded){
    if (tableTree.qbegin(bgi::intersects(r)) == tableTree.qend()){
      tableTree.insert(r);
      ret.push_back(r);
    }
  }
  
  return ret;
}
  

list<Rect> findNRLT(Mat& text, Mat& nontext){  
  return expand2(text);
}
