#include "recursive_filter.hpp"
#include <algorithm>
#include <cmath>
#include <functional>
#include <list>
#include <utility>
#include <map>
#include <opencv2/opencv.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/global_fun.hpp>
#include "util.hpp"
#include "utility.hpp"
#include "image_util.hpp"

#include "debug_header.hpp"

using namespace std;
using namespace cv;
using namespace boost::multi_index;
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

using IntPair = pair<int, int>;
using DoublePair = pair<double, double>;

int widthExtractor(const ComponentStats& cs){
  return cs.r.width;
}

int heightExtractor(const ComponentStats& cs){
  return cs.r.height;
}

using NodeDB = multi_index_container<
  ComponentStats,
  indexed_by<
    ordered_non_unique<member<ComponentStats, int, &ComponentStats::area>, greater<int> >,
    ordered_non_unique<global_fun<const ComponentStats&, int, &widthExtractor>, greater<int> >,
    ordered_non_unique<global_fun<const ComponentStats&, int, &heightExtractor>, greater<int> >,
    ordered_unique<member<ComponentStats, int, &ComponentStats::index> >
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
  median[0] += ita->area;
  median[1] += itw->r.width;
  median[2] += ith->r.height;
  return median;
}

vector<double> getMeans(NodeDB& nodes){
  auto& iv = nodes.get<3>();
  vector<double> mean(3,0);
  auto it = iv.begin();
  for (auto it = iv.begin(); it != iv.end(); ++it){
    mean[0] += it->area;
    mean[1] += it->r.width;
    mean[2] += it->r.height;
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

double minDist(bgi::rtree<ComponentStats, bgi::quadratic<16> >& tree, const Rect& r){
  auto it1 = tree.qbegin(bgi::nearest(r, 1) && !bgi::covered_by(r));
  if (it1 != tree.qend()){
    return bg::comparable_distance(r, it1->r);
  }
  return 0;
}

list<DoublePair> getKBest(set<double>& distances, int k = 2){
  OnlineStat stat;
  multimap<int, DoublePair> best;
  DoublePair p(0,0);
  int count = 0;
  for (int e : distances){
    double var =stat.welfordStep(e);
    if (var > 2){
      best.insert(pair<int, DoublePair>(count, p));
      stat.reset();
      count = 0;
      var = stat.welfordStep(e);
    }
    p.first = stat.getMean();
    p.second = var;
    count++;
  }
  best.insert(pair<int, DoublePair>(count, p));
  auto it = best.rbegin();
  int c = 0;
  list<DoublePair> ret;
  while (it != best.rend() && c < k){
    ret.push_back(it->second);
    c++;
  }
  return ret;
}

void classifyAll(NodeDB& nodes, list<int>& suspects){
  bgi::rtree<ComponentStats, bgi::quadratic<16> > tree(nodes);
  set<double> distances;
  for (auto it = nodes.begin(); it != nodes.end(); it++){
    double dist = minDist(tree, it->r);
    distances.insert(dist);
  }
  list<DoublePair> best = getKBest(distances);  
  for (auto it = suspects.begin(); it != suspects.end();){
    Rect r = nodes.get<3>().find(*it)->r;
    double dist = minDist(tree, r);
    bool removed = false;
    for (DoublePair p : best){
      if (gaussWeight(dist, p.first, p.second)){
	it = suspects.erase(it);
	removed = true;
	break;
      }
    }
    if (!removed){
      //DEBUG
      Mat regionDebug = textDebug(r);
      it++;
    }
  }
}

bool median_filter(Rect& r, function<bool(int, int)> f){
  return f(r.area(), 0) && (f(r.width, 1) || f(r.height, 2));
}

void maximum_median_filter(NodeDB& nodes, DataBox& box, list<int>& suspects){  
  auto& as = nodes.get<0>();
  auto& ws = nodes.get<1>();
  auto& hs = nodes.get<2>();  
  while (nodes.size()){
    auto ai = as.begin();
    Rect r = ai->r;
    if (!median_filter(r, [&box](int l, int i){return l > box.medians[i]*box.k[i];})){
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
    Rect r = ai->r;
    if (!median_filter(r, [&box](int l, int i){return l < box.medians[i]/box.k[i];})){
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
  
  //DEBUG
  textDebug = text;
  nontextDebug = nontext;
  Mat cc;
  statistics(text, cc, inserter(nodes, nodes.begin()));
  int count = nodes.size();
  list<int> toRemove = nontextElements(nodes);
  for (int e : toRemove){
    move2(text, nontext, cc, e);
  }
  return toRemove.size() > 0 && toRemove.size() != count;
}
