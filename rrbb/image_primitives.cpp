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

bool regionIsRectangle(const ComponentStats& cs){
  return (cs.bb_area - cs.area)/(double)cs.bb_area > 0.85;
}

bool isColorBlock(ImageMeta& im, const ComponentStats& cs){
  Rect r = cs.r;
  Point tl(r.x-r.width, r.y-r.height);
  Rect vicinity(tl.x, tl.y, r.width*3, r.height*3);
  return search_tree(im.nt_tree, vicinity).size() > 3 && cs.area/cs.bb_area >= 0.9;
  }

bool containsManyElements(RT& tree, const ComponentStats& cs){
  return search_tree(tree, cs.r).size() >= 10;
}

bool manySmallRect(Mat& text, ComponentStats& cs){  
  Mat cc, stats, centroids;
  int labels = connectedComponentsWithStats(text, cc, stats, centroids, 8, CV_32S);
  int area = 0;
  for (int i = 1; i < labels; i++){
    ComponentStats csl = stats2component(stats, i);
    if (regionIsRectangle(cs))
      area += csl.bb_area;
    else
      return false;
  }
  return (area+cs.bb_area-cs.area)/(double)cs.bb_area >= 0.9;
}

vector<vector<TextLine> > partitionBlocks(list<TextLine>& lineBoxes, vector<Line>& space){
  int inf = 1000000;
  map<int, int> table;
  for (int i = 0; i < space.size(); i++){
    table[space[i].l] = i;    
  }
  table[inf] = space.size();
  vector<vector<TextLine> > partitions(space.size()+1, vector<TextLine>());
  for (TextLine& line : lineBoxes){
    auto it = table.upper_bound(line.getBox().x-1);
    if (it != table.end()){
      partitions[it->second].push_back(line);
    }else
      cout << "Left index is larger than " << inf << endl;
  }
  return partitions;
}

vector<Rect> verticalMerge(vector<TextLine>& lines){
  vector<Rect> boxes;
  if (lines.empty())
    return boxes;
  for (int i = 0; i < lines.size(); i++){
    boxes.push_back(lines[i].getBox());
  }
  sort(boxes.begin(), boxes.end(), [](const Rect& a, const Rect& b){return a.y < b.y;});
  vector<Rect> merged;
  Rect current = boxes[0];  
  for (int i = 1; i < boxes.size(); i++){
    if (current.y <= boxes[i].y && current.br().y >= boxes[i].y){
      current |= boxes[i];
    }else{
      merged.push_back(current);
      current = boxes[i];
    }
  }
  merged.push_back(current);
  return merged;
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
      rectangle(localText, tl.getBox(), Scalar(255), CV_FILLED);
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
  vector<vector<TextLine> > partitions = partitionBlocks(lines, space);
  if (partitions.size() < 2)
    return false;
  for (int i = 0; i < partitions.size(); i++){
    vector<Rect> merged = verticalMerge(partitions[i]);
    double mes = binapprox<TextLine, double>(partitions[i], [](TextLine tl){return  tl.getMeanLength();});
    double lv = welford<Rect, int>(merged, [](Rect r){return r.x;});
    double cv = welford<Rect, double>(merged, [](Rect r){return (r.x+r.br().x)*0.5;});
    double rv = welford<Rect, int>(merged, [](Rect r){return r.br().x;});    
    if (lv > mes && cv > mes && rv > mes){
      return false;
    }
  }
  return true;
}

bool noCut(RT& tree, Rect& r){
  return cut_tree(tree, r);
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

bool mostlyText(const Mat& nontext){
  vector<ComponentStats> stats = statistics(nontext);
  return mostlyText(stats);
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


bool similarElementHeight(const vector<ComponentStats>& stats){
  double var = welford<ComponentStats, int>(stats, [](const ComponentStats& cs){return cs.r.height;});
  return var < 10;
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
