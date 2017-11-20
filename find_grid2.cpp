#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <multimap>
#include <opencv2/opencv.hpp>
#include "find_grid.hpp"

using namespace std;
using namespace cv;

using map<int, vector<int> > = mapiv;

//Take a function as input to get width or heigth
mapiv overlapping(map<int, int>& tree, vector<Rect>& boundingBoxes, function<int(Rect)> val){
  map<int, int> overlap;
  int left = right = tree.begin().first;
  vector<int> a0;
  for (auto p : tree){
    int ll = p.first;
    int lr = p.first + val(boundingBoxes[p.second]);
    if (ll <= right && lr >= left){
      left = min(left, ll);
      right = max(right, lr);
      a0.push_back(p.second);
    }else{
      sort(a0.begin(), a0.end())
      overlap.insert({left, a0});
      a0.clear();
      left = ll;
      right = lr;
    }
  }
  if (overlap.lower_bound(left) == overlap.end()){
    sort(a0.begin(), a0.end());
    overap.insert({left, a0});
  }
  return overlap;
}

vector<Rect> split(mapiv& cols,mapiv& rows, vector<Rect> boundingBoxes){
  for (int i = 0; i < boundingBoxes.size(); i++){
    Rect r = boundingBoxes[i];
    Point tl = r.tl();
    Point br = r.br();
    auto itlow_y = rows.lower_bound(br.y+1);
    auto itlow_x = floor_it(cols, tl.x);
    vector<int> intersect;
    set_intersection(itlow_y->second.begin(), itlow_y->second.end(),
		     itlow_x->second.begin(), itlow_x->second.end(), intersect.begin());
    vector<int> overlap_index;
    for (int j= 0; j < intersect.size(); j++){      
      if (compareRect(r, boundingBox[intersect[j]])){
	overlap_index.push_back(intersect[j]);
      }
    }
    if (overlap_index.size() > 1){
      Rect a0 = boundingBoxes[overlap_index[0]];
      boundingBoxes[i] = Rect(a0.x, a0.width,r.y,r.height);
      for (int j = 1; j < overlap_index.size(); j++){
	a0 = boundingBoxes[overlap_index[j]];	
	boundingBoxes.push_back(Rect(a0.x, a0.width, r.y, r.height);
      }
    }
  }
  return boundingBoxes;
}

  //IMPROVEMENT
  //detect headers ahead of the overlap
vector<vector<string> > find_grid(const vector<Rect>& boundingBoxes,
				  const vector<string>& text)
{
  map<int, int> xtree;
  map<int, int> ytree;
  for (int i = 0; i < boundingBoxes.size(); i++){
    Rect r = boundingBoxes[i];
    xtree.insert({r.x, i});
    ytree.insert({r.y, i});
  }
  mapiv cols = overlapping(xtree, boundingBoxes, [](const Rect& r){return r.width;});
  mapiv rows = overlapping(ytree, boundingBoxes, [](const Rect& r){return r.height;});  
  split(rows, cols, boundingsBoxes);
  vector<vector<string> > table(cols.size(), vector<string>(rows.size(),""));  
  int i = 0, j;
  for (auto c : cols){
    j = 0;
    for (auto r: rows){
      vector<int> intersect;
      set_intersect(c.second.begin(), c.second.end(),
		    r.second.begin(), r.second.end(), intersect.begin());
      for (int k = 0; k < intersect.size(); k++){
	table[i][j] += text[intersect[k]];
      }
      j++;
    }    
    i++
  }  
  return table;
}
