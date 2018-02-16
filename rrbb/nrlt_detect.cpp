#include <vector>
#include <list>
#include <algorithm>
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

list<Rect> findKernels(Mat& localText, RT& tree, vector<TextLine>& tls){
  list<Rect> regions = homogenous_recursive(localText);
  
  Mat mask(localText.size(), CV_8UC1, Scalar(0));
  for (Rect& r : regions){
    rectangle(mask, r, Scalar(255));
  }
  Mat overlay = mask + localText;
  list<Rect> kernels;
  for (Rect region : regions){
    vector<int> hits = search_tree(tree, region);
    vector<TextLine> localLines;
    Rect kernel;
    for (int i = 0; i < hits.size(); i++){
      localLines.push_back(tls[hits[i]]);
      kernel |= tls[hits[i]].getBox();
    }
    Mat localMat = localText(kernel);
    if (hits.size() > 1 && verticalArrangement(localMat, localLines)){
      
      kernels.push_back(kernel);
    }
  }
  return kernels;
}

bool criteria9(int nhtl, int nvtl, int wc, int w){
  return (nhtl >= 1 && nvtl == 1 && wc <= 2*w/3) ||
    (nhtl >= 1 && nvtl <= 2 && abs(wc-w) <= 0.05*w);
}



Rect process(vector<int> index, vector<Rect> bb, Rect kernel){
  int nhtl = 0, nvtl = 0;
  Rect last = kernel;
  for (Rect& r : bb){
    if (max(r.y, last.y) - min(r.br().y, last.br().y) < 0) nhtl++;
    else nvtl++;
    if (!criteria9(nhtl, nvtl, r.width, kernel.width))
      break;
    last = r;
  }
  return last;
}

Rect expandKernel(Rect& kernel, RT& tree, vector<Rect>& tbl){
  const int inf = 1000000;
  Rect boxAbove(kernel.x, 0, kernel.width, kernel.y-1);
  Rect boxBelow(kernel.x, kernel.br().y+1, kernel.width, inf);
  vector<int> above = search_tree(tree, boxAbove);
  vector<int> below = search_tree(tree, boxBelow);
  sort(above.begin(), above.end(), [&tbl](int a, int b){return tbl[a].br().y > tbl[b].br().y;});  
  sort(below.begin(), below.end(), [&tbl](int a, int b){return tbl[a].y < tbl[b].y;});
  Rect a = process(above, tbl, kernel);
  Rect b = process(below, tbl, kernel);
  return a|b;  
}

list<Rect> findNRLT(Mat& text, list<Rect> tables){
  Mat localText = text.clone();
  for (Rect& table : tables){
    Mat roi = localText(table);
    roi.setTo(0);
  }
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
  list<Rect> kernels = findKernels(text, tree, tls);
  list<Rect> nrlTables;
  for (Rect& kernel : kernels){

    Mat debugKernel = text(kernel);
    
    Rect expanded = expandKernel(kernel, tree, tbl);
    Mat region = localText(expanded);
    vector<int> inside = search_tree(tree, expanded);
    vector<TextLine> ltl;
    for (int v : inside){
      ltl.push_back(tls[v]);
    }
    if (verticalArrangement(region, ltl))
      nrlTables.push_back(expanded);
  }
  return nrlTables;
}
