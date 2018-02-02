#include "tree_helper.hpp"

using namespace std;
using namespace cv;
using RT = RTree<int, int, 2, float>;

bool callback(int id, void* arg){
  vector<int>* v = static_cast<vector<int>*>( arg );
  v->push_back(id);
  return true;
}

void insert2tree(RT& tree, const Rect& r, int i){
  int tl[] = {r.x, r.y};
  int br[] = {r.br().x, r.br().y};
  tree.Insert(tl, br, i);
}

int search_tree(RT& tree, Rect r, vector<int>& vec){
  int tl[] = {r.x, r.y};
  int br[] = {r.br().x, r.br().y};
  return tree.Search(tl, br, callback,(static_cast<void*>(&vec)));
}

int search_tree(RT& tree, Rect r){
  vector<int> junk;
  return search_tree(tree, r, junk);
}

int search_tree(RT& tree, Rect r, int& index, bool (*f)(int, void*)){
  int tl[] = {r.x, r.y};
  int br[] = {r.br().x, r.br().y};
  return tree.Search(tl, br, f, (static_cast<void*>(&index)));
}

void remove_tree(RT& tree, Rect r, int index){
  int tl[] = {r.x, r.y};
  int br[] = {r.br().x, r.br().y};
  tree.Remove(tl, br, index);
}
