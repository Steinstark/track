#include <algorithm>
#include <opencv2/opencv.hpp>
#include <set>
#include <iostream>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include "RTree.h"

using namespace std;
using namespace cv;
using namespace boost::multi_index;

struct AreaNode{
  int a, w, h, index;
  AreaNode(int a, int w, int h, int index): a(a), w(w), h(h), index(index){}
  int operator[](size_t id){
    if (id == 0) return a;
    if (id == 1) return w;
    if (id == 2) return h;
    if (id == 3) return index;
    return -1;
  }
};

using NodeDB = multi_index_container<
  AreaNode,
  indexed_by<
    ordered_non_unique<member<AreaNode, int, &AreaNode::a>, greater<int> >,
    ordered_non_unique<member<AreaNode, int, &AreaNode::w>, greater<int> >,
    ordered_non_unique<member<AreaNode, int, &AreaNode::h>, greater<int> >,
    ordered_unique<member<AreaNode, int, &AreaNode::index> >
    >
  >;

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

vector<int> minimum_median_filter(NodeDB nodes){
  return vector<int>();
}

double k_calc(double mean, double med){
  return max(mean/med, med/mean);  
}

vector<int> classify_elements(const vector<int>& se){
  return vector<int>();
}

bool isSuspected(NodeDB& nodes,
			const vector<double>& means,
			const vector<double>& medians){
  int k[3];
  for (int i = 0; i < 3; i++){
    k[i] = k_calc(means[i], medians[i]);		    
  }  
  auto n1 = *nodes.get<0>().begin();
  if (n1.a > k[0]*medians[0]){
    auto n2 = *nodes.get<1>().begin();      
    if (n1.index  == n2.index && n2.w > k[1]*medians[1])
      return true;
    n2 = *nodes.get<2>().begin();
    if (n1.index == n2.index && n2.h > k[2]*medians[2])
      return true;
  }    
  return false;
}

//TODO
//implement median for a NodeDB
vector<double> getMedians(NodeDB nodes){
  vector<double>();
}

//TODO
//implement mean for a NodeDB
vector<double> getMeans(NodeDB nodes){
  return vector<double>();
}

vector<int> suspected_elements(NodeDB& nodes){
  vector<int> v;
  while (1){
    vector<double> medians = getMedians(nodes);
    vector<double> means = getMeans(nodes);
    bool suspect = isSuspected(nodes, means, medians);
    if (!suspect)
      break;
    auto& p = nodes.get<0>();
    v.push_back(p.begin()->index);
    p.erase(p.begin());
  }
  return v;
}

vector<int> recursive_filter(const Mat& img, const Mat& stats){
  int labels = stats.rows;
  RTree<int, int, 2, float> tree;
  vector<Rect> bb(labels);
  NodeDB nodes;
  for (int i = 1; i < labels; i++){
    int left = stats.at<int>(i, CC_STAT_LEFT);
    int top = stats.at<int>(i, CC_STAT_TOP);
    int width = stats.at<int>(i, CC_STAT_WIDTH);
    int height = stats.at<int>(i, CC_STAT_HEIGHT);
    int area = stats.at<int>(i, CC_STAT_AREA);
    bb[i] = Rect(left, top, width, height);
    nodes.insert({area, width, height, i});
    insert2tree(tree, bb[i], i);
  }
  vector<int> se = suspected_elements(nodes);
  vector<int> ce = classify_elements(se);
  vector<int> me = minimum_median_filter(nodes);
  
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
