#include <algorithm>
#include <queue>

#include "homogenous_regions.hpp"

using namespace std;
using namespace cv;

//BUG
//crashed for some input images due to creating rects of 0 height
void displayHist(const Mat& img, string str){
  int m = 0;
  for (int i = 0; i < img.rows; i++){
    int val = img.at<double>(i,0);
    m = max(m,val);
  }
  Mat hist = Mat::zeros(img.rows, m+50, CV_8U);
  for (int i = 0; i < img.rows; i++){
    for (int j = 0; j < img.at<double>(i,0); j++){
      hist.at<uchar>(i,j) = 255;
    }
  }
  imshow(str, hist);
  waitKey(0);
}

void findLines(const Mat& hist, vector<Rect>& text, vector<Rect>& space, Rect r){
  int t = 0;
  for (int i = 1; i < hist.rows; i++){
    if (hist.at<double>(i,0) > 0 && hist.at<double>(i-1,0) == 0){
      space.push_back(Rect(r.x, t, r.width, i-t) + r.tl());
      t = i;
    }
    else if (hist.at<double>(i,0) == 0 && hist.at<double>(i-1,0) > 0){
      text.push_back(Rect(r.x, t, r.width, i-t) + r.tl());
      t = i;
    }
  }
  int index = hist.rows - 1;
  double last = hist.at<double>(index, 0);
  if (t != index){
    if (last) text.push_back(Rect(r.x,t,r.width, index-t) + r.tl());
    else space.push_back(Rect(r.x, t, r.width, index-t) + r.tl());
  }
  
}

struct ABRect{
  Rect r;
  int a;
  int b;
  ABRect(Rect r, int a, int b): r(r),a(a), b(b){}
};

ABRect getABRect(const vector<Rect>& v, int i){
  int a = 0, b = 0;
  if (i != 0){
    a = v[i].y - v[i-1].br().y;
  }
  if (i != v.size()-1){
    b = v[i+1].y - v[i].br().y;
  }
  return ABRect(v[i], a, b);
}

void split_text(queue<Rect>& q, ABRect sr, Rect r){
  Rect c = sr.r;
  int a = sr.a;
  int b = sr.b;
  int split1 = c.y-a/2;
  int split2 = c.br().y+b/2;
  if (a > 0)
    q.push(Rect(r.x, r.y, r.width, split1 - r.y));
  q.push(Rect(r.x, split1, r.width, split2-split1));
  if (sr.b > 0)
    q.push(Rect(r.x, split2, r.width, r.br().y-split2));
}

template <class T>
double getMedian(const vector<T>& v, function<int(T)> val){
  int mid = v.size()/2;
  if (v.size() % 2)
    return val(v[mid]);
  return (val(v[mid]) +val(v[mid-1]))/2.0;
}

void split_block(const Mat& hist, queue<Rect>& q, vector<Rect>& v, Rect r){
vector<Rect> text;
vector<Rect> space;
  findLines(hist, text, space, r);	 
  if (text.size() == 0 || space.size() == 0){
    cout << "Size is 0" << endl;
    return;
  }
  vector<ABRect> a1;
  for (int i = 0; i < text.size(); i++){
    a1.push_back(getABRect(text, i));
  }
  sort(a1.begin(), a1.end(), [](ABRect l, ABRect r){return l.r.height < r.r.height;});
  sort(space.begin(), space.end(), [](Rect l, Rect r){return l.height < r.height; });
  int mint = a1[0].r.height;
  int mins = space[0].height;
  int maxt = a1.back().r.height;
  int maxs = space.back().height;  
  double medianSpace = getMedian<Rect>(space, [](Rect f){return f.height;});
  double medianText = getMedian<ABRect>(a1, [](ABRect f){return f.r.height;});
  int splitPos;
  if (maxs > medianSpace){
    Rect a0 = space.back();
    splitPos = a0.y +a0.height/2;
    q.push(Rect(r.x, r.y, r.width, splitPos-r.y));
    q.push(Rect(r.x, splitPos+1, r.width, r.br().y - splitPos - 1));
  }
  else if (maxt > medianText){
    ABRect a0 = a1.back();
    split_text(q, a0, r);
  }
  else{
    cout << "BAD BAD BAD (not good)" << endl;
    v.push_back(r);
  }
  return;
}

bool isExtrema(double a, double b){
  return (a < 0 && b > 0) || (a > 0 && b < 0);
}

double homogenity_stats(Mat& bw, Mat& hist){
  Mat red;
  int s = 30;
  //histogram
  int scale = 255;
  //  imshow("img", bw);
  //  waitKey(0);
  reduce(bw, hist, 1, CV_REDUCE_SUM, CV_64F);
  Mat sred = hist / scale;  
  Mat blurred;
  //medelkernel 
  blur(sred, blurred, Size(1,s), Point(-1,-1), BORDER_CONSTANT);  
  //gradienten
  //  displayHist(blurred, "img");
  Mat sob;
  Scharr(blurred, sob, -1, 0, 1);
  vector<int> a0;
  Mat sSob;
  blur(sob, sSob, Size(1,10));  
  for (int i = 1; i < sSob.rows ; i++){
    if (isExtrema(sSob.at<double>(i,0), sSob.at<double>(i-1,0))){
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

vector<Rect> homogenous_regions(const Mat& img){
  queue<Rect> q;
  vector<Rect> v;
  double varmax = 1.2;
  q.push(Rect(0,0, img.cols, img.rows));
  while(!q.empty()){
    Rect r = q.front();
    q.pop();
    Mat block = img(r), hist;    
    double var = homogenity_stats(block, hist);
    if (var > varmax){
      split_block(hist, q, v, r);
    }
    else
      v.push_back(r);
  }
  return v;
}
