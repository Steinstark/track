#include <algorithm>
#include <opencv2/opencv.hpp>
#include <set>
#include "RTree.h"

using namespace std;
using namespace cv;

struct AreaNode{
  int a, w, h, index;
  AreaNode(int a, int w, int h, int index): a(a), w(w), h(h), index(index){}
  bool operator<(AreaNode other) const{
    return a > other.a;
  }
};

bool callback(int id, void* arg){
  vector<int>* v = static_cast<vector<int>*>( arg );
  v->push_back(id);
  return true;
}

void insert2tree(RTree<int, int, 2, float>& tree, const Rect& r, int i){
  int tl[] = {r.x, r.y};
  int br[] = {r.br().x, r.br().y};
  tree.Insert(tl, br, i);
}

int search_tree(RTree<int, int, 2, float>& tree, Rect r, vector<int>& vec){
  int tl[] = {r.x, r.y};
  int br[] = {r.br().x, r.br().y};
  return tree.Search(tl, br, callback,(static_cast<void*>(&vec)));
}

double median(const vector<int>& v){
  int n = v.size();
  if (n % 2)
    return v[n/2];
  return (v[n/2-1] + v[n/2])/2;   
}

double mean(const vector<int>& v){
  int m = 0; 
  for (int i = 0; i < v.size(); i++){
    m += v[i];
  }
  return m;
}

vector<int> minimum_median_filter(){
  return vector<int>();
}

double k_calc(double mean, double med){
  return max(mean/med, med/mean);  
}

vector<int> classify_elements(const vector<int>& se){
  return vector<int>();
}

vector<int> suspected_elements(const multiset<AreaNode>& area_set,
			       const multiset<int>& width_set,
			       const multiset<int>& height_set){
  vector<int>  v;
  while (){
    auto it = area_set.begin();
    int area = it->area;
    int width = it->width;
    int height = it->height;
    int index = it->index;
    int wm = width_set.begin();
    int hm = height_set.begin();
    double meana, meanw, meanh;
    double meda, medw, medh;    
    double k1 = k_calc(meana, meda);
    double k2 = k_calc(meanw, medw);
    double k3 = k_calc(meanh, medh);
    if (a > k1*meda &&
	((width == wm && width > k2*medw) ||
	 (height == hm && height > k3*medh))){       
      area_set.erase(it);
      width_set.erase(width_set.find(width));
      height_set.erase(height_set.find(height));
      v.push_back(index);
    }else
      break;
  }
  return v;
}

void recursive_filter(const Mat& img){
  const int inf = 1000000;
  Mat cc, stats, centroids;
  int labels = connectedComponentsWithStats(img, cc, stats, centroids, 8, CV_32S);
  RTree<int, int, 2, float> tree;
  vector<Rect> bb(labels);
  vector<int> areas(labels);
  for (int i = 1; i < labels; i++){
    int left = stats.at<int>(i, CC_STAT_LEFT);
    int top = stats.at<int>(i, CC_STAT_TOP);
    int width = stats.at<int>(i, CC_STAT_WIDTH);
    int height = stats.at<int>(i, CC_STAT_HEIGHT);
       bb[i] = Rect(left, top, width, height);
    areas[i] = stats.at<int>(i, CC_STAT_AREA);
    insert2tree(tree, bb[i], i);
  }
  multiset<AreaNode> area_set;
  multiset<int> width_set;
  multiset<int> height_set;
  for (int i = 0; i < bb.size(); i++){
    area_set.insert(AreaNode(areas[i], bb[i].width, bb[i].height, i));
    width_set.insert(bb[i].width);
    height_set.insert(bb[i].height);    
  }
  vector<int> se = suspected_elements(area_set, width_set, height_set);
  vector<int> ce = classify_elements(se);
  vector<int> me = minimum_median_filter();

 
  /*  for (int i = 1; i < labels; i++){
    vector<int> ln;
    Rect lr(-inf, bb[i].y, bb[i].x, bb[i].height);
    search_tree(tree, lr, ln);
    vector<int> rn;q
    Rect rr(bb[i].br().x, bb[i].y, inf, bb[i].height);
    search_tree(tree, rr, rn);
    sort(ln.begin(), ln.end(), [&bb](int a, int b){return bb[a].br().x > bb[b].br().x;});
    sort(rn.begin(), rn.end(), [&bb](int a, int b){return bb[a].x  < bb[b].x;});   
    }*/
  
}


int main(){
  
}
