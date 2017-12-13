#include <algorithm>
#include <opencv2/opencv.hpp>
#include <set>
#include <iostream>
#include <iterator>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include "RTree.h"
#include "recursive_filter.hpp"

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

int search_tree(RT& tree, Rect r, vector<int>& vec){
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

int getMax(const vector<int>&v){
  int maxv = -1;
  for (int i = 0; i < v.size(); i++){
    if (v[i] > maxv)
      maxv = v[i];
  }
  return maxv;
}

vector<int> getSpace(const vector<SpaceNode>& space){
  vector<int> v(space.size());
  for (int i  = 0; i < space.size(); i++){
    v.push_back(space[i].rc);
  }
  return v;
}

vector<int> getRightCount(const vector<SpaceNode>& space){
  vector<int> v(v.size());
  for (int i = 0; i  < space.size(); i++){
    v.push_back(space[i].rnv.size());
  }
  return v;
}

vector<int> getLeftCount(const vector<SpaceNode>& space){
  vector<int> v(v.size());
  for (int i = 0; i  < space.size(); i++){
    v.push_back(space[i].lnv.size());
  }
  return v;
}

//TODO
//implement median for a NodeDB
vector<double> getMedians(NodeDB& nodes){
  vector<double> median(3,0);
  auto& av = nodes.get<0>();
  auto& wv = nodes.get<1>();
  auto& hv = nodes.get<2>();
  size_t n = av.size();
  int a = n/2;
  int b = !(n % 2);  
  auto ita = av.begin();
  auto itw = wv.begin();
  auto ith = hv.begin();
  advance(ita,a);
  advance(itw,a);
  advance(ith,a);
  median[0] += ita->a;
  advance(ita, b);
  median[0] += ita->a;
  median[1] += itw->w;
  advance(itw, b);
  median[1] += itw->w;  
  median[2] += ith->h;
  advance(ith, b);
  median[2] += ith->h;   
  for (int i = 0; i < 3; i++){
    median[i] = median[i]/2;
  }
  return median;
}

//TODO
//implement mean for a NodeDB
vector<double> getMeans(NodeDB& nodes){
  auto& iv = nodes.get<3>();
  vector<double> mean(3,0);
  int size = 0;
  auto it = iv.begin();
  for (auto it = iv.begin(); it != iv.end(); ++it){
    mean[0] += it->a;
    mean[1] += it->w;
    mean[2] += it->h;
    size++;
  }
  for (int i = 0; i < 3; i++){
    mean[i]/size;
  }
  return mean;
}

double getMean(vector<int> v){
  double mean = 0;
  for (int i = 0; i < v.size(); i++){
    mean += v[i];
  }
  return mean/v.size();
}

double getMedian(vector<int> v){
  int n = v.size();
  if (n % 2){
    return v[n/2];
  }
  return (v[n/2] + v[n/2-1])/2;
}

//IMPROVEMENT
//nonlinear search for closest neighbour
SpaceNode create_space(RT& tree, const vector<Rect>& bb, int i){
  int inf = 1000000;
  SpaceNode sn;
  sn.index = i;
  Rect r = bb[i];
  Rect leftRect(0, r.y, r.x, r.height);
  Rect rightRect(r.br().x, r.y, inf, r.height);
  search_tree(tree, leftRect, sn.lnv);
  search_tree(tree, rightRect, sn.rnv);
  int minws = inf;
  for (int j = 0; j < sn.lnv.size(); j++){
    int ind = sn.lnv[j];
    int val = r.x-bb[ind].br().x;
    if (val > 0 && val < minws)
      minws = val;
  }
  sn.lc = minws;
  minws = inf;
  for (int j = 0; j < sn.lnv.size(); j++){
    int ind = sn.rnv[j];
    int val = bb[ind].x-r.br().x;
    if (val > 0 && val < minws)
      minws = val;
  }
  sn.rc = minws;
  return sn;
}

vector<int> classify_elements(RT& tree, const vector<int>& se, vector<Rect> bb){
  vector<SpaceNode> space;
  for (int i = 0; i < bb.size(); i++){
    SpaceNode sn = create_space(tree, bb, i);
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
  vector<int> v;
  v.insert(v.end(), ce.begin(), ce.end());
  v.insert(v.end(), me.begin(), me.end());
  return v;
}
