#include "segment.hpp"
#include <algorithm>
#include <iostream>
#include <csdtdlib>
#include <opencv2/opencv.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include "homogenous_regions.hpp"
#include "utility.hpp"
#include "RTree.h"
#include "tree_helper.hpp"

using namespace std;
using namespace cv;
using namespace boost::multi_index;

using RT = RTree<int, int, 2, float>;
using CompDB = multi_index_container<
  ComponentStats,
  indexed_by<
    ordered_non_unique<member<ComponentStats, int, &ComponentStats::area>, greater<int> >,
    ordered_non_unique<member<ComponentStats, double, &ComponentStats::density>, greater<double> >,
    ordered_non_unique<member<ComponentStats, double, &ComponentStats::hwratio>, greater<double> >,
    ordered_non_unique<member<ComponentStats, double, &ComponentStats::density>, less<double> >
    >
  >;

bool isHorizontalLine(ComponentStats& cs, int imageWidth){
  return cs.hwratio > 0.01 &&
    cs.r.width > imageWidth/8;    
}

bool regionIsRectangle(ComponentStats& cs){
  return (cs.bb_area - cs.area)/(double)cs.bb_area) > 0.9
}

bool isColorBlock(ComponentStats& cs, Rt& nt_tree){
  Rect& r = cs.r;
  Point tl(r.x-r.width, r.y-r.height);
  Rect vicinity(tl, r.width*3, r.height*3);
  return tree_search(nt_tree, vicinity) > 3 && cs.area/cs.bb_area >= 0.9;
}

bool containsManyTextElements(ComponentStats& cs, RT& t_tree){
  return search_tree(tree, cs.r) >= 10;
}

vector<int> rlt_candidates(CompDB& db, Mat& cc, RT& t_tree){
  vector<int> candidates;
  for (auto& cs : db){
    if (isHorizontalLine(cs, cc.cols) ||
	regionIsRectangle(cs) ||
	isColorBlock(cs) ||
	containtManyTextElements(cs, t_tree)){
      candidates.push_back(cs.index);
    }    
  }
  return candidates;
}

vector<Rect> textExtraction(vector<Rect>& bb, vector<int>& candidates){
  sort(candidates.begin(), candidates.end(), [&bb](const int a, const int b){return bb[a].y < bb[b].y;});
  Rect prev = bb[candidates[0]];
  vector<TextLine> lines;
  lines.push_back(TextLine());
  lines[0].elements.push_back(prev);
  int index = 0;
  for (int i = 1; i < candidates.size(); i++){
    Rect current = bb[candidates[i]];
    if (max(prev.y, current.y) - min(prev.br().y, current.br().y) < 0 &&
	min(abs(prev.br().x-current.x), abs(prev.x-current.br.x)) <= max(prev.height, current.height) &&
	max(prev.height, current.height) <= min(prev.height, current.height)){
    }else{
      lines.push_back(TextLine());
      index++;
    }
    lines[index].elements.push_back(current);
    prev = current;
  }
  return lines;
}

bool detect_color(){
  return true;
}

bool detect_rlt(Mat& text,ComponentStats& cs, RT& t_tree, RT& nt_tree,vector<TextLine>& lines){  
  return (hasLowDensity(cs) &&
	  regionIsRectangle() &&
	  manySmallRects(text(cs.r), cs) &&
	  noCut(t_tree, cs.r) &&
	  onlyText(nt_tree, cs.r) &&
	  verticalArrangement(text(cs.r), lines));  
}

double overlap(Rect& a, Rect& b){
  return (a.width - (b.x-a.x))/(double)b.width
}

vector<int> overlappingParalellLines(vector<Rect>& lines){
  sort(lines.begin(), lined.end(), [](Rect l, Rect r){return l.x < r.x;});
  vector<int> overlapIndex;
  for (int i = 0; i < lines.size();){
    int j = 1;
    while(i+j < lines.size() && overlap(lines[i], lines[i+j]) > 0.95) j++;
    overlapIndex.push_back(i+j-1);
    i += j;
  }
  return overlapIndex;
}

bool separationCriteria(){
  int nh, nv, mintw, w, yl, yr;
  double meanH;
  bool elementApproxEqual = true;
  bool criteria1 = nh >= 1 && nv == 1 && mintw >= 2*w/3;
  bool criteria2 = nh >= 1 && nv <= 2 && elementApproxEqual;
  bool criteria3 = nh == 0 && abs(yl-yr) > meanH;
  return criteria1 || criteria2 || criteria3;  
}

bool expandRegion(Rect r){
  
  return 0;
}

Rect kernelExpand(vector<Rect> lines, vector<Rect> textLines, l , r){
  int width = lines[l].width;
  vector<Rect> inside = textInside(lines[l].x, lines[l].br().x, textLines);
  //sort by distance from the textline
  int index = 0;
  Rect region
  while (expandRegion(inside[index])){
    region |= inside[index];
    index++;
  }
  verifyRegion(region);
  return region;
}

void kernelMultipleExpand(){
  
}

//many placeholders
vector<Rect> paralell_table(vector<Rect> lines){
  vector<int> opl= overlappingParalellLines(lines);
  vector<Rect> tables;
  int l = 0, r = 0;
  for (int i = 0; i < opl.size(); i++){
    r = opl[i];
    if (r-l == 0){
      kernelExpand(lines, l, r);      
    }
    else{
      kernelMultipleExpand(lines, l, r);
    }
    verify_kernel(kernel); //Should maybe move this one
    l = r+1;
  }
  return tables;
}

void region_refinement(){  
}

Mat nrlt_kernels(){
}

void nrlt_expansion(Mat& text){
  
}
