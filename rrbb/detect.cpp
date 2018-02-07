#include "detect.hpp"
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

vector<Rect> mergeIntersecting(vector<Rect>& tables){
  set<int> toVisit;
  vector<Rect> merged;
  RT tree;
  for (int i = 0; i < tables.size(); i++){
    toVisit.insert(i);
    insert2tree(tree, tables[i], i);
  }
  while (toVisit.size()){
     int index = *toVisit.begin();
     Rect r = tables[index];
     while (true){
       vector<int> hits = search_tree(tree, r);
       if (hits.empty())
	 break;
       for (int i = 0; i < hits.size(); i++){
	 r |= tables[hits[i]];
	 toVisit.erase(hits[i]);
	 remove_tree(tree, tables[hits[i]], hits[i]);
       }
     }     
     merged.push_back(r);
  }
  return merged;
}

vector<Rect> detect(Mat& text, Mat& nontext){
  ImageDataBox imd(text, nontext);
  ImageMeta im(text.cols, text.rows, imd.textData, imd.nontextData);
  vector<Rect> rlTables = findRLT(imd, im);  
  //vector<Rect> colorTables = findCLT(imd, im); //is untested. Result will not be appended even assuming it works
  //vector<Rect> nrlTables = findNRLT(text, rlTables); //currently only serve to degrade performance
  //rlTables.insert(rlTables.end(), nrlTables.begin(), nrlTables.end());
  vector<Rect> merged = mergeIntersecting(rlTables);
  return merged;
}
