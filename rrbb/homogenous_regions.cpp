#include <algorithm>
#include <queue>

#include "homogenous_regions.hpp"

using namespace std;
using namespace cv;

struct Line{
  int l, r;
  Line(int l, int r): l(l), r(r){};
  int length() const;
};

int Line::length() const{
  return r-l;
}

bool isExtrema(double a, double b){
  return (a < 0 && b > 0) || (a > 0 && b < 0);
}

vector<Rect> split_rectangles(Rect r, const vector<int>& split, int dim){
  function<Rect(int, int)> create;
  vector<Rect> splitted;
  int end;
  if (dim){
    create = [&r](int a, int b){return Rect(r.x,r.y + a, r.width, b-a);};
    end = r.height;
  }else{
    create = [&r](int a, int b){return Rect(r.x+ a,r.y, b-a, r.height);};
    end = r.width;
  }
  int a = 0, b = 0;
  for (int i = 0; i < split.size(); i++){
    b = split[i];
    if (a != b)
      splitted.push_back(create(a,b));
    a = b;
  }
  if (b != end)
    splitted.push_back(create(b,end));
  return splitted;
}

template <class T>
double getMedian(const vector<T>& v, function<int(T)> val){
  int mid = v.size()/2;
  if (v.size() % 2)
    return val(v[mid]);
  return (val(v[mid]) +val(v[mid-1]))/2.0;
}

vector<int> range(int l){
  vector<int> r(l);
  for (int i = 0; i < l; i++){
    r[i] = i;   
  }
  return r;
}

void split_text(const vector<Line>& text, const vector<Line>& space, const vector<int>& text_perm, vector<int>& split){  
  if (space.empty()){
    //should never happen if the program is working correctly
    cout << "No space to split" << endl;
    return;
  }
  if (text.size() < 2){
    //should never happen if the program is working correctly
    cout << "Not splitting single text region" << endl;
    return;
  }  
  int i = text_perm[0];  
  if (i > 0)
    split.push_back((space[i-1].l + space[i-1].r)/2);
  else
    split.push_back((space[i].l + space[i].r)/2);
}

void split_space(const vector<Line>& space, const vector<int>& space_perm, vector<int>& split){
  int i = space_perm[0];
  split.push_back((space[i].l+space[i].r)/2);  
}


void find_lines(const Mat& hist, vector<Line>& text, vector<Line>& space){
  int length = max(hist.rows, hist.cols);
  int t = 0;
  for (int i = 1; i < length; i++){
    if (!hist.at<double>(i-1) && hist.at<double>(i)){
      t = i;
    }
    else if (hist.at<double>(i-1) && !hist.at<double>(i)){
      text.push_back(Line(t,i));
      t = -1;
    }
  }
  if (t != -1)
    text.push_back(Line(t,length-1));
  for (int i = 1; i < text.size(); i++){
    space.push_back(Line(text[i-1].r+1, text[i].l-1));
  }
}
	 
vector<int> split_block(const Mat& hist){
  vector<Line> text;
  vector<Line> space;
  find_lines(hist, text, space);
  vector<int> text_perm = range(text.size());
  vector<int> space_perm = range(space.size());
  sort(text_perm.begin(), text_perm.end(),
       [&text](int a, int b){return text[a].length() > text[b].length();});
  sort(space_perm.begin(), space_perm.end(),
       [&space](int a, int b){return space[a].length() > space[b].length();});
  vector<int> split;
  if (space.empty() || text.empty())
    return split;
  int maxs = space[space_perm[0]].length();  
  int maxt = text[text_perm[0]].length();
  double meds = getMedian<int>(space_perm, [&space](int a){return space[a].length();});
  double medt = getMedian<int>(text_perm, [&text](int a){return text[a].length();});
  if (maxs > meds){
    split_space(space, space_perm, split);
  }
  else if (maxt > medt){
    split_text(text, space, text_perm, split);          
  }			
  return split;
}
	 
//row 1 col 0
double homogenity_stats(Mat& bw, Mat& hist, int dim){
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
  function<int(Rect)> getVal;
  queue<Rect> q;
  q.push(bb);
  double varmax = 1.2;
  vector<Rect> v;
  while(!q.empty()){
    Rect r = q.front();
    q.pop();
    Mat block = img(r), hist;
    double var = homogenity_stats(block, hist, dim);
    vector<int> split = split_block(hist);      
    if (var > varmax && !split.empty()){
      vector<Rect> rvec = split_rectangles(r, split, dim);
      for (int i = 0; i < rvec.size(); i++)
	q.push(rvec[i]);
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
  Mat disp = img.clone();
  for (int i = 0; i < regions.size(); i++){
    rectangle(disp, regions[i], Scalar(255));
  }
  imshow("homogen", disp);
  waitKey(0);
  return regions;
}
