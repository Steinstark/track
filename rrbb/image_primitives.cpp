#include "image_primitives.hpp"
#include "image_util.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <functional>
#include <type_traits>
#include <set>
#include <map>
#include <boost/geometry.hpp>
#include "util.hpp"
#include "utility.hpp"

using namespace std;
using namespace cv;
using namespace tree;
namespace bgi = boost::geometry::index;

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

list<Line> getParts(int rows, int partitions, vector<Line>& text){  
  int start = 0, goal = rows/partitions, pr = 0;
  int prevDist = goal - start;
  list<Line> bound;
  for (Line& l : text){
    int dist = abs(goal - l.r+1);
    if (dist > prevDist){
      bound.push_back(Line(start, pr));
      start = pr;
      goal = start + (rows - start)/--partitions;
      prevDist = goal - l.r;
    }else {
      prevDist = dist;
      pr = l.r+1;
    }
  }
  bound.push_back(Line(start, goal));
  return bound;
}

bool verticalArrangement(Mat& text){
  int score = 0, rows = 0;
  Mat yHist, tmp;
  int limY = text.cols*255*0.1;
  reduce(text, yHist, 1, CV_REDUCE_SUM, CV_64F);  
  for (int i = 0; i < yHist.rows; i++){
    if (yHist.at<double>(i) < limY){
      text.row(i).setTo(0);
    }
  }  
  vector<Line> textLine, spaceLine;
  find_lines(yHist, textLine, spaceLine, limY);
  int partitions = min(3, (int)textLine.size());
  if (partitions <= 1){
    return 0;
  }
  list<Line> parts = getParts(text.rows, partitions, textLine);
  map<int, int> freqTable;
  for (Line& part : parts){
    Rect r(0, part.l ,text.cols, part.length());
    Mat local = text(r), xHist;
    reduce(local, xHist, 0, CV_REDUCE_SUM, CV_64F);
    int limX = 255*local.rows*0.1;
    for (int i = 0; i < xHist.cols; i++){
      if (xHist.at<double>(i) < limX){
	local.col(i).setTo(0);
      }
    }
    list<TextLine> tls = findLines(local, false);
    for (TextLine& tl : tls){
      freqTable[tl.elements.size()]++;
    }
    rows += tls.size();
    score += verticalArrangement(local, tls);
  }  
  return (score >= partitions/2 && freqTable.size() <= 4);
}

bool verticalArrangement(Mat& textTable, list<TextLine>& lines){
  Mat hist;
  vector<Line> text, space;
  reduce(textTable, hist, 0, CV_REDUCE_SUM, CV_64F);
  find_lines(hist, text, space);
  vector<TextLine> partitions = partitionBlocks(lines, space);
  double med = 3;
  for (int i = 0; i < partitions.size(); i++){
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

bool isTableLike(const Mat& inverted){
  Mat hist;
  reduce(inverted, hist, 0, CV_REDUCE_SUM, CV_64F);
  vector<Line> text, space;
  find_lines(hist, text, space);
  if (text.size() < 2 && (double)inverted.cols / inverted.rows < 0.3)
      return false;
  return true;
}

bool hasOnewayLines(const Mat horizontal, const Mat vertical){
  vector<ComponentStats> hStats = statistics(horizontal);
  vector<ComponentStats> vStats = statistics(vertical);
  bgi::rtree<ComponentStats, bgi::quadratic<16> > hTree(hStats);
  int countV = 0;
  for (ComponentStats& cs : vStats){
    if (distance(hTree.qbegin(bgi::intersects(cs)), hTree.qend()) < 2){
      countV++;
    }
  }
  bgi::rtree<ComponentStats, bgi::quadratic<16> > vTree(vStats);
  int lim = horizontal.cols * 0.1;
  int countH = 0;
  for (ComponentStats& cs : hStats){
    if (cs.r.width > lim && distance(vTree.qbegin(bgi::intersects(cs)), vTree.qend()) < 2){
      countH++;
    }        
  }
  if (countV > vStats.size()*0.2 || countH > hStats.size()*0.2)
    return true;
  return false;
}

bool lowCrapRatio(const Mat text, const Mat nontext){
  vector<ComponentStats> textStats = statistics(text);
  vector<ComponentStats> nontextStats = statistics(nontext);
  return textStats.size() > nontextStats.size();
}

bool hasLargeGraphElement(Rect r, vector<ComponentStats>& statsNontext){
  double lim = 0.05*r.area();
  int areaSum = 0;
  for (ComponentStats& cs : statsNontext){
    if (cs.hwratio < 0.06)
      areaSum += 0.025*r.area();
    areaSum += cs.bb_area;
    if (areaSum > lim)
      return true;
  }
  return false;
}

bool hasLargeGraphElement(const Mat nontext){
  vector<ComponentStats> stats = statistics(nontext);
  return hasLargeGraphElement(Rect(0, 0, nontext.cols, nontext.rows), stats);
}

bool verifyReg(Mat& text, Mat& nontext){
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
  bool isTable=  verticalArrangement(text) &&
    mostlyText(statsNontext) &&
    manyRows(text) &&
    !hasLargeGraphElement(r, statsNontext);
  return isTable;
}
