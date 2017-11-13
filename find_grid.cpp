#include <algorithm>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

struct rectSortX
{
  bool operator()(const cv::Rect &a, const cv::Rect &b) const{
    return a.tl().x < b.tl().x;
  }
};

struct rectSortY
{
  bool operator()(const cv::Rect &a, const cv::Rect &b) const{
    return a.tl().y < b.tl().y;
  }
};

int bin_search(vector<int>& v, int val){
  int l = 0;
  int r = v.size()-1;  
  while (l < r){
    int mid = l + r;
    if (mid+1 == v.size() || (v[mid] <= val && v[mid+1] > val))
      return mid;
    if (v[mid] > val)
      r = mid-1;
    else
      l = mid+1;    
  }
  //Should never happen given the input
  return -1;
}

vector<int> rect2vec(cv::Rect r){
  vector<int> v(4);
  Point p = r.tl();
  v[0] = p.x;
  v[2] = p.y;
  p = r.br();
  v[1] = p.x;
  v[3] = p.y;
}
//IMPROVEMENT
//Better way of chosing index to work with
vector<Rect> overlapping(vector<Rect>& bb, int p0){ 
  vector<int> b = rect2vec(bb[0]);
  int p1 = p0+1, s0 = (p0+2)%4 , s1 = (p0+3)%4; 
  for (int i = 0; i < bb.size(); i++){
    vector<int> cb = rect2vec(bb[i]);
    if (b[p1] > cb[p0]){
      if (cb[p1] > b[p1]){
	b[p1] = cb[p1];
      }
      if (cb[s1] > b[s1]){
	b[s1] = cb[s1];
      }
      if (cb[s0] < b[s0]){
	b[s0] = cb[s0];
      }
    }
    else{
      bb.push_back(Rect(b[0],b[1],b[2],b[3]));
      b = cb;
    }	
  }
}

vector<vector<string> > find_grid(vector<Rect> bb, vector<string> text){
  vector<Rect> textboxes = bb;
  sort(bb.begin(), bb.end(), rectSortX());
  vector<Rect> cols = overlapping(cols, 0);
  sort(bb.begin(), bb.end(), rectSortY());
  vector<Rect> rows = overlapping(rows, 2);
  vector<vector<string> > table(cols.size(), vector<string>());
  vector<int> xvec(cols.size());
  for (int i = 0; i < cols.size(); i++){
    xvec[i] = cols[i].tl().x;
  }
  vector<int> yvec(rows.size());
  for (int i = 0; i < rows.size(); i++){
    yvec[i] = rows[i].tl().y;
  }
  for (int i = 0; i < text.size(); i++){    
    int x_index = bin_search(xvec, bb[i].tl().x);
    int y_index = bin_search(yvec,bb[i].tl().y);
    table[x_index][y_index] += text[i];
  }
  return table;
} 
