#include "rlt_detect.hpp"
#include <list>
#include <opencv2/opencv.hpp>
#include "utility.hpp"
#include "image_util.hpp"
#include "tree_helper.hpp"
#include "image_primitives.hpp"

using namespace std;
using namespace cv;
using namespace tree;

list<Rect> findRLT(Mat& text, Mat& nontext){
  int vl = nontext.rows/11, hl = nontext.cols/7;
  Mat elementV = getStructuringElement(MORPH_RECT, Size(1, vl), Point(-1, -1));
  Mat elementH = getStructuringElement(MORPH_RECT, Size(hl, 1), Point(-1, -1));
  Mat vertical, horizontal;
  lineSep(nontext, vertical, elementV);
  lineSep(nontext, horizontal, elementH);
  Mat intersect = vertical & horizontal;
  list<Rect> ib, nb;
  boundingVector(intersect, back_inserter(ib));
  Mat cc;
  vector<ComponentStats> stats = statistics(nontext, cc);
  RTBox tree;
  for (Rect& r : ib){
    insert2tree(tree, r);
  }
  list<Rect> tables;
  for (ComponentStats& cs : stats){
    Rect& r = cs.r;
    Mat textRegion = text(r);
    if (search_tree(tree, r).size() >= 10 && verticalArrangement(textRegion)){
      Mat tmp(Size(r.width, r.height), CV_8UC1, Scalar(0));
      Mat nontextRegion = nontext(r);
      move2(nontextRegion, tmp, cc(r) , cs.index+1);
      if (!hasLargeGraphElement(nontextRegion)){
	tables.push_back(r);
      }
    }
  }
  return tables;
}
