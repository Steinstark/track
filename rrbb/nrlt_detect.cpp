#include <vector>
#include <opencv2/opencv.hpp>
#include "homogenous_regions.hpp"
#include "utility.hpp"
#include "image_primitives.hpp"
#include "image_util.hpp"
#include "text_tools.hpp"
#include "nrlt_detect.hpp"
#include "tree_helper.hpp"

using namespace std;
using namespace cv;
using namespace tree;

vector<Rect> findKernels(Mat& localText){
  vector<Rect> bb = boundingVector(localText);  
  vector<TextLine> tls = findLines(bb);
  vector<Rect> tbl;
  RT tree;
  for (int i = 0; i < tls.size(); i++){
    Rect r = tls[i].getBox();
    insert2tree(tree, r, i);
    tbl.push_back(r);
    rectangle(localText, r, Scalar(255), CV_FILLED);
  }
  vector<Rect> regions = homogenous_regions(localText);
  vector<Rect> kernels;
  for (int i = 0; i < regions.size(); i++){
    vector<int> hits = search_tree(tree, regions[i]);
    vector<TextLine> localLines;
    for (int i = 0; i < hits.size(); i++){
      localLines.push_back(tls[hits[i]]);
    }
    Mat localMat = localText(regions[i]);
    if (hits.size() > 1 && verticalArrangement(localMat, localLines)){
      kernels.push_back(regions[i]);
    }
  }
  return kernels;
}

vector<Rect> expandKernels(){
  return vector<Rect>();
}

vector<Rect> findNRLT(Mat& text, vector<Rect> tables){
  Mat localText = text.clone();
  for (int i = 0; i < tables.size(); i++){
    Mat roi = localText(tables[i]);
    roi.setTo(0);
  }
  return findKernels(localText);
}
