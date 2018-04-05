#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <utility>
#include <opencv2/opencv.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include "tree_helper.hpp"
#include "recursive_filter.hpp"
#include "utility.hpp"
#include "util.hpp"
#include "image_util.hpp"

#include "debug_header.hpp"

using namespace std;
using namespace cv;
using namespace boost::multi_index;
using namespace tree;

using IntPair = pair<int, int>;

struct AreaNode{
  int a, w, h, index;
  Rect r;
  AreaNode(int a, int w, int h, int index, Rect r): a(a), w(w), h(h), index(index), r(r){}
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

vector<double> k_calc(const vector<double>& means, const vector<double>& medians){
  vector<double> k(3);
  for (int i = 0; i < 3; i++){
    k[i] = max(means[i]/medians[i], medians[i]/means[i]);
  }
  return k;
}

//Technically not median for even length, but doesnt really matter
vector<double> getMedians(NodeDB& nodes){
  vector<double> median(3,0);
  auto& av = nodes.get<0>();
  auto& wv = nodes.get<1>();
  auto& hv = nodes.get<2>();
  size_t n = nodes.size();
  int a = n/2;
  auto ita = av.begin();
  auto itw = wv.begin();
  auto ith = hv.begin();
  advance(ita,a);
  advance(itw,a);
  advance(ith,a);
  median[0] += ita->a;
  median[1] += itw->w;
  median[2] += ith->h;
  return median;
}

vector<double> getMeans(NodeDB& nodes){
  auto& iv = nodes.get<3>();
  vector<double> mean(3,0);
  auto it = iv.begin();
  for (auto it = iv.begin(); it != iv.end(); ++it){
    mean[0] += it->a;
    mean[1] += it->w;
    mean[2] += it->h;
  }
  for (int i = 0; i < 3; i++){
    mean[i] /= nodes.size();
  }
  return mean;
}

struct DataBox{
  vector<double> means, medians, k;
  DataBox(NodeDB& nodes): means(getMeans(nodes)), medians(getMedians(nodes)){
    k = k_calc(means, medians);
  }
};

bool classify(int minSpace, double medws, double meanws){
  return minSpace > max(medws, meanws) &&  minSpace > 3*meanws;
}

int minDist(RT& tree, const Rect& r){
  int inf = 1000000;
  vector<Rect> left = closestBox(tree, r.tl(), Rect(0,r.y, r.x-1, r.height));
  vector<Rect> right = closestBox(tree, r.br(), Rect(r.br().x+1, r.y, inf, r.height));
  vector<int> dist;
  if (left.size())
    dist.push_back(min(abs(r.x - left[0].br().x)+1, abs(left[0].x-r.br().x)+1));
  if (right.size())
    dist.push_back(min(abs(right[0].x - r.br().x) + 1, abs(r.x-right[0].br().x)+1));
  if (dist.empty())
    return 0;
  int closest = inf;

  //DEBUG
  Mat region = textDebug(r);
  
  for (int e : dist){    
    if (e < closest){
      closest = e;
    }
  }
  return closest;
}

void classifyAll(NodeDB& nodes, list<int>& suspects){
  RT tree;
  for (auto it = nodes.begin(); it != nodes.end(); it++){
    insert2tree(tree, it->r, it->index);
  }
  vector<int> distances;
  for (auto it = nodes.begin(); it != nodes.end(); it++){
    int dist = minDist(tree, it->r);
    if (dist){
      distances.push_back(dist);
    }      
  }
  double medWS = binapprox<int, double>(distances, [](int v){return v;});  
  double meanWS = mean<int, int>(distances, [](int v){return v;});    
  for (auto it = suspects.begin(); it != suspects.end();){
    Rect r = nodes.get<3>().find(*it)->r;
    int dist = minDist(tree, r);
    if (!classify(dist, medWS, meanWS)){
      it = suspects.erase(it);
    }else{
      //DEBUG
      Mat regionDebug = textDebug(r);
      it++;
    }
  }
}

bool median_filter(const AreaNode& r, const AreaNode& extreme, function<bool(int, int)> f){
  return r.a == extreme.a && f(r.a, 0) && ((r.w == extreme.w && f(r.w, 1)) || (r.h == extreme.h && f(r.h, 2)));
}

void maximum_median_filter(NodeDB& nodes, DataBox& box, list<int>& suspects){  
  auto& as = nodes.get<0>();
  auto& ws = nodes.get<1>();
  auto& hs = nodes.get<2>();  
  while (nodes.size()){
    auto ai = as.begin();
    AreaNode extreme(ai->a, ws.begin()->w, hs.begin()->h, 0, Rect());
    if (!median_filter(*ai, extreme, [&box](int l, int i){return l > box.medians[i]*box.k[i];})){
      break;
    }
    suspects.push_back(ai->index);
    as.erase(ai);
  }
}

void minimum_median_filter(NodeDB& nodes, DataBox& box, list<int>& suspects){
  auto& as = nodes.get<0>();
  auto& ws = nodes.get<1>();
  auto& hs = nodes.get<2>();
  auto ai = as.rbegin();
  while (ai != as.rend()){
    AreaNode extreme(ai->a, ws.rbegin()->w, hs.rbegin()->h, 0, Rect());
    if (!median_filter(*ai, extreme, [&box](int l, int i){return l < box.medians[i]/box.k[i];})){
      break;
    }
    suspects.push_back(ai->index);
    auto tmp1 = ai.base();
    auto tmp2 = --tmp1;
    ai = NodeDB::reverse_iterator{nodes.erase(tmp2)};
  }
}

list<int> nontextElements(NodeDB& nodes){
  DataBox box(nodes);
  list<int> suspects;
  NodeDB local = nodes;
  minimum_median_filter(local, box, suspects);
  maximum_median_filter(local, box, suspects);  
  classifyAll(nodes, suspects);
  return suspects;
}

bool recursive_filter(Mat& text, Mat& nontext){
  NodeDB nodes;
  Mat cc, stats, centroids;

  //DEBUG
  textDebug = text;
  nontextDebug = nontext;
  
  int labels = connectedComponentsWithStats(text, cc, stats, centroids, 8, CV_32S);
  for (int i = 1; i < labels; i++){
    Rect r = stats2rect(stats, i);
    nodes.insert(AreaNode(r.area(), r.width, r.height, i, r));
  }
  list<int> toRemove = nontextElements(nodes);
  for (int e : toRemove){
    move2(text, nontext, cc, e);
  }
  return toRemove.size() > 0 && toRemove.size() != stats.rows-1;
}
