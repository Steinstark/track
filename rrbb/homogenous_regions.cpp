#include <algorithm>
#include <list>
#include <set>
#include <utility>
#include "homogenous_regions.hpp"
#include "image_util.hpp"
#include "utility.hpp"
#include "util.hpp"

#include "debug_header.hpp"

using namespace std;
using namespace cv;

using IntPair = pair<int, int>;

set<int> split(const Mat& hist){
  int s = 10;
  vector<Line> text, space;
  find_lines(hist, text, space);
  auto it = space.begin();
  set<int> splitSet;
  OnlineStat stat;
  Line* old;
  int count = 0;
  for (Line& l : space){
    double var = stat.welfordStep(l.length());
    if (!count){
      old = &l;
    }      
    if (var > 3.5){
      stat.reset();
      if (count < 2 && old->length() > l.length()){
	splitSet.insert((old->l + old->r)/2);
	stat.welfordStep(l.length());
	old = &l;
	count = 0;
      }
      else{
	splitSet.insert((l.l + l.r)/2);
	count = -1;
      }
    }
    count++;
  }
  return splitSet;
}

list<IntPair> splitRegion(Mat& hist){
  list<IntPair> qout;
  set<int> positions = split(hist);
  int prev = 0;
  for (int current : positions){
    qout.push_back(IntPair(prev, current));
    prev = current;
  }
  qout.push_back(IntPair(prev, hist.rows));
  return qout;
}

list<IntPair> homogenous_rows(const Mat& img){
  Mat histRow;
  reduce(img, histRow, 1, CV_REDUCE_SUM, CV_64F);
  return splitRegion(histRow);
}

list<IntPair> homogenous_cols(const Mat& img){
  Mat histCol;
  reduce(img, histCol, 0, CV_REDUCE_SUM, CV_64F);
  transpose(histCol, histCol);
  return splitRegion(histCol);
}

list<Rect> homogenous_recursive(const Mat& img){
  list<IntPair> cols = homogenous_cols(img);
  list<Rect> regions;
  for (IntPair col : cols){
    Mat local = img(pos2rect(col.first, 0, col.second, img.rows));
    list<IntPair> rows = homogenous_rows(local);
    for (IntPair row : rows){
      regions.push_back(pos2rect(col.first, row.first, col.second, row.second));
    }
  }
  return regions;
}

list<Rect> homogenous_regions(const Mat& img){
  list<IntPair> rows = homogenous_rows(img);
  list<IntPair> cols = homogenous_cols(img);
  list<Rect> regions;
  for (IntPair row : rows){
    for (IntPair col : cols){
      regions.push_back(pos2rect(col.first, row.first, col.second, row.second));
    }
  }  
  return regions;
}
