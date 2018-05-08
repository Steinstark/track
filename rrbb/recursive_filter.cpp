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
#include <boost/multi_index/ranked_index.hpp>
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

struct Area{};
struct Width{};
struct Height{};
struct Index{};

using NodeDB = multi_index_container<
  ComponentStats,
  indexed_by<
    ranked_non_unique<tag<Area>, member<ComponentStats, int, &ComponentStats::area>, greater<int> >,
    ranked_non_unique<tag<Width>, global_fun<const ComponentStats&, int, &widthExtractor>, greater<int> >,
    ranked_non_unique<tag<Height>, global_fun<const ComponentStats&, int, &heightExtractor>, greater<int> >,
    ordered_unique<tag<Index>, member<ComponentStats, int, &ComponentStats::index> >
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
  int mid = nodes.size()*1/2;
  median[0] = nodes.get<Area>().nth(mid)->r.area();
  median[1] = nodes.get<Width>().nth(mid)->r.width;
  median[2] = nodes.get<Height>().nth(mid)->r.height;
  return median;
}

vector<double> getMeans(NodeDB& nodes){
  auto& iv = nodes.get<Index>();
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

list<int> classifyAll(const NodeDB& nodes, const set<int>& suspects){
  bgi::rtree<ComponentStats, bgi::quadratic<16> > tree(nodes);
  double mean = 0;
  double var = welford(nodes.begin(),
		       nodes.end(),
		       [&tree](const ComponentStats& cs){return minDist(tree, cs.r);},
		       mean);
  list<int> confirmed;
  for (int e : suspects){
    Rect r = nodes.get<Index>().find(e)->r;
    double dist = sqrt(minDist(tree, r));
    if (dist > 2*var && dist > 4*mean){
      confirmed.push_back(e);
    }   
  }
  return confirmed;
}

bool median_filter(Rect& r, function<bool(int, int)> f){
  return f(r.area(), 0) && (f(r.width, 1) || f(r.height, 2));
}

bool max_median(Rect& r, DataBox& box){
  return median_filter(r, [&box](int l, int i){return l > box.medians[i]*box.k[i];});
}

bool min_median(Rect& r, DataBox& box){
  return median_filter(r, [&box](int l, int i){return l < box.medians[i]/box.k[i];});
}

template <typename Iterator, typename Fun>
void general_filter(Iterator first, Iterator last, set<int>& suspects, Fun filter){
  while (first != last){
    if (!filter(first->r)){
      break;
    }
    suspects.insert(first++->index);
  }
}

list<int> nontextElements(NodeDB& nodes){
  DataBox box(nodes);
  set<int> suspects;
  general_filter(nodes.get<Area>().begin(),
                 nodes.get<Area>().end(),
                 suspects,
                 [&box](Rect r){return max_median(r, box);});
  general_filter(nodes.get<Area>().rbegin(),
                 nodes.get<Area>().rend(),
                 suspects,
                 [&box](Rect r){return min_median(r, box);});
  return classifyAll(nodes, suspects);
}

bool recursive_filter(Mat& text, Mat& nontext){
  NodeDB nodes;
  Mat local;
  dilate(text, local, Mat());
  //DEBUG
  textDebug = local;
  Mat cc;
  statistics(local, cc, inserter(nodes, nodes.begin()));
  if (nodes.empty())
    return false;
  int count = nodes.size();
  list<int> toRemove = nontextElements(nodes);
  for (int e : toRemove){
    move2(text, nontext, cc, e);
  }
  return toRemove.size() > 0 && toRemove.size() != count;
}
