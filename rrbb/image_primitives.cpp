#include "image_primitives.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <functional>
#include <type_traits>
#include "util.hpp"
#include "tree_helper.hpp"

using namespace std;
using namespace cv;
using namespace tree;

bool isHorizontalLine(const ComponentStats& cs){
  return cs.hwratio > 0.05;  
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

vector<vector<TextLine> > partitionBlocks(vector<TextLine>& tls, vector<Line>& space){
  int inf = 1000000;
  map<int, int> table;
  for (int i = 0; i < space.size(); i++){
    table[space[i].l] = i;    
  }
  table[inf] = space.size();
  vector<vector<TextLine> > partitions(space.size()+1, vector<TextLine>());
  for (int i = 0; i < tls.size(); i++){
    Rect r = tls[i].getBox();
    auto it = table.upper_bound(r.x-1);
    if (it != table.end()){
      partitions[it->second].push_back(tls[i]);
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


bool verticalArrangement(Mat& textTable, vector<TextLine>& lines){
  Mat hist;
  vector<Line> text, space;
  reduce(textTable, hist, 0, CV_REDUCE_SUM, CV_64F);
  find_lines(hist, text, space);
  vector<vector<TextLine> > partitions = partitionBlocks(lines, space);
  if (partitions.size() < 2)
    return false;
  double ms = mean<Line, int>(space, [](Line l){return l.length();});
  for (int i = 0; i < partitions.size(); i++){
    vector<Rect> merged = verticalMerge(partitions[i]);
    double mes = mean<TextLine, double>(partitions[i], [](TextLine tl){return  tl.getSpace();});
    double met = mean<Rect, int>(merged, [](const Rect& r){return r.width;});
    double lv = welford<Rect, int>(merged, [](Rect r){return r.x;});
    double cv = welford<Rect, double>(merged, [](Rect r){return (r.x+r.br().x)*0.5;});
    double rv = welford<Rect, int>(merged, [](Rect r){return r.br().x;});
    if (lv >= met || cv >= met || rv >= met || ms <= mes)
      return false;    
  }
  return true;
}

bool hasLowDensity(ComponentStats& cs){
  return (double)cs.area/cs.bb_area;
}


bool noCut(RT& tree, Rect& r){
  return cut_tree(tree, r);
}

bool mostlyText(vector<ComponentStats> stats){
  if (stats.size() < 2)
    return true;
  int area = 0;
  Rect r = stats[0].r;
  for (auto stat : stats){
    r |= stat.r;
    area += stat.r.area();
  }
  return 1.2*r.area() > area;
}

bool manyRows(Mat& img){
  Mat hist;
  reduce(img, hist, 1, CV_REDUCE_SUM, CV_64F);
  transpose(hist, hist);
  vector<Line> text, space;
  find_lines(hist, text, space);
  return text.size() > 1;
}

bool verify(Mat& region,
	       ImageMeta& im,
	       vector<ComponentStats>& textData,
	       ComponentStats& cs)
{
  if (hasLowDensity(cs) &&
      regionIsRectangle(cs) &&
      manySmallRect(region, cs) &&
      noCut(im.t_tree, cs.r)){
    vector<TextLine> tls = linesInRegion(im.t_tree, textData, cs.r);
    Mat tableCopy = region.clone();
    for (int i = 0; i < tls.size(); i++){
      rectangle(tableCopy, tls[i].getBox(), Scalar(255), CV_FILLED);
    }
    return verticalArrangement(tableCopy, tls);  
  }
  return false;
}

bool verifyReg(Mat& text, Mat& nontext){
  vector<ComponentStats> statsText = statistics(text);
  vector<ComponentStats> statsNontext = statistics(nontext);
  RT tree;
  Rect r(0,0, text.cols, text.rows);
  for (int i = 0; i < statsText.size(); i++)
    insert2tree(tree, statsText[i].r, i); 
  vector<TextLine> tls = linesInRegion(tree, statsText, r);
  Mat tableCopy = text.clone();
  for (int i = 0; i < tls.size(); i++){
    rectangle(tableCopy, tls[i].getBox(), Scalar(255), CV_FILLED);
  }
  bool lowDensity = true;;
  for (ComponentStats cs : statsNontext){
    lowDensity = hasLowDensity(cs);
  }
  return verticalArrangement(tableCopy, tls) && mostlyText(statsNontext) && lowDensity && manyRows(text);  
}
