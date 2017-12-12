#include <iostream>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>

using namespace std;
using namespace boost::multi_index;

struct AreaNode{
  int a, w, h, index;
  AreaNode(int a, int w, int h, int index): a(a), w(w), h(h), index(index){}
};

using NodeDB = multi_index_container<
  AreaNode,
  indexed_by<
    ordered_non_unique<member<AreaNode, int, &AreaNode::a>, greater<int> >,
    ordered_non_unique<member<AreaNode, int, &AreaNode::w>, greater<int> >,
    ordered_non_unique<member<AreaNode, int, &AreaNode::h>, greater<int> >,
    ordered_unique<member<AreaNode, int, &AreaNode::index> >,
    ordered_non_unique<member<AreaNode, int, &AreaNode::w>, less<int> >,
    ordered_non_unique<member<AreaNode, int, &AreaNode::h>, less<int> >
    >
  >;


int main(){
  NodeDB test;
  test.insert({0,1,2,3});
  /*test.insert({4,5,6,7});
  test.insert({8,9,10,11});
  
  //  by_area& area_index = test.get<1>();
  // by_area::const_iterator it = area_index.lower_bound(0);
  //ut << it->a << endl;
  auto &v = test.get<1>();
  cout << v.find(4)->index << endl;
  cout << v.rbegin()->w << endl;
  auto t = v.rbegin();
  ++t;
  auto iter = t.base();
  v.erase(iter);
  for (AreaNode e : v){
    cout << e.a << endl;
    }*/
  return 0;
}
