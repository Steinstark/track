#include <algorithm>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include "find_grid.hpp"

using namespace std;
using namespace cv;

template <typename T, typename Compare>
std::vector<std::size_t> sort_permutation(
    const std::vector<T>& vec,
    Compare compare)
{
    std::vector<std::size_t> p(vec.size());
    std::iota(p.begin(), p.end(), 0);
    std::sort(p.begin(), p.end(),
        [&](std::size_t i, std::size_t j){ return compare(vec[i], vec[j]); });
    return p;
}

template <typename T>
std::vector<T> apply_permutation(
    const std::vector<T>& vec,
    const std::vector<std::size_t>& p)
{
    std::vector<T> sorted_vec(vec.size());
    std::transform(p.begin(), p.end(), sorted_vec.begin(),
        [&](std::size_t i){ return vec[i]; });
    return sorted_vec;
}

template <typename T>
void apply_permutation_in_place(
    std::vector<T>& vec,
    const std::vector<std::size_t>& p)
{
    std::vector<bool> done(vec.size());
    for (std::size_t i = 0; i < vec.size(); ++i)
    {
        if (done[i])
        {
            continue;
        }
        done[i] = true;
        std::size_t prev_j = i;
        std::size_t j = p[i];
        while (i != j)
        {
            std::swap(vec[prev_j], vec[j]);
            done[j] = true;
            prev_j = j;
            j = p[j];
        }
    }
}


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
  while (l <= r){
    int mid =(l + r)/2;
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
  return v;
}

Rect vec2rect(vector<int> v){
  return Rect(v[0], v[2], v[1]-v[0], v[3]-v[2]);
}

vector<Rect> overlapping(vector<Rect>& bb, int p0){
  vector<Rect> obb;
  vector<int> b = rect2vec(bb[0]);
  int p1 = p0+1, s0 = (p0+2)%4 , s1 = (p0+3)%4;
  vector<int> cb;
  for (int i = 0; i < bb.size(); i++){
    cb = rect2vec(bb[i]);
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
      obb.push_back(vec2rect(b));
      b = cb;
    }
  }
  if (cb != b)
    obb.push_back(vec2rect(b));
  return obb;
}

vector<Rect> inside(vector<Rect> v, Rect bb){
  vector<Rect> inside;
  for (int i = 0; i < v.size(); i++){
    if ( bb.x <= v[i].x &&
	 bb.x+bb.width >= v[i].x + v[i].width &&
	 bb.y < v[i].y &&
	 bb.y + bb.heigth >= v[i].y + v[i].heigth){
      inside.push_back(bb[i]);
    }    
  }
  return inside;
}

bool overlap(Rect r1, Rect r2){
  vector<int> a = rect2vec(r1);
  vector<int> b = rect2vec(r2);
  return a[1] >= b[0] &&  a[0] <= b[1];
}

//IMPROVEMENT
//do binary search instead of linear search
int index_below(vector<Rect> rv, Rect r){
  vector<int> a = rect2vec(r);
  for (int i = 0; i < rv.size(); i++){
    if (rv[i].y <= a[3] && rv[i].br().y >= a[4])
      return i;
  }
  return -1;
}

//IMPROVEMENT
//Inneficient to use vector here. Should use list or other structure
vector<int> split(vector<Rect> tb, vector<Rect> rows Rect bb){
  vector<Rect> ibb = inside(tb, bb);
  for (int i = 0; i < tb.size(); i++){
    int index = index_below(tb[i]);
    Rect r = vec2rect(vector<int>{tb[i].x, tb[i].x+tb[i].width, rows[index], rows[index].br().y});    
    vector<Rect> tbb = inside(tb, r);
    int count = 0;
    for (Rect e : tbb){
      if (overlap(tb[i], e))
	count++;      
    }
    if (count > 1){
      //TODO
      //Remove boxes with too much overlap
    }
  }
}

//IMPROVEMENT
//Remove outliers
//Strip newlines from string here or elsewhere
vector<vector<string> > find_grid(vector<Rect> bb, vector<string> text){
  auto p = sort_permutation(bb, [](Rect const& a, Rect const& b){
      if (a.y == b.y)
	return a.x < b.x;
      return a.y < b.y;
    });
  vector<string> sts = apply_permutation(text, p);  
  vector<Rect> stb = apply_permutation(bb, p);
  sort(bb.begin(), bb.end(), rectSortX());
  vector<Rect> cols = overlapping(bb, 0);
  sort(bb.begin(), bb.end(), rectSortY());
  vector<Rect> rows = overlapping(bb, 2);
  vector<vector<string> > table(cols.size(), vector<string>(rows.size(),""));
  vector<int> xvec(cols.size());
  for (int i = 0; i < cols.size(); i++){
    xvec[i] = cols[i].tl().x;
  }
  vector<int> yvec(rows.size());
  for (int i = 0; i < rows.size(); i++){
    yvec[i] = rows[i].tl().y;
  }
  split();
  for (int i = 0; i < sts.size(); i++){    
    int x_index = bin_search(xvec, stb[i].tl().x);
    int y_index = bin_search(yvec,stb[i].tl().y);
    table[x_index][y_index] += sts[i];
  }
  return table;
} 
