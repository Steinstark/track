#include <vector>
#include <algorithm>
#include <queue>

#include "detect_table.hpp"
#include "RTree.h"
#include "heuristic_filter.hpp"
#include "recursive_filter.hpp"
#include "mla.hpp"
#include "mlc.hpp"
#include "noise.hpp"
#include "segment.hpp"
#include "utility.hpp"

using namespace std;
using namespace cv;

vector<Rect> mat2rects(Mat& blob){
  Mat cc, stats, centroids;
  int labels = connectedComponentsWithStats(blob, cc, stats, centroids, 8, CV_32S);
  vector<ComponentStats> components;
  vector<Rect> tables;
  components.push_back(stats2component(stats, 0));
  for (int i = 1; i < labels; i++){
    ComponentStats cs = stats2component(stats, i);
    tables.push_back(cs.r);
  }
  return tables;
}

//img in grayscale
vector<Rect> detect_tables(Mat& text){
  Mat nontext(text.size(), CV_8UC1, Scalar(0));
  heuristic_filter(text, nontext);
  multi_level_analysis(text, nontext);
  multi_level_classification(text, nontext);
  remove_noise(text, nontext);
  Mat tableBlob = segment(text, nontext);
  vector<Rect> tables = mat2rects(tableBlob);
  /*  for (Rect& table : tables){
    rectangle(text, table, Scalar(255));
  }
  imshow("table text image", text);
  waitKey(0);*/
  return tables;
}
