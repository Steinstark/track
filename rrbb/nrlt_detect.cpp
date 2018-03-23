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

using namespace std;
using namespace cv;
using namespace tree;

//DEBUG
Mat textDebug;
Mat lineDebug;
Mat expandedDebug;

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

list<Rect> findKernels(Mat& localText){
  list<Rect> regions = homogenous_recursive(localText);  

  //DEBUG
  Mat mask(localText.size(), CV_8UC1, Scalar(0));
  for (Rect& r : regions){
    rectangle(mask, r, Scalar(255));
  }
  Mat overlay = mask + localText;
  
  list<Rect> kernels;
  for (Rect region : regions){
    Mat localRegion = localText(region);
    if (verticalArrangement(localRegion)){      
      kernels.push_back(region);
    }
  }
  return kernels;
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
  Mat hist(Size(textImg.cols, 1), CV_64F, Scalar(0));
  int cols = 0, headerCount = 0, dataCount = 0;
  Rect r;
  if (begin == end)
    return r;
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
    list<Line> text, space;
    find_lines(histComplete, text, space);
    int dist = distance(r, line);        
    if (text.size() < cols || text.size() == 1){
      headerCount++;
      if (headerCount - dataCount > 1 || dist > boxHeight){
	break;
      }
    }else if ((cols == cols && dist <= 4*boxHeight) || dist < boxHeight){
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
      if (distance(r1, r2) < 0.8*it->height){
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

list<Rect> findNRLT(Mat& text, Mat& nontext, ImageMeta& im){
  Mat localText = text.clone();
  vector<Rect> bb;
  boundingVector(localText, back_inserter(bb));  
  vector<TextLine> tls = findLines(bb);
  vector<Rect> tbl;
  RT tree;
  for (int i = 0; i < tls.size(); i++){
    Rect r = tls[i].getBox();
    insert2tree(tree, r, i);
    tbl.push_back(r);
    rectangle(localText, r, Scalar(255), CV_FILLED);
  }
  Mat expandedLine = localText.clone();
  expandLine(expandedLine);

  //DEBUG
  expandedDebug = expandedLine;
  textDebug = text;
  lineDebug = localText;
  
  list<Rect> kernels = findKernels(localText);
  list<Rect> expandedKernels = expandKernels(localText, expandedLine, kernels);
  list<Rect> nrlTables;  
  for (Rect& expanded : expandedKernels){    
    Mat regionText = localText(expanded);
    Mat regionNontext = nontext(expanded);
    if (verifyReg(regionText, regionNontext, search_tree(im.nt_tree, expanded).size())){
      nrlTables.push_back(expanded);
    }
  }
  return nrlTables;
}
