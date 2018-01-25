#include "image_primitives.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <functional>
#include <type_traits>
#include "util.hpp"
#include "tree_helper.hpp"

using namespace std;
using namespace cv;

bool isHorizontalLine(ImageMeta& im, const ComponentStats& cs){
  return cs.hwratio > 0.01 &&
    cs.r.width > im.width/8;    
}

bool regionIsRectangle(const ComponentStats& cs){
  return (cs.bb_area - cs.area)/(double)cs.bb_area > 0.9;
}

bool isColorBlock(ImageMeta& im, const ComponentStats& cs){
  Rect r = cs.r;
  Point tl(r.x-r.width, r.y-r.height);
  Rect vicinity(tl.x, tl.y, r.width*3, r.height*3);
  return search_tree(im.nt_tree, vicinity) > 3 && cs.area/cs.bb_area >= 0.9;
}

bool containsManyTextElements(ImageMeta& im, const ComponentStats& cs){
  return search_tree(im.t_tree, cs.r) >= 10;
}


//TODO
//all primitives below
bool manySmallRect(Mat& text, ComponentStats& cs){  
  Mat cc, stats, centroids;
  int labels = connectedComponentsWithStats(text, cc, stats, centroids, 8, CV_32S);
  int area = 0;
  for (int i = 1; i < labels; i++){
    ComponentStats csl = stats2component(stats, i);
    if (regionIsRectangle(cs))
      area += csl.bb_area;
    else
      return false;
  }
  return (area+cs.area)/cs.bb_area >= 0.9;
}

bool verticalArrangement(Mat& textTable, vector<TextLine>& lines){
  Mat hist;
  vector<Line> text, space;
  reduce(textTable, hist, 0, CV_REDUCE_SUM, CV_64F);
  find_lines(hist, text, space);
  double ms = mean<Line, int>(space, [](Line l){return l.length();});
  return true;
  /*
  double mes = mean<TextLine, double>(lines, [](TextLine tl){return  tl.getSpace();});
  double met = mean<TextLine, double>(lines, [](TextLine tl){return tl.getMeanLength();});
  vector<Rect> bb(lines.size());
  for (int i = 0; i < lines.size(); i++){
    bb[i] = lines[i].getBox();
  }
  double lv = variance<Rect, int>(bb, [](Rect r){return r.x;});
  double cv = variance<Rect, double>(bb, [](Rect r){return (r.x+r.br().x)*0.5;});
  double rv = variance<Rect, int>(bb, [](Rect r){return r.br().x;});
  return (lv < met &&
	  cv < met &&
	  rv < met &&
	  ms > mes);*/
}

bool hasLowDensity(ComponentStats& cs){
  return (double)cs.area/cs.bb_area;
}


bool noCut(ImageMeta& im, Rect r){
  Rect left(r.x-1, r.y, 1, r.height);
  Rect right(r.x+r.width, r.y, 1, r.height);
  Rect top(r.x, r.y-1, r.width, 1);
  Rect bottom(r.x, r.y+r.height, r.width, 1);
  return !search_tree(im.t_tree, left) &&
    !search_tree(im.t_tree, top) &&
    !search_tree(im.t_tree, right) &&
    !search_tree(im.t_tree, bottom);    
}

bool onlyText(ImageMeta& im, Rect r){
  return search_tree(im.nt_tree, r) < 2;
}
