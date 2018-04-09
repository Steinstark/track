#include "image_primitives.hpp"
#include "image_util.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <functional>
#include <type_traits>
#include <set>
#include "util.hpp"
#include "utility.hpp"
#include "tree_helper.hpp"

using namespace std;
using namespace cv;
using namespace tree;

bool isLine(const ComponentStats& cs){
  return cs.hwratio < 0.10;  
}

vector<TextLine> partitionBlocks(list<TextLine>& lineBoxes, vector<Line>& space){
  int inf = 1000000;
  map<int, int> table;
  for (int i = 0; i < space.size(); i++){
    table[space[i].l] = i;    
  }
  table[inf] = space.size();
  vector<TextLine> partitions(space.size()+1);
  for (TextLine& line : lineBoxes){
    for (Rect& e : line.elements){
      auto it = table.upper_bound(e.x-1);
      if (it != table.end()){
	partitions[it->second].addSegment(e);
      }else
	cout << "Left index is larger than " << inf << endl;
    }
  }
  return partitions;
}

list<int> getParts(int length, int n){
  list<int> parts;
  if (!length || !n)
    return parts;
  int rem = length % n;
  int part = length / n;
  int current = part;
  for (int i = 0; i < n; i++){
    if (rem){
      current++;
      rem--;
      parts.push_back(current);
    }else{
      parts.push_back(current);
    }
    current += part;
  }
  return parts;
}

bool verticalArrangement(Mat& text){
  int score = 0;
  size_t partitions = 3;
  Mat hist;
  reduce(text, hist, 1, CV_REDUCE_SUM, CV_64F);
  vector<Line> textLine, spaceLine;
  find_lines(hist, textLine, spaceLine);
  list<int> parts = getParts(spaceLine.size(), min(spaceLine.size(), partitions));  
  int current = 0;
  for (int part : parts){
    Rect r(0, current,text.cols, spaceLine[part-1].l-current);
    current = spaceLine[part-1].r;
    Mat localText = text(r).clone();
    list<TextLine> tls = findLines(localText);
    for (TextLine tl : tls){
      for (Rect& seg : tl.elements){
	rectangle(localText, seg, Scalar(255), CV_FILLED);
      }
    }
    score += verticalArrangement(localText, tls);
  }
  return score > partitions/2;
}

bool verticalArrangement(Mat& textTable, list<TextLine>& lines){
  Mat hist;
  vector<Line> text, space;
  reduce(textTable, hist, 0, CV_REDUCE_SUM, CV_64F);
  find_lines(hist, text, space);
  vector<TextLine> partitions = partitionBlocks(lines, space);
  if (partitions.size() < 2)
    return false;
  for (int i = 0; i < partitions.size(); i++){
    double med = binapprox<Rect, int>(partitions[i].elements, [](Rect r){return  r.width;});
    double lv = welford<Rect, int>(partitions[i].elements, [](Rect r){return r.x;});
    double cv = welford<Rect, double>(partitions[i].elements, [](Rect r){return (r.x+r.br().x)*0.5;});
    double rv = welford<Rect, int>(partitions[i].elements, [](Rect r){return r.br().x;});    
    if (lv > med && cv > med && rv > med){
      return false;
    }
  }
  return true;
}

bool mostlyText(vector<ComponentStats>& stats){
  if (stats.size() < 2)
    return true;
  int area = 0;
  Rect r = stats[0].r;
  for (auto stat : stats){
    r |= stat.r;
    if (!isLine(stat))
      area += stat.area;
  }
  return 0.01*r.area() > area;
}

bool manyRows(Mat& img){
  Mat hist;
  reduce(img, hist, 1, CV_REDUCE_SUM, CV_64F);
  transpose(hist, hist);
  vector<Line> text, space;
  find_lines(hist, text, space);
  return text.size() > 1;
}

bool hasLargeGraphElement(Rect r, vector<ComponentStats> statsNontext){
  for (ComponentStats& cs : statsNontext){
    if (cs.bb_area  >= 0.1*r.area())
      return true;
  }
  return false;
}

bool hasLargeGraphElement(const Mat nontext){
  vector<ComponentStats> stats = statistics(nontext);
  return hasLargeGraphElement(Rect(0, 0, nontext.cols, nontext.rows), stats);
}

bool verifyReg(Mat& text, Mat& nontext, int count){
  vector<ComponentStats> statsText = statistics(text);
  RT tree;
  Rect r(0,0, text.cols, text.rows);
  for (int i = 0; i < statsText.size(); i++)
    insert2tree(tree, statsText[i].r, i); 
  Mat mask, withoutLines;
  mask = lineMask(nontext);
  bitwise_not(mask, withoutLines, nontext);
  vector<ComponentStats> statsNontext = statistics(withoutLines);
  vector<ComponentStats> statsComplete = statistics(nontext);
  bool notInside = count == statsComplete.size();
  bool isTable=  verticalArrangement(text) &&
    mostlyText(statsNontext) &&
    manyRows(text) &&
    !hasLargeGraphElement(r, statsNontext) &&
    notInside;
  return isTable;
}
