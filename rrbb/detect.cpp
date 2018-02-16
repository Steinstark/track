#include "detect.hpp"
#include <list>
#include <vector>
#include <opencv2/opencv.hpp>
#include "utility.hpp"
#include "image_primitives.hpp"
#include "image_util.hpp"
#include "text_tools.hpp"
#include "tree_helper.hpp"
#include "rlt_detect.hpp"
#include "clt_detect.hpp"
#include "nrlt_detect.hpp"

using namespace std;
using namespace cv;
using namespace tree;

list<Rect> mergeIntersecting(list<Rect>& tables){
  list<Rect> merged;
  RTBox tree;
  for (Rect& table : tables){
    insert2tree(tree, table);
  }
  while (tree.size()){
    auto it = tree_begin(tree);
    Rect r = bb2cvr(*it);
    while (true){
      list<Rect> intersects = search_tree(tree, r);
      if (intersects.empty())
	break;
      for (Rect& intersect: intersects){
	r |= intersect;
	remove_tree(tree, intersect);
      }
    }
    merged.push_back(r);
  }
  return merged;
}

list<Rect> detect(Mat& text, Mat& nontext){
  ImageDataBox imd(text, nontext);
  ImageMeta im(text.cols, text.rows, imd.textData, imd.nontextData);
  list<Rect> rlTables;// = findRLT(imd, im);  
  //vector<Rect> colorTables = findCLT(imd, im); //is untested. Result will not be appended even assuming it works
  list<Rect> nrlTables = findNRLT(text, rlTables); //currently only serve to degrade performance
  //rlTables.insert(rlTables.end(), nrlTables.begin(), nrlTables.end());
  rlTables.splice(rlTables.end(), nrlTables);
  list<Rect> merged = mergeIntersecting(rlTables);
  return merged;
}
