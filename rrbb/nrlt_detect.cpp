#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <opencv2/opencv.hpp>
#include "homogenous_regions.hpp"
#include "utility.hpp"
#include "image_primitives.hpp"
#include "image_util.hpp"
#include "text_tools.hpp"
#include "nrlt_detect.hpp"
#include "tree_helper.hpp"

#include "debug_header.hpp"

using namespace std;
using namespace cv;
using namespace tree;

void expandLine(Mat& unexpanded){
  vector<ComponentStats> stats = statistics(unexpanded);
  if (stats.empty())
    return;
  sort(stats.begin(),
       stats.end(),
       [](const ComponentStats& a, const ComponentStats& b){return a.r.y < b.r.y;});
  Rect box;
  for (ComponentStats cs : stats){
    if (cs.r.y <= box.br().y){
      box |= cs.r;
    }else{
      unexpanded(box) = Scalar(255);
      box = cs.r;
    } 
  }
  unexpanded(box) = Scalar(255);
}

int distance(const Rect& a, const Rect& b){
  int dist1 = abs(a.y-b.br().y);
  int dist2 = abs(a.br().y-b.y);
  if (dist1 == dist2 || a.width == 0 || b.width == 0)
    return 0;
  return min(dist1, dist2);
}

template <typename Iterator>
Rect expand(const Mat& textImg, Iterator begin, Iterator end){
  if (begin == end)
    return Rect();
  Mat hist(Size(textImg.cols, 1), CV_64F, Scalar(0));
  int cols = 1, headerCount = 0, dataCount = 0;
  Rect r = *begin, headerRect;
  int boxHeight = begin->height;
  while (begin != end){
    Rect line = *begin++;
    
    //DEBUG
    Mat debug1 = textDebug(line);
    Mat debug2 = lineDebug(line);
    Mat debug3 = expandedDebug(line);
    
    Mat histRow, histComplete;
    reduce(textImg(line), histRow, 0, CV_REDUCE_SUM, CV_64F);
    histComplete = hist.clone();
    histComplete(Rect(line.x, 0, line.width, 1)) += histRow;
    list<Line> text, space, textRow, spaceRow;
    find_lines(histRow, textRow, spaceRow);
    find_lines(histComplete, text, space);
    int dist = distance(r, line);        
    if (text.size() <= cols && textRow.size() < 2){
      headerRect = line;
      headerCount++;
      if (headerCount - dataCount > 1 || dist > boxHeight || line.y > r.br().y || line.x < r.x || line.br().x > r.br().x){
	break;
      }
    }else if (cols <= text.size() && dist <= 2*boxHeight){
      dataCount++;
      hist = histComplete;
      cols = text.size();
    }else{
      break;
    }
    r |= line;
    boxHeight = line.height;
  }
  if (dataCount)
    return r;
  return Rect();
}

//TODO
//add header expansion
list<Rect> expandKernels(const Mat& text, const Mat& expandedLine, const list<Rect>& kernels){
  auto belowComp = [](const Rect& a, const Rect& b){return a.y < b.y;};
  set<Rect, decltype(belowComp)> all(belowComp);        
  boundingVector(expandedLine, inserter(all, all.begin()));
  list<Rect> expandedKernels;
  for (Rect kernel : kernels){
    int mid = (kernel.y + kernel.br().y)/2;
    Rect midKernel(0, mid, 1, 1);
    set<Rect>::iterator it = all.lower_bound(midKernel);
    set<Rect>::reverse_iterator itr(it);    
    Rect r1 = expand(text, itr, all.rend());
    Rect r2 = expand(text, it, all.end());
    Rect r = r1 | r2;
    if (r1 != r2){
      if (distance(r1, r2) < 2*it->height){
	expandedKernels.push_back(r);
      }
      else{
	expandedKernels.push_back(r1);
	expandedKernels.push_back(r2);
      }
      all.erase(all.lower_bound(r), all.upper_bound(Rect(0, r.br().y, 1, 1)));
    }
  }
  return expandedKernels;
}

list<Rect> findNRLT(Mat& text, Mat& nontext){  
  list<Rect> kernels = homogenous_recursive(text);
  list<TextLine> tls =  findLines(text);
  Mat lines(text.size(), CV_8UC1, Scalar(0));
  for (TextLine& tl : tls){
    rectangle(lines, tl.getBox(), Scalar(255), CV_FILLED);
  }
  list<Rect> tables;
  for (Rect& kernel : kernels){
    Mat textRegion = text(kernel);
    tables.push_back(kernel);
  }
  /*
  list<Rect> expandedKernels = expandKernels(text, expandedLine, kernels);
  list<Rect> nrlTables;  
  for (Rect& expanded : expandedKernels){    
    Mat regionText = text(expanded);
    Mat regionNontext = nontext(expanded);
    if (verifyReg(regionText, regionNontext)){
      nrlTables.push_back(expanded);
    }
    }*/
    return tables;
}
