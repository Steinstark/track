#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <multimap>
#include <opencv2/opencv.hpp>
#include "find_grid.hpp"

using namespace std;
using namespace cv;


//Take a function as input to get width or heigth
map<int, int> overlapping(map<int, int>& tree, vector<Rect>& boundingBoxes, ){
  map<int, int> overlap;
  int left = right = tree.begin().first;
  for (auto p : tree){
    int ll = p.first;
    int lr = p.first + boundingBoxes[p.second].width;
    if (ll <= right && lr >= left){
      left = min(left, ll);
      right = max(right, lr);
    }else{
      overlap.insert({left, right});
      left = ll;
      right = lr;
    }
  }
  return overlap;
}

vector<vector<string> > find_grid(const vector<Rect>& boundingBoxes,const vector<string>& text){
  map<int, int> xtree;
  map<int, int> ytree;
  for (int i = 0; i < boundingBoxes.size(); i++){
    Rect r = boundingBoxes[i];
    xtree.insert({r.x, i});
    ytree.insert({r.y, i});
  }
  map<int, int> cols = overlapping(xtree, boundingBoxes);
  map<int, int> rows = overlapping(ytree, boundingBoxes);
  split(cols, rows, boundingBoxes);
  vector<vector<string> > table(cols.size(), vector<string>(rows.size(),""));
  for (int i = 0; i < text.size(); i++){
    Rect r = boundingBoxes[i];
    auto itx = cols.lower_bound(r.x);
    auto ity = rows.lower_bound(r.y);
    if (itx->first != r.x)
      itx--;
    if (ity->first !=r.y)
      ity--;
    table[itx->first][ity->first] += text[i];
  }    
  return table;
}
