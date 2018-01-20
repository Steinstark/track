#include "find_grid.hpp"

#include <algorithm>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <utility>
#include <functional>
#include <opencv2/opencv.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include "RTree.h"
#include "util.hpp"
#include "tree_helper.hpp"
#include "node.hpp"

using namespace std;
using namespace std::placeholders;
using namespace cv;
using namespace boost::multi_index;

using IndexPair = pair<int, int>;
using RT = RTree<int, int, 2, float>;

struct Part{
  int maxIndex, voteCount, maxLength;
  map<int,int> visited;
  Part(): voteCount(0), maxLength(0){}
  void update(int gridIndex, int length, int bbIndex){
    visited[gridIndex]++;
    if (visited[gridIndex] == 2)
      voteCount++;
    if (length > maxLength){
      maxLength = length;
      maxIndex = bbIndex;
    }
  }
  bool majorityRule(){
    return (double)voteCount/visited.size();
  }
};

using NodeElementDB = multi_index_container<
  NodeElement,
  indexed_by<
    ordered_non_unique<member<NodeElement, int, &NodeElement::a>, less<int> >,
    ordered_unique<member<NodeElement, int, &NodeElement::index>, less<int> >
    >
  >;

std::ostream& operator<<(std::ostream& os, const Cell& c){
  os << "Type: " << c.type;
  if (c.xSpan.first == c.xSpan.second)
    os << " Col: " << c.xSpan.first;
  else
    os << " Col: <" << c.xSpan.first << "," << c.xSpan.second << ">";
  if (c.ySpan.first == c.ySpan.second)
    os << " Row: " << c.ySpan.first;
  else
    os << " Row: <" << c.ySpan.first << "," << c.ySpan.second << ">";
  return os;
}

map<int, int> dimGrid(NodeElementDB& db){
  map<int, int> dimVal;
  auto& dbSet = db.get<0>();
  auto it = dbSet.begin();
  int a = it->a, b = it->b, gridIndex = 0;
  while (it != dbSet.end()){
    if (it->a <= b){
      dimVal[it->index] = gridIndex;
      if (it->b > b){
	b = it->b;
      }
      it++;
    }else{
      a = it->a;
      b = it->b;
      gridIndex++;
    }
  }
  return dimVal;
}

//IMPROVEMENT
//data assumed to be in sync is brittle. Force data to be in sync.
map<int, IndexPair> gridify(NodeElementDB& dbCol, NodeElementDB& dbRow)
{
  map<int, int> first_index = dimGrid(dbCol);
  map<int, int> second_index = dimGrid(dbRow);
  map<int, IndexPair> grid;
  //first and second is assumed to have exactly same key
  auto itf = first_index.begin();
  auto its = second_index.begin();
  while (itf != first_index.end()){
    grid[itf->first] = IndexPair(itf->second, its->second);
    itf++;
    its++;
  }
  return grid;
}

map<IndexPair, Rect>  merge(map<int, IndexPair>& grid, vector<Rect>& bb){
  map<IndexPair, vector<int> > toMerge;
  for (auto p : grid){
    toMerge[p.second].push_back(p.first);
  }
  map<IndexPair, Rect> merged;
  for (auto& pv : toMerge){
    Rect r;
    for (int e : pv.second){
      r |= bb[e];
    }
    merged[pv.first] = r;
  }
  return merged;
}

set<int> findHeaders(map<int, Part>& data){
  set<int> headerIndex;
  for (auto it = data.begin(); it != data.end(); it++){
    if (it->second.majorityRule()){
      headerIndex.insert(it->second.maxIndex);
    }
  }
  return headerIndex;
}

set<int> getHeaderIndex(map<int, IndexPair>& grid, vector<Rect>& bb){
  map<int, Part> cols;
  //map<int, Part> rows;

  for (auto p : grid){
    cols[p.second.first].update(p.second.second, bb[p.first].width, p.first);
    //rows[p.second.second].update(p.second.first, bb[p.first].height, p.first);
  }
  set<int> headerIndex = findHeaders(cols);
  //findHeaders(rows, headerIndex);
  return headerIndex;
}

Cell headerCell(RT& tree, map<int, IndexPair>& grid, Rect rect, int index){
  const int inf = 1000000;
  vector<int> overlapCol;
  vector<int> overlapRow;
  Rect col(rect.x, -inf, rect.width, 2*inf);
  Rect row(-inf, rect.y, 2*inf, rect.height);
  search_tree(tree, col, overlapCol);
  search_tree(tree, row, overlapRow);
  int l = inf, t = inf, r = -inf, b = -inf;
  for (int i = 0; i < overlapCol.size(); i++){
    if (overlapCol[i] != index){
      IndexPair p = grid[overlapCol[i]];
      l = min(l, p.first);
      r = max(r, p.first);
    }
  }
  for (int i = 0; i < overlapRow.size(); i++){
    if (overlapRow[i] != index){
      IndexPair p = grid[overlapRow[i]];
      t = min(t, p.second);
      b = max(r, p.second);
    }
  }
  return Cell(CellType::TITLE, IndexPair(l,r), IndexPair(t,b), rect);
}

vector<Cell> find_grid(vector<Rect>& bb){
  vector<Cell> table;
  NodeElementDB dbCol, dbRow;
  RT tree;
  for (int i = 0; i < bb.size(); i++){
    Rect r =  bb[i];
    Point br = r.br();
    dbCol.insert(NodeElement(r.x, br.x, i));
    dbRow.insert(NodeElement(r.y, br.y, i));
    insert2tree(tree, bb[i], i);
  }
  map<int, IndexPair> grid =  gridify(dbCol, dbRow);
  set<int> headerIndex = getHeaderIndex(grid, bb);
  auto& cip = dbCol.get<1>();
  auto& rip = dbRow.get<1>();
  for (auto it = headerIndex.begin(); it != headerIndex.end(); it++){
    cip.erase(cip.find(*it));
    rip.erase(rip.find(*it));
  }
  grid = gridify(dbCol, dbRow);
  map<IndexPair, Rect> merged = merge(grid, bb);
  vector<Cell> cells;
  for (auto p : merged){
    int x = p.first.first;
    int y = p.first.second;
    cells.push_back(Cell(CellType::DATA, IndexPair(x,x), IndexPair(y, y), p.second));
  }
  for (int e : headerIndex){
    cells.push_back(headerCell(tree, grid, bb[e], e));
  }
  return cells;
}
