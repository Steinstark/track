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

using RT = RTree<int, int, 2, float>;

struct SpaceNode{
  int index, lc, rc;
  vector<int> lnv,rnv;  
  size_t left_num(){
    return lnv.size();
  }
  size_t right_num(){
    return rnv.size();
  }
  SpaceNode(){
    index = 0;
    lc = 0;
    rc = 0;
  }
};

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
    ordered_unique<member<AreaNode, int, &AreaNode::index> >,
    ordered_non_unique<member<AreaNode, int, &AreaNode::w>, less<int> >,
    ordered_non_unique<member<AreaNode, int, &AreaNode::h>, less<int> >
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


vector<double> k_calc(vector<double> means, vector<double> medians){
  vector<double> k(3);
  for (int i = 0; i < 3; i++){
    k.push_back(max(means[i]/medians[i], medians[i]/means[i]));
  }
  return k;
}

//TODO
//implement
int getMax(const vector<int>&v){
  return 0;
}

//TODO
//implement
vector<int> getSpace(const vector<SpaceNode>& v){
  return vector<int>();
}

//TODO
//implement
vector<int> getRightCount(const vector<SpaceNode>& v){
  return vector<int>();
}

//TODO
//implement
vector<int> getLeftCount(const vector<SpaceNode>& v){
  return vector<int>();
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

//TODO
//implement mean for a NodeDB
double getMean(vector<int> v){
  return 0;
}

//TODO
//implement mean for a NodeDB
double getMedian(vector<int> v){
  return 0;
}

SpaceNode create_space(RT& tree, const Rect& r){
  return SpaceNode();
}

vector<int> classify_elements(RT& tree, const vector<int>& se, vector<Rect> bb){
  vector<SpaceNode> space;
  for (int i = 0; i < bb.size(); i++){
    SpaceNode sn= create_space(tree, bb[i]);
    space.push_back(sn);
  }
  vector<int> ws = getSpace(space);
  vector<int> nlnv = getLeftCount(space);
  vector<int> nrnv = getRightCount(space);
  double median = getMedian(ws);
  double mean = getMean(ws);
  vector<int> v;
  for (int i = 0; i < se.size(); i++){
    if (min(space[i].lc, space[i].rc) > max(median, mean) &&
	(max(space[i].lc, space[i].rc) == getMax(ws) ||
	 min(space[i].lc, space[i].rc) > 2*mean))
      v.push_back(se[i]);
    else if ((space[i].lnv.size() == getMax(nlnv) && space[i].lnv.size() > 2) ||
	     space[i].rnv.size() == getMax(nrnv) && space[i].rnv.size() > 2)
      v.push_back(se[i]);    
  }
  return v;
}

bool isSuspected(NodeDB& nodes, const vector<double>& medians, const vector<double> k){
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

vector<int> suspected_elements(NodeDB& nodes){
  vector<int> v;
  while (1){
    vector<double> medians = getMedians(nodes);
    vector<double> means = getMeans(nodes);
    vector<double> k = k_calc(means, medians);
    bool suspect = isSuspected(nodes, medians, k);
    if (!suspect)
      break;
    auto& p = nodes.get<0>();
    v.push_back(p.begin()->index);
    p.erase(p.begin());
  }
  return v;
}

vector<int> minimum_median_filter(NodeDB& nodes){
  vector<int> v;
  while (1){
    vector<double> medians = getMedians(nodes);
    vector<double> means = getMeans(nodes);
    vector<double> k = k_calc(means, medians);
    auto& ws = nodes.get<4>();
    auto& hs = nodes.get<5>();
    auto w = ws.begin();
    auto h = hs.begin();
    if (w->w < medians[1]/k[1]){
      v.push_back(w->index);
      ws.erase(w);   
    }else if (h->h < medians[2]/k[2]){
      v.push_back(h->index);
      hs.erase(h);
    }
    else
      break;
  }
  return v;
}

vector<int> recursive_filter(const Mat& img, const Mat& stats){
  int labels = stats.rows;
  RT tree;
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
  vector<int> ce = classify_elements(tree, se, bb);
  vector<int> me = minimum_median_filter(nodes);
  return vector<int>();
}
 

int main(){
  
}
