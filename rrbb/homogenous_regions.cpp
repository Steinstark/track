#include <algorithm>
#include <list>
#include <set>
#include <utility>
#include "homogenous_regions.hpp"
#include "image_util.hpp"
#include "utility.hpp"

using namespace std;
using namespace cv;

using IntPair = pair<int, int>;

int splitIndex(vector<Line> lines){
  int maxVal = -1;
  int index;
  for (int i = 0; i < lines.size(); i++){
    if (lines[i].length() > maxVal){
      maxVal = lines[i].length();
      index  = i;
    }
  }
  return index;
}

bool isExtreme(double a, double b){
  return (a < 0 && b >= 0) || (a >= 0 && b < 0);
}

	  
double similarity(const Mat& hist){
  int s = 10;
  Mat blurred, sob;
  blur(hist, blurred, Size(1,s), Point(-1,-1), BORDER_CONSTANT);
  
  //Scharr(blurred, sob, -1, 0, 1);
  vector<Line> text, space;
  vector<int> a0, a1;
  find_lines(blurred, text, space);
  for (int i = 0; i < text.size(); i++){
    a0.push_back(text[i].length());
  }
  for (int i = 0; i < space.size(); i++){
    a1.push_back(space[i].length());
  }
  Mat deltaText = Mat(a0.size(), 1, CV_32S, a0.data()), meanText, devText;
  Mat deltaSpace = Mat(a1.size(), 1, CV_32S, a1.data()), meanSpace, devSpace;
  meanStdDev(deltaText, meanText, devText);
  meanStdDev(deltaSpace, meanSpace, devSpace);
  double td = devText.at<double>(0,0);
  double sd = devText.at<double>(0,0);  
  return (td*td + sd*sd)/2;
}

set<int> split(Mat& hist){
  vector<Line> text, space;
  find_lines(hist, text, space);
  set<int> p;
  if (space.empty()){
    //cout << "Can't split region without space. Returning split 0" << endl;
    return p;
  }
  int spaceIndex = splitIndex(space);
  p.insert((space[spaceIndex].l + space[spaceIndex].r)/2);
  int textIndex = splitIndex(text);  
  if (textIndex < space.size()){
    p.insert((space[textIndex].l + space[textIndex].r)/2);
  }else if (textIndex != spaceIndex){
    p.insert((space[textIndex-1].l + space[textIndex-1].r)/2);
  }
  return p;
}

list<IntPair> splitRegion(Mat& hist){
  list<IntPair> qin, qout;
  qin.push_back(IntPair(0, hist.rows));  
  while (qin.size()){
    IntPair p = qin.front();    
    qin.pop_front();
    Rect r = pos2rect(0, p.first, 1, p.second);
    Mat localHist = hist(r);
    set<int> positions = split(localHist);
    if (similarity(localHist) <= 1.2 || positions.empty())
      qout.push_back(p);
    else{
      int prev = p.first;
      for (int current : positions){
	qin.push_back(IntPair(prev, p.first+current));
	prev = p.first + current;
      }
      qin.push_back(IntPair(prev, p.second));
    }
  }
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
  Mat mask(img.size(), CV_8UC1, Scalar(0));
  for (Rect& region : regions){
    rectangle(mask, region, Scalar(255));
  }
  Mat overlay = img+mask;
  return regions;
}
