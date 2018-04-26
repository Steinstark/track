#include "nrlt_detect.hpp"
#include <list>
#include <opencv2/opencv.hpp>
#include "homogenous_regions.hpp"
#include "text_tools.hpp"

#include "debug_header.hpp"

using namespace std;
using namespace cv;
using namespace tree;

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
    Rect r;
    for (TextLine& tl : tls){
      int val = ++columnToCount[tl.elements.size()];
      if (val > num){
	num = val;
	cols = tl.elements.size();
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

list<Rect> findNRLT(Mat& text, Mat& nontext){  
  return expand(text);
}
