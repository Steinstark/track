#include <algorithm>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <iterator>

#include <opencv2/opencv.hpp>
#include "find_grid.hpp"

using namespace std;
using namespace cv;

using mapiv = map<int, vector<int> >;

bool compareRect(const Rect& a, const Rect& b){
  return a.x <= b.br().x && b.x <= a.br().x;
}

template<typename Map> typename Map::const_iterator 
greatest_less(Map const& m, typename Map::key_type const& k) {
  typename Map::const_iterator it = m.lower_bound(k);    
  if(it == m.begin()) {
    return it;
  }
  return --it;
}

template<typename Map> typename Map::iterator 
greatest_less(Map & m, typename Map::key_type const& k) {
  typename Map::iterator it = m.lower_bound(k);
  if(it == m.begin()) {
    return it;
  }
  return --it;
  }

//Take a function as input to get width or heigth
mapiv overlapping(multimap<int, int>& tree, vector<Rect>& boundingBoxes, function<int(Rect)> val){
  mapiv overlap;
  int left = tree.begin()->first;
  int right = left;
  vector<int> a0;
  for (auto p : tree){
    int ll = p.first;
    int lr = p.first + val(boundingBoxes[p.second]);
    if (ll <= right && lr >= left){
      left = min(left, ll);
      right = max(right, lr);
      a0.push_back(p.second);
    }else{
      sort(a0.begin(), a0.end());
      overlap.insert({left, a0});
      a0.clear();
      a0.push_back(p.second);
      left = ll;
      right = lr;
    }
  }
  if (overlap.lower_bound(left) == overlap.end()){
    sort(a0.begin(), a0.end());
    overlap.insert({left, a0});
  }
  return overlap;
}

void split(mapiv& cols,mapiv& rows, vector<Rect>& boundingBoxes, vector<string>& text){
  int length = boundingBoxes.size();
  for (int i = 0; i < length; i++){
    Rect r = boundingBoxes[i];
    Point tl = r.tl();
    Point br = r.br();
    auto itlow_y = rows.lower_bound(br.y+1);
    if (itlow_y == rows.end())
      continue;
    auto itlow_x = greatest_less(cols, tl.x);
    vector<int> intersect;
    set_intersection(itlow_y->second.begin(), itlow_y->second.end(),
		     itlow_x->second.begin(), itlow_x->second.end(),
		     back_inserter(intersect));
    vector<int> overlap_index;
    for (int j= 0; j < intersect.size(); j++){      
      if (compareRect(r, boundingBoxes[intersect[j]])){
	overlap_index.push_back(intersect[j]);
      }
    }
    if (overlap_index.size() > 1){
      Rect a0 = boundingBoxes[overlap_index[0]];
      boundingBoxes[i] = Rect(a0.x,r.y, a0.width,r.height);
      for (int j = 1; j < overlap_index.size(); j++){
	a0 = boundingBoxes[overlap_index[j]];	
	boundingBoxes.push_back(Rect(a0.x, r.y, a0.width, r.height));
	text.push_back(text[i]);
      }
    }
  }
}

//IMPROVEMENT
//detect headers ahead of the overlap
vector<vector<string> > find_grid(vector<Rect> boundingBoxes,
				  vector<string> text)
{
  multimap<int, int> xtree;
  multimap<int, int> ytree;
  for (int i = 0; i < boundingBoxes.size(); i++){
    Rect r = boundingBoxes[i];
    xtree.insert({r.x, i});
    ytree.insert({r.y, i});
  }
  mapiv cols = overlapping(xtree, boundingBoxes, [](const Rect& r){return r.width;});
  mapiv rows = overlapping(ytree, boundingBoxes, [](const Rect& r){return r.height;});  
  split(cols, rows, boundingBoxes, text);
  xtree.clear();
  ytree.clear();
  for (int i = 0; i < boundingBoxes.size(); i++){
    Rect r = boundingBoxes[i];
    xtree.insert({r.x, i});
    ytree.insert({r.y, i});
  }
  cols = overlapping(xtree, boundingBoxes, [](const Rect& r){return r.width;});
  rows = overlapping(ytree, boundingBoxes, [](const Rect& r){return r.height;});  
  vector<vector<string> > table(cols.size(), vector<string>(rows.size(),""));  
  int i = 0, j;
  for (auto c : cols){
    j = 0;
    for (auto r: rows){
      vector<int> intersect;
      set_intersection(c.second.begin(), c.second.end(),
		       r.second.begin(), r.second.end(), back_inserter(intersect));
      for (int k = 0; k < intersect.size(); k++){
	table[i][j] += text[intersect[k]];
      }
      j++;
    }    
    i++;
  }  
  return table;
}
