#include <iostream>
#include "RTree.h"

using namespace std;

bool callback(int id, void* arg){
  vector<int>* v = static_cast<vector<int>*>( arg );
  v->push_back(id);
}

int main(){
   RTree<int, int, 2, float> tree;
  int a[] = {1 ,2};
  int b[] = {3, 4};
  int c[] = {0, 0};
  int d[] = {5, 5};
  tree.Insert(a,b,0);
  vector<int> vec;
  int hits = tree.Search(c, d, callback, (static_cast<void*>(&vec)));
  for (int e : vec){
    cout << e << endl;
  }
  return 0;
}
