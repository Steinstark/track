#include <algorithm>
#include "RTree.h"

using namespace std;
using namespace cv;

bool callback(int id, void* arg){
  vector<int>* v = static_cast<vector<int>*>( arg );
  v->push_back(id);
  return true;
}

void insert2tree(RTree& tree, const Rect& r, int i){
  int tl[] = {r.x, r.y};
  int br[] = {r.br().x, r.br().y};
  tree.Insert(tl, br, i);
}

int search_tree(const RTree& tree, Rect r, vector<int>& vec){
  int tl[] = {r.x, r.y};
  int br[] = {r.br().x, r.br().y};
  return tree.search(tl, br, callback,(static_cast<void*>(&vec)));
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


double k_calc(double mean, double med){
  return max(mean/med, med/mean);  
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
  Area
  sort(
  
  for (int i = 1; i < labels; i++){
    vector<int> ln;
    Rect lr(-inf, bb[i].y, bb[i].x, bb[i].height);
    search_tree(tree, lr, ln);
    vector<int> rn;
    Rect rr(bb[i].br().x, bb[i].y, inf, bb[i].height);
    search_tree(tree, rr, rn);
    sort(ln.begin(), ln.end(), [&bb](int a, int b){return bb[a].br().x > bb[b].br().x;});
    sort(rn.begin(), rn.end(), [&bb](int a, int b){return bb[a].x  < bb[b].x;});
    
    
  }

  
}
