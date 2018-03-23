#include "text_tools.hpp"
#include <set>
#include <algorithm>
#include <functional>
#include "util.hpp"
#include "tree_helper.hpp"

using namespace std;
using namespace cv;
using namespace tree;

template <typename InputIterator>
TextLine::TextLine(InputIterator first, InputIterator last, const vector<Rect>& rects){  
  while (first != last){
    int index = *first++;
    box |= rects[index];
    elements.push_back(rects[index]);    
  }
}

Rect TextLine::getBox() const{
  return box;
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
    if (c.x - r.br().x <= max(r.height, c.height)){
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
    Rect r = rects[cl];
    while(true){
      Rect possible(r.x, r.y, r.width + r.height, r.height);
      vector<int> interesting = search_tree(tree, possible);
      set<int> overlapping = indexOnLine(r, rects, interesting);
      for (int o : overlapping){
	line.insert(o);
	toVisit.erase(o);
	r |= rects[o];
      }
      int nf = *line.rbegin();
      if (cl == nf)
	break;
      cl = nf;
    }
    textVector.push_back(TextLine(line.begin(), line.end(), rects));
  }
  return textVector;
}

vector<TextLine> linesInRegion(RT& tree, vector<ComponentStats>& textData, Rect region){
  vector<int> inside = search_tree(tree, region);
  vector<Rect> rects;
  for (int i = 0; i < inside.size(); i++){
    rects.push_back(textData[inside[i]].r-region.tl());
  }
  return findLines(rects);    
}
