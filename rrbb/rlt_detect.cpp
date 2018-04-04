#include "rlt_detect.hpp"
#include <list>
#include <opencv2/opencv.hpp>
#include "utility.hpp"
#include "image_util.hpp"
#include "tree_helper.hpp"

using namespace std;
using namespace cv;
using namespace tree;

list<Rect> findRLT(Mat& text, Mat& nontext){
  int vl = nontext.rows/10, hl = nontext.cols/7;
  Mat elementV = getStructuringElement(MORPH_RECT, Size(1, vl), Point(-1, -1));
  Mat elementH = getStructuringElement(MORPH_RECT, Size(hl, 1), Point(-1, -1));
  Mat vertical, horizontal;
  lineSep(nontext, vertical, elementV);
  lineSep(nontext, horizontal, elementH);
  Mat intersect = vertical & horizontal;
  list<Rect> ib, nb;
  boundingVector(intersect, back_inserter(ib));
  boundingVector(nontext, back_inserter(nb));
  RTBox tree;
  for (Rect& r : ib){
    insert2tree(tree, r);
  }
  list<Rect> tables;
  for (Rect& r : nb){
    if (search_tree(tree, r).size() >= 10){
      tables.push_back(r);
    }      
  }
  return tables;
}
