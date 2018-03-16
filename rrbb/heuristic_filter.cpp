#include "heuristic_filter.hpp"

#include <vector>
#include "utility.hpp"
#include "image_util.hpp"
#include "tree_helper.hpp"

using namespace std;
using namespace cv;
using namespace tree;

void find_lines(Mat& text, Mat& nontext){
  Mat element = getStructuringElement(MORPH_RECT, Size(1, 29), Point(-1, -1));
  Mat vertical, horizontal;
  erode(text, vertical, element);
  dilate(vertical, vertical, element);
  transpose(element, element);
  erode(text, horizontal, element);
  dilate(horizontal, horizontal, element);
  Mat lines = horizontal | vertical;
  nontext |= lines;
  bitwise_not(lines, text, text);
}

void heuristic_filter(Mat& text, Mat& nontext){
  Mat cc, stats, centroids;
  find_lines(text, nontext);
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
	search_tree(tree, cs.r).size() > 5 ||
	(cs.hwratio < 0.06 && cs.r.width > cs.r.height) ||      
	cs.density < 0.06){
      move2(text, nontext, cc, i);
    }
  }  
}
