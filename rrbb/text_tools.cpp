#include "text_tools.hpp"
#include <set>
#include <algorithm>
#include <functional>
#include "util.hpp"
#include "tree_helper.hpp"

using namespace std;
using namespace cv;
using namespace tree;

Rect TextLine::getBox() const{
  if (elements.empty())
    return Rect();
  Rect r = elements[0];
  for (int i = 1; i < elements.size(); i++){
      r |= elements[i];
  }
  return r;
}

double TextLine::getSpace(){
  std::sort(elements.begin(), elements.end(), [](const Rect& a, const Rect& b){return a.x < b.x;});
  int val = 0;
  for (int i = 1; i < elements.size(); i++){
      val += elements[i].x-elements[i-1].br().x;
  }
  return (double)val/(elements.size());
}

double TextLine::getMeanLength(){
  return mean<cv::Rect, int>(elements, [](cv::Rect r){return r.width;});
}

set<int> indexOnLine(Rect r, vector<Rect>& rects, vector<int>& interesting){
  set<int> lines;
  for (int e : interesting){
    Rect c = rects[e];
    if (max(r.y, c.y) - min(r.br().y, c.br().y) < 0 &&
	c.x - r.br().x <= max(r.height, c.height) &&
	max(r.height, c.height) <= 2*min(r.height, c.height)){
      lines.insert(e);
    }
  }  
  return lines;
}
vector<TextLine> findLines(vector<Rect>& rects){
  std::sort(rects.begin(), rects.end(), [](const Rect& a, const Rect& b){return a.x < b.x;});
  RT tree;
  set<int> toVisit;
  for (int i = 0; i < rects.size(); i++){
    insert2tree(tree, rects[i], i);
    toVisit.insert(i);
  }
  vector<TextLine> textVector;
  while (toVisit.size()){
    set<int> line;
    int cl = *toVisit.begin();
    while(true){
      Rect r = rects[cl];
      Rect possible(r.x, r.y, r.width + r.height/2, r.height);
      vector<int> interesting = search_tree(tree, possible);
      set<int> overlapping = indexOnLine(r, rects, interesting);
      for (int o : overlapping){
	line.insert(o);
	toVisit.erase(o);
      }
      int nf = *line.rbegin();
      if (cl == nf)
	break;
      cl = nf;
    }
    TextLine tl;
    for (int e : line){
      tl.elements.push_back(rects[e]);
    }
    textVector.push_back(tl);
  }
  return textVector;
}


vector<TextLine> linesInRegion(ImageMeta& im, vector<ComponentStats> textData, Rect region){
  vector<int> inside = search_tree(im.t_tree, region);
  vector<Rect> rects;
  for (int i = 0; i < inside.size(); i++){
    rects.push_back(textData[inside[i]].r-region.tl());
  }
  return findLines(rects);    
}
