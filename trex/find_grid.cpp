#include "find_grid.hpp"

#include <algorithm>
#include <vector>
#include <string>
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

using namespace std;
using namespace std::placeholders;
using namespace cv;
using namespace boost::multi_index;

using IndexPair = pair<int, int>;
using RT = RTree<int, int, 2, float>;

struct NodeElement{
  int a, b, index;
  NodeElement(int a, int b, int index): a(a), b(b), index(index){}
};

using NodeElementDB = multi_index_container<
  NodeElement,
  indexed_by<
    ordered_non_unique<member<NodeElement, int, &NodeElement::a>, less<int> >,
    ordered_unique<member<NodeElement, int, &NodeElement::index>, less<int> >
    >
  >;

vector<int> dimGrid(NodeElementDB& db){
  vector<int> dimVal(db.size());
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

vector<IndexPair> gridify(NodeElementDB& dbCol, NodeElementDB& dbRow)
{
  vector<int> first = dimGrid(dbCol);
  vector<int> second = dimGrid(dbRow);
  int n = dbCol.size();
  vector<IndexPair> grid(n);
  for (int i = 0; i < n; i++){
    grid[i].first = first[i];
    grid[i].second = second[i];
  }
  return grid;
}

vector<IndexPair> find_grid(vector<Rect>& bb, vector<string>& text){
  vector<Cell> table;
  NodeElementDB dbCol, dbRow;
  for (int i = 0; i < bb.size(); i++){
    Rect r =  bb[i];
    Point br = r.br();
    dbCol.insert(NodeElement(r.x, br.x, i));
    dbRow.insert(NodeElement(r.y, br.y, i));
  }
  vector<IndexPair> grid = gridify(dbCol, dbRow);
  return grid;
}
