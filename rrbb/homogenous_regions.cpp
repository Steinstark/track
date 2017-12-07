#include <algorithm>
#include <queue>

#include "homogenous_regions.hpp"

using namespace std;
using namespace cv;

//TODO
//find vertical homogenous regions
void displayHist(const Mat& img, string str){
  int m = 0;
  int length = max(img.rows, img.cols);
  for (int i = 0; i < length; i++){
    int val = img.at<double>(i);
    m = max(m,val);
  }
  Mat hist;
  if (img.rows > img.cols){
    hist = Mat::zeros(img.rows, m+50, CV_8U);
    for (int i = 0; i < img.rows; i++){
      for (int j = 0; j < img.at<double>(i,0); j++){
	hist.at<uchar>(i,j) = 255;
      }
    }
  }else{
    hist = Mat::zeros(m+50, img.cols, CV_8U);
    for (int i = 0; i < img.cols; i++){
      for (int j = 0; j < img.at<double>(0, i); j++){
	hist.at<uchar>(j,i) = 255;
      }
    }
  }

  imshow(str, hist);
  waitKey(0);
}

void findLines(const Mat& hist, vector<Rect>& text, vector<Rect>& space, function<Rect(int, int)> createRect){
  int t = 0;
  int length = max(hist.rows, hist.cols);
  for (int i = 1; i < length; i++){
    if (hist.at<double>(i) > 0 && hist.at<double>(i-1) == 0){
      //      space.push_back(Rect(r.x, t, r.width, i-t) + r.tl());
      space.push_back(createRect(t,i));
      t = i;
    }
    else if (hist.at<double>(i) == 0 && hist.at<double>(i-1) > 0){
      //text.push_back(Rect(r.x, t, r.width, i-t) + r.tl());
      text.push_back(createRect(t,i));
      t = i;
    }
  }
  int index = length;
  double last = hist.at<double>(index);
  if (t != index){
    //    if (last) text.push_back(Rect(r.x,t,r.width, index-t) + r.tl());
    //    else space.push_back(Rect(r.x, t, r.width, index-t) + r.tl());
    if (last) text.push_back(createRect(t, index));
    else space.push_back(createRect(t, index));
  }  
}

struct ABRect{
  Rect r;
  int a;
  int b;
  ABRect(Rect r, int a, int b): r(r),a(a), b(b){}
};

ABRect getABRect(const vector<Rect>& v, int i, function<int(Rect)> getVal, function<int(Point)> pointVal){
  int a = 0, b = 0;
  if (i != 0){
    a = getVal(v[i]) -  pointVal(v[i-1].br());
  }
  if (i != v.size()-1){
    b = getVal(v[i+1]) - pointVal(v[i].br());
  }
  return ABRect(v[i], a, b);
}

vector<int> split_text(int low, int high, int a, int b){
  vector<int> v;
  if (a > 0)
    v.push_back(low-a/2);
  if (b > 0)
    v.push_back(high+b/2);
  return v;
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

void split_block(const Mat& hist, queue<Rect>& q, vector<Rect>& v, Rect r, int dim){
  function<Rect(int, int)> createRect1;
  function<Rect(int, int)> createRect2;
  function<bool(Rect, Rect)> sortRect;
  function<int(const Rect&)> getLength;
  function<int(Rect)> getVal;
  function<int(Point)> pointVal;
  if (dim == 0){
    createRect1 = [&r](int low, int high){return Rect(r.x+low, r.y, high-low,r.height);};
    createRect2 = [&r](int low, int high){return Rect(low, r.y, high-low,r.height);};
    sortRect = [](const Rect& a, const Rect& b){return a.width < b.width;};
    getLength = [](Rect a){return a.width;};
    getVal = [](Rect a){return a.x;};
    pointVal = [](Point p){return p.x;};
  }
  else{
    createRect1 = [&r](int low, int high){return Rect(r.x, r.y+low, r.width, high-low);};
    createRect2 = [&r](int low, int high){return Rect(r.x, low, r.width, high-low);};
    sortRect = [](const Rect& a, const Rect& b){return a.height < b.height;};
    getLength = [](const Rect& a){return a.height;};
    getVal = [](Rect a){return a.y;};
    pointVal = [](Point p){return p.y;};
  }
  vector<Rect> text;
  vector<Rect> space;
  findLines(hist, text, space, createRect1);	 
  if (text.size() == 0 || space.size() == 0){
    cout << "Size is 0" << endl;
    v.push_back(r);
    return;
  }
  vector<ABRect> a1;
  for (int i = 0; i < text.size(); i++){
    a1.push_back(getABRect(text, i, getVal, pointVal));
  }
  sort(a1.begin(), a1.end(), [&sortRect](ABRect l, ABRect r){return sortRect(l.r, r.r);});
  sort(space.begin(), space.end(), sortRect);
  int maxt = getLength(a1.back().r);
  int maxs = getLength(space.back());  
  double medianSpace = getMedian<Rect>(space, getLength);
  double medianText = getMedian<ABRect>(a1, [&getLength](ABRect f){return getLength(f.r);});
  vector<int> split;
  split.push_back(getVal(r));
  if (maxs > medianSpace){
    Rect a0 = space.back();
    split.push_back(getVal(a0) + getLength(a0)/2);
  }
  else if (maxt > medianText){
    ABRect a0 = a1.back();
    vector<int> tmp = split_text(getVal(a0.r), getVal(a0.r) + getLength(a0.r), a0.a, a0.b);
    split.insert(split.end(), tmp.begin(), tmp.end());
  }
  else{
    cout << "BAD BAD BAD (not good)" << endl;
    v.push_back(r);
    return;
  }
  split.push_back(getVal(r)+getLength(r)-1);
  splitAndQueue(q, split, createRect2);
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
  //displayHist(sred, "img");
  Mat blurred;
  blur(sred, blurred, Size(s,s), Point(-1,-1), BORDER_CONSTANT);  
  Mat sob;
  Scharr(blurred, sob, -1, xder, yder);
  vector<int> a0;
  Mat sSob;
  blur(sob, sSob, Size(10,10));
  //displayHist(sob, "img");
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
      split_block(hist, q, v, r, dim);
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
