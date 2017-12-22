#include "heuristic_filter.hpp"

#include <vector>
#include "RTree.h"
#include "utility.hpp"
#include "tree_helper.hpp"

using namespace std;
using namespace cv;

using RT = RTree<int, int, 2, float>;

void heuristic_filter(Mat& text, Mat& nontext){
  Mat cc, stats, centroids;
  int labels = connectedComponentsWithStats(text, cc, stats, centroids, 8, CV_32S);
  vector<ComponentStats> components;
  components.push_back(stats2component(stats, 0));
  RT tree;
  for (int i = 1; i < labels; i++){
    ComponentStats cs = stats2component(stats, i);
    components.push_back(cs);
    insert2tree(tree, cs.r, i);
  }
  vector<int> overlap;
  for (int i = 1; i < labels; i++){
    ComponentStats cs = components[i];
    if (cs.area < 6 ||
	search_tree(tree, cs.r) > 5 ||
	//	(cs.hwratio < 0.06 && cs.r.width > cs.r.height) ||      
	cs.density < 0.06){
      move2(text, nontext, cc, i);
    }
  }  
}
