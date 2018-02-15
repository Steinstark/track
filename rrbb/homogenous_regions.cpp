#include <algorithm>
#include <list>
#include <utility>
#include "homogenous_regions.hpp"
#include "image_util.hpp"
#include "utility.hpp"

using namespace std;
using namespace cv;

using IntPair = pair<int, int>;

int isOutlier(vector<Line> lines){
  int maxVal = -1;
  int index;
  for (int i = 0; i < lines.size(); i++){
    if (lines[i].length() > maxVal){
      maxVal = lines[i].length();
      index  = i;
    }
  }
  sort(lines.begin(), lines.end(), [](const Line& a, const Line& b){return a.length() > b.length();});
  int mid = lines.size()/2;
  int median = lines[mid].length();
  if (maxVal > median) return index;
  return 0;
}
	  
double similarity(const Mat& hist){
  int s = 5;
  Mat blurred, sob;
  blur(hist, blurred, Size(1,s), Point(-1,-1), BORDER_CONSTANT);
  Scharr(blurred, sob, -1, 0, 1);
  vector<Line> text, space;
  vector<int> a0;
  find_lines(sob, text, space);
  for (int i = 0; i < space.size(); i++){
    a0.push_back(space[i].length());
  }  
  Mat delta = Mat(a0.size(), 1, CV_32S, a0.data());
  Mat mean, stddev;
  meanStdDev(delta, mean, stddev);
  double st = stddev.at<double>(0,0);
  return st*st;
}

int split(Mat& hist){
  vector<Line> text, space;
  find_lines(hist, text, space);
  if (space.empty()){
    //cout << "Can't split region without space. Returning split 0" << endl;
    return 0;
  }
  int spaceIndex = isOutlier(space);
  if (spaceIndex){
    return (space[spaceIndex].l + space[spaceIndex].r)/2;
  }
  int textIndex = isOutlier(text);
  if (textIndex){
    if (textIndex == space.size())
      return (space[textIndex-1].l + space[textIndex-1].r)/2;
    return (space[textIndex].l + space[textIndex].r)/2;
  }
  //cout << "Region can't be split. Returning split 0" << endl;
  return 0;
}

list<IntPair> splitRegion(Mat& hist){
  list<IntPair> qin, qout;
  qin.push_back(IntPair(0, hist.rows));  
  while (qin.size()){
    IntPair p = qin.front();    
    qin.pop_front();
    Rect r = pos2rect(0, p.first, 1, p.second);
    Mat localHist = hist(r);
    int pos = split(localHist);
    if (similarity(localHist) <= 1.2 || !pos)
      qout.push_back(p);
    else{
      pos += p.first;
      qin.push_back(IntPair(p.first, pos));
      qin.push_back(IntPair(pos, p.second));
    }
  }
  return qout;
}

list<Rect> homogenous_regions(const Mat& img){
  Mat histRow;
  reduce(img, histRow, 1, CV_REDUCE_SUM, CV_64F);
  list<IntPair> rows = splitRegion(histRow);
  Mat histCol;
  reduce(img, histCol, 0, CV_REDUCE_SUM, CV_64F);
  transpose(histCol, histCol);
  list<IntPair> cols = splitRegion(histCol);
  list<Rect> regions;
  for (IntPair row : rows){
    for (IntPair col : cols){
      regions.push_back(pos2rect(col.first, row.first, col.second, row.second));
    }
  }
  
  Mat mask(img.size(), CV_8UC1, Scalar(0));
  for (Rect& region : regions){
    rectangle(mask, region, Scalar(255));
  }
  Mat overlay = img+mask;
  return regions;
}
