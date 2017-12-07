#include <algorithm>
#include <queue>

#include "homogenous_regions.hpp"

using namespace std;
using namespace cv;

struct Line{
  int l, r;
  Line(int l, int r): l(l), r(r){};
  int length();
};

int Line::length(){
  return r - l;
}

void findLines(const Mat& hist, vector<Line>& text, vector<Line>& space){
  int length = max(hist.rows, hist.cols);
  int t = 0;
  for (int i = 1; i < length; i++){
    if ((hist.at<double>(i) > 0 && hist.at<double>(i-1) == 0) ||
	(hist.at<double>(i) && i == length-1)){
      space.push_back(Line(t,i));
      t = i;
    }
    else if ((hist.at<double>(i) == 0 && hist.at<double>(i-1) > 0) ||
	     !hist.at<double>(i) && i == length - 1){
      text.push_back(Line(t,i));
      t = i;
    }
  }
}
 
template <class T>
double getMedian(const vector<T>& v, function<int(T)> val){
  int mid = v.size()/2;
  if (v.size() % 2)
    return val(v[mid]);
  return (val(v[mid]) +val(v[mid-1]))/2.0;
}

void splitAndQueue(queue<Rect>& q, const vector<int>& v, function<Rect(int, int)> createRect){
  if (v.empty())
    return;
  for (int i = 1; i < v.size(); i++){
    if (v[i-1] != v[i])
      q.push(createRect(v[i-1], v[i]));
  }
  //  q.push(createRect(v.back()));
}

vector<int> range(int l){
  vector<int> r(l);
  for (int i = 0; i < l; i++){
    r[i] = i;   
  }
  return r;
}

void split_text(const vector<Line>& text, const vector<int>& text_perm, vector<int>& split){
  int i = text_perm[0];  
  if (i > 0)
    split.push_back(text[i-1].r + (text[i].l - text[i-1].r)/2);
  if (i < text.size())
    split.push_back(text[i].r + (text[i+1].l - text[i].r)/2);
}

void split_space(const vecto<Line>& space, const vector<int>& space_perm, vector<int>& split){
  int i = space_perm[0];
  split.push_back(space[i] + space.length()/2);
}

vector<int> split_block2(const Mat& hist){
  vector<int> split;
  vector<Line> text;
  vector<Line> space;
  find_lines(hist, text, space);
  vector<int> text_perm = range(text.size());
  vector<int> space_perm = range(space.size());
  sort(text_perm.begin(), text_perm.end(), [&text](int a, int b){return text[a].length() > text[b].length();});
  sort(space_perm.begin(), space_perm.end(), [&space](int a, int b){return space[a].length() > space[b].length();});
  
  if (maxs > meds){
    split_space(space, split);
  }
  else if (maxt > medt){
    split_text(text, text_perm, split);          
  }			
  return split;
}

bool isExtrema(double a, double b){
  return (a < 0 && b > 0) || (a > 0 && b < 0);
}

//1 row 0 col
double homogenity_stats(Mat& bw, Mat& hist, int dim){
  //  imshow("img", bw);
  int xder = 0;
  int yder = 0;
  if (dim == 0)
    xder = 1;
  if (dim == 1)
    yder = 1;
  int s = 30;
  int scale = 255;
  reduce(bw, hist, dim, CV_REDUCE_SUM, CV_64F);
  Mat sred = hist / scale;
  Mat blurred;
  blur(sred, blurred, Size(s,s), Point(-1,-1), BORDER_CONSTANT);  
  Mat sob;
  Scharr(blurred, sob, -1, xder, yder);
  vector<int> a0;
  Mat sSob;
  blur(sob, sSob, Size(10,10));
  int length = max(sSob.rows, sSob.cols);
  for (int i = 1; i < length ; i++){
    if (isExtrema(sSob.at<double>(i), sSob.at<double>(i-1))){
      a0.push_back(i);
    }
  }
  if (a0.size() < 2)
    return 0;
  Mat peaks(a0.size(), 1, CV_32S, a0.data());
  vector<int> a1;
  for (int i = 1; i < a0.size(); i++){
    a1.push_back(a0[i] - a0[i-1]);
  }
  Mat delta = Mat(a0.size()-1, 1, CV_32S, a1.data());
  Mat mean, stddev;
  meanStdDev(delta, mean, stddev);
  double st = stddev.at<double>(0,0);
  return st*st;
}

vector<Rect> get_regions(const Mat& img, Rect bb, int dim){
  queue<Rect> q;
  q.push(bb);
  double varmax = 1.2;
  vector<Rect> v;
  while(!q.empty()){
    Rect r = q.front();
    q.pop();
    Mat block = img(r), hist;
    double var = homogenity_stats(block, hist, dim);
    if (var > varmax){
      vector<int> split = split_block(hist);
      splitAndQueue(q, split, 
    }
    else
      v.push_back(r);
  }
  return v;
}

vector<Rect> homogenous_regions(const Mat& img){
  vector<Rect> h = get_regions(img, Rect(0,0, img.cols, img.rows), 1);
  vector<Rect> regions;
  for (int i = 0; i < h.size(); i++){
    vector<Rect> tmp = get_regions(img, h[i], 0);
    regions.insert(regions.end(), tmp.begin(), tmp.end());
  }
  return regions;
}
